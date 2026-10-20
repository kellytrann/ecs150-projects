#include <cstdio> 
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
using namespace std; 

void error(){
    char msg[30] = "An error has occurred\n";
    write(STDERR_FILENO, msg, strlen(msg));
}

// helper function to tokenize input line -> splits string on whitespace and returns vector of tokens
vector<string> tokenize(const string& s){
    vector<string> out;                     // finished list of words
    string cur;                             // word curr building
    for (char c : s){
        if(c == ' ' || c == '\t' || c == '\n'){
            // whitespace 
            if(!cur.empty()){
                out.push_back(cur); 
                cur.clear(); 
            }
        } else {
            // no whitespcae
            cur += c; 
        }
    }
    if (!cur.empty()){
        out.push_back(cur); 
    }
    return out; 
}

// loop over path directories and check if command exists in any of them
// return full path if found, else return empty string
string findExe(const string& cmd, const vector<string>& path){
    for (const string& dir : path){
        string full = dir + "/" + cmd;
        if (access(full.c_str(), X_OK) == 0){
            return full; 
        }
    }
    return "";              // not found
}

// helper function to split on string at every occurence of specific character
// splits on char not whitespace - differs from tokenize 
vector<string> split(const string& s, char delim){
    vector<string> parts;               // finished pieces
    string cur;                         // current piece being built
    for (char c : s){
        if ( c == delim){
            parts.push_back(cur);       // cut character and save piece 
            cur.clear(); 
        } else {
            cur += c;                   // keep building piece 
        }
    }
    parts.push_back(cur);               // save final piece 
    return parts; 
}  

// returns chhild pid or -1 if there is nothing to wait for
pid_t launch(const string& segment, vector<string>& path){
    // redirection
    vector<string> sides = split(segment, '>');
    if (sides.size() > 2) {
        error(); 
        return -1; 
    }
    vector<string> tokens = tokenize(sides[0]);
    string outfile = "";                        // where output goes if redirecting
    bool redirect = (sides.size() == 2); 

    if (redirect){
        vector<string> files = tokenize(sides[1]);
        // if there is more than one file, error
        if (files.size() != 1){
            error(); 
            return -1; 
        }
        outfile = files[0]; 
    }

    // dealing with blank segment 
    // ex: > file.txt 
    // error bc there is no command to redirect
    if (tokens.empty()){
        if (redirect){
            error(); 
        }
        return -1; 
    }

    // exit
    if(tokens[0] == "exit"){
        if (tokens.size() != 1){
            error(); 
        } else {
            exit(0); 
        }
        return -1; 
    } 
    // change directory -> exactly one argument, and chdir()
    if (tokens[0] == "cd"){
        if (tokens.size() != 2 || chdir(tokens[1].c_str()) != 0){
            error(); 
        }
        return -1; 
    } 
    // replace search path with given directories 
    if (tokens[0] == "path"){
        path.assign(tokens.begin() + 1, tokens.end()); 
        return -1; 
    }   

    string exe = findExe(tokens[0], path);
    if (exe == ""){
        error(); 
        return -1; 
    }   

    // buikds argv for execv
    // execv needs char* array 
    vector<char*> args; 
    for (string& t : tokens) args.push_back((char*)t.c_str());
    args.push_back(nullptr);

    // fork() makes copy of running shell 
    // runs the same code: parent and child 
    // returns diff values so each can tell which it is 
    pid_t pid = fork(); 
    if (pid < 0){
        error(); 
        return -1;
    }
    if(pid == 0){
        // redirection happens only in child 
        if (redirect){
            int fd = open(outfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(fd < 0){
                error(); 
                exit(1); 
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd); 
        }
        // replaces chuild with requested program 
        execv(exe.c_str(), args.data());
        error(); 
        exit(1); 
    }
    return pid; 

}

// sets up shell, loops forever -> read, run, wait 
int main(int argc, char* argv[]){
    FILE* input = stdin; 
    bool interactive = true; 

    if (argc > 2){
        error(); 
        exit(1); 
    }
    if (argc == 2){
        input = fopen(argv[1], "r"); 
        if (!input){
            error(); 
            exit(1);
        }
        interactive = false;
    }

    vector<string> path = {"/bin"}; 
    char* line = nullptr; 
    size_t cap = 0; 

    while(true){
        if (interactive){
            printf("wish> "); 
            fflush(stdout);
        }
        if (getline(&line, &cap, input) == -1){
            exit(0);            // EOF
        }

        vector<string> segments = split(string(line), '&');
        vector<pid_t> pids;

        for (const string& seg : segments){
            pid_t pid = launch(seg, path);
            if (pid > 0){
                pids.push_back(pid); 
            }
        }
        for (pid_t p : pids){ 
            waitpid(p, nullptr, 0); 
        }
    }

}
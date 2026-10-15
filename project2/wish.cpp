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
vector<string> tokensize(const string& s){
    vector<string> out; 
    string cur; 
    for (char c : s){
        if(c == '' || c == '\t' || c == '\n'){
            if(!cur.empty()){
                out.push_back(cur); 
                cur.clear(); 
            } else {
                cur += c; 
            }
        }
        if (!cur.empty()){
            out.push_back(cur); 
        }
    }
    return out; 
}

// loop over path directories and check if command exists in any of them
// return full path if found, else return empty string
string findExe(const string& cmd, const vector<string>& path){
    for (constr string& dir : path){
        string full = dir + "/" + cmd;
        if (access(full.c_str(), X_OK) == 0){
            return full; 
        }
    }
    return "";              // not found
}

// if it returns "", print error and move on 
// if path is empty -> nothing found
vector<char*> args; 
for (string& t : tokens) args.push_back((char*)t.c_str()); 
args.push_back(nullptr);

// fork and execute
pid_t pid = fork(); 
if (pid < 0){
    error(); 
} else if (pid == 0){
    execv(exe.c_str(), args.data()); 
    error();                                // only if execv fails 
    exit(1); 
} else {
    waitpid(pid, nullptr, 0); 
}
        

int main(int argc, char* argv[]){
    FILE* input = stdin; 
    bool interactive = true; 

    if (argc == 2){
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

        vector<string> tokens = tokensize(line);
        if (tokens.empty()){
            continue; 
        }

        string exe = findExe(tokens[0], path);
        if (exe == ""){
            error(); 
            continue; 
        }

        vector<char*> args; 
        for (string )




    }

}
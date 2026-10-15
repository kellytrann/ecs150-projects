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
    }

}
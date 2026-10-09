#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std; 

// helper function
/*
* reads frm file descriptor and prints out every line tha contains the term (foo)
*/
void grepFd(int fd, const string& term){
    string pending; 
    char buffer[4096]; 
    ssize_t bytesRead;

    // read from file descriptor -> returns num of bytes read
    // loop until there are no more bytes to read 
    while((bytesRead = read(fd, buffer, sizeof(buffer))) >  0){
        pending.append(buffer, bytesRead); 

        // chunk can contain several complete lines so we need to continue extracting lines until no newline remains
        size_t pos; 
        while((pos = pending.find('\n')) != string::npos){
            // copy line and remove it and the newline from pending
            string line = pending.substr(0, pos); 
            pending.erase(0, pos + 1); 
            
            // case sensitive substring matching 
            if(line.find(term) != string::npos){
                line += '\n';
                if (write(STDOUT_FILENO, line.c_str(),line.size()) < 0 ){
                    exit(1); 
                }
            }
        }
    }

    // if file did not end with newline -- last line still in pending and has not been checked 
    if (!pending.empty() && pending.find(term) != string::npos){
        pending += '\n'; 
        if(write(STDOUT_FILENO, pending.c_str(), pending.size()) < 0){
            exit(1); 
        }
    }
}


int main(int argc, char* argv[]){
    // argv[0] is program name and argv[1] is search term -> need at least 2 entries 
    if (argc < 2){
        cout << "wgrep: searchterm [file ...]" << endl; 
        exit(1); 
    }
    string term = argv[1]; 
    
    if (argc == 2){
        // call on the helper function
        grepFd(STDIN_FILENO, term); 
    } else {
        // seraching files in order
        for (int i = 2; i < argc; i++){
            int fd = open(argv[i], O_RDONLY); 
            if (fd < 0){
                cout << "wgrep: cannot open file" << endl; 
                exit(1); 
            }
            grepFd(fd, term); 
            close(fd);
        }
    }
    return 0; 
}
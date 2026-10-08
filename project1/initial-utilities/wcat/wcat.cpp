#include <fcntl.h> 
#include <unistd.h> 
#include <iostream>
#include <cstdlib>

using namespace std; 

int main(int argc, char* argvp[]){

    for(int i = 1; i < argc; i++){
        int fd = open(argvp[i], O_RDONLY);
        if (fd < 0){
            cerr << "Error opening file: " << argvp[i] << endl;
            continue; 
        }
    

    char buffer[4096]; 
    ssize_t bytesRead; 
    while((bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0){
        ssize_t bytesWritten = write(STDOUT_FILENO, buffer, bytesRead); 
        }
    close(fd); 
    }
    return 0; 
}
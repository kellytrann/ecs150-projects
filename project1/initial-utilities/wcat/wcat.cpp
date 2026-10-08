#include <fcntl.h> 
#include <unistd.h> 
#include <iostream>
#include <cstdlib>

using namespace std; 

int main(int argc, char* argvp[]){

    for(int i = 1; i < argc; i++){
        int fd = open(argvp[i], O_RDONLY);
        if (fd < 0){
            cout << "wcat: cannot open file" << endl;
            exit(1);
        }
    

    char buffer[4096]; 
    ssize_t bytesRead; 
    while((bytesRead = read(fd, buffer, sizeof(buffer))) > 0){
        ssize_t bytesWritten = write(STDOUT_FILENO, buffer, bytesRead); 
        if (bytesWritten < 0) {
            exit(1); 
        }
    }
    close(fd); 
    }
    return 0; 
}
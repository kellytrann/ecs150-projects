#include <fcntl.h> 
#include <unistd.h> 
#include <iostream>
#include <cstdlib>

using namespace std; 

int main(int argc, char* argvp[]){

    // loop over each file name in argv
    for(int i = 1; i < argc; i++){
        // open file for reading 
        int fd = open(argvp[i], O_RDONLY);
        // if no file, print error message -> exit with 1 
        if (fd < 0){
            cout << "wcat: cannot open file" << endl;
            exit(1);
        }
    

    // reading a chunk, writing out the chunk line by line until there are no more liens
    char buffer[4096]; 
    ssize_t bytesRead; 
    // while there is still text to read, read all that is within the file
    while((bytesRead = read(fd, buffer, sizeof(buffer))) > 0){
        // write out what is in the file
        ssize_t bytesWritten = write(STDOUT_FILENO, buffer, bytesRead); 
        // if nothing left, exit
        if (bytesWritten < 0) {
            exit(1); 
        }
    }
    close(fd); 
    }
    return 0; 
}
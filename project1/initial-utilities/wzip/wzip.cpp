#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>

using namespace std; 
static char outBuf[65536]; 
static size_t outLen = 0; 
// helper function because file too big in test case 6

// send everything collected so far to standard output 
void flushOut(){ 
    size_t offset = 0; 
    while(offset < outLen){
        // write() will not write everythign in one call, so keep looping 
        ssize_t w = write(STDOUT_FILENO, outBuf + offset, outLen - offset);
        if (w < 0){
            exit(1); 
        }
        offset += w; 
    }
    outLen = 0; 
}



// helper function 
/* 
* writes one finished streak -> count as 4 raw bytes then the character 
*/
void writeRun(uint32_t count, char ch){
    if (outLen + 5 > sizeof(outBuf)){
        // no room for 5 more bytes -> empty buffer first 
        // allow for no hanging to happen... 
        flushOut(); 
    }
    memcpy(outBuf + outLen, &count, 4); 
    outBuf[outLen + 4] = ch;
    outLen += 5; 
}

int main(int argc, char* argv[]){

    // no files given 
    if (argc < 2){
        cout << "wzip: file1 [file2 ...]" << endl;
        exit(1);
    }

    // length of current streak
    uint32_t count = 0; 
    char current = 0; 

    for (int i = 1; i < argc; i++){
        int fd = open(argv[i], O_RDONLY); 
        if (fd < 0){ 
            cout << "wzip: cannot open file" << endl; 
            exit(1); 
        }

        char buffer[4096];
        ssize_t bytesRead;
        while((bytesRead = read(fd, buffer, sizeof(buffer))) > 0){ 
            for (ssize_t j = 0; j < bytesRead; j++){ 
                char c = buffer[j]; 

                if(count == 0){
                    // first character -> no streak so we count at 1 
                    current = c; 
                    count = 1; 
                    // if the character is the same as the current -> make streak longer
                } else if (c == current){ 
                    count++; 
                } else { 
                    // if character is different -> write out current streak and start new one 
                    writeRun(count, current); 
                    current = c;
                    count = 1; 
                }

            }
        }
        close (fd); 
    }

    if (count > 0){ 
        writeRun(count, current);
    }
    flushOut(); 
    return 0; 
}
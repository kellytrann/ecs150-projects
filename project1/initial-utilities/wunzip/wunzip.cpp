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

// adds count copies of ch to the output buffer -> flushes whenever it fills 
void emit(char ch, uint32_t count){
    while(count > 0){ 
        if (outLen == sizeof(outBuf)){
            // when the buffer is full, empty it using our helper function from wzip.cpp 
            flushOut(); 
        }
        // copy as many as fit in the remaining space 
        size_t space = sizeof(outBuf) - outLen;
        size_t n = (count < space) ? count : space;
        memset(outBuf + outLen, ch, n);
        outLen += n; 
        count -= n; 
    }
}


// helper function to read exactly n bytes into the buffer
// returns num of bytes actually read -> less than n only if file ended 
size_t readFull(int fd, void* buf, size_t n){ 
    size_t total = 0; 
    while(total < n){ 
        ssize_t r = read(fd, (char*)buf + total, n - total);
        if(r < 0){ 
            exit(1); 
        }
        if (r == 0){ 
            break; 
        }
        total += r;
    }
    return total; 
}

int main(int argc, char* argv[]){
      // no files given 
    if (argc < 2){
        cout << "wunzip: file1 [file2 ...]" << endl;
        exit(1);
    }

    for (int i = 1; i < argc; i++){ 
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0){ 
            cout << "wunzip: cannot open file" << endl; 
            exit(1); 
        }

        while (true) { 
            uint32_t count;
            char ch; 
            // fewer than 4 bytes left -> end of file so break 
            if (readFull(fd, &count, sizeof(count)) < sizeof(count)){ 
                break; 
            }
            // count with no character after it -> so break 
            if(readFull(fd, &ch, 1) < 1){ 
                break; 
            }
            emit(ch, count);
        }
        close(fd);
    }
    flushOut(); 
    return 0; 
}
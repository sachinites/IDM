#include <stdio.h>
#include "FileDownLoader.h"

#define printf(a, ...) 

FD::FD () {

}

FD::~FD() {
    
}

void 
FD::SetByteRange (int low, int high) {

    this->low_byte = low;
    this->high_byte = high;

    printf ("Download Range : [%d - %d]\n", low, high);

    if (low || high) {
        this->flags |= FD_DNLOAD_PARTIAL_REQ;
    }
    
}

int
FD::GetFileSize() {

    int n =  ((this->low_byte == 0 && this->high_byte == 0) ? this->file_size :   \
                                (this->high_byte - this->low_byte + 1 ));
    return n;
}
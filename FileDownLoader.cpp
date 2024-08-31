#include <stdio.h>
#include "FileDownLoader.h"

FD::FD () {

}

FD::~FD() {
    
}

void 
FD::SetByteRange (int low, int high) {

    this->low_byte = low;
    this->high_byte = high;

    printf ("Download Range : [%d - %d]\n", low, high);
}
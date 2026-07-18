#include <stdio.h>
#include <Windows.h>

long long free_disk_space(const char *driver){
    ULARGE_INTEGER freebytes;
    if(GetDiskFreeSpaceExA(
        driver,
        &freebytes,
        NULL,
        NULL)){
            return freebytes.QuadPart;
        }
    return -1;
}


int main(){
    long long space = free_disk_space("c:\\");
    if(space != -1){
        printf("\n\tFree Space is : %lld bytes\n", space);
    }else{
        printf("\n\tError reading disk space !\n");
    }


    return 0;
}


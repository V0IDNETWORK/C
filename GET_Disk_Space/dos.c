#include <stdio.h>
#include <conio.h>
#include <dos.h>

long free_disk(int driver);

void main(){
    int driver = 0;
    long int spc;

    spc = free_disk(driver);
    printf("\n free space is : %ld", spc);
    getch();
}

long free_disk(int driver){
    union REGS in, out;
    in.h.ah = 0x36;
    in.h.dl = driver;
    intdos(&in, &out);
    return((long) out.x.ax * (long) out.x.bx * (long) out.x.cx);
}

//typedef to prevent errors in old sys declaration that has not been deleted by freebsd developers
//typedef unsigned long uintfptr_t;

//typedef unsigned long intrmask_t;

#include <sys/types.h>
#include <sys/module.h>
#include <sys/systm.h>  /* uprintf */
#include <sys/param.h>  /* defines used in kernel.h */
#include <sys/kernel.h> /* types used in module initialization */
#include <sys/conf.h>   /* cdevsw struct */
#include <sys/uio.h>    /* uio struct */
#include <sys/malloc.h>
#include <sys/kthread.h>
#include <sys/unistd.h>//int start();

#include "whatif.h"




int main() {

    printf("Hello world!\n");
    rustp();
    //printf("Number is %d.\n", start());
    return 0;

}

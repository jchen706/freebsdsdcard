
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <sys/bus.h>
#include <sys/conf.h>
#include <sys/bio.h>
#include <sys/kthread.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/uio.h>
#include <machine/bus.h>
#include <geom/geom_disk.h>




//Find a way to probe SD Card first

// struct disk *
// disk_alloc(void);a

// void disk_create(struct disk *disk, int version);

// void disk_destroy(struct disk *disk);



static int skel_loader(struct module *m, int what, void *arg)
{
        //printf("event skel is called");
        int err = 0;
        
        //0        


        switch (what) {
        case MOD_LOAD:
                int sd_card = open("/dev/mmcsd0s1", O_RDONLY)
                uprintf("Open the sd Card. Number is %d \n", sd_card);
                uprintf("Skeleton KLD loaded.\n");
                break;
        case MOD_UNLOAD:
                
                
                uprintf("Skeleton KLD unloaded.\n");
                break;
        default:
                uprintf("Error.\n");
                err = EOPNOTSUPP;
                break;
        }
        //printf(err);
        return err;
}

static moduledata_t skel_mod = {
        "skel",
        skel_loader,
        NULL
};

DECLARE_MODULE(skeleton, skel_mod, SI_SUB_KLD, SI_ORDER_ANY);




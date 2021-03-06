#include <sys/types.h>
#include <sys/module.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/kernel.h>

static int skel_loader(struct module *m, int what, void *arg)
{
        //printf("event skel is called");
        int err = 0;

        switch (what) {
        case MOD_LOAD:
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


// reference with FreeBSD Device Driver by Joseph Kong



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

#include <dev/spibus/spi.h>
#include <dev/mmc/bridge.h>
#include <dev/mmc/mmc_private.h>
#include <dev/mmc/mmc_subr.h>
#include <dev/mmc/mmcreg.h>
#include <dev/mmc/mmcbrvar.h>
#include <dev/mmc/mmcvar.h>

#include "mmcbr_if.h"
#include "mmcbus_if.h"
//#include "spibus_if.h"
//#include "mmcreg.h"
//response size is 48 bits



#define	SD_GO_IDLE_STATE	0
#define	SD_SEND_IF_COND		8
#define	SD_READ_SINGLE_BLOCK	17
#define	SD_READ_MULTIPLE_BLOCK	18
#define	ACMD_SD_SEND_OP_COND	41
#define	SD_STOP_TRANSMISSION	12
#define SD_APP_CMD    55


struct sd_softc {
    device_t     dev;
    struct mtx                      sd_mtx;
    struct intr_config_hook         sd_ich;
    struct disk                    *sd_disk;
    struct bio_queue_head           sd_bioq;
    struct proc                    *sd_proc;
    struct card_reg                *card_register;
    
};


struct card_reg {
    uint64_t cid[2];  //card identification number
   
    uint16_t rca;        //relative card address, local system address of the card
                         //dynamically suggested by the card and approved by the host during intialization
    
    uint16_t dsr;           //driver state register, to configure the card's output drivers
    uint64_t csd[2];     // card specific data, information about the card's operation conditions
       
    uint64_t scr;         //sd configuration register
    uint32_t ocr;    // operation condition register
    uint8_t ssr[512];   //sd status
    uint32_t csr;     //information about card status
}

// struct sd_card_vars {
//     // help reference from mmcsd.c source 
//     device_t     sd_dev;
//     device_t     mmcbus;                //the device bus to get 
//     struct sd_softc sd_device_part     //disk driver
//     //enum         sd_card_mode mode; 
//     //__daddr_t    block_start, block_end; //used for disk address space type long 
//     int running;
//     int suspend;
// }


//device routine to probe a device information
static int sd_probe(device_t dev){

        //device set description 
        device_set_desc(dev, "2020 SD Driver Test Code");
        return (BUS_PROBE_SPECIFIC);
}

static int sd_detach(device_t dev){
    //physical input or ouput error that will be reported
    //after subsequent operation on the same file descriptor
    return (EIO)
}



// attach device to the kernel system
static int sd_attach(device_t dev) {

    //int error;

    //returns driver-specific software context of dev
    //softc is automatically allcated and zerod by device is attached 
    struct sd_softc *sc = device_get_softc(dev);

    sc->dev = dev;


    //initialize the mutex
    mtx_init(&(_sc)->sd_mtx, device_get_nameunit(_sc->dev), "sd_card", MTX_DEF);

   

    //from the mmc.c 
    // We'll probe and attach our children later, but before / mount */
    sc->config_intrhook.ich_func = sd_delayed_attach;
    sc->config_intrhook.ich_arg = sc;

    if (config_intrhook_establish(&sc->config_intrhook) != 0)
        device_printf(dev, "config_intrhook_establish failed\n");
    return (0)

}


//get the status of the register
static int sd_get_status(device_t dev) {
    
    struct spi_command cmd;
    uint8_t tx_buf[8], rx_buf[8];

    memset(&cmd, 0, sizeof(cmd));
    memset(tx_buf, 0, sizeof(tx_buf));
    memset(rx_buf, 0, sizeof(rx_buf));

    tx_buf[0] = ACMD_SD_SEND_OP_COND; // check if the card is ready  41
    cmd.tx_cmd = &tx_buf[0];
    cmd.rx_cmd = &rx_buf[0];
    cmd.tx_cmd_sz = 6;
    cmd.rx_cmd_sz = 6;
    SPIBUS_TRANSFER(device_get_parent(dev), dev, &cmd);

    return (rx_buf[0]);

}

//set the sd card into idle mode 
static int sd_get_idle(struct sd_softc *sc) {
    
    struct spi_command cmd;
    uint8_t tx_buf[8], rx_buf[8];

    memset(&cmd, 0, sizeof(cmd));
    memset(tx_buf, 0, sizeof(tx_buf));
    memset(rx_buf, 0, sizeof(rx_buf));

    tx_buf[0] = SD_GO_IDLE_STATE; // check if the card is ready  41
    cmd.tx_cmd = &tx_buf[0];
    cmd.rx_cmd = &rx_buf[0];
    cmd.tx_cmd_sz = 6;
    cmd.rx_cmd_sz = 6;
    SPIBUS_TRANSFER(device_get_parent(sc->dev), sc->dev, &cmd);

    return (rx_buf[0]);

}



//wait for the device to initalize
static int sd_wait_for_device_ready(device_t dev) {

    while((sd_get_status(dev))!=0) {
        continue;
    }

    return 0;

}


static void sd_delayed_attach(void *arg)
{
          struct s_softc *sc = arg;

          sd_get_idle(sc);
          //set cs signal low, set to negative mmcbr_set_chip_select funtion is in 

          mmcbr_set_chip_select(dev, -1);
         
          sd_wait_for_device_ready(sc->dev);

          sc->sd_disk = disk_alloc();
          sc->sd_disk->d_name = "sd";
          sc->sd_disk->d_unit = device_get_unit(sc->dev);
          sc->sd_disk->d_strategy = sd_strategy;
          sc->sd_disk->d_sectorsize = 512;
         
         
          sc->s_disk->d_drv1 = sc;

          bioq_init(&sc->sd_bioq);
          kproc_create(&sd_task, sc, &sc->sd_proc, 0, 0, "sd");

          disk_create(sc->sd_disk, DISK_VERSION);
          config_intrhook_disestablish(&sc->sd_ich);
}

static void sd_strategy(struct bio *bp){
          //strategy for the bio
          struct sd_softc *sc = bp->bio_disk->d_drv1;

          mtx_lock(&sc->sd_mtx);
          bioq_disksort(&sc->sd_bioq, bp);
          wakeup(sc);
          mtx_unlock(&sc->sd_mtx);

}

static void sd_task(void *arg) {
    //utilize from FreeBSD device driver book
    //task is 

    struct sd_softc *sc = arg;
    struct bio *bp;
    struct spi_command cmd;
    uint8_t tx_buf[8], rx_buf[8];
    int ss = sc->sd_disk->d_sectorsize;
    daddr_t block, end;
    char *vaddr;

    for (;;) {
            mtx_lock(&sc->sd_mtx);
            do {
                    bp = bioq_first(&sc->sd_bioq);
                    if (bp == NULL)
                            mtx_sleep(sc, &sc->sd_mtx, PRIBIO,
                                "sd", 0);
            } while (bp == NULL);
            bioq_remove(&sc->sd_bioq, bp);
            mtx_unlock(&sc->sd_mtx);

            end = bp->bio_pblkno + (bp->bio_bcount / ss);
            for (block = bp->bio_pblkno; block < end; block++) {
                    vaddr = bp->bio_data + (block - bp->bio_pblkno) * ss;
                    if (bp->bio_cmd == BIO_READ) {
                            tx_buf[0] = SD_READ_SINGLE_BLOCK;
                            cmd.tx_cmd_sz = 6;
                            cmd.rx_cmd_sz = 6;
                    } else {
                            tx_buf[0] = SD_STOP_TRANSMISSION;
                            cmd.tx_cmd_sz = 6;
                            cmd.rx_cmd_sz = 6;
                    }

                    /* FIXME: This works only on certain devices. */
                    tx_buf[1] = 0;
                    tx_buf[2] = 0;
                    tx_buf[3] = 0;
                    tx_buf[4] = 0;
                    cmd.tx_cmd = &tx_buf[0];
                    cmd.rx_cmd = &rx_buf[0];
                    cmd.tx_data = vaddr;
                    cmd.rx_data = vaddr;
                    cmd.tx_data_sz = ss;
                    cmd.rx_data_sz = ss;
                    SPIBUS_TRANSFER(device_get_parent(sc->sd_dev),
                        sc->sd_dev, &cmd);
            }
            biodone(bp);
    }
}

static device_method_t sd_methods[] = {
          /* Device interface. */
          DEVMETHOD(device_probe,         sd_probe),
          DEVMETHOD(device_attach,        sd_attach),
          DEVMETHOD(device_detach,        sd_detach),
          { 0, 0 }
};

static driver_t sd_driver = {
          "sd_driver",
          sd_method,
          sizeof(struct sd_softc)
};

DRIVER_MODULE(sd, spibus, sd_driver, sd_devclass, 0, 0);




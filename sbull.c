/*
 * Sample disk driver, from the beginning.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/timer.h>
#include <linux/types.h>	/* size_t */
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/hdreg.h>	/* HDIO_GETGEO */
#include <linux/kdev_t.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>	/* invalidate_bdev */
#include <linux/bio.h>
#include <linux/lz4.h>
#include <linux/lzo.h>
#include <linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");

static int sbull_major = 0;
static int hardsect_size = 512;
static int nsectors = 1024 * 1024;	/* How big the drive is */
static int ndevices = 1;

#define n_sects 1024 * 1024

/*
 * Minor number and partition management.
 */
#define SBULL_MINORS	16
#define MINOR_SHIFT	4
#define DEVNUM(kdevnum)	(MINOR(kdev_t_to_nr(kdevnum)) >> MINOR_SHIFT

/*
 * We can tweak our hardware sector size, but the kernel talks to us
 * in terms of small sectors, always.
 */
#define KERNEL_SECTOR_SIZE	512

/*
 * After this much idle time, the driver will simulate a media change.
 */
#define INVALIDATE_DELAY	30*HZ

/*
 * Table for each sector
 *  index 0 : com_ret, 1 : src_len, 2 : is_compress
 */
int table[n_sects][3] = {{0}};
		
/*
 * The internal representation of our device.
 */
struct sbull_dev {
    /* TODO : Write your codes */
	int size;
	int users;
	char *data;
	spinlock_t lock;

	struct request_queue *queue;
	struct gendisk* gd;
};

static struct sbull_dev *Devices = NULL;

/*
 * Handle an I/O request.
 */

static void sbull_transfer(struct sbull_dev *dev, unsigned long sector,
		unsigned long nsect, char *buffer, int write)
{
    /* TODO : Write your codes */
	unsigned long offset = sector * KERNEL_SECTOR_SIZE;
	unsigned long nbytes = nsect * KERNEL_SECTOR_SIZE;

	int src_len = 0;
	unsigned long dst_buf_size = 0;
	int com_ret = 0;

	if((offset + nbytes) > dev->size) {
		printk(KERN_NOTICE "Beyond-end write (%ld %ld)\n", offset, nbytes);
		return;
	}
	

	if(write) {
		printk("write -------------------------\n");
		src_len = strlen(buffer);
		
		if (src_len == 0) {
			// not compression mode
			// Because input size is 0
			memcpy(dev->data + offset, buffer, nbytes);
		} else {
			// compression mode
			printk(KERN_INFO "[compress] Compress mode turn on !!!\n");
			unsigned char *buf;
			unsigned char *dst;
			
			buf = kmalloc(LZ4_MEM_COMPRESS, GFP_KERNEL);
			dst_buf_size = LZ4_compressBound(strlen(buffer));
			dst = kmalloc(dst_buf_size, GFP_KERNEL);
			memset(dst, 0, dst_buf_size);
			com_ret = LZ4_compress_default(buffer, dst, src_len, dst_buf_size, buf);
			if (!com_ret) {
				pr_err("LZ4 compress_default error: compression failed!\n");
			}
			
			// if the input size is so small, compressed data size is larger than source data
			// So, in that case, we do not compress data
			if (com_ret > src_len) {
				memcpy(dev->data + offset, buffer, nbytes);
				table[sector][2] = 1;
			} else {
				memcpy(dev->data + offset, dst, nbytes);
				table[sector][0] = com_ret;
				table[sector][1] = src_len;
			}
			printk("[compress] source: %s, len: %d\n", buffer, strlen(buffer));
			printk("[compress] encode: %s, len: %d\n", dst, com_ret);
			
			kfree(buf);
			kfree(dst);
		}	
	}
	else {
		printk("read -------------------------\n");
		com_ret = table[sector][0];
		src_len = table[sector][1];
		
		int is_compress = table[sector][2];
		
		printk("com_ret: %d, src_len: %d\n", com_ret, src_len);
		
		// If src_len is 0, the input data size is 0 -> we did not compress data
		// If is_compress is 1, we did not compress data
		// Because the compressed data size is larger than source data size
		if (src_len == 0 || is_compress == 1) {
			memcpy(buffer, dev->data + offset, nbytes);
		} else {
			// decompression mode
			unsigned char* dst;
			
			dst = kmalloc(com_ret, GFP_KERNEL);
			memcpy(dst, dev->data + offset, com_ret);
			
			printk("[decompress] dst: %s, len: %d\n", dst, com_ret);
			
			unsigned char *dec;
			int dcom_ret;
			dec = kmalloc(src_len, GFP_KERNEL);
			dcom_ret = LZ4_decompress_safe(dst, dec, com_ret, src_len);
			if (dcom_ret < 0) {
				// If decompression error, we do not decompress
				// It is just for to prevent stop program
				memcpy(buffer, dev->data + offset, nbytes);
				pr_err("LZ4_decompress_safe error, ret = %d\n", dcom_ret);
			} else {
				printk("[decompress] dec: %s, len: %d\n", dec, src_len);
				memcpy(buffer, dec, nbytes);
			}

			kfree(dst);
			kfree(dec);
		}
	}
	// It is to show sector and offset value
	printk("sector: %ld, nsect: %ld\n", sector, nsect);
	printk("offset: %ld, nbytes: %ld\n", offset, nbytes);
}


/*
 * Transfer a single BIO.
 */
static int sbull_xfer_bio(struct sbull_dev *dev, struct bio *bio)
{
    /* TODO : Write your codes */
	struct bio_vec bvec;
	struct bvec_iter i;
	sector_t sector = bio->bi_iter.bi_sector;

	bio_for_each_segment(bvec, bio, i) {
		char *buffer = __bio_kmap_atomic(bio, i);
	
	    sbull_transfer(dev, sector, bio_cur_bytes(bio) >> 9,
				buffer, bio_data_dir(bio) == WRITE);

		sector += bio_cur_bytes(bio) >> 9;
		__bio_kunmap_atomic(buffer);	
	}
//    sbull_transfer(dev, sector, bio_cur_bytes(bio) >> 9,
//				buffer, bio_data_dir(bio) == WRITE);

//    sector += bio_cur_bytes(bio) >> 9;
	return 0; /* Always "succeed" */
}


/*
 * The direct make request version.
 */
static void sbull_make_request(struct request_queue *q, struct bio *bio)
{
	struct sbull_dev *dev = q->queuedata;
	int status;

	status = sbull_xfer_bio(dev, bio);
    bio_endio(bio);	
}


/*
 * Open and close.
 */
/* changed open and release function */

static int sbull_open(struct block_device *bdev, fmode_t mode)
{
	
    struct sbull_dev *dev = bdev->bd_disk->private_data;
    /* file->private_data = dev is deleted*/
    /* TODO : Write your codes */
	spin_lock(&dev->lock);
	if(!dev->users) {
		// do something
	}
	dev->users++;
	spin_unlock(&dev->lock);
    return 0;
}

static void sbull_release(struct gendisk *disk, fmode_t mode)
{
	struct sbull_dev *dev = disk->private_data;
    spin_lock(&dev->lock);
	dev->users--;
	if(!dev->users) {
		// do something
	}
	spin_unlock(&dev->lock);
	return 0;

}

/*
 * The ioctl() implementation
 */

int sbull_ioctl (struct block_device *bdev, fmode_t mode,
                 unsigned int cmd, unsigned long arg)
{
    /* TODO : Write your codes */
	int ret = 0; 
	return ret; /* unknown command */
}



/*
 * The device operations structure.
 */
static struct block_device_operations sbull_ops = {
    /* TODO : Write your codes */
	.owner = THIS_MODULE,
	.open = sbull_open,
	.release = sbull_release,
	.ioctl = sbull_ioctl,
};


/*
 * Set up our internal device.
 */
static void setup_device(struct sbull_dev *dev, int which)
{
    /* TODO : Write your codes */
    /*
     * Get some memory.
     */

	memset(dev, 0, sizeof(struct sbull_dev));

    dev->size = nsectors*hardsect_size;

	dev->data = vmalloc(dev->size);
	if(dev->data == NULL) {
		printk(KERN_NOTICE "vmalloc failure.\n");
		return;
	}

	spin_lock_init(&dev->lock);

    /*
     * The I/O queue, depending on whether we are using our own
     * make_request function or not.
     */
    dev->queue = blk_alloc_queue(GFP_KERNEL);
    if (dev->queue == NULL)
        goto out_vfree;
    blk_queue_make_request(dev->queue, (make_request_fn *)sbull_make_request);		
    dev->queue->queuedata = dev;
    /*
     * And the gendisk structure.
     */

	dev->gd = alloc_disk(SBULL_MINORS);
	if(!dev->gd) {
		printk(KERN_NOTICE "alloc_disk failure\n");
		goto out_vfree;
	}
	dev->gd->major = sbull_major;
	dev->gd->first_minor = which*SBULL_MINORS;
	dev->gd->fops = &sbull_ops;
	dev->gd->queue = dev->queue;
	dev->gd->private_data = dev;
	snprintf(dev->gd->disk_name, 32, "sbull%c", which+'a');
	set_capacity(dev->gd, nsectors*(hardsect_size/KERNEL_SECTOR_SIZE));
	add_disk(dev->gd);
    
    return;

out_vfree:
    if (dev->data)
        vfree(dev->data);
}



static int __init sbull_init(void)
{
	int i;
	/*
	 * Get registered.
	 */
	sbull_major = register_blkdev(sbull_major, "sbull");
	if (sbull_major <= 0) {
		printk(KERN_WARNING "sbull: unable to get major number\n");
		return -EBUSY;
	}
	/*
	 * Allocate the device array, and initialize each one.
	 */
	Devices = kmalloc(ndevices*sizeof (struct sbull_dev), GFP_KERNEL);
	if (Devices == NULL)
		goto out_unregister;
	for (i = 0; i < ndevices; i++) 
		setup_device(Devices + i, i);

    return 0;

  out_unregister:
	unregister_blkdev(sbull_major, "sbd");
	return -ENOMEM;
}

static void sbull_exit(void)
{
	int i;

	for (i = 0; i < ndevices; i++) {
		struct sbull_dev *dev = Devices + i;

		if (dev->gd) {
			del_gendisk(dev->gd);
			printk("gendisk 1??\n");
			put_disk(dev->gd);
			printk("gendisk 2 ??\n");
		}
		if (dev->queue) {
				kobject_put (&dev->queue->kobj);
		}
		if (dev->data)
			vfree(dev->data);
	}
	unregister_blkdev(sbull_major, "sbull");
	kfree(Devices);
}
	
module_init(sbull_init);
module_exit(sbull_exit);

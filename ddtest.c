/*
 *	Author: Mihir J. Vegad
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

static int ddtest_init(void)
{
/*	 struct file *f;
        char buf[1024];

        mm_segment_t fs;
        int i;
        for(i=0;i<1024;i++)
                buf[i]=0;

        f = filp_open("/dev/shm/ivshmem", O_RDONLY, 0);
        if (f==NULL)
                printk(KERN_ALERT "filp_open error.\n");
        else
        {
                fs = get_fs();
                set_fs(get_ds());
                f->f_op->read(f,buf,1024,&f->f_pos);
                set_fs(fs);
                printk(KERN_INFO "buf:%s\n",buf);
        }
        filp_close(f,NULL);
*/
	int shm_fd;
	void *ptr;
	int i;

	/*open the shared memory segment*/
	shm_fd = shm_open("ivshmem", O_RDONLY, 0666);
	if (shm_fd==-1)
                printk(KERN_ALERT "shm_open error.\n");
        else
        {
        /*Now map the shared memory segment in the address space of the process*/
		ptr=mmap(0, 1048576, PROT_READ, MAP_SHARED, shm_fd, 0);
		if(ptr==MAP_FAILED)
			 printk(KERN_INFO "Map failed.\n");

                printk(KERN_INFO "buf:%s\n",ptr);
        }

	/* remove the shared memory segment */
	if (shm_unlink("ivshmem") == -1) {
		printk(KERN_INFO "Error removing ivshmem\n");
		//exit(-1);
	}

	printk(KERN_ALERT "testing module.\n");
	return 0;
}

static void ddtest_exit(void)
{
	printk(KERN_ALERT "exiting testing module.\n");
}

module_init(ddtest_init);
module_exit(ddtest_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MIHIR");
MODULE_DESCRIPTION("Device Driver Testing module");




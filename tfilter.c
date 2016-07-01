#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/netfilter_bridge.h>
#include <linux/neighbour.h>
#include <net/arp.h>
#include <linux/export.h>
#include <linux/rculist.h>
#include <linux/time.h>
#include <linux/random.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/kfifo.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/slab.h>


int filter_init(void)
{

	 char string[] = "A B C";
    char *stringp = string;
    const char *delim = " ";
    char *token;

    // stringp is updated to point to the next token 'B'
    token = strsep(&stringp, delim);
    printk(KERN_ALERT "token = '%s', ", token);

    // stringp is updated to point to the next token 'C'
    token = strsep(&stringp, delim);
    printk(KERN_ALERT "token = '%s', ", token);
    
    // In case no delimiter was found, stringp is made NULL.
    token = strsep(&stringp, delim);
    printk(KERN_ALERT "token = '%s', ", token);
    

        printk(KERN_ALERT "tfilter working.\n");
        return 0;
}

int filter_exit(void)
{
        return 0;
}


module_init(filter_init);
module_exit(filter_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MIHIR");
MODULE_DESCRIPTION("Tokenizer testing.");


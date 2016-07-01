/*
 *	Author: Mihir J. Vegad
 */

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

extern int (*bridge_filter)(struct sk_buff *);


static int do_bfilter(struct sk_buff *skb)
{
	struct iphdr* iph = ip_hdr(skb);
        struct ethhdr* mh = eth_hdr(skb);
        struct tcphdr* tcph = tcp_hdr(skb);

	((unsigned char *)&(iph->saddr))[0]=10;
	((unsigned char *)&(iph->saddr))[1]=126;
	((unsigned char *)&(iph->saddr))[2]=17;
	((unsigned char *)&(iph->saddr))[3]=255;

	(mh->h_dest)[0]=0x1A;
	(mh->h_dest)[1]=0x1B;
	(mh->h_dest)[2]=0xff;
	(mh->h_dest)[3]=0xff;
	(mh->h_dest)[4]=0x55;
	(mh->h_dest)[5]=0xff;

	printk(KERN_ALERT "New IP is:%d.%d.%d.%d\n", ((unsigned char *)&(iph->saddr))[0],((unsigned char *)&(iph->saddr))[1],((unsigned char *)&(iph->saddr))[2],((unsigned char *)&(iph->saddr))[3]);
	printk(KERN_ALERT "New Mac is:%02X:%02X:%02X:%02X:%02X:%02X\n", (mh->h_dest)[0], (mh->h_dest)[1], (mh->h_dest)[2], (mh->h_dest)[3], (mh->h_dest)[4], (mh->h_dest)[5]);

	return 0;
}

static int hello_init(void)
{
	bridge_filter = do_bfilter;
	return 0;
}

static void hello_exit(void)
{
	bridge_filter = NULL;
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MIHIR");
MODULE_DESCRIPTION("Testing module");




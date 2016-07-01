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


int intToBinary(int num);

void tostring(char str[], int num);

/* mutex for middlebox state linked list access */
//extern struct mutex mbllmutex;

extern void (*bridge_lb)(struct sk_buff *);     //changed

/* To get commnad from middlebox */
extern struct Mbinfo* mymb; 

// head node of middlebox linked list
extern struct mbState1 mbState_list1;
//EXPORT_SYMBOL(mbState_list);

/* return value - 1 (drop the packet) | 0 (forward the packet) */
static void do_blb(struct sk_buff *skb)
{
	struct iphdr* iph = ip_hdr(skb);
	struct ethhdr* mh = eth_hdr(skb);
	//struct tcphdr* tcph = tcp_hdr(skb);

    

	// traversing linkedlist of the registered middleboxes 
	struct list_head *ptr;
	struct mbState1 *entry;

	list_for_each(ptr,&mbState_list1.list) {
		entry = list_entry(ptr, struct mbState1, list);
		//printk(KERN_ALERT "bfilter_check for dest:%s\n", &(entry->macaddr));
        //printk(KERN_ALERT "MAC Addr matched:%d\n", strcmp(entry->macaddr,mh->h_dest));
        unsigned char t1[18];   
        snprintf(t1,18,"%pM",&(mh->h_dest));

        // flags for non-recursive mutexes
        int m1=0;
        mutex_lock(&entry->node);
	if (strcmp(entry->macaddr,t1)==0)
	{
            m1=1;
            mutex_unlock(&entry->node);
           // printk(KERN_ALERT "MAC Addr matched:%pM\n", &(entry->macaddr));
            mutex_lock(&entry->node);
	    if(entry->balanceType==0)
	    {
		unsigned char temps[16];
		//strcpy(entry->destip,temps);
		int j=0;
		for(;j<16;j++)
		{
			temps[j]=entry->destip[j];
		}
	//	printk(KERN_ALERT "temps is:%s\n", temps);
		char *strp = temps;
		const char *delim = ".";
		//char *token;
	//	printk(KERN_ALERT "strp is:%s\n", strp);
		long val=0;
		int i=0;
                i = kstrtol(strsep(&strp, delim),10,&val);
		//printk(KERN_ALERT "nxt strsep is:%s\n", strsep(&strp, delim));
	//	printk(KERN_ALERT "ip0:%d,", val);
                if (i!=0)
                {       printk(KERN_ALERT "kstrtol failed.\n");
                        return;
                }
		((unsigned char *)&(iph->daddr))[0]=val;

		//long val1=0;
		i = kstrtol(strsep(&strp, delim),10,&val);
	//	printk(KERN_ALERT "ip1:%d,", val);
                if (i!=0)
                {       printk(KERN_ALERT "kstrtol failed.\n");
                        return;
                }
                ((unsigned char *)&(iph->daddr))[1]=val;

		i = kstrtol(strsep(&strp, delim),10,&val);
		//printk(KERN_ALERT "ip2:%d,", val);
                if (i!=0)
                {       printk(KERN_ALERT "kstrtol failed.\n");
                        return;
                }
                ((unsigned char *)&(iph->daddr))[2]=val;

		i = kstrtol(strsep(&strp, delim),10,&val);
		//printk(KERN_ALERT "ip3:%d\n", val);
                if (i!=0)
                {       printk(KERN_ALERT "kstrtol failed.\n");
                        return;
                }
                ((unsigned char *)&(iph->daddr))[3]=val;
	//	 printk(KERN_ALERT "New IP is:%d.%d.%d.%d\n", ((unsigned char *)&(iph->daddr))[0],((unsigned char *)&(iph->daddr))[1],((unsigned char *)&(iph->daddr))[2],((unsigned char *)&(iph->daddr))[3]);
               
		//	((unsigned char *)&(iph->daddr))[0]=10;
		//	((unsigned char *)&(iph->daddr))[1]=129;
		//	((unsigned char *)&(iph->daddr))[2]=26;
		//	((unsigned char *)&(iph->daddr))[3]=116;

			 mutex_unlock(&entry->node);
		
	    	}	
		else
		{
     			mutex_unlock(&entry->node);
				//  To match ip header fields
		}
		}
        if(m1==0)
            mutex_unlock(&entry->node);
	else
	    break;
	}	

	//printk(KERN_ALERT "IP header details==> SrcIP:%pI4, DstIP:%pI4, TOS:%c, PROTOCOL:%c, TTL:%c\n", &(iph->saddr), &(iph->daddr), iph->tos, iph->protocol, iph->ttl);
	//printk(KERN_ALERT "MAC header details==> SrcMAC:%pM, DstMAC:%pM, PROTOCOL_TYPE:%d\n", &(mh->h_source), &(mh->h_dest), mh->h_proto);
        //printk(KERN_ALERT "TRANSPORT header details==> Srcport:%d, Dstport:%d, Sequence no.:%2d, Window:%d\n", tcph->source, tcph->dest, tcph->seq, tcph->window);

	return;
}
/*static int do_bfilter(struct sk_buff *skb)
{
    struct iphdr* iph = ip_hdr(skb);
    struct ethhdr* mh = eth_hdr(skb);
    struct tcphdr* tcph = tcp_hdr(skb);

    char *tok = "52:54:00:12:34:56";
    unsigned char macaddr[18];
    unsigned char t[18];
    snprintf(t,18,"%pM",&(mh->h_dest));
    strcpy(macaddr,tok);

    printk(KERN_ALERT "macaddr:%s\n",macaddr);
    printk(KERN_ALERT "t:%s, strcmp:%d\n",t,strcmp(macaddr,t));
    //printk(KERN_ALERT "strcmp:%d\n",strcmp(macaddr,mh->h_dest));
    if(strcmp(macaddr,t)==0)
        return 1;

    return 0;
} */   


static int filter_init(void)
{
    //To initialize the mutex for the linked list
	//mutex_init(&mbllmutex);

	//To initialize the mbState linked list;
	INIT_LIST_HEAD(&mbState_list1.list);
	printk("Linked list initialized.\n");
    bridge_lb = do_blb;
    return 0;
}

static void filter_exit(void)
{
        bridge_lb = NULL;
}


/* utility functions */
int intToBinary(int num)
{
    int remainder, base = 1, binary = 0;
    
    while (num > 0)
    {
        remainder = num % 2;
        binary = binary + remainder * base;
        num = num / 2;
        base = base * 10;
    }
    return binary;
}

void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
    char filters[5];
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        filters[len - (i + 1)] = rem + '0';
    }
    filters[len] = '\0';

    if(strlen(filters)<4)
    {
        int t2=4-strlen(filters);
        int t3,t4=0;
        for(t3=0;t3<t2;t3++)
            str[t3]='0';
        for(;t3<4;t3++)
        {    
            str[t3]=filters[t4];
            t4++;
        }    
        str[t3]='\0';
    }  
    else
        strcpy(str,filters);  
}


/*void tostring(char str[], int num)
{
    int i, rem, len = 0, n;

    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}*/

module_init(filter_init);
module_exit(filter_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MIHIR");
MODULE_DESCRIPTION("Filter testing.");


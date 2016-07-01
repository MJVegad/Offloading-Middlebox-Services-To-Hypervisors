#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kfifo.h>
#include <linux/mutex.h>

struct Mbinfo *mymb;
EXPORT_SYMBOL(mymb);

struct kfifo commandQ;

// head node of middlebox linked list
struct mbState mbState_list;
EXPORT_SYMBOL(mbState_list);

// mutex for middlebox state linked list access 
//struct mutex mbllmutex;
//EXPORT_SYMBOL(mbllmutex);

asmlinkage long sys_catsh(const char* newdata)
{

	//char buf[6];
	//unsigned char i;

/*	 struct mbState *temp;
	 struct list_head *ptr;
         struct mbState *entry;
*/
	/* Tokenizing the command to store update the linkedlist */
        char *strp = newdata;
        //char *strp = str;
        const char *delim = " ";
        char *token;	

	token = strsep(&strp, delim);
       
       // printk(KERN_ALERT "token1 = '%s', ", token);

	if (strcmp(token,"R")==0)
	{
		struct mbState *temp;
        	temp = kmalloc(sizeof(*temp), GFP_KERNEL);
        	strcpy(temp->macaddr,strsep(&strp, delim));
        	//printk(KERN_ALERT "token1 = '%s', ", token);
        	strcpy(temp->ipaddr,strsep(&strp, delim));
        	//printk(KERN_ALERT "token2 = '%s', ", token);
		temp->ipfilters=0;
		temp->macfilters=0;
		temp->tcpfilters=0;

		mutex_init(&temp->node);

		//mutex_lock(&mbllmutex);
		list_add(&temp->list,&mbState_list.list);
		//mutex_unlock(&mbllmutex);
		 printk(KERN_ALERT "New MB Registered.\nNode:%s,%s || ipfilters:%ld || macfilters:%ld || tcpfilters:%ld\n", temp->macaddr, temp->ipaddr, temp->ipfilters, temp->macfilters, temp->tcpfilters);

	}
	else if(strcmp(token,"A")==0)
	{
		int flag=0;
		struct list_head *ptr;
	    struct mbState *entry;

		char *temp = strsep(&strp, delim);

		list_for_each(ptr,&mbState_list.list) {
                	entry = list_entry(ptr, struct mbState, list);
                	//printk(KERN_ALERT "command:%s\n", entry->macaddr);
            int m1=0;    	
            mutex_lock(&entry->node);    	
			if(strcmp(entry->macaddr,temp)==0)
			{
				m1=1;
				mutex_unlock(&entry->node);
				char *temp1=strsep(&strp, delim);
				
				if(strcmp(temp1,"I")==0)
				{
					long val=0;
					int i = kstrtol(strsep(&strp, delim),10,&val);
					if (i!=0)
					{	printk(KERN_ALERT "kstrtol failed.\n");
						return 0;
					}

					mutex_lock(&entry->node);
					entry->ipfilters = (entry->ipfilters) | val;
					mutex_unlock(&entry->node);

					while(strp!=NULL)
					{
						char *temp2=strsep(&strp, delim);
						if(strcmp(temp2,"s")==0)
						{	
							mutex_lock(&entry->node);			
							strcpy(entry->srcip,strsep(&strp, delim));
							mutex_unlock(&entry->node);
						}	
						else if(strcmp(temp2,"t")==0)
						{
							long val1=0;
        		            int i1 = kstrtol(strsep(&strp, delim),10,&val1);
							if (i1!=0)
                		    {       printk(KERN_ALERT "kstrtol failed.\n");
		                            return 0;
                            }
                            mutex_lock(&entry->node);
							entry->tos = val1;
							mutex_unlock(&entry->node);
						}
						else if(strcmp(temp2,"p")==0)
                        {
                            long val1=0;
                            int i1 = kstrtol(strsep(&strp, delim),10,&val1);
                            if (i1!=0)
                            {       printk(KERN_ALERT "kstrtol failed.\n");
                                    return 0;
                            }
                            mutex_lock(&entry->node);
                            entry->protocol = val1;
                            mutex_unlock(&entry->node);
                        }
                        else if(strcmp(temp2,"d")==0)
                        {
                        	//Nothing to write here.
                        }	
						else
							printk("Invalid command.\n");
					}

					
				} 
				else if(strcmp(temp1,"M")==0)
				{
					long val=0;
                    int i = kstrtol(strsep(&strp, delim),10,&val);
                    if (i!=0)
                    {   printk(KERN_ALERT "kstrtol failed.\n");
                        return 0;
                    }
                    mutex_lock(&entry->node);
                    entry->macfilters = (entry->macfilters) | val;
					mutex_unlock(&entry->node);

					while(strp!=NULL)
                    {
                        char *temp2=strsep(&strp, delim);
                        if(strcmp(temp2,"s")==0)
                        {
                        	mutex_lock(&entry->node);
                            strcpy(entry->srcmac,strsep(&strp, delim));
                            mutex_unlock(&entry->node);
                        }   
                        else if(strcmp(temp2,"p")==0)
                        {
                            long val1=0;
                            int i1 = kstrtol(strsep(&strp, delim),10,&val1);
                            if (i1!=0)
                            {       printk(KERN_ALERT "kstrtol failed.\n");
                                    return 0;
                            }
                            mutex_lock(&entry->node);
                            entry->ethprotocol = val1;
                            mutex_unlock(&entry->node);
                        }
                        else if(strcmp(temp2,"d")==0)
                        {
                        	//Nothing to write here.
                        }
                        else
                        printk("Invalid command.\n");
                    }

				}
				else if(strcmp(temp1,"T")==0)
				{
					long val=0;
                    int i = kstrtol(strsep(&strp, delim),10,&val);
                    if (i!=0)
                    {       printk(KERN_ALERT "kstrtol failed.\n");
                            return 0;
                    }
                    mutex_lock(&entry->node);
                    entry->tcpfilters = (entry->tcpfilters) | val;
					mutex_unlock(&entry->node);

					while(strp!=NULL)
                    {
                        char *temp2=strsep(&strp, delim);
                        if(strcmp(temp2,"s")==0)
                        {
                            long val1=0;
                            int i1 = kstrtol(strsep(&strp, delim),10,&val1);
                            if (i1!=0)
                            {       printk(KERN_ALERT "kstrtol failed.\n");
                                    return 0;
                            }
                            mutex_lock(&entry->node);
                            entry->srcport = val1;
                            mutex_unlock(&entry->node);
                        }
                        else if(strcmp(temp2,"d")==0)
                        {
                            long val1=0;
                            int i1 = kstrtol(strsep(&strp, delim),10,&val1);
                            if (i1!=0)
                            {   printk(KERN_ALERT "kstrtol failed.\n");
                                return 0;
                            }
                            mutex_lock(&entry->node);
                            entry->dstport = val1;
                            mutex_unlock(&entry->node);
                        }
                        else
                            printk("Invalid command.\n");
                    }

				}
				else
				{
					printk(KERN_ALERT "Invalid command.\n");
				}
				flag=1;
				 printk(KERN_ALERT "New filters added for the MB:%s,%s || ipfilters:%ld || macfilters:%ld || tcpfilters:%ld\n", entry->macaddr, entry->ipaddr, entry->ipfilters, entry->macfilters, entry->tcpfilters);	
				break;
			}
			if(m1==0)
				mutex_unlock(&entry->node);
        	}
        	if(flag==0)
        	printk(KERN_ALERT "Middlebox not registered for the service.\n");


	}
	else if(strcmp(token,"D")==0)
	{
		int flag=0;
		struct list_head *ptr;
	    struct mbState *entry;

		char *temp = strsep(&strp, delim);

		list_for_each(ptr,&mbState_list.list) {
            entry = list_entry(ptr, struct mbState, list);
            //printk(KERN_ALERT "command:%s\n", entry->macaddr);
            int m1=0;
            mutex_lock(&entry->node);
			if(strcmp(entry->macaddr,temp)==0)
			{
				m1=1;
				mutex_unlock(&entry->node);
				char *temp1=strsep(&strp, delim);
				
				if(strcmp(temp1,"I")==0)
				{
					long val=0;
					int i = kstrtol(strsep(&strp, delim),10,&val);
					if (i!=0)
					{	printk(KERN_ALERT "kstrtol failed.\n");
						return 0;
					}
					mutex_lock(&entry->node);
					entry->ipfilters = (entry->ipfilters) & val;
					mutex_unlock(&entry->node);

					while(strp!=NULL)
					{
						char *temp2=strsep(&strp, delim);
						if(strcmp(temp2,"s")==0)
						{
							mutex_lock(&entry->node);	
							strcpy(entry->srcip,"");
							mutex_unlock(&entry->node);
						}	
						else if(strcmp(temp2,"t")==0)
						{
							mutex_lock(&entry->node);
							entry->tos = 0;
							mutex_unlock(&entry->node);
						}
						else if(strcmp(temp2,"p")==0)
                        {
                        	mutex_lock(&entry->node);
                            entry->protocol = 0;
                            mutex_unlock(&entry->node);
                        }
                        else if(strcmp(temp2,"d")==0)
                        {
                        	//Nothing to write here.
                        }
						else
							printk("Invalid command.\n");
					}
				} 
				else if(strcmp(temp1,"M")==0)
				{
					long val=0;
                    int i = kstrtol(strsep(&strp, delim),10,&val);
                    if (i!=0)
                    {   printk(KERN_ALERT "kstrtol failed.\n");
                        return 0;
                    }
                    mutex_lock(&entry->node);
                    entry->macfilters = (entry->macfilters) & val;
					mutex_unlock(&entry->node);

					while(strp!=NULL)
                    {
                        char *temp2=strsep(&strp, delim);
                        if(strcmp(temp2,"s")==0)
                        {
                        	mutex_lock(&entry->node);
                        	strcpy(entry->srcmac,"");
                        	mutex_unlock(&entry->node);
                        }	
                        else if(strcmp(temp2,"p")==0)
                        {   
                        	mutex_lock(&entry->node);                             
                            entry->ethprotocol = 0;
                            mutex_unlock(&entry->node);
                        }
                        else if(strcmp(temp2,"d")==0)
                        {
                        	//Nothing to write here.
                        }
                        else
                        printk("Invalid command.\n");
                    }

				}
				else if(strcmp(temp1,"T")==0)
				{
					long val=0;
                    int i = kstrtol(strsep(&strp, delim),10,&val);
                    if (i!=0)
                    {       printk(KERN_ALERT "kstrtol failed.\n");
                            return 0;
                    }
                    mutex_lock(&entry->node);
                    entry->tcpfilters = (entry->tcpfilters) & val;
                    mutex_unlock(&entry->node);
					
					while(strp!=NULL)
                    {
                        char *temp2=strsep(&strp, delim);
                        if(strcmp(temp2,"s")==0)
                        {
                        	mutex_lock(&entry->node);
                            entry->srcport = 0;
                            mutex_unlock(&entry->node);
                        }
                        else if(strcmp(temp2,"d")==0)
                        {
                        	mutex_lock(&entry->node);
                            entry->dstport = 0;
                            mutex_unlock(&entry->node);
                        }
                        else
                            printk("Invalid command.\n");
                    }

				}
				else
				{
					printk(KERN_ALERT "Invalid command.\n");
				}
				flag=1;
				printk(KERN_ALERT "Filters removed for the MB:%s,%s || ipfilters:%ld || macfilters:%ld || tcpfilters:%ld\n", entry->macaddr, entry->ipaddr, entry->ipfilters, entry->macfilters, entry->tcpfilters);	
				break;
			}
			if (m1==0)
				mutex_unlock(&entry->node);
        	}
			if(flag==0)
        	printk(KERN_ALERT "Middlebox not registered for the service.\n");        	

	}
	else if(strcmp(token,"X")==0)
	{
		struct list_head *ptr;
	    struct mbState *entry;
	    int flag=0;
		char *temp = strsep(&strp, delim);

		list_for_each(ptr,&mbState_list.list) {
            entry = list_entry(ptr, struct mbState, list);
            int m1=0;
            mutex_lock(&entry->node);
            if(strcmp(entry->macaddr,temp)==0)
			{
				m1=1;
				printk(KERN_ALERT "Registration canceled for the MB:%s\n", entry->macaddr);
            	list_del(&entry->list);
            	mutex_unlock(&entry->node);
            	flag=1;
            	break;
            }
            if (m1==0)
            	mutex_unlock(&entry->node);		
        }  

        if(flag==0)
        	printk(KERN_ALERT "Middlebox not registered for the service.\n");  	
	}
	else
	{
		printk(KERN_ALERT "Invalid command.\n");
	}


	/* Test case: Register operation */
/*	struct list_head *ptr6;
    struct mbState *entry6;
	printk(KERN_ALERT "Middleboxes registered.\n==============================\n");
	 list_for_each(ptr6,&mbState_list.list) {
                entry6 = list_entry(ptr6, struct mbState, list);
                printk(KERN_ALERT "Node:%s,%s || ipfilters:%ld || macfilters:%ld || tcpfilters:%ld\n", entry6->macaddr, entry6->ipaddr, entry6->ipfilters, entry6->macfilters, entry6->tcpfilters);
	}
*/

	return 0;
}

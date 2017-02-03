#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define NUM_PAGES_MEM 4	//number of pages
#define PAGE_SIZE_MEM 4096 //in bytes


static int mdn;
static char *name = "my_c_ramdisk_driver";
char *mem_start;
static int bufferSize;

static DECLARE_WAIT_QUEUE_HEAD(read_queue);
static DECLARE_WAIT_QUEUE_HEAD(write_queue);

static int readingFlag;
static int writingFlag;

static int ramdisk_open(struct inode *i, struct file *file)
{
	return 0;
}

static int ramdisk_release(struct inode *i, struct file *file)
{
	return 0;
}

static ssize_t ramdisk_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	int i;
	if((*ppos)+ lbuf > bufferSize){
		printk(KERN_INFO "Read request exceeds size of disk.\n");
		return -ENOMEM;
	}
	wait_event_interruptible(read_queue, readingFlag == 0);
	readingFlag = 1;
	for(i=0; i < lbuf; i++){
		put_user(mem_start[*ppos + i], buf + i);
	}
	*ppos += lbuf;
	readingFlag = 0;
	wake_up_interruptible(&read_queue);
	return lbuf;
}

static ssize_t ramdisk_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
	int i;
	if((*ppos)+lbuf > bufferSize){
		printk(KERN_INFO "Write request exceeds size of disk.\n");
		return -ENOMEM;
	}
	wait_event_interruptible(write_queue, writingFlag == 0);
	writingFlag = 1;
	for(i = 0; i < lbuf; i++){
		get_user(mem_start[*ppos + i], buf + i);
	}
	*ppos += lbuf;
	writingFlag = 0;
	wake_up_interruptible(&write_queue);
	return lbuf;
}

static struct file_operations full_ops =
{
	.owner = THIS_MODULE,
	.open = ramdisk_open,
	.release = ramdisk_release,
	.read = ramdisk_read,
	.write = ramdisk_write
};

int __init init_ramdisk(void)
{
	bufferSize = NUM_PAGES_MEM * PAGE_SIZE_MEM;
	printk(KERN_INFO "Registering device.\n");
	mdn = register_chrdev(0, name, &full_ops);
	if (mdn < 0) {
		printk(KERN_ALERT "Failed to register driver.\n");
		return -1;
	}
	printk(KERN_INFO "Allocating memory.\n");
	mem_start = kmalloc(bufferSize, GFP_KERNEL);
	if(mem_start == NULL){
		printk(KERN_ALERT "Failed to reserve memory.\n");
		return -ENOMEM;
	}
	readingFlag = 0;
	writingFlag = 0;
	return 0;
}

void __exit cleanup_ramdisk(void)
{
	printk(KERN_INFO "Freeing memory.\n");
	kfree(mem_start);
	printk(KERN_INFO "Unregistering device.\n");
	unregister_chrdev(mdn, name);
}

module_init(init_ramdisk);
module_exit(cleanup_ramdisk);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RamDisk driver");

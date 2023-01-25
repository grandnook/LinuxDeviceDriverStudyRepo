#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define DRIVER_NAME "MyDevice_NAME"

// デバイスドライバのマイナー番号の開始番号と総デバイス数
static const unsigned int MINOR_BASE = 0;
static const unsigned int MINOR_NUM  = 2;

static unsigned int mydevice_major;

static struct cdev mydevice_cdev;

// open
static int myDevice_open(struct inode *inode, struct file *file){
    printk("mydevice_open\n");
    return 0;
}

// close
static int myDevice_close(struct inode *inode, struct file *file){
    printk("mydevice_close\n");
    return 0;
}

// read
static ssize_t myDevice_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){
    printk("mydevice_read\n");
    buf[0] = 'A';
    return 1;
}

// write
static ssize_t myDevice_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos){
    printk("mydevice_write\n");
    return 1;
}

// 各システムコールに対応するハンドラテーブル
struct file_operations s_mydevice_fops = {
    .open = myDevice_open,
    .release = myDevice_close,
    .read = myDevice_read,
    .write = myDevice_write,
};

static struct class *mydevice_class = NULL;

static int myDevice_init(void){
    printk("myDevice_init\n");
    // register_chrdev(DRIVER_MAJOR, DRIVER_NAME, &s_myDevice_fops);

    int alloc_ret = 0;
    int cdev_err = 0;
    dev_t dev;

    alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DRIVER_NAME);
    if (alloc_ret != 0){
        printk(KERN_ERR "alloc_chrdev_region = %d\n", alloc_ret);
        return -1;
    }

    mydevice_major = MAJOR(dev);
    dev = MKDEV(mydevice_major, MINOR_BASE);

    cdev_init(&mydevice_cdev, &s_mydevice_fops);
    mydevice_cdev.owner = THIS_MODULE;

    cdev_err = cdev_add(&mydevice_cdev, dev, MINOR_NUM);
    if (cdev_err != 0){
        printk(KERN_ERR  "cdev_add = %d\n", cdev_err);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    mydevice_class = class_create(THIS_MODULE, "mydevice");
    if (IS_ERR(mydevice_class)) {
        printk(KERN_ERR  "class_create\n");
        cdev_del(&mydevice_cdev);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    for (int minor = MINOR_BASE; minor < MINOR_BASE + MINOR_NUM; minor++) {
        device_create(mydevice_class, NULL, MKDEV(mydevice_major, minor), NULL, "mydevice%d", minor);
    }

    return 0;
}

static void myDevice_exit(void){
    printk("myDevice_exit\n");

    dev_t dev = MKDEV(mydevice_major, MINOR_BASE);

    for (int minor = MINOR_BASE; minor < MINOR_BASE + MINOR_NUM; minor++) {
        device_destroy(mydevice_class, MKDEV(mydevice_major, minor));
    }

    class_destroy(mydevice_class);

    cdev_del(&mydevice_cdev);

    unregister_chrdev_region(dev, MINOR_NUM);
}

module_init(myDevice_init);
module_exit(myDevice_exit);

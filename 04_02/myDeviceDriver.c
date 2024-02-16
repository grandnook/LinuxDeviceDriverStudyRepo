#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/current.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
#define DRIVER_NAME "MyDevice"               // /proc/devicesなどで表示されるデバイス名(キャラクタデバイス名)
static const unsigned int MINOR_BASE = 0;    // マイナー番号の開始番号と個数
static const unsigned int MINOR_NUM  = 2;    // 今回は 0 あるいは 1 になる
static unsigned int mydevice_major;          // 動的に決めたメジャー番号を保持する変数
static struct cdev mydevice_cdev;            // キャラクタデバイスのオブジェクト
static struct class *mydevice_class = NULL;  // デバイスドライバのクラスオブジェクト

#define NUM_BUFFER 256
struct _mydevice_file_data {
    unsigned char buffer[NUM_BUFFER];
};

static int myDevice_open(struct inode *inode, struct file *file)
{
    printk("myDevice_open\n");

    struct _mydevice_file_data *p = kmalloc(sizeof(struct _mydevice_file_data), GFP_KERNEL);
    if (p == NULL) {
        printk(KERN_ERR "kmalloc\n");
        return -ENOMEM;
    }

    strlcat(p->buffer, "dummy", 5);

    file->private_data = p;

    return 0;
}

static int myDevice_close(struct inode *inode, struct file *file)
{
    printk("myDevice_close\n");
    
    if (file->private_data) {
        kfree(file->private_data);
        file->private_data = NULL;
    }

    return 0;
}

static ssize_t myDevice_read(struct file *filp, char __user *buf, size_t count, loff_t *f_ops)
{
    printk("myDevice_read\n");

    if (count > NUM_BUFFER) {
        count = NUM_BUFFER;
    }

    struct _mydevice_file_data *p = filp->private_data;
    if (copy_to_user(buf, p->buffer, count) != 0) {
        return -EFAULT;
    }

    return count;
}

static ssize_t myDevice_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_ops)
{
    printk("myDevice_write\n");

    struct _mydevice_file_data *p = filp->private_data;
    if (copy_from_user(p->buffer, buf, count) != 0) {
        return -EFAULT;
    }

    return count;
}

// 各種システムコールに対応するハンドラテーブル
struct file_operations s_myDevice_fops = {
    .open    = myDevice_open,
    .release = myDevice_close,
    .read    = myDevice_read,
    .write   = myDevice_write,
};

static int myDevice_init(void)
{
    printk("myDevice_init\n");

    int alloc_ret = 0;
    int cdev_err = 0;
    dev_t dev;  // デバイス番号 = メジャー番号 + マイナー番号

    // 1. 空いているメジャー番号を動的に確保したdev(デバイス番号)を取得
    alloc_ret = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUM, DRIVER_NAME);
    if (alloc_ret != 0) {
        printk(KERN_ERR "alloc_chrdev_region = %d\n", alloc_ret);
        return -1;
    }

    // 2. 取得したdev(デバイス番号)からメジャー番号を取得しておく
    mydevice_major = MAJOR(dev);
    // dev = MKDEV(mydevice_major, MINOR_BASE);  // 不要？


    // 3. cdev構造体の初期化とシステムコールハンドラテーブルの登録
    cdev_init(&mydevice_cdev, &s_myDevice_fops);
    mydevice_cdev.owner = THIS_MODULE;

    // 4. このデバイスドライバ(cdev)をカーネルに登録
    cdev_err = cdev_add(&mydevice_cdev, dev, MINOR_NUM);
    if (cdev_err != 0) {
        printk(KERN_ERR "cdev_add = %d\n", cdev_err);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    // 5. このデバイスのクラス登録をする(/sys/class/mydevice/を作成する)
    mydevice_class = class_create(THIS_MODULE, "mydevice");
    if (IS_ERR(mydevice_class)) {
        printk(KERN_ERR "class_create\n");
        cdev_del(&mydevice_cdev);
        unregister_chrdev_region(dev, MINOR_NUM);
        return -1;
    }

    // 6. /sys/class/mydevice/mydevice*を作成する
    for (int minor = MINOR_BASE; minor < MINOR_BASE + MINOR_NUM; minor++) {
        device_create(mydevice_class, NULL, MKDEV(mydevice_major, minor), NULL, "mydevice%d", minor);
    }

    return 0;
}

static void myDevice_exit(void)
{
    printk("myDevice_exit\n");

    dev_t dev = MKDEV(mydevice_major, MINOR_BASE);

    // 7. /sys/class/mydevice/mydevice*を削除する
    for (int minor = MINOR_BASE; minor < MINOR_BASE + MINOR_NUM; minor++) {
        device_destroy(mydevice_class, MKDEV(mydevice_major, minor));
    }

    // 8. このデバイスドライバのクラス登録を取り除く(/sys/class/mydevice/を削除する)
    class_destroy(mydevice_class);

    // 9. このデバイスドライバ(cdev)をカーネルから取り除く
    cdev_del(&mydevice_cdev);

    // 10. このデバイスドライバで使用していたメジャー番号の登録を取り除く
    unregister_chrdev_region(dev, MINOR_NUM);
}

module_init(myDevice_init);
module_exit(myDevice_exit);

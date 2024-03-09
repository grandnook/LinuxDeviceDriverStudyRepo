#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>

MODULE_LICENSE("Dual BSD/GPL");
#define DRIVER_NAME "MyDevice"      // /proc/devicesなどで表示されるデバイス名(キャラクタデバイス名)


// デバッグ用変数
struct dentry *debug_entry_dir;  // debugfsディレクトリのエントリ
static int debug_prm1;           // デバッグ用パラメータ(テスト用)
static int debug_read_size = 0;  // 1回のopenでreadするバイト数


static int mydevice_debug_open(struct inode *inode, struct file *file)
{
	printk("mydevice_debug_open\n");
    debug_read_size = 4;  // 1回につき 4 Byte 分だけreadする
	return 0;
}

// /sys/kernel/debug/MyDevice/debug_prm1のread時に呼ばれる関数
static ssize_t mydevice_debug_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	printk("mydevice_debug_read\n");
	
    if (debug_read_size > 0) {
        // まだ出力すべきデータがあるとき，保持している整数型の数字(debug_prm1)を文字列で出力する
        int len;
        len = sprintf(buf, "%d\n", debug_prm1);
        debug_read_size -= 4;
        return len;
    } else {
        return 0;
    }
}

// /sys/kernel/debug/MyDevice/debug_prm1のwrite時に呼ばれる関数
static ssize_t mydevice_debug_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("mydevice_debug_write\n");

    // 入力された文字列を整数型の数字(debug_prm1)として保持する
    sscanf(buf, "%d", &debug_prm1);  // 本当はcopy_from_userすべき
	return count;
}

static struct file_operations debug_debug_prm1_fops = {
	.owner = THIS_MODULE,
	.open  = mydevice_debug_open,
	.read  = mydevice_debug_read,
	.write = mydevice_debug_write,
};

static int mydevice_init(void)
{
    printk("mydevice_init\n");

    debug_entry_dir = debugfs_create_dir(DRIVER_NAME, NULL);
    if (debug_entry_dir == NULL) {
		printk(KERN_ERR "debugfs_create_dir\n");
		return -ENOMEM;
    }

    // 方法1: ハンドラテーブル登録方式
    debugfs_create_file("prm1", S_IRUGO | S_IWUGO, debug_entry_dir, NULL, &debug_debug_prm1_fops);
    
    // 方法2: ヘルパー関数方式
    debugfs_create_u32("_prm1", S_IRUGO | S_IWUGO, debug_entry_dir, &debug_prm1);
	debugfs_create_x32("_prm1_hex", S_IRUGO | S_IWUGO, debug_entry_dir, &debug_prm1);

    return 0;
}

static void mydevice_exit(void)
{
    printk("mydevice_exit\n");

    debugfs_remove_recursive(debug_entry_dir);
}


module_init(mydevice_init);
module_exit(mydevice_exit);

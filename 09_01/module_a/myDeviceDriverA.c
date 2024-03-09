#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
#define DRIVER_NAME "MyDeviceA"      // /proc/devicesなどで表示されるデバイス名(キャラクタデバイス名)

void mydevicea_func(void)
{
	printk("This is a message in mydevicea_func\n");
}
// カーネルのシンボルテーブルに登録する．他のカーネルモジュールから呼べるようにする
EXPORT_SYMBOL(mydevicea_func);


static int mydevicea_init(void)
{
	printk("[A]: mydevicea_init\n");
	mydevicea_func();

	return 0;
}

static void mydevicea_exit(void)
{
	printk("[A]: mydevicea_exit\n");
}

module_init(mydevicea_init);
module_exit(mydevicea_exit);

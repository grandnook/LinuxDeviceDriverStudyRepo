#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
#define DRIVER_NAME "MyDeviceB"      // /proc/devicesなどで表示されるデバイス名(キャラクタデバイス名)

static int mydeviceb_init(void)
{
	printk("[B]: mydeviceb_init\n");

	extern void mydevicea_func(void);
	mydevicea_func();

	return 0;
}

static void mydeviceb_exit(void)
{
	printk("[B]: mydeviceb_exit\n");
}

module_init(mydeviceb_init);
module_exit(mydeviceb_exit);

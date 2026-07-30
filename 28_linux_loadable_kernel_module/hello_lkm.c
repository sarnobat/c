#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sarnobat");
MODULE_DESCRIPTION("Hello world Linux loadable kernel module");

static int __init hello_lkm_init(void)
{
	pr_info("hello_lkm: hello from a loadable kernel module\n");
	return 0;
}

static void __exit hello_lkm_exit(void)
{
	pr_info("hello_lkm: goodbye from a loadable kernel module\n");
}

module_init(hello_lkm_init);
module_exit(hello_lkm_exit);

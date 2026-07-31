#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sarnobat");
MODULE_DESCRIPTION("Log reads and writes through a procfs file");

static struct proc_dir_entry *sridhar_proc_entry;
static char last_write[128] = "(nothing written yet)\n";

static ssize_t sridhar_proc_read(struct file *file, char __user *user_buffer, size_t count, loff_t *position)
{
	pr_info("SRIDHAR proc_read_write_lkm: read called\n");
	return simple_read_from_buffer(user_buffer, count, position, last_write, strlen(last_write));
}

static ssize_t sridhar_proc_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *position)
{
	size_t bytes_to_copy = min_t(size_t, count, sizeof(last_write) - 1);

	if (copy_from_user(last_write, user_buffer, bytes_to_copy))
		return -EFAULT;

	last_write[bytes_to_copy] = '\0';
	pr_info("SRIDHAR proc_read_write_lkm: write called bytes=%zu text=%s", bytes_to_copy, last_write);

	return count;
}

static const struct proc_ops sridhar_proc_ops = {
	.proc_read = sridhar_proc_read,
	.proc_write = sridhar_proc_write,
};

static int __init proc_read_write_lkm_init(void)
{
	sridhar_proc_entry = proc_create("sridhar_lkm", 0666, NULL, &sridhar_proc_ops);
	if (!sridhar_proc_entry)
		return -ENOMEM;

	pr_info("SRIDHAR proc_read_write_lkm: created /proc/sridhar_lkm\n");
	return 0;
}

static void __exit proc_read_write_lkm_exit(void)
{
	proc_remove(sridhar_proc_entry);
	pr_info("SRIDHAR proc_read_write_lkm: removed /proc/sridhar_lkm\n");
}

module_init(proc_read_write_lkm_init);
module_exit(proc_read_write_lkm_exit);

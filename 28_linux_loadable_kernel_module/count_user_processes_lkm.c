#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sarnobat");
MODULE_DESCRIPTION("Count user processes from a Linux loadable kernel module");

static unsigned int count_user_processes(void)
{
	struct task_struct *task;
	unsigned int count = 0;

	rcu_read_lock();
	for_each_process(task) {
		if (!(task->flags & PF_KTHREAD))
			count++;
	}
	rcu_read_unlock();

	return count;
}

static int __init count_user_processes_lkm_init(void)
{
	pr_info("SRIDHAR count_user_processes_lkm: hello from a loadable kernel module\n");
	pr_info("SRIDHAR count_user_processes_lkm: user process count=%u\n", count_user_processes());
	return 0;
}

static void __exit count_user_processes_lkm_exit(void)
{
	pr_info("SRIDHAR count_user_processes_lkm: goodbye from a loadable kernel module\n");
}

module_init(count_user_processes_lkm_init);
module_exit(count_user_processes_lkm_exit);

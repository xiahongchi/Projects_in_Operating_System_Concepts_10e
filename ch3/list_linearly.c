#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>

/* This function is called when the module is loaded. */
static int list_linearly_init(void)
{
    struct task_struct *tsk;
    char buffer[80];
    printk(KERN_INFO "Loading list_linearly Module\n");
    for_each_process(tsk)
    {
        sprintf(buffer, "command = [%s] pid = [%d] state = [%lu]", tsk->comm, tsk->pid, tsk->state);
        printk(KERN_INFO "%s\n", buffer);
    }
    return 0;
}

/* This function is called when the module is removed. */
static void list_linearly_exit(void)
{
    printk(KERN_INFO "Removing list_linearly Module\n");
}

/* Macros for registering module entry and exit points. */
module_init(list_linearly_init);
module_exit(list_linearly_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("list_linearly Module");
MODULE_AUTHOR("SGG");
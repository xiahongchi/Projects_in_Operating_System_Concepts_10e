#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/sched/task.h>

static void listdfs(struct task_struct *task);
static int layer = 0;
/* This function is called when the module is loaded. */
static int list_DFS_init(void)
{
    struct task_struct *task;
    struct list_head *list;
    char buffer[80];

    printk(KERN_INFO "Loading list_DFS Module\n");
    task = &init_task;
    sprintf(buffer, "|");
    sprintf(buffer, "%s pid = [%d]\t state = [%lu]\t command = [%s]\t",
            buffer, task->pid, task->state, task->comm);
    printk(KERN_INFO "%s\n", buffer);
    layer++;
    list_for_each(list, &((&(init_task))->children))
    {
        int i;
        task = list_entry(list, struct task_struct, sibling);
        sprintf(buffer, "\0");
        for (i = 0; i < layer; i++)
            sprintf(buffer, "%s\t", buffer);
        sprintf(buffer, "%s|", buffer);
        sprintf(buffer, "%s pid = [%d]\t state = [%lu]\t command = [%s]\t",
                buffer, task->pid, task->state, task->comm);
        printk(KERN_INFO "%s\n", buffer);
        layer++;
        listdfs(task);
        layer--;
    }
    layer--;
    return 0;
}

static void listdfs(struct task_struct *task)
{
    struct list_head *list;
    struct task_struct *t;
    char buffer[80];
    list_for_each(list, &(task->children))
    {
        int i;
        t = list_entry(list, struct task_struct, sibling);
        sprintf(buffer, "\0");
        for (i = 0; i < layer; i++)
            sprintf(buffer, "%s\t", buffer);
        sprintf(buffer, "%s|", buffer);
        sprintf(buffer, "%s pid = [%d]\t state = [%lu]\t command = [%s]\t",
                buffer, t->pid, t->state, t->comm);
        printk(KERN_INFO "%s\n", buffer);
        layer++;
        listdfs(t);
        layer--;
    }
}

/* This function is called when the module is removed. */
static void list_DFS_exit(void)
{
    printk(KERN_INFO "Removing list_DFS Module\n");
}

/* Macros for registering module entry and exit points. */
module_init(list_DFS_init);
module_exit(list_DFS_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("list_DFS Module");
MODULE_AUTHOR("SGG");
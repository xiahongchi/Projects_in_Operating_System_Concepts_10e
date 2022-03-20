#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>

static int start = 25;
module_param(start, int, 0);

struct collatz
{
    int n;
    struct list_head list;
};

static LIST_HEAD(collatz_list);

static int list_collatz_init(void)
{
    int value = start;
    struct collatz *p;
    printk(KERN_INFO "Loading list_collatz Module...\n");
    if (value <= 0)
        return 1;
    while (true)
    {
        p = kmalloc(sizeof(*p), GFP_KERNEL);
        p->n = value;
        INIT_LIST_HEAD(&p->list);
        list_add_tail(&p->list, &collatz_list);
        if (value == 1)
            break;
        value = (value % 2) ? (3 * value + 1) : (value / 2);
    }
    list_for_each_entry(p, &collatz_list, list)
    {
        printk(KERN_INFO "%d ", p->n);
    }
    return 0;
}
static void list_collatz_exit(void)
{
    printk(KERN_INFO "Removing list_collatz Module...\n");
    struct collatz *ptr, *next;
    list_for_each_entry_safe(ptr, next, &collatz_list, list)
    {
        list_del(&ptr->list);
        kfree(ptr);
    }
}
module_init(list_collatz_init);
module_exit(list_collatz_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("list_collatz Module");
MODULE_AUTHOR("SGG");
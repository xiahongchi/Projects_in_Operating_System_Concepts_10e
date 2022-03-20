#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>

struct color
{
    int red;
    int blue;
    int green;
    struct list_head list;
};
static LIST_HEAD(color_list);

static int list_color_init(void)
{
    printk(KERN_INFO "Loading list_color Module...\n");

    struct color *violet;
    violet = kmalloc(sizeof(*violet), GFP_KERNEL);
    violet->red = 138;
    violet->blue = 43;
    violet->green = 226;
    INIT_LIST_HEAD(&violet->list);
    list_add_tail(&violet->list, &color_list);

    struct color *LightGreen;
    LightGreen = kmalloc(sizeof(*LightGreen), GFP_KERNEL);
    LightGreen->red = 144;
    LightGreen->green = 238;
    LightGreen->blue = 144;
    INIT_LIST_HEAD(&LightGreen->list);
    list_add_tail(&LightGreen->list, &color_list);

    struct color *OliveDrab;
    OliveDrab = kmalloc(sizeof(*OliveDrab), GFP_KERNEL);
    OliveDrab->red = 192;
    OliveDrab->green = 255;
    OliveDrab->blue = 62;
    INIT_LIST_HEAD(&OliveDrab->list);
    list_add_tail(&OliveDrab->list, &color_list);

    struct color *LightBlue;
    LightBlue = kmalloc(sizeof(*LightBlue), GFP_KERNEL);
    LightBlue->red = 173;
    LightBlue->green = 216;
    LightBlue->blue = 230;
    INIT_LIST_HEAD(&LightBlue->list);
    list_add_tail(&LightBlue->list, &color_list);

    struct color *ptr;
    list_for_each_entry(ptr, &color_list, list)
    {
        printk(KERN_INFO "color RGB: %d %d %d\n", ptr->red, ptr->green, ptr->blue);
    }
    return 0;
}
static void list_color_exit(void)
{
    printk(KERN_INFO "Removing list_color Module...\n");
    struct color *ptr, *next;
    list_for_each_entry_safe(ptr, next, &color_list, list)
    {
        list_del(&ptr->list);
        kfree(ptr);
    }
}
/* Macros for registering module entry and exit points. */
module_init(list_color_init);
module_exit(list_color_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("list_color Module");
MODULE_AUTHOR("SGG");
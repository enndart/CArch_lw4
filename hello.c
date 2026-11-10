// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/errno.h>

MODULE_AUTHOR("Serhii Stetsyna");
MODULE_DESCRIPTION("Laboratory work: Linux Kernel Module with list & params");
MODULE_LICENSE("GPL");

static uint hello_count = 1;
module_param(hello_count, uint, 0644);
MODULE_PARM_DESC(hello_count, "Number of times to print Hello, world!");

struct event_time_entry {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(event_list_head);

static int __init hello_init(void)
{
	unsigned int i;
	struct event_time_entry *entry;

	if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10)) {
		pr_warn("Warning: hello_count is %u (0 or between 5 and 10)\n",
			hello_count);
	} else if (hello_count > 10) {
		pr_err("Error: hello_count (%u) is greater than 10!\n",
			hello_count);
		return -EINVAL;
	}

	for (i = 0; i < hello_count; i++) {
		entry = kmalloc(sizeof(*entry), GFP_KERNEL);
		if (!entry)
			return -ENOMEM;

		entry->time = ktime_get();
		list_add_tail(&entry->list, &event_list_head);

		pr_info("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct event_time_entry *entry, *tmp;

	list_for_each_entry_safe(entry, tmp, &event_list_head, list) {
		pr_info("Event time: %lld ns\n", ktime_to_ns(entry->time));
		list_del(&entry->list);
		kfree(entry);
	}

	pr_info("Module unloaded successfully.\n");
}

module_init(hello_init);
module_exit(hello_exit);


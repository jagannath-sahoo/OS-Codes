#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/init_task.h>

struct mydrv_struct
{
    struct list_head list;//Linking to the next node
    struct task_struct *task;
    char data[32];//passed via profs layer
};

//Initialize the list_head
static LIST_HEAD(mydrv_list);
//Initialize proc fs entry directory
static struct proc_dir_entry *entry = NULL;
static struct proc_dir_entry *parent = NULL;
/*
 *
 * struct seq_operations {
        void * (*start) (struct seq_file *m, loff_t *pos);
        void (*stop) (struct seq_file *m, void *v);
        void * (*next) (struct seq_file *m, void *v, loff_t *pos);
        int (*show) (struct seq_file *m, void *v);
};
*/
//seq start method
static void* mydrv_seq_start(struct seq_file *seq, loff_t *pos)
{
    struct task_struct *ptr;
    loff_t off = 0;//Initialize offset for circular buffer
    dump_stack();
    //Navigate through the list to reach the position
    for_each_process(ptr) 
    {
        if(*pos == off++){
	    //printk("Check PID: %d\n",ptr->tasks.pid);
            printk(KERN_DEBUG "in start: success %lld\n",*pos);
            return ptr;
        }
    }
    //Reached at the end: NULL
    printk(KERN_DEBUG "in seq_start: Over");
    return NULL;
}

//seq_next
//Advace to next position and return ptr
static void* mydrv_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
    struct list_head *next = ((struct task_struct *)v)->tasks.next;
    ++*pos;//Increment position
    printk("In seq_next: POS: %lld\n",*pos);
    if(next != NULL){
        //Add to the list
        list_entry(next,struct task_struct, tasks);
    }
    else{
        return NULL;
    }
}


//seq_show():
static int mydrv_seq_show(struct seq_file *file, void *v)
{
    const struct task_struct *ptr = v;
    char *info_str = kmalloc(256,GFP_KERNEL);
    sprintf(info_str,"pid: %d, ppid: %d, tgid: %d, comm: %s\n",ptr->pid,ptr->parent->pid,ptr->tgid, ptr->comm);
    seq_printf(file,info_str);
    kfree(info_str);
    return 0;
}

static void mydrv_seq_stop(struct seq_file *file, void *v)
{
    printk("in stop\n");
}

//Register the callbacks of seq_operations 
static struct seq_operations mydrv_seq_ops = {
    .start = mydrv_seq_start,
    .next = mydrv_seq_next,
    .show = mydrv_seq_show,
    .stop = mydrv_seq_stop,
};

static int mydrv_seq_open(struct inode *inode, struct file *file)
{
    printk("mydrv_seq_open");
    dump_stack();
    return seq_open(file,&mydrv_seq_ops);
}

//callbacks for mydrv procfs layer
static struct file_operations mydrv_proc_fops = {
    .owner = THIS_MODULE, //Provide ptr to our module
    .open = mydrv_seq_open,
    .read = seq_read,//Build-in handled
    .llseek = seq_lseek,//Build-in handled
    .release = seq_release,//Build-in handled
};

static int __init mydrv_init(void)
{
    struct proc_dir_entry *parent_entry;
    parent = proc_mkdir("JAGANNATH", NULL);
    entry = proc_create("mydrv",S_IRUSR, parent,&mydrv_proc_fops);
    if(entry == NULL)
    {
        return -EINVAL;
    }
    printk(KERN_DEBUG"INIT FUNCTION");
    return 0;
}

static void __exit mydrv_exit(void)
{
    struct mydrv_struct *prev, *next;
    list_for_each_entry_safe(prev, next, &mydrv_list, list){
        kfree(prev);
    }
    remove_proc_entry("JAGANNATH", NULL);
    remove_proc_entry("mydrv",NULL);
    printk(KERN_DEBUG "mydrv_exit just executed\n");
}

module_init(mydrv_init);
module_exit(mydrv_exit);

MODULE_LICENSE("GPL v2");

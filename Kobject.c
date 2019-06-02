/*
 * Kobject Sample 
 * */

#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

static int aman;
static char abdul;

static char str[32];
static struct kobject *cdac;

static int __init cdac_init(void);
static void __exit cdac_exit(void);

//Attribute specific callback
static ssize_t aman_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", aman);
}

//Attribute specific callback
static ssize_t aman_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
	//int temp = 0;
	sscanf(buf,"%d",&aman);
        return count;
}

//Define the attribute callbacks with the attribute
static struct kobj_attribute aman_attribute=
	__ATTR(aman, 0600, aman_show, aman_store);

//Attribute specific callback
static ssize_t abdul_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        return sprintf(buf, "%d\n", abdul);
}

//Attribute specific callback
static ssize_t abdul_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
        //int temp = 0;
        sscanf(buf,"%c",&abdul);
        return count;
}

//Define the attribute callbacks with the attribute
static struct kobj_attribute abdul_attribute=
        __ATTR(abdul, 0600, abdul_show, abdul_store);


//Attribute specific callback
static ssize_t str_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        return sprintf(buf, "%s\n", str);
}

//Attribute specific callback
static ssize_t str_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
        //int temp = 0;
        sscanf(buf,"%s",str);
        return count;
}

//Define the attribute callbacks with the attribute
static struct kobj_attribute str_attribute =
        __ATTR(str, 0600, str_show, str_store);

/*
 * Create a group of attribute
 * */
static struct attribute *attrs[] = {
	&aman_attribute.attr,
	&abdul_attribute.attr,
	NULL,
};

//Add group attribute
static struct attribute_group attr_group = {
	.attrs = attrs,
};


static int __init cdac_init(void)
{
	/*
	 * p1: Name of Kobject
	 * p2: Parent kobject name
	 * 	Here kernel_kobj is the parent object
	 * */
	cdac = kobject_create_and_add("cdac",kernel_kobj);
	if(!cdac)
	{
		return -ENOMEM;
	}
	/*
	 * p1: Kobject created
	 * p2:attribute to attach with Kernel
	 * */
	int ret =  sysfs_create_file(cdac, &str_attribute.attr);
	if(ret)
	{
		//Delete cdac kobject
		kobject_put(cdac);
	}
	/*
	 * Add group of attributes into profs
	 * */
	ret = sysfs_create_group(cdac, &attr_group);
	if(ret)
	{
		 //Delete cdac kobject
                kobject_put(cdac);
	}
	printk("Module Initialization!\n");
	return ret;
}

static void __exit cdac_exit(void)
{
	printk("__exit cdac kobject");
	kobject_put(cdac);
}

module_init(cdac_init);
module_exit(cdac_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("JAGNNATH");

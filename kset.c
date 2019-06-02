#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/slab.h>


/*
 * Private device structure 
 * */
struct prv_obj{
	struct kobject kobj;
	int aman;
	char abdul;
	char str[32];
};

// private object attributes
struct prv_obj_attribute{
	struct attribute attr;
	ssize_t (*show)(struct prv_obj *prv, struct prv_obj_attribute *attr, char *buf);
	ssize_t (*store)(struct prv_obj *prv, struct prv_obj_attribute *attr,const char *buf, size_t	count);
};

/*
 * Structure:

struct device_attribute {
        struct attribute        attr;
        ssize_t (*show)(struct device *dev, struct device_attribute *attr,
                        char *buf);
        ssize_t (*store)(struct device *dev, struct device_attribute *attr,
                         const char *buf, size_t count);
};

 * */
#define to_prv_obj(x)		container_of(x, struct prv_obj, kobj)
#define to_prv_obj_attribute(x) container_of(x, struct prv_obj_attribute, attr)

//defining attribute specific callbacks
static ssize_t prv_obj_show(struct kobject *prv, struct attribute *attr, char *buf)
{
	struct prv_obj *obj;
	struct prv_obj_attribute *attribute;

	attribute = to_prv_obj_attribute(attr);
	obj = to_prv_obj(prv);

	if(!attribute->show)//check whether IO method present or not
	{
		return -EIO;
	}
	return attribute->show(obj,attribute,buf);
}

static ssize_t prv_obj_store(struct kobject *prv, struct attribute *attr,const char *buf, size_t count)
{
	struct prv_obj *obj;
        struct prv_obj_attribute *attribute;

        attribute = to_prv_obj_attribute(attr);
        obj = to_prv_obj(prv);

       	if(!attribute->store)//check whether IO method present or not
        {
                return -EIO;
        }
        return attribute->store(obj,attribute,buf,count);
}

static const struct sysfs_ops prv_obj_sysfs_ops = {
	.show = prv_obj_show,
	.store = prv_obj_store,
};


//used by kernel to free the memory
static void prv_obj_release(struct kobject *kobj)
{
	struct prv_obj *obj;
	obj = to_prv_obj(kobj);
	kfree(obj);
}


/*
 * The "aman" file where the .aman varivale is read from and write to
 * */
//Define the attribute callbacks 
static ssize_t aman_show(struct prv_obj *prv, struct prv_obj_attribute *attr, char *buf)
{
	dump_stack();
	return sprintf(buf,"%d\n",prv->aman);
}

static ssize_t aman_store(struct prv_obj *prv, struct prv_obj_attribute *attr,const char *buf, size_t count)
{
        dump_stack();
        sscanf(buf,"%du", &prv->aman);
	return count;
}
//attach the callbacks with the specific attribute
static struct prv_obj_attribute aman_attribute = 
		__ATTR(aman, 0600, aman_show, aman_store);

//Define the attribute callbacks 
static ssize_t abdul_show(struct prv_obj *prv, struct prv_obj_attribute *attr, char *buf)
{
        dump_stack();
        return sprintf(buf,"%c",prv->abdul);
}

static ssize_t abdul_store(struct prv_obj *prv, struct prv_obj_attribute *attr,const char *buf, size_t count)
{
        dump_stack();
        sscanf(buf,"%c", &prv->abdul);
	return count;
}
//attach the callbacks with the specific attribute
static struct prv_obj_attribute abdul_attribute = __ATTR(abdul, 0600, abdul_show, abdul_store);

//Define the attribute callbacks
static ssize_t str_show(struct prv_obj *prv, struct prv_obj_attribute *attr, char *buf)
{
        dump_stack();
        return sprintf(buf,"%s",prv->str);
}

static ssize_t str_store(struct prv_obj *prv, struct prv_obj_attribute *attr,const char *buf, size_t count)
{
        dump_stack();
        sscanf(buf,"%s", prv->str);
	return count;
}
//attach the callbacks with the specific attribute
static struct prv_obj_attribute str_attribute = __ATTR(str, 0600, str_show, str_store);


//Create group attributes
static struct attribute *prv_obj_default_atts[] = {
	&aman_attribute.attr,
	&abdul_attribute.attr,
	&str_attribute.attr,
	NULL,//Need to terminate the attributes
};

//Callbacks for each device(under kset) attribute registered with sysfs layer
static struct kobj_type prv_obj_ktype = {
	.sysfs_ops = &prv_obj_sysfs_ops,
	.release = prv_obj_release,
	.default_attrs = prv_obj_default_atts,
};


static struct kset *prv_obj_example;
static struct prv_obj *aman_obj;
static struct prv_obj *abdul_obj;
static struct prv_obj *str_obj;

//Function method for creating device within the kset
static struct prv_obj *create_prv_obj(const char *name)
{
	struct prv_obj *obj;
	int ret;
	/*
	 * kzalloc — allocate memory. The memory is set to zero.
	 * p1: size
	 * p2: from where to allocate
	 * */
	obj = kzalloc(sizeof(*obj),GFP_KERNEL);
	if(!obj)//Error handling
	{
		return NULL;
	}
	//We need to set it before calling kobject core
	obj->kobj.kset = prv_obj_example;
	//Initialize the kobject and add with the kset
	ret = kobject_init_and_add(&obj->kobj, &prv_obj_ktype, NULL, "%s", name);
	if(ret)//error handling
	{
		kfree(obj);
		return NULL;
	}
	return obj;
}

static void destroy_prv_obj(struct prv_obj *prv)
{
	//Delete the kobj
	kobject_put(&prv->kobj);
}

static int __init example_init(void)
{
	//Create Registry with sysfs layer
	//p2: kset_uevent_ops * uevent_ops:Null
	//kernel_kobj: parent kobject
	prv_obj_example = kset_create_and_add("kset-example4",NULL,kernel_kobj);
	if(!prv_obj_example)
	{
		return -ENOMEM;
	}
	//Create Device 
	aman_obj = create_prv_obj("aman");
	if(!aman_obj)
	{
		destroy_prv_obj(aman_obj);
		return -EINVAL;
	}
	 //Create Device
	abdul_obj = create_prv_obj("abdul");
	if(!abdul_obj)
        {
                destroy_prv_obj(abdul_obj);
		return -EINVAL;
        }
	 //Create Device
	str_obj = create_prv_obj("str");
	if(!str_obj)
        {
                destroy_prv_obj(str_obj);
		return -EINVAL;
        }
	return 0;
}

static void __exit example_exit(void)
{
	//Destroy all the devices within the kset
	destroy_prv_obj(aman_obj);
	destroy_prv_obj(abdul_obj);
	destroy_prv_obj(str_obj);
	kset_unregister(prv_obj_example);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Jagannath <jagannath@jnsystem.com>");

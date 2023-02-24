
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "mydev"
#define BUF_LEN 1024

static int major;                   // major number for device driver
static char msg[BUF_LEN];           // message buffer
static char *msg_ptr = msg;         // pointer to message buffer

// called when the device is opened
static int mydev_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);
    return 0;
}

// called when the device is closed
static int mydev_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    return 0;
}

// called when data is read from the device
static ssize_t mydev_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
    int bytes_read = 0;

    // copy data from message buffer to user buffer
    while (length && *msg_ptr) {
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }

    return bytes_read;
}

// called when data is written to the device
static ssize_t mydev_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    int bytes_written = 0;

    // copy data from user buffer to message buffer
    while (length && (msg_ptr - msg < BUF_LEN - 1)) {
        get_user(*(msg_ptr++), buffer++);
        length--;
        bytes_written++;
    }

    // add null terminator to message buffer
    *msg_ptr = '\0';

    // echo back the message to the user
    printk(KERN_INFO "mydev: received %d bytes: %s\n", bytes_written, msg);

    return bytes_written;
}

// file operations struct for the device driver
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = mydev_open,
    .release = mydev_release,
    .read = mydev_read,
    .write = mydev_write,
};

// called when the module is loaded
static int __init mydev_init(void)
{
    printk(KERN_INFO "mydev: initializing\n");

    // register the device driver with the kernel
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "mydev: failed to register device: %d\n", major);
        return major;
    }
    printk(KERN_INFO "mydev: registered device with major number %d\n", major);

    return 0;
}

// called when the module is unloaded
static void __exit mydev_exit(void)
{
    printk(KERN_INFO "mydev: exiting\n");

    // unregister the device driver from the kernel
    unregister_chrdev(major, DEVICE_NAME);
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arsen Serikov");
MODULE_AUTHOR(“Didar Sailauov”);
MODULE_AUTHOR(“Dinmukhamed Meirambek”);
MODULE_AUTHOR(“Dias Amitullayev”);
MODULE_DESCRIPTION("A virtual character device driver made for final exam");


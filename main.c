#include <linux/init.h>           
#include <linux/module.h>         
#include <linux/device.h>         
#include <linux/kernel.h>
#include <linux/fs.h>            
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>          
#define  DEVICE_NAME "main_device"            
 
MODULE_LICENSE("GPL");           
MODULE_AUTHOR("Jack Dickinson");    
         
 
int   majorNumber;     
int majorNum = 473;                
char*   device_buffer;                
short  size_of_message;              
int    numberOpens = 0;              

 
 

 int     my_open(struct inode *, struct file *);
 int     my_close(struct inode *, struct file *);
 ssize_t my_read(struct file *, char *, size_t, loff_t *);
 ssize_t my_write(struct file *, const char *, size_t, loff_t *);
 int main_device_init(void);
 void main_device_exit(void); 
 loff_t my_seek(struct file*, loff_t, int)

 struct file_operations fops =
{
   .open = my_open,
   .read = my_read,
   .write = my_write,
   .release = my_close,
   .llseak = my_seek,
};
 

 int main_device_init(void)
 {
   printk(KERN_INFO "Main device: Initializing the main_device LKM\n");
 
   
   majorNumber = register_chrdev(majorNum, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "Main device failed to register a major number\n");
      return majorNumber;
   }
 
   
   printk(KERN_INFO "Main device: registered correctly with major number %d\n", majorNumber);
 
    device_buffer = kmalloc(1, GFP_KERNEL);
 
 if (!device_buffer)
 {
   majorNumber = -ENOMEM;
   goto fail;
 }
 
 memset(device_buffer, 0, 1);
 
 
    printk("<1>Inserting memory module\n");
    return 0;
   
   
   fail: 
     main_device_exit();
     return majorNumber;
}
 

 void main_device_exit(void)
 {
   unregister_chrdev(majorNum, DEVICE_NAME);
      if(device_buffer)
      {
       kfree(device_buffer);
      }
   printk(KERN_INFO "Main device: Goodbye from the LKM!\n");
}
 

 int my_open(struct inode *inodep, struct file *filep)
 {
   numberOpens++;
   printk(KERN_INFO "Main Device: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}
 

 ssize_t my_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
 {
   int error_count = 0;
   error_count = copy_to_user(buffer, device_buffer, size_of_message);
 
   if (error_count==0){            
      printk(KERN_INFO "Main device: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  
   }
   else {
      printk(KERN_INFO "Main Device: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              
   }
}


 


 ssize_t my_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
 {
   sprintf(device_buffer, "%s(%zu letters)", buffer, len);   
   size_of_message = strlen(device_buffer);                 
   printk(KERN_INFO "Main Device: Received %zu characters from the user\n", len);
   return len;
}
 

 int my_close(struct inode *inodep, struct file *filep)
 {
   printk(KERN_INFO "Main device: Device successfully closed\n");
   return 0;
 }

loff_t my_seek(struct file*filep, loff_t offset, int whence)
{
 
 loff_t newpos = lseek(filep, offset,;
  switch (whence):
 {
  case 0: 
   newpos = lseek(filep, offset, SEEK_SET);
   break;
  
  case 1:
   newpos = lseek(filep, offset, SEEK_CUR);
   break;
  
  case 2:
   newpos = lseek(filep, offset, SEEK_END);
  
  default:
   return -EINVAL;
 }
                       
 if (newpos<0)
  {
           return -EINVAL;
  }
      filep->f_pos = newpos;
      return newpos;
                       
       
}

 

module_init(main_device_init);
module_exit(main_device_exit);

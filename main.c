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
 
#define BUFFER_SIZE 1024


MODULE_LICENSE("GPL");           
MODULE_AUTHOR("Jack Dickinson");    
         
 
int   majorNumber;     
int majorNum = 473;                
char*   device_buffer;                
short  messageSize;              
int    numberOpens = 0;              
char message[BUFFER_SIZE];
 
 

 int     my_open(struct inode *, struct file *);
 int     my_close(struct inode *, struct file *);
 ssize_t my_read(struct file *, char __user *, size_t, loff_t *);
 ssize_t my_write(struct file *, const char __user *, size_t, loff_t *);
 int main_device_init(void);
 void main_device_exit(void); 
 loff_t my_seek(struct file*, loff_t, int);

 struct file_operations fops =
{
   .open = my_open,
   .read = my_read,
   .write = my_write,
   .release = my_close,
   .llseek = my_seek,
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
 
    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
 
 if (!device_buffer)
 {
   majorNumber = -ENOMEM;
   return majorNumber;
 }
 
 
    printk("<1>Inserting memory module\n");
    return 0;
  
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
 

 ssize_t my_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
 {
   int error_count = 0;
   if (len > BUFFER_SIZE)
    {
       printk("%zu", len);
       printk("read overflow");
       return -EFAULT;
    } 
   error_count = copy_to_user(buffer, device_buffer, len);
    
   if (error_count==0){            
      offset = offset + len;
      printk(KERN_INFO "Main device: Sent %d characters to the user\n", len);
      
      return 0;  
   }
   else {
      printk(KERN_INFO "Main Device: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;             
   }
}


 


 ssize_t my_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
 {
    int checker = 0;
     if (len > BUFFER_SIZE)
  {
    printk("Too big to write\n");
    printk("device buffer is: %p", (void *)device_buffer);
    
    printk("Current pos is: %lld", filep->f_pos);
    return -1;
  }
    checker = copy_from_user(device_buffer, buffer, len);
  if (checker == 0)
  {
   offset = offset + len;
   printk(KERN_INFO "Main Device: Received %zu characters from the user\n", len);
   printk("Current pos is: %lld", filep->f_pos);
   printk("Offset is %lld", *offset);
   return len;
  }
  else
   return -EFAULT;
}
 

 int my_close(struct inode *inodep, struct file *filep)
 {
   printk(KERN_INFO "Main device: Device successfully closed\n");
   return 0;
 }

loff_t my_seek(struct file* filep, loff_t offset, int whence)
{
 
 loff_t newpos;
  switch (whence)
 {
  case 0: 
   newpos = offset;
   break;
  
  case 1:
   newpos = filep->f_pos + offset;
   break;
  
  case 2:
   newpos = sizeof(filep) + offset;
  
  default:
   return -EINVAL;
 }
           printk("Offset is %lld", offset);

        
 if (newpos<0)
  {
           return -EINVAL;
  }
      filep->f_pos = newpos;
      printk("Current p is %lld", filep->f_pos);
      return newpos;

}



module_init(main_device_init);
module_exit(main_device_exit);

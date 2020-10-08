#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#define  DEVICE_NAME "main_device"    
#define  CLASS_NAME  "ebb"        
 
MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Jack Dickinson");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for the BBB");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users
 
static int   majorNumber;     
static int majorNum;                ///< Stores the device number -- determined automatically
static char*   device_buffer;                ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  ebbcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ebbcharDevice = NULL; ///< The device-driver device struct pointer
 
// The prototype functions for the character driver -- must come before the struct definition
static int     my_open(struct inode *, struct file *);
static int     my_close(struct inode *, struct file *);
static ssize_t my_read(struct file *, char *, size_t, loff_t *);
static ssize_t my_write(struct file *, const char *, size_t, loff_t *);
 
/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = my_open,
   .read = my_read,
   .write = my_write,
   .release = my_close,
};
 
/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int main_device_init(void){
   printk(KERN_INFO "Main device: Initializing the main_device LKM\n");
 
   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "Main device failed to register a major number\n");
      return majorNumber;
   }
 
   
   printk(KERN_INFO "Main device: registered correctly with major number %d\n", majorNumber);
 
    device_buffer = kmalloc(1, GFP_KERNEL);
 
 if (!device_buffer)
 {
   majorNumber = -ENOMEN;
   goto fail;
 }
 
 memset(device_buffer, 0, 1);
 
 
    printk("<1>Inserting memory module\n");
    return 0;
   
   
   fail: 
     main_device_exit();
     return majorNumber;
}
 
/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void main_device_exit(void){
   unregister_chrdev(majorNum, DEVICE_NAME);
      if(device_buffer)
      {
       kfree(device_buffer);
      }
   printk(KERN_INFO "Main device: Goodbye from the LKM!\n");
}
 
/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int my_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "Main Device: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}
 
/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t my_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   error_count = copy_to_user(buffer, device_buffer, size_of_message);
 
   if (error_count==0){            // if true then have success
      printk(KERN_INFO "Main device: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "Main Device: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}
 
/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t my_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(device_buffer, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(device_buffer);                 // store the length of the stored message
   printk(KERN_INFO "Main Device: Received %zu characters from the user\n", len);
   return len;
}
 
/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int my_close(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Main device: Device successfully closed\n");
   return 0;
}
 
/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(main_device_init);
module_exit(main_device_exit);

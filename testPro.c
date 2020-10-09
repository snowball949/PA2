#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
 
#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
 char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM
 char option;
 int bytes;
 string userS;
 off_t offset;
 int whence;
  
int main(){
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   printf("Starting device test code example...\n");
   fd = open("/dev/main_device", O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
 while (option != 'e')
 {
  printf("Enter 'r' to tell the program to read a certain number of bytes.\n");
  printf("Enter 'w' to enter data you want to write.\n");
  printf("Enter 's' to alter second and thrid parameters.\n);
  printf("Enter 'e' to exit the program.\n);
  option = getchar();
  if (option == 'r')
         {
           printf("Enter the number of bytes you want to read:\n ");
           scanf("%d", bytes);
           ret = read(fd, receive, bytes);
           if (ret < 0)
           {
            printf("Failed to read file");
            return errno;
           }
          printf("You have read [%s]", receive);
         }
   else if (option == 'w')
         {
          printf("Enter the data you want to write:\n ");
          scanf("%s", userS);
          ret = write(fd, userS, strlen(userS));
          if (ret < 0)
          {
           printf("Failed to write to file");
           return errno;
          }
         }
    else if (option == 's')
         {
          printf("Enter an offset: \n");
          scanf("%lld", offset);
          printf("Enter a whence: \n");
          scanf("%d", whence);
          ret = lseek(fd, offset, whence);
          if (ret < 0)
          {
           printf("Failed to seek\n");
           return errno;
          }
         }
         else if (option == 'e')
         {
          printf("You have quit the program");
          return 0;
         }
         else
         {
          printf("Please type in a correct selection.\n");
         }
 }
   
  
}

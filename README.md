# Contact Info

Name: Jack Dickinson

Email: jadi6143@colorado.edu


# Programs
main.c: This program is my device driver implementation.


testPro.c: This program is my user space test program


Makefile: this file provides a macro for the terminal where by just typing in make you can compile both main.c and testPro.c. 

# Steps

1. Unzip my files

2. Make a directory called /home/kernel/modules

3. Then place Makefile, main.c, and testPro.c in this directory. 

4. Run the command "sudo mknod -m 777 /dev/main_device c 473 0

5. run insmod

6. Type ./test to run my user space test program













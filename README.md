# Linux-OS-kernel-module

## REQUIREMENTS


Linux kernel version 5.*

Run the following command to build your module: 
- `sudo su`
- `make -C /lib/modules/$(uname -r)/build M=$(pwd) modules`

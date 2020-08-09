# virtual-file-system
Virtual UNIX/Linux File Subsystem that resides on the RAM
Give it a try - https://repl.it/@vedangwartikar/virtual-file-system

# VFS
__Virtual File System__
- - - -

<div align="center">
    <img src="https://github.com/vedangwartikar/virtual-file-system/blob/master/vfs.png" width="427" height="317"/>
</div>

Virtual File System provides multiple functionalities that are similar to the UNIX/Linux File Subsystem. It provides necessary commands and system call implementations of the file system through customized shell. All necessary data structures of file system like Incore Inode Table, File Table, User File Descriptor Table are implemented. The entire file manipulation occurs on RAM and does not affect any secondary storage device.


Command | Description
------- | ------------------------------------------
ls      | To list out all the files
clear   | To clear the console
create  | Create a new file
open    | Open specific file
close   | Close specific file
closeall| Close all the opened files
read    | To read contents from the file
write   | To write contents into the file
truncate| To remove all the data from the file
rm      | To delete the file
stat    | Display information about the file
fstat   | Display information using the File Descriptor
exit    | To terminate the File System


References:
> The Design of the UNIX Operating System by Maurice J. Bach

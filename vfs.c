#define _CRT_SECURE_NO_WARNINGS
#define MAXINODE 50
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 2
#define START 0
#define CURRENT 1
#define END 2

#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<io.h>

struct superblock
{
    int TotalInodes;
    int FreeInodes;
};

typedef struct superblock SUPERBLOCK;
typedef struct superblock *PSUPERBLOCK;

struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int Permission;
    struct inode * next;
};

typedef struct inode INODE;
typedef struct inode *PINODE;
typedef struct inode **PPINODE;


struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;
    PINODE ptrinode;
};

typedef struct filetable FILETABLE;
typedef struct filetable *PFILETABLE;

struct ufdt
{
    PFILETABLE ptrfiletable;
};

typedef struct ufdt UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

void InitializeSuperBlock()
{
    int i = 0;
    while(i<MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }
    SUPERBLOCKobj.FreeInodes = MAXINODE;
    SUPERBLOCKobj.TotalInodes = MAXINODE;
}

void CreateDILB()
{
    PINODE newn = NULL;
    PINODE temp = head;

    int i = 0;
    while(i < 50)
    {
        newn = (PINODE)malloc(sizeof(INODE));
        newn->LinkCount = 0;
        newn->ReferenceCount = 0;
        newn->FileType = 0;
        newn->FileSize = 0;
        newn->Buffer = NULL;
        newn->next = NULL;
        newn->InodeNumber = i;

        if (temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next = newn;
            temp = temp->next;
        }
        i++;
    }
}

void ls_file()
{
    PINODE temp = head;

    if(SUPERBLOCKobj.FreeInodes == MAXINODE)
    {
        printf("Error: There are no files\n");
        return;
    }

    printf("-----------------------------------------------------------\n");
    printf("File Name\tInode number\tFile Size\tLink Count\n");
    printf("-----------------------------------------------------------\n");

    while(temp != NULL)
    {
        if(temp->FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n", temp->FileName, temp->InodeNumber, temp->FileActualSize, temp->LinkCount);
        }
        temp = temp->next;
    }
    printf("-----------------------------------------------------------\n");
}

void CloseAllFile()
{
    int i = 0;
    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset = 0;
            UFDTArr[i].ptrfiletable->writeoffset = 0;
            (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
        }
        i++;
    }
}

void DisplayHelp()
{
    printf("ls : To list out all the files\n");
    printf("clear : To clear console\n");
    printf("create : Create a new file\n");
    printf("open : Open specific file\n");
    printf("close : Close specific file\n");
    printf("closeall : Close all the opened files\n");
    printf("read : To read contents from the file\n");
    printf("write : To write contents into the file\n");
    printf("exit : To terminate the File System\n");
    printf("stat : Display information about the file\n");
    printf("fstat : Display information using the File Descriptor\n");
    printf("truncate : To remove all the data from the file\n");
    printf("rm : To delete the file\n");
}

int stat_file(char *name)
{
    PINODE temp = head;

    if(name == NULL)
    {
        return -1;
    }

    while(temp != NULL)
    {
        if(_stricmp(name, temp->FileName) == 0)
        {
            break;
        }
        temp = temp->next;
    }

    if(temp == NULL)
    {
        return -2;
    }

    printf("----------------Statistical Information about File----------------\n");
    printf("File Name: %s\n", temp->FileName);
    printf("Inode Number: %d\n", temp->InodeNumber);
    printf("File Size: %d\n", temp->FileSize);
    printf("Actual File Size: %d\n", temp->FileActualSize);
    printf("Link Count: %d\n", temp->LinkCount);
    printf("Reference Count: %d\n", temp->ReferenceCount);

    if(temp->Permission == 1)
    {
        printf("File permission: Read Only\n");
    }
    else if(temp->Permission == 2)
    {
        printf("File permission: Write Only\n");
    }
    else if(temp->Permission == 3)
    {
        printf("File permission: Read and Write\n");
    }
    printf("------------------------------------------------------------------\n");
    return 0;
}

int fstat_file(int fd)
{
    PINODE temp = head;

    if(fd < 0)
    {
        return -1;
    }

    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -2;
    }

    temp = UFDTArr[fd].ptrfiletable->ptrinode;

    printf("----------------Statistical Information about File----------------\n");
    printf("File Name: %s\n", temp->FileName);
    printf("Inode Number: %d\n", temp->InodeNumber);
    printf("File Size: %d\n", temp->FileSize);
    printf("Actual File Size: %d\n", temp->FileActualSize);
    printf("Link Count: %d\n", temp->LinkCount);
    printf("Reference Count: %d\n", temp->ReferenceCount);

    if(temp->Permission == 1)
    {
        printf("File permission: Read Only\n");
    }
    else if(temp->Permission == 2)
    {
        printf("File permission: Write Only\n");
    }
    else if(temp->Permission == 3)
    {
        printf("File permission: Read and Write\n");
    }
    printf("------------------------------------------------------------------\n");

    return 0;
}

int GetFDFromName(char *name)
{
    int i = 0;

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            if(stricmp((UFDTArr[i].ptrfiletable->ptrinode->FileName), name) == 0)
                break;
        }
        i++;
    }

    if(i == 50)
    {
        return -1;
    }
    else
    {
        return i;
    }
}

int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);

    if(i ==-1)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

    return 0;
}

int rm_File(char *name)
{
    int fd = 0;

    fd = GetFDFromName(name);

    if(fd == -1)
    {
        return -1;
    }

    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

    if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
        free(UFDTArr[fd].ptrfiletable);
    }

     UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInodes)++;
}

void man(char *name)
{
    if(name == NULL)
    {
        return;
    }

    if(_stricmp(name, "create") == 0)
    {
        printf("Description: Used to Create new regular file\n");
        printf("Usage: create File_name Permission\n");
    }
    else if(_stricmp(name, "read") == 0)
    {
        printf("Description: Used to read data from a regular file\n");
        printf("Usage: read File_name No_of_bytes_to_read\n");
    }
    else if(_stricmp(name, "write") == 0)
    {
        printf("Description: Used to write data into a regular file\n");
        printf("Usage: write File_name\n");
    }
    else if(_stricmp(name, "ls") == 0)
    {
        printf("Description: Used to list all information about file\n");
        printf("Usage: ls\n");
    }
    else if(_stricmp(name, "stat") == 0)
    {
        printf("Description: Used to display information of a file\n");
        printf("Usage: stat File_name\n");
    }
    else if(_stricmp(name, "fstat") == 0)
    {
        printf("Description: Used to display information of a file\n");
        printf("Usage: stat File_descriptor\n");
    }
    else if(_stricmp(name, "truncate") == 0)
    {
        printf("Description: Used to remove all the data from a file\n");
        printf("Usage: truncate File_name\n");
    }
    else if(_stricmp(name, "open") == 0)
    {
        printf("Description: Used open existing file\n");
        printf("Usage: open File_name mode\n");
    }
    else if(_stricmp(name, "close") == 0)
    {
        printf("Description: Used to close an opened file\n");
        printf("Usage: close File_name\n");
    }
    else if(_stricmp(name, "closeall") == 0)
    {
        printf("Description: Used to close all the files\n");
        printf("Usage: closeall\n");
    }
    else if(_stricmp(name, "lseek") == 0)
    {
        printf("Description: Used to change file offset\n");
        printf("Usage: lseek File_name Change_in_Offset Start_Point\n");
    }
    else if(_stricmp(name, "rm") == 0)
    {
        printf("Description: Used to delete a specific file\n");
        printf("Usage: rm File_name\n");
    }
    else
    {
        printf("ERROR: No manual entry available!\n");
    }

}

int WriteFile(int fd, char *arr, int isize)
{
    if(((UFDTArr[fd].ptrfiletable->mode) != WRITE) && (UFDTArr[fd].ptrfiletable->mode) != READ+WRITE)
    {
        return -1;
    }
    if(((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != READ+WRITE))
    {
        return -1;
    }
    if((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
    {
        return -2;
    }
    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
    {
        return -3;
    }
    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset), arr, isize);

    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + isize;

    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + isize;

    return isize;
}

int truncate_File(char *name)
{
    int fd = GetFDFromName(name);
    if(fd == -1)
    {
        return -1;
    }

    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer, 0, 1024);
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;
}

PINODE Get_Inode(char *name)
{
    PINODE temp = head;

    if(name == NULL)
    {
        return NULL;
    }

    while(temp != NULL)
    {
        if(strcmp(name, temp->FileName) == 0)
        {
            break;
        }
        temp = temp->next;
    }

    return temp;
}

int CreateFile(char *name, int Permission)
{
    int i = 0;
    PINODE temp = head;

    if((name == NULL) || (Permission == 0) || (Permission > 3))
    {
        return -1;
    }
    if(SUPERBLOCKobj.FreeInodes == 0)
    {
        return -2;
    }
    if(Get_Inode(name) != NULL)
    {
        return -3;
    }

    (SUPERBLOCKobj.FreeInodes)--;

    while(temp != NULL)
    {
        if(temp->FileType == 0)
        {
            break;
        }
        temp = temp->next;
    }

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable == NULL)
    {
        return -4;
    }

    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = Permission;
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;

    UFDTArr[i].ptrfiletable->ptrinode = temp;
    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName, name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->Permission = Permission;
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);
    memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer, 0, 1024);

    return i;
}

int OpenFile(char *name, int mode)
{
    int i = 0;

    PINODE temp = NULL;

    if(name == NULL || mode <= 0)
    {
        return -1;
    }

    temp = Get_Inode(name);

    if(temp == NULL)
    {
        return -2;
    }

    if(temp->Permission < mode)
    {
        return -3;
    }

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

    if(UFDTArr[i].ptrfiletable == NULL)
    {
        return -1;
    }

    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = mode;

    if (mode == READ + WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    else if(mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }

    UFDTArr[i].ptrfiletable->ptrinode = temp;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

    return i;
}

int ReadFile(int fd, char *arr, int isize)
{
    int read_size = 0;

    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }
    if((UFDTArr[fd].ptrfiletable->mode != READ) && (UFDTArr[fd].ptrfiletable->mode != READ + WRITE))
    {
           return -2;
    }
    if((UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ) && (UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ + WRITE))
    {
           return -2;
    }
    if(UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
    {
        return -3;
    }
    if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
    {
        return -4;
    }
    read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);

    if(read_size < isize)
    {
        strncpy(arr, (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset), read_size);
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + read_size;
    }
    else
    {
        strncpy(arr, (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset), isize);
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + isize;
    }

    return isize;
}

int main()
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;
    char command[4][80], str[80], arr[1024];

    InitializeSuperBlock();
    CreateDILB();

    while(1)
    {
        fflush(stdin);
        strcpy(str, "");

        printf("Virtual File System: > ");
        fgets(str, 80, stdin);

        count = sscanf(str, "%s%s%s%s", command[0], command[1], command[2], command[3]);

        if(count == 1)
        {
            if(_stricmp(command[0], "ls") == 0)
            {
                ls_file();
            }
            else if(_stricmp(command[0], "closeall") == 0)
            {
                CloseAllFile();
                printf("All files closed successfully\n");
                continue;
            }
            else if(_stricmp(command[0], "clear") == 0)
            {
                system("cls");
                continue;
            }
            else if(_stricmp(command[0], "help") == 0)
            {
                DisplayHelp();
                continue;
            }
            else if(_stricmp(command[0], "exit") == 0)
            {
                printf("Terminating the Virtual File System...\n");
                break;
            }
            else
            {
                printf("ERROR: Command not found!!!\n");
                continue;
            }
        }
        else if(count == 2)
        {
            if(_stricmp(command[0], "stat") == 0)
            {
                ret = stat_file(command[1]);

                if(ret == -1)
                {
                    printf("ERROR: Incorrect parameters!!!\n");
                }
                if(ret == -2)
                {
                    printf("ERROR: There is no such file!!!\n");
                }
                continue;
            }
            else if(_stricmp(command[0], "fstat") == 0)
            {
                ret = fstat_file(atoi(command[1]));

                if(ret == -1)
                {
                    printf("ERROR: Incorrect parameters!!!\n");
                }
                if(ret == -2)
                {
                    printf("ERROR: There is no such file!!!\n");
                }
                continue;
            }
            else if(_stricmp(command[0], "close") == 0)
            {
                ret = CloseFileByName(command[1]);

                if(ret == -1)
                {
                    printf("ERROR: There is no such file!!!\n");
                }
                continue;
            }
            else if(_stricmp(command[0], "rm") == 0)
            {
                char ans;
                printf("Do you want to permanently delete this file?[y/N]");
                scanf("%s", &ans);
                if(ans == 'y' || ans == 'Y')
                {
                    ret = rm_File(command[1]);
                }

                if(ret == -1)
                {
                    printf("ERROR: There is no such file!!!\n");
                }
                continue;
            }
            else if(_stricmp(command[0], "man") == 0)
            {
                man(command[1]);
            }
            else if(_stricmp(command[0], "write") == 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR: Incorrect parameters!!!\n");
                    continue;
                }
                printf("Enter the data: \n");
                scanf("%[^\n]", arr);

                ret = strlen(arr);
                if(ret == 0)
                {
                    printf("ERROR: Incorrect parameters!!!\n");
                    continue;
                }

                ret = WriteFile(fd, arr, ret);

                if(ret == -1)
                {
                    printf("ERROR: Permission denied!!!\n");
                }
                if(ret == -2)
                {
                    printf("ERROR: No sufficient memory to write!!!\n");
                }
                if(ret == -3)
                {
                    printf("ERROR: It is not a regular file!!!\n");
                }
            }
            else if(_stricmp(command[0], "truncate") == 0)
            {
                ret = truncate_File(command[1]);

                if(ret == -1)
                {
                    printf("ERROR: Incorrect parameters!!!\n");
                    continue;
                }
            }
            else
            {
                printf("ERROR: Command not found!!!\n");
                continue;
            }
        }
        else if(count == 3)
        {
            if(_stricmp(command[0], "create") == 0)
            {
                ret = CreateFile(command[1], atoi(command[2]));

                if(ret >= 0)
                {
                    printf("File is successfully created with file descriptor: %d\n", ret);
                }
                if(ret == -1)
                {
                    printf("ERROR: Incorrect parameters!!!\n");
                }
                if(ret == -2)
                {
                    printf("ERROR: There are no inodes!!!\n");
                }
                if(ret == -3)
                {
                    printf("ERROR: File already exists!!!\n");
                }
                if(ret == -4)
                {
                    printf("ERROR: Memory allocation failure!!!\n");
                }
                continue;
            }
            else if(_stricmp(command[0], "open") == 0)
            {
                ret = OpenFile(command[1], atoi(command[2]));

                if(ret >= 0)
                {
                    printf("File is successfully opened with file descriptor: %d\n", ret);
                }
                if(ret == -1)
                {
                    printf("ERROR: Incorrect parameters!!!\n");
                }
                if(ret == -2)
                {
                    printf("ERROR: File not present!!!\n");
                }
                if(ret == -3)
                {
                    printf("ERROR: Permission denied!!!\n");
                }
                continue;
            }
            else if(_stricmp(command[0], "read") == 0)
            {
                fd = GetFDFromName(command[1]);

                if(fd == -1)
                {
                    printf("ERROR: Incorrect Parameters!!!\n");
                    continue;
                }

                ptr = (char *)malloc(atoi(command[2]) + 1);

                if(ptr == NULL)
                {
                    printf("ERROR: Memory allocation failure\n");
                    continue;
                }

                ret = ReadFile(fd, ptr, atoi(command[2]));

                if(ret == -1)
                {
                    printf("ERROR: File does not exist!!!");
                }
                if(ret == -2)
                {
                    printf("ERROR: Permission denied!!!");
                }
                if(ret == -3)
                {
                    printf("ERROR: Reached at the end of file!!!");
                }
                if(ret == -4)
                {
                    printf("ERROR: It is not a regular file!!!\n");
                }
                if(ret == 0)
                {
                    printf("ERROR: File is empty");
                }
                if(ret > 0)
                {
                    _write(2, ptr, ret);
                }
                printf("\n");
                continue;
            }
            else
            {
                printf("ERROR: Command not found!!!\n");
                continue;
            }
        }
    }
    return 0;
}

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
                printf("Terminating the Virtual File System\n");
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

        }
    }
    return 0;
}

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

    printf("\nFile Name\tInode number\tFile Size\tLink Count\n");
    printf("-----------------------------------------------------------");

    while(temp != NULL)
    {
        if(temp->FileType != 0)
        {
            printf("%s\t%d\t%d\t%d\t%d\n", temp->FileName, temp->InodeNumber, temp->FileSize, temp->LinkCount);
        }
        temp = temp->next;
    }
    printf("-----------------------------------------------------------");
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
    printf("----------------------------------------------------------");
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
    printf("----------------------------------------------------------");

    return 0;
}
#ifndef _FILEEXPLORER_H
#define _FILEEXPLORER_H

#include <stdio.h>
#include <sys/unistd.h>
#include <string.h>
#include <stdlib.h>
#include<dirent.h>
#include <sys/stat.h>



#define MAX_PATH_LEN (256)
#define MAX_FILE_NAME (32)

typedef struct
{

}FileInfo_t;

struct xffList;
struct xListNode;

typedef struct xListNode
{
    struct xListNode *    next;
    struct xListNode *    prev;
    struct xffList *      pxContainer;

    struct dirent   dirp;
}ListNode_t;

typedef struct xffList
{
    volatile uint32_t uxNumberOfItems;
    ListNode_t * pxHead;
    ListNode_t * pxTail;
} ffList_t;


typedef struct
{
    const char* base_path;
    char        current_path[MAX_PATH_LEN];
    int         entry_count;
    int         selected_idx;
    char        selected_name[MAX_FILE_NAME];
    int         selected_type;
}FileExplorer_t;

void FileExplorerInit(FileExplorer_t **pFileExplorer);

void FileExplorer_cd(const char* dirName);

void FileExplorer_cat(const char* dirName);

/*void FileExplorer_ls();*/

void FileExplorer_nextEntry();

void FileExplorer_preEntry();

void FileExplorer_getSelectedEntry(char** name,int *pIsDir);

#endif

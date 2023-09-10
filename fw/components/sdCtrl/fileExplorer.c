#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sdCtrl.h"
#include "fileExplorer.h"

static const char *TAG = "fileExplorer";

#define MOUNT_POINT "/sdcard"
#define UP_PATH ".."



typedef struct
{
    const char*     base_path;
    char            current_path[MAX_PATH_LEN];
    ListNode_t *    pSelectEntry;
    FileExplorer_t  fileExplorer;
    ffList_t        entryList;
}ff_t;

ff_t s_ff;




void ffListInsert(ffList_t *pList,struct dirent * pEntry)
{
    ListNode_t *pNode = calloc(1, sizeof(ListNode_t));
    ESP_LOGD(TAG, "insert node %s", pEntry->d_name);

    pList->uxNumberOfItems++;
    pNode->pxContainer = pList;

    pNode->next = NULL;
    pNode->prev = pList->pxTail;
    if (pList->pxHead == NULL)
    {
        pList->pxHead = pNode;
    }
    if (pList->pxTail != NULL)
    {
        pList->pxTail->next = pNode;
    }
    pList->pxTail = pNode;

    memcpy(&(pNode->dirp), pEntry, sizeof(struct dirent));
}

void ffListRemove(ListNode_t *pNode)
{
    ffList_t *pList = pNode->pxContainer;

    pList->uxNumberOfItems--;

    if (pList->pxHead == pNode)
    {
        pList->pxHead = pNode->next;
    }

    if (pList->pxTail == pNode)
    {
        pList->pxTail = pNode->prev;
    }

    if (pNode->prev != NULL)
    {
        pNode->prev->next = pNode->next;
    }

    if (pNode->next != NULL)
    {
        pNode->next->prev = pNode->prev;
    }

    ESP_LOGD(TAG, "remove node %s", pNode->dirp.d_name);
    free(pNode);
}

void ffListClear(ffList_t *pList)
{
    while(pList->pxHead)
    {
        ffListRemove(pList->pxHead);
    };
}

void FileExplorer_updatCurrentPath()
{
    ffList_t *pList = &(s_ff.entryList);
    ffListClear(pList);

    struct dirent uperDirp;
    strncpy(uperDirp.d_name, UP_PATH, sizeof(uperDirp.d_name)/sizeof(uperDirp.d_name[0]));
    uperDirp.d_type = DT_DIR;
    uperDirp.d_ino = 0;
    printf("%s$ ls\n",s_ff.current_path);
    printf("%s$ DIR  - %s\n",s_ff.current_path,UP_PATH);
    ffListInsert(pList,&uperDirp);

    DIR * dp = opendir(s_ff.current_path);

    struct dirent *dirp = NULL;
    if (dp != NULL)
    {
        while((dirp = readdir(dp)) != NULL)
        {
            //ESP_LOGI(s_ff.current_path, "   %s - %s", dirp->d_name,(DT_DIR == dirp->d_type)?"DIR":"FILE");
            printf("%s$ %s - %s\n",s_ff.current_path,(DT_DIR == dirp->d_type)?"DIR ":"FILE",dirp->d_name);
            ffListInsert(pList,dirp);
        }
        closedir(dp);
    }

    s_ff.pSelectEntry = (pList->pxHead->next != NULL)?(pList->pxHead->next):(pList->pxHead);
    ESP_LOGD(TAG, "select %s - %s", s_ff.pSelectEntry->dirp.d_name,(DT_DIR == s_ff.pSelectEntry->dirp.d_type)?"DIR":"FILE");
}

void FileExplorer_cd(const char* dirName)
{
    if (0 == strcmp(UP_PATH,dirName))
    {
        uint32_t len = strlen(s_ff.current_path);
        int i = len - 1;
        for(;i >= 0; i--)
        {
            if (s_ff.current_path[i] == '/')
            {
                break;
            }
        }
        if (i != 0)
        {
            s_ff.current_path[i] = '\0';
            printf("cd ..\n");
            FileExplorer_updatCurrentPath();
        }
        else
        {
            ESP_LOGI(TAG, "is root patch! %s", s_ff.current_path);
        }
    }
    else
    {
        bool isValidDir = 0;
        DIR * dp = opendir(s_ff.current_path);

        struct dirent *dirp = NULL;
        if (dp != NULL)
        {
            while((dirp = readdir(dp)) != NULL)
            {
                if (DT_DIR == dirp->d_type)
                {
                    if (0 == strcmp(dirp->d_name,dirName))
                    {
                        isValidDir = 1;
                        break;
                    }
                }
            }
            closedir(dp);
        }

        if (isValidDir && strlen(dirName) + strlen(s_ff.current_path) + 2 < MAX_PATH_LEN)
        {
            strcat(s_ff.current_path, "/");
            strcat(s_ff.current_path, dirName);

            //ESP_LOGI(TAG, "cd %s", s_ff.current_path);
            printf("cd %s\n", s_ff.current_path);
            FileExplorer_updatCurrentPath();
        }
    }
}

void FileExplorer_cat(const char* dirName)
{
    uint32_t len = strlen(dirName);
    if (len > 4)//len of "*.txt"
    {
        if(0 == strcmp(&(dirName[len-4]),".txt"))
        {
            if (strlen(dirName) + strlen(s_ff.current_path) + 2 < MAX_PATH_LEN)
            {
                char fileName[MAX_PATH_LEN];
                strncpy(fileName, s_ff.current_path, MAX_PATH_LEN);
                strcat(fileName, "/");
                strcat(fileName, dirName);

                FILE *f = fopen(fileName, "r");
                if (f != NULL)
                {
                    // Read a line from file
                    char line[256];
                    while (fgets(line, sizeof(line), f) != NULL)
                    {
                        printf("%s",line);
                    }
                    printf("\n");
                    fclose(f);
                }
            }
        }
    }

}


void FileExplorer_nextEntry()
{
    ffList_t *pList = &(s_ff.entryList);
    s_ff.pSelectEntry = (s_ff.pSelectEntry->next != NULL)?(s_ff.pSelectEntry->next):(pList->pxHead);
    ESP_LOGD(TAG, "select %s - %s", s_ff.pSelectEntry->dirp.d_name,(DT_DIR == s_ff.pSelectEntry->dirp.d_type)?"DIR":"FILE");
}

void FileExplorer_preEntry()
{
    ffList_t *pList = &(s_ff.entryList);
    s_ff.pSelectEntry = (s_ff.pSelectEntry->prev != NULL)?(s_ff.pSelectEntry->prev):(pList->pxTail);
    ESP_LOGD(TAG, "select %s - %s", s_ff.pSelectEntry->dirp.d_name,(DT_DIR == s_ff.pSelectEntry->dirp.d_type)?"DIR":"FILE");
}

void FileExplorer_getSelectedEntry(char** name,int *pIsDir)
{
    *name = s_ff.pSelectEntry->dirp.d_name;
    *pIsDir = (s_ff.pSelectEntry->dirp.d_type == DT_DIR);
}


void FileExplorerInit(FileExplorer_t **pFileExplorer)
{
    SDCardCtrlInit();
    sdCtrlSelectSDCard();

    s_ff.base_path = MOUNT_POINT;
    strncpy(s_ff.current_path, s_ff.base_path, MAX_PATH_LEN);
    *pFileExplorer = &(s_ff.fileExplorer);
    s_ff.pSelectEntry = NULL;

    SDIOInit(s_ff.base_path);

    FileExplorer_updatCurrentPath();

    //esp_vfs_fat_sdcard_unmount(mount_point, card);
    //ESP_LOGI(TAG, "Card unmounted");
}


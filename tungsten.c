/* File: tungsten.c
 * Creation Date: December 17th, 2009
 * Last Modified Date: January 9th, 2010
 * Version: 0.1.14
 * Contact: Adam Lamers <adam@millenniumsoftworks.com>
*/

#include "tungsten.h"
#include <stdio.h>
#include <windows.h>

char iPhone_LastError[255];

char *iPhone_GetLastError()
{
    return iPhone_LastError;
}

void iPhone_SetLastError(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(iPhone_LastError, format, args);
    va_end(args);
}

void iPhone_init(iPhone *iphone)
{
    void* notification = malloc(sizeof(void*));
    int ret = AMDeviceNotificationSubscribe(iphone->dnc, 0, 0, 0, notification);
    if(ret != 0)
        iPhone_SetLastError("AMDeviceNotificationSubscribe failed with error %d", ret);
    
    ret = AMRestoreRegisterForDeviceNotifications(iphone->drn1, iphone->drn2, iphone->drn3, iphone->drn4, 0, NULL);
    if(ret != 0)
        iPhone_SetLastError("AMRestoreRegisterForDeviceNotifications failed with error %d", ret);
    free(notification);
}

BOOL iPhone_FileExists(iPhone *iphone, char *path)
{
    void *data = NULL;
    int ret = AFCFileInfoOpen(iphone->hAFC, path, &data);
    if(ret == 0) 
    {
        AFCKeyValueClose(data);
        return TRUE;
    }
    else
        return FALSE;
}

BOOL GetFiles(iPhone* iphone, char *path)
{
    if(iphone->connected == FALSE){ iPhone_SetLastError("GetFiles() failed, device not connected."); return FALSE; } //If we aren't connected, we can't get files.
    void *hDir = NULL;
    if(AFCDirectoryOpen(iphone->hAFC, path, &hDir) != 0){ iPhone_SetLastError("AFCDirectoryOpen failed"); return FALSE; }
    char *buffer = NULL;
    while(1)
    {
        AFCDirectoryRead(iphone->hAFC, hDir, &buffer);
        if(buffer == NULL) break;
        printf("%s\n", buffer);
    }
    AFCDirectoryClose(iphone->hAFC, hDir);
    return TRUE;
}

BOOL iPhone_Connect(iPhone *iphone)
{
    if(AMDeviceConnect(iphone->handle) == 1)   iPhone_SetLastError("Device is in recovery mode.");
    if(AMDeviceIsPaired(iphone->handle) == 0){ iPhone_SetLastError("AMDeviceIsPaired failed."); return FALSE; }
    if(AMDeviceValidatePairing(iphone->handle) != 0){ iPhone_SetLastError("AMDeviceValidatePairing failed."); return FALSE; }
    if(AMDeviceStartSession(iphone->handle) == 1){ iPhone_SetLastError("AMDeviceStartSession failed."); return FALSE; }
    BOOL isAfc2 =( AMDeviceStartService(iphone->handle, __CFStringMakeConstantString("com.apple.afc2"), &iphone->hService, NULL) == 0);
    if(!isAfc2)
    {
        BOOL isAfc = (AMDeviceStartService(iphone->handle, __CFStringMakeConstantString("com.apple.afc"), &iphone->hService, NULL) == 0);
        if(!isAfc) return FALSE;
    }
    else
        iphone->wasAFC2 = TRUE;
    if(AFCConnectionOpen(iphone->hService, 0, &iphone->hAFC) != 0){ iPhone_SetLastError("AFCConnectionOpen failed."); return FALSE; }
    iphone->connected = TRUE;
    return TRUE;
}

void iPhone_WaitForConnect(iPhone *iphone)
{
    while(iphone->connected == FALSE);
}

int iPhone_GetFileType(iPhone *iphone, char *path)
{
    if(iphone->connected)
    {
        void *data = NULL;
        int ret = AFCFileInfoOpen(iphone->hAFC, path, &data);
        if(ret == 0 && data != NULL)
        {
            void *name, *val;
            while(AFCKeyValueRead(data, &name, &val) == 0 && name != NULL && val != NULL)
            {
                if(strcmp(name, "st_ifmt") == 0)
                {
                    if(strcmp(val, "S_IFREG") == 0) return AMD_FILE;
                    else if(strcmp(val, "S_IFDIR") == 0) return AMD_DIR;
                    else if(strcmp(val, "S_IFLNK") == 0) return AMD_LINK;
                    else return AMD_UNKNOWN;
                }
            }
            AFCKeyValueClose(data);
        }
    }
    return AMD_UNKNOWN;
}

long iPhone_GetFileSize(iPhone *iphone, char *path)
{
    if(iphone->connected)
    {
        void *data = NULL;
        int ret = AFCFileInfoOpen(iphone->hAFC, path, &data);
        if(ret == 0 && data != NULL)
        {
            void *name, *val;
            while(AFCKeyValueRead(data, &name, &val) == 0 && name != NULL && val != NULL)
            {
                if(strcmp(name, "st_size") == 0)
                {
                    return atol(val);
                }
            }
            AFCKeyValueClose(data);
        }
    }
    return 0;
}

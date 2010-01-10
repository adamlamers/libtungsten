/* File: tungsten.c
 * Creation Date: December 17th, 2009
 * Last Modified Date: January 9th, 2010
 * Version: 0.1.14
 * Contact: Adam Lamers <adam@millenniumsoftworks.com>
*/

/********************************************************************************
* This file is part of Tungsten.                                                *
*                                                                               *
* Tungsten is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU General Public License as published by          *
* the Free Software Foundation, either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* Tungsten is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU General Public License for more details.                                  *
*                                                                               *
* You should have received a copy of the GNU General Public License             *
* along with Tungsten.  If not, see <http://www.gnu.org/licenses/>.             *
********************************************************************************/

#include "tungsten.h"
#include <stdio.h>
#include <windows.h>

char iPhone_LastError[255];

/** Retrieves the error string of the last error that occured in the library
  * @return char* containing the error text.
  */
char *iPhone_GetLastError()
{
    return iPhone_LastError;
}

/** Internal function: Set the error text of the last error that occured. */
void iPhone_SetLastError(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(iPhone_LastError, format, args);
    va_end(args);
}

/** Initializes an iPhone, registering it's callbacks to recieve device notifications.
  * @param iphone Pointer to an iPhone struct with the dnc member is initialized to a DeviceNotification callback.
  */  
void iPhone_init(iPhone *iphone)
{
    void* notification = malloc(sizeof(void*));
    int ret = AMDeviceNotificationSubscribe(iphone->dnc, 0, 0, 0, notification);
    if(ret != 0)
        iPhone_SetLastError("AMDeviceNotificationSubscribe failed with error %d", ret);
    
    /*
    ret = AMRestoreRegisterForDeviceNotifications(iphone->drn1, iphone->drn2, iphone->drn3, iphone->drn4, 0, NULL);
    if(ret != 0)
        iPhone_SetLastError("AMRestoreRegisterForDeviceNotifications failed with error %d", ret);
    */ /* Can't really do anything in recovery mode anyway. */
    free(notification);
}

/** Determines whether or not a file exists on the device.
  * @param iphone The iPhone on which to locate the file.
  * @param path   The UNIX style path to the file on the device.
  * @return BOOL indicating whether or not the file exists. (TRUE = exists)
  */
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

/** Debug function, retrieves all files in a directory and prints them to stdout */
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

/** Connect to an iPhone device, and register callbacks.
  * Members of the iPhone struct that must be valid: dnc
  * for device notifications
  */
BOOL iPhone_Connect(iPhone *iphone)
{
    if(AMDeviceConnect(iphone->handle) == 1)
    { 
        iPhone_SetLastError("Device is in recovery mode. Must be activated with iTunes."); 
        return FALSE ; 
    }
    
    if(AMDeviceIsPaired(iphone->handle) == 0)
    { 
        iPhone_SetLastError("AMDeviceIsPaired failed."); 
        return FALSE; 
    }
    
    if(AMDeviceValidatePairing(iphone->handle) != 0)
    { 
        iPhone_SetLastError("AMDeviceValidatePairing failed."); 
        return FALSE; 
    }
    
    if(AMDeviceStartSession(iphone->handle) == 1)
    { 
        iPhone_SetLastError("AMDeviceStartSession failed."); 
        return FALSE; 
    }
    
    if(AMDeviceStartService(iphone->handle, __CFStringMakeConstantString("com.apple.afc2"), &iphone->hService, NULL) != 0)
    {
        if(AMDeviceStartService(iphone->handle, __CFStringMakeConstantString("com.apple.afc"), &iphone->hService, NULL) != 0)
            return FALSE;
    }
    
    if(AFCConnectionOpen(iphone->hService, 0, &iphone->hAFC) != 0)
    { 
        iPhone_SetLastError("AFCConnectionOpen failed."); 
        return FALSE; 
    }
    
    iphone->connected = TRUE;
    return TRUE;
}

/** Waits for the specified iPhone to connect. */
void iPhone_WaitForConnect(iPhone *iphone)
{
    while(iphone->connected == FALSE);
}

/** Retrieves the type of the file.
  * @param iphone The iPhone on which to locate the file.
  * @param path   The UNIX style path to the file on the iPhone.
  * @return AMD_FILE for regular file, AMD_DIR for directory, and AMD_LINK for hard or symbolic links.
  */
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

/** Retrieves the file size of a file on an iPhone.
  * @param iphone The iPhone on which to locate the file.
  * @param path   The UNIX style path to the file on the iPhone.
  * @return The size of the file.
  */ 
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

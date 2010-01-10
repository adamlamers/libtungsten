/* File: tungsten.h
 * Creation Date: December 17th, 2009
 * Last Modified Date: January 9th, 2010
 * Version: 0.0.10
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

#ifndef __TUNGSTEN_H__
#define __TUNGSTEN_H__
#include <windows.h>
#include "iTunesMobileDevice.h"

enum { AMD_FILE = 1, AMD_DIR, AMD_LINK, AMD_UNKNOWN };
enum { AMDFM_READ = 1, AMDFM_WRITE, AMDFM_READWRITE };

typedef struct iPhoneFile_tag
{
    uint64_t handle;
    int      mode;
    long     size;
}iPhoneFile;

typedef struct iPhone_tag
{
    DeviceNotificationCallback        dnc;  /** < Standard device notification callback */

    void* handle;                           /** < Handle to device. */
    void* hAFC;                             /** < com.apple.AFC service handle */
    void* hService;                         /** < Service handle */
    BOOL  connected;                        /** < Connection status */ 
}iPhone;

typedef struct iPhoneFileInfo_tag
{
    int    blocks;
    int    nlink;
    long   size;
    char  *ifmt;
} iPhoneFileInfo;

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void       DLL_EXPORT iPhone_init(iPhone*);
BOOL       DLL_EXPORT iPhone_Connect(iPhone*);
BOOL       DLL_EXPORT GetFiles(iPhone*,char*);
BOOL       DLL_EXPORT iPhone_FileExists(iPhone*,char*);
long       DLL_EXPORT iPhone_GetFileSize(iPhone*, char*);
void       DLL_EXPORT iPhone_WaitForConnect(iPhone*);
char       DLL_EXPORT *iPhone_GetLastError();
iPhoneFile DLL_EXPORT *iPhone_fopen(iPhone *iphone, char *path, char *mode);
void       DLL_EXPORT iPhone_fclose(iPhone *iphone, iPhoneFile *file);
long       DLL_EXPORT iPhone_ftell(iPhone *iphone, iPhoneFile *file);
int        DLL_EXPORT iPhone_fread(iPhone *iphone, iPhoneFile *file, void *buffer, int count);
int        DLL_EXPORT iPhone_CopyFileFromDevice(iPhone*, char*, char*);

/* Private functions */
void iPhone_SetLastError(char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // __TUNGSTEN_H__

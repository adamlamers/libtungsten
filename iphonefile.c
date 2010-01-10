/* File: iphonefile.c
 * Creation Date: January 9th, 2010
 * Last Modified Date: January 9th, 2010
 * Version: 0.0.6
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

/** Opens a file on the device for reading or writing.
  * @param iphone The iPhone on which to locate the file.
  * @param path   The UNIX style path to the file on the device.
  * @param mode   The mode string. ("r" = read only, "w" = write only, "rw" = read and write);
  * @return Pointer to an iPhoneFile structure on success, NULL on failure.
  */
iPhoneFile* iPhone_fopen(iPhone *iphone, char *path, char *mode)
{
    iPhoneFile *file = malloc(sizeof(iPhoneFile));
    memset(file, 0, sizeof(iPhoneFile));
    
    file->size = iPhone_GetFileSize(iphone, path);
    
    int modeflag = 0;
    if(strchr(mode, 'r') != NULL)
        modeflag |= AMDFM_READ;
    if(strchr(mode, 'w') != NULL)
        modeflag |= AMDFM_WRITE;
    
    file->mode = modeflag;
    
    int ret = 0;
    if((ret = AFCFileRefOpen(iphone->hAFC, path, modeflag, 0, &file->handle)) != 0)
    {
        iPhone_SetLastError("AFCFileRefOpen failed with error %d", ret);
        return NULL;
    }
    return file;
}

/** Close a handle to an iPhoneFile.
  * @param iphone The iPhone which the file belongs to.
  * @param file Pointer to an iPhoneFile structure to release.
  */
void iPhone_fclose(iPhone *iphone, iPhoneFile *file)
{
    int ret = AFCFileRefClose(iphone->hAFC, file->handle);
    file->handle = 0;
    file->size = 0;
    file->mode = 0;
    if(ret != 0)
        iPhone_SetLastError("AFCFileRefClose failed with error %d", ret);
}

/** Tell the current position within a file.
  * @param iphone The iPhone which the file belongs to.
  * @param file   Pointer to an iPhoneFile to retrieve position from.
  * @return Current position within the file.
  */
long iPhone_ftell(iPhone *iphone, iPhoneFile *file)
{
    long pos = 0;
    int ret = AFCFileRefTell(iphone->hAFC, file->handle, &pos);
    if(ret != 0)
        iPhone_SetLastError("AFCFileRefTell failed with error %d", ret);
    return pos;
}

/** Read a sequence of bytes from a file on the device into a buffer.
  * @param iphone The iPhone which the file belongs to.
  * @param file   The file to read from.
  * @param buffer Buffer of length of at least "count"
  * @param count  Number of bytes to read.
  * @return Number of bytes sucessfully read.
  */
int iPhone_fread(iPhone *iphone, iPhoneFile *file, void *buffer, int count)
{
    void *buf;
    buf = malloc(count);
    
    int bytesRead = count;
    int ret = AFCFileRefRead(iphone->hAFC, file->handle, buf, &bytesRead);
    if(ret != 0)
    {
        iPhone_SetLastError("AFCFileRefRead failed with error %d", ret);
        free(buf);
        return 0;
    }
    memcpy(buffer, buf, bytesRead);
    free(buf);
    return bytesRead;
}

/** Copy an entire file from the device to the local machine.
  * @param iphone The iPhone which the file belongs to.
  * @param pathOnDevice The UNIX style path to the file on the device.
  * @param localPath The path to the local file to write to (will be overwritten if it exists)
  * @return Number of bytes successfully copied.
  */
int iPhone_CopyFileFromDevice(iPhone *iphone, char *pathOnDevice, char *localPath)
{
    FILE *outfile = fopen(localPath, "wb");
    if(outfile == NULL) return 0;
    
    iPhoneFile *devicefile = iPhone_fopen(iphone, pathOnDevice, "r");
    if(devicefile == NULL) return 0;
    
    void *buf = malloc(BUFSIZ);
    if(buf == NULL) return 0;
    memset(buf, 0, BUFSIZ);
    
    int bytesRead = 0;
    int totalRead = 0;
    do
    {
        bytesRead = iPhone_fread(iphone, devicefile, buf, BUFSIZ);
        totalRead += bytesRead;
        fwrite(buf, bytesRead, 1, outfile);
    }while(bytesRead > 0);
    
    iPhone_fclose(iphone, devicefile);
    free(buf);
    fclose(outfile);
    return totalRead;
}

/* File: iphonefile.c
 * Creation Date: January 9th, 2010
 * Last Modified Date: January 9th, 2010
 * Version: 0.0.1
 * Contact: Adam Lamers <adam@millenniumsoftworks.com>
*/

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

int iPhone_fread(iPhone *iphone, iPhoneFile *file, void *buffer, int offset, int count)
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
    memcpy(buffer + offset, buf, bytesRead);
    free(buf);
    return bytesRead;
}

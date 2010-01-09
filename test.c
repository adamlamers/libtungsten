/* File: ${filename}.c
 * Creation Date: ${date} ${time}
 * Last Modified Date: ${date} ${time}
 * Version: 0.0.1
 * Contact: Adam Lamers <adam@millenniumsoftworks.com>
*/
#include "tungsten.h"
#include "iTunesMobileDevice.h"
#include <stdio.h>

iPhone *phone;

void NotifyCallback(AMDeviceNotificationCallbackInfo* callback)
{
    switch(callback->msg)
    {
        case Connected:
            phone->handle = callback->dev;
            if(iPhone_Connect(phone) == TRUE)
            {
                //GetFiles(phone, "/private/var/mobile/Media/iTunes_Control/iTunes/");
            }
        break;
        case Disconnected:
            phone->connected = FALSE;
        break;
        default:
        break;
    }
}

int main(int argc, char **argv)
{
    phone = malloc(sizeof(iPhone));
    memset(phone, 0, sizeof(iPhone));
    phone->dnc = &NotifyCallback;
    iPhone_init(phone);
    iPhone_WaitForConnect(phone);
    
    FILE *outfile = fopen("test.bin", "wb");
    
    iPhoneFile *iTunesDB = iPhone_fopen(phone, "/private/var/mobile/Media/iTunes_Control/iTunes/iTunesCDB", "r");
    void *buf = malloc(iTunesDB->size);
    memset(buf, 0, iTunesDB->size);
    int bytesRead = iPhone_fread(phone, iTunesDB, buf, 0, iTunesDB->size);
    printf("%d bytes read.", bytesRead);
    fwrite(buf, iTunesDB->size, 1, outfile);
    
    iPhone_fclose(phone, iTunesDB);
    free(buf);
    fclose(outfile);
    
    getchar();
    return 0;
}

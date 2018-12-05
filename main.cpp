#include "mbed.h"
#include "SDFileSystem.h"
#include "WAV_Reader.h"

//TODO 1: read data from SD card 
//FIXME: copied example below, unedited
//Create an SDFileSystem object
// DigitalOut led_strip(p15); //TODO: get actual output type and pin(s)
SDFileSystem sd(p11, p12, p13, p14, "sd");
WAV_Reader wav();

int main()
{
    //Mount the filesystem
    sd.mount();

    //Perform a read test
    printf("Reading from SD card...");
    fp = fopen("/sd/sdtest.txt", "r"); // TODO: find a song in SD card and put its name here
    if (fp != NULL) {
        //TODO 2: open and read song
        wav.open(fp);
        if (wav.read(buffer, size, len))
            //TODO 3: take the audio in the buffer and apply some feature function to it
            
            //TODO 4: output the altered audio data to the LED strip
            
        
//        char c = fgetc(fp);
//        if (c == 'W')
//            printf("success!\n");
//        else
//            printf("incorrect char (%c)!\n", c);
        fclose(fp);
    } else {
        printf("failed!\n");
    }

    //Unmount the filesystem
    sd.unmount();
}

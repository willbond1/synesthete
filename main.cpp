#include "mbed.h"
#include <errno.h>

#include "SDBlockDevice.h"
#include "FATFileSystem.h"

#include "WAV_Reader.h"

//TODO 1: read data from SD card 
//FIXME: copied example below, unedited
//Create an SDFileSystem object
DigitalOut myled(LED1); // test light
SDBlockDevice bd(p11, p12, p13, p14); // uses spi pins
FATFileSystem fs("sd");

#define LED_NUM 30 // number of LEDs
#define LED_FREQ 500000 // spi
SPI led_spis(p5, p6, p7)
int led_buf[LED_NUM];

WAV_Reader wav();


int main()
{
    // Try to mount the filesystem
    int err = fs.mount(&bd);
    if (err) {
        // Reformat if we can't mount the filesystem
        // this should only happen on the first boot
        fflush(stdout);
        err = fs.reformat(&bd);
        if (err) {
            error("error: %s (%d)\n", strerror(-err), err);
        }
    }
    // Open the wav file
    FILE *f = fopen("/sd/test.wav", "r");
    if (!f) {
        // Create the numbers file if it doesn't exist
        if (!f) {
            error("error: %s (%d)\n", strerror(errno), -errno);
        }
        for (int i = 0; i < 10; i++) {
            err = fprintf(f, "    %d\n", i);
            if (err < 0) {
                error("error: %s (%d)\n", strerror(errno), -errno);
            }
        }
        err = fseek(f, 0, SEEK_SET);
        }
    }
    
    //TODO 2: open and read song
    wav.open(f);
    wav.loop(true);
    //TODO: determine length
    // Note: there are 30 LEDs in the strip we have
    int window = 5; // how many audio signals will be in each window
    int len = 90; // the length of the buffer
    int size = sizeof(char); // the size of the elements in the buffer
    int *buf = (char *) malloc(len*size);// the buffer to put the audio signal into
    
    while (wav.read(buf, size, len)) {
        //TODO 3: take the audio in the buffer and apply some feature function to it
        int i = 0;
        int tri_signals = [LED_NUM][3] // a value for r,g,b
        while i < num_leds {
            int sigR = buf[i];
            int sigG = buf[i+1];
            int sigB = buf[i+2];
            tri_signals[i][0] = sigR;
            tri_signals[i][1] = sigG;
            tri_signals[i][2] = sigB;
            }
        }
        brightness = 10// is between 0 and 31
        //TODO 4: output the altered audio data to the LED strip
        int i;
        
        spi.frequency(LED_FREQ); //TODO: replace with custom frequency
        
        for (i = 0; i < LED_NUM; i ++) {
            led_buf[i] = (tri_signals[i][0] << 4) | (tri_signal[i][1] << 2) | (tri_signal[i][2]);// 0xffffff = white, (think RGB)
        }
        
        //DOTSTAR
        int i;
        
        // start frame
        for (i = 0; i < 4; i ++) {
            spi.write(0);
        }
        // led frame
        for (i = 0; i < LED_NUM; i ++) {
            spi.write((7<<5) | brightness);
            spi.write((led_buf[i] >> 16) & 0xff); // B
            spi.write((led_buf[i] >> 8) & 0xff); // G
            spi.write(led_buf[i] & 0xff); // R
        }
        // end frame
        for (i = 0; i < 4; i ++) {
            spi.write(1);
        }
        myled = !myled;
        wait(0.05);
    }
        
        
//        char c = fgetc(fp);
//        if (c == 'W')
//            printf("success!\n");
//        else
//            printf("incorrect char (%c)!\n", c);
    err = fclose(f);
    if (err < 0) {
        error("error: %s (%d)\n", strerror(errno), -errno);
    }

    //Unmount the filesystem
    fs.unmount();
}

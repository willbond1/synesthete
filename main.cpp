#include "mbed.h"
#include <errno.h>

#include "SDFileSystem.h"

#include "WAV_Reader.h"

//TODO 1: read data from SD card
//Create an SDFileSystem object
DigitalOut myled(LED1); // test light
SDFileSystem fs(p11, p12, p13, p14, "sd"); // uses spi pins

#define LED_NUM 30 // number of LEDs
#define LED_FREQ 500000 // spi
SPI led_spi(p5, p6, p7);
int led_buf[LED_NUM];

WAV_Reader wav;

//AnalogOut DACout(p18); //set up speaker
//wave_player waver(&DACout); //set up wave player library
//int pos = 0; // index of the song
//int vol = 0; // volume controller

int main()
{
    // Try to mount the filesystem
    fs.mount();
    int err;
    // Open the wav file
    FILE *fp = fopen("/sd/test.wav", "r");
    if (!fp) {
        // Create the numbers file if it doesn't exist
        if (!fp) {
            error("error: %s (%d)\n", strerror(errno), -errno);
        }
        for (int i = 0; i < 10; i++) {
            err = fprintf(fp, "    %d\n", i);
            if (err < 0) {
                error("error: %s (%d)\n", strerror(errno), -errno);
            }
        }
        err = fseek(fp, 0, SEEK_SET);
    }
    wait(0.1); // wait for opening
    
    // open and read song
    wav.open(&fp);
    wait(0.5);
//    wav.loop(true);
    // Note: there are 30 LEDs in the strip we have
    int len = 90; // the length of the buffer
    int size = 32; // the size of the elements in the buffer
    int *buf = (int *) malloc(len*size);// the buffer to put the audio signal into
    
    int tri_signals[LED_NUM][3]; // a value for r,g,b
    
    while (wav.read(buf, size, len)) {
        // take the audio in the buffer and apply some feature function to it
        int i = 0;
        
        //while(i < LED_NUM) {
//            int sigR = buf[i]%0xff;
//            int sigG = buf[i+1]%0xff;
//            int sigB = buf[i+2]%0xff;
//            tri_signals[i][0] = sigR;
//            tri_signals[i][1] = sigG;
//            tri_signals[i][2] = sigB;
//            i += 3;
//        }
        int brightness = 10; // is between 0 and 31
        
        // output the altered audio data to the LED strip
        led_spi.frequency(LED_FREQ);
        
        for (i = 0; i < LED_NUM; i ++) {
//            led_buf[i] = (tri_signals[i][0] << 16) | (tri_signals[i][1] << 8) | (tri_signals[i][2]);// 0xffffff = white, (think RGB)
            led_buf[i] = 0xffff;// 0xffffff = white, (think RGB)
        }
        
        //DOTSTAR
        // start frame
        for (i = 0; i < 4; i ++) {
            led_spi.write(0);
        }
        // led frame
        for (i = 0; i < LED_NUM; i ++) {
            led_spi.write((7<<5) | brightness);
            led_spi.write(0xff); // B
            led_spi.write(0xff); // G
            led_spi.write(0xff); // R
        }
        // end frame
        for (i = 0; i < 4; i ++) {
            led_spi.write(1);
        }
        myled = !myled;
        
    }
    
    err = fclose(fp);
    if (err < 0) {
        error("error: %s (%d)\n", strerror(errno), -errno);
    }

    //Unmount the filesystem
    fs.unmount();
}

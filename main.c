#include "led_control.h"

#define PIN_LED1 18
#define PIN_LED2 23

int main() {

    /* GPIO Setup */
    int led_green = configurePin(PIN_LED1);
    int led_red = configurePin(PIN_LED2);


    sleep(5);

	releasePin(led_green);
	releasePin(led_red);
    return 0;

    if(led_green == -1 || led_red == -1){
        return 1;
    }

    setPinValue(led_green, 1);
    setPinValue(led_red, 1);

    bool exit = false;
    while(!exit) {

    //wait for storage
    if(false){
        //storage bad
        setPinValue(led_green, 0);
        setPinValue(led_red, 1);
        //blink red?
        continue;
    }

    //check storage for file
    if(false){
        setPinValue(led_green, 0);
        setPinValue(led_red, 1);
        continue;
    }

    setPinValue(led_green, 1);
    setPinValue(led_red, 0);

    //wait for button press
//    while(true) {}

    //blink green led -> timer interrupt?
    //run openocd
    for(int i = 0; i < 10;){
        setPinValue(led_green, 0);
        sleep(1);
        setPinValue(led_green, 1);
        sleep(2);
    }

    //stop timer
    if(true){
        //success - solid green LED
        setPinValue(led_green, 1);
    }
    else {
        //failed to program - solid red LED
        setPinValue(led_red, 1);
    }


    exit = true;
    }
    sleep(5);

    setPinValue(led_green, 0);
    setPinValue(led_red, 0);

    /* GPIO clean up */
    close(led_green);
    close(led_red);
    releasePin(PIN_LED1);
    releasePin(PIN_LED2);
    return 0;
}

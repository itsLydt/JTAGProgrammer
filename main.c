#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#include "led_control.h"
#include "file_utilities.h"

#define PIN_LED1 18
#define PIN_LED2 23

#define ON true
#define OFF false

bool flash(char* filename);
void timerCallback(int sig_num);
void cleanup();

enum STATUS {
    STARTING = 1,
    WAITING_STORAGE = 2,
    WAITING_READY = 3,
    PROGRAMMING = 4,
    ERRORED_STORAGE = 5,
    ERRORED_PROGRAMMER_FAILED = 6,
    UNKNOWN
};

struct ledStatus {
    enum STATUS state;
    int led_green;
    int led_red;
    bool led_green_state;
    bool led_red_state;
} devState;


void sleeps(int seconds){
    struct timespec req;
    struct timespec rem;
    req.tv_sec = seconds;
    req.tv_nsec = 0;

    while(true){
        int v = nanosleep(&req, &rem);
        if(v == -1){
            if(errno == EINTR){
                req.tv_nsec = rem.tv_nsec;
                req.tv_sec = rem.tv_sec;
                continue;
            }
            else return;
        }
        else return;
    }
}

int main() {
    devState.state = STARTING;

    /* GPIO Setup */
    devState.led_green = configurePin(PIN_LED1);
    devState.led_red = configurePin(PIN_LED2);

    if(devState.led_green == -1 || devState.led_red == -1){
        perror("Failed to configure LED pins.");
        return 1;
    }

    if(setPinValue(devState.led_green, ON))
        devState.led_green_state = ON;


    if(setPinValue(devState.led_red, ON))
        devState.led_red_state = ON;

    sleep(3);
    printf("Starting timer.\r\n");

    /* Timer setup */
    signal(SIGALRM, timerCallback);   

    struct itimerval newTimer;
    newTimer.it_value.tv_sec = 0; 
    newTimer.it_value.tv_usec = 1000; //start timer one ms from now
    newTimer.it_interval.tv_sec = 0;
    newTimer.it_interval.tv_usec = 250 * 1000; //generate timer interrupts every x ms

    if(setitimer(ITIMER_REAL, &newTimer, NULL) == -1){
        perror("Failed to configure timer.");
        cleanup();
        return 1;
    }

    //wait for storage to exist
    devState.state = WAITING_STORAGE;
    sleeps(5);

    if(false){
        //storage bad
        devState.state = ERRORED_STORAGE;
        cleanup();
        return 1;
    }

    //check storage for file

    char* filename = "~/test.hex";
    if(false){
        devState.state = ERRORED_STORAGE;
        cleanup();
        return 1;
    }

    devState.state = WAITING_READY;

    //wait for button press
    sleeps(2);
    

    //run openocd

    bool success = flash(filename);
    if(success){
        devState.state = WAITING_READY;
    }
    else {
        devState.state = ERRORED_PROGRAMMER_FAILED;
    }
    sleeps(5);

    cleanup();
    return 0;
}

bool flash(char* filename){
    devState.state = PROGRAMMING;
    char* commandTemplate = "openocd -f ../openocd.cfg -c \"program %s verify reset exit\"";
    int len = snprintf(NULL, 0, commandTemplate, filename);
	char* commandStr = malloc(len + 1);
	snprintf(commandStr, len + 1, commandTemplate, filename);
    printf("Starting: %s\r\n", commandStr);
    int retCode = system(commandStr);
    free(commandStr);
    return retCode == 0;
}

void timerCallback(int sig_num){
    if(sig_num == SIGALRM)
    {
        bool green_setting;
        bool red_setting;
        bool flashing;
        switch (devState.state){
            case STARTING:
                green_setting = ON;
                red_setting = ON;
                flashing = false;
                printf("devState = STARTING\r\n");    
                break;
            case WAITING_STORAGE:
                green_setting = ON;
                red_setting = ON;
                flashing = true;
                printf("devState = WAITING_STORAGE\r\n");    
                break;
            case WAITING_READY:
                green_setting = ON;
                red_setting = OFF;
                flashing = false;
                printf("devState = WAITING_READY\r\n");
                break;
            case PROGRAMMING:
                green_setting = ON;
                red_setting = OFF;
                flashing = true;
                printf("devState = PROGRAMMING\r\n");
                break;
            case ERRORED_STORAGE:
                green_setting = OFF;
                red_setting = ON;
                flashing = true;
                printf("devState = ERRORED_STORAGE\r\n");
                break;
            case ERRORED_PROGRAMMER_FAILED:
                green_setting = OFF;
                red_setting = ON;
                flashing = false;
                printf("devState = ERRORED_PROGRAMMER_FAILED\r\n");
                break;
        }

        //printf("G: %d R: %d F: %d\r\n", green_setting, red_setting, flashing);

        if(flashing){
            if(green_setting){
                //invert current value of green led
                if(setPinValue(devState.led_green, !devState.led_green_state))
                    devState.led_green_state = !devState.led_green_state;
            }
            else {
                if(devState.led_green_state && setPinValue(devState.led_green, 0)){
                    devState.led_green_state = false;
                }
            }

            if(red_setting){
                //invert current value of red led
                if(setPinValue(devState.led_red, !devState.led_red_state))
                    devState.led_red_state = !devState.led_red_state;
            }
            else {
                if(devState.led_red_state && setPinValue(devState.led_red, 0)){
                    devState.led_red_state = false;
                }
            }
        }
        else {
            if(devState.led_green_state != green_setting && setPinValue(devState.led_green, green_setting)){
                devState.led_green_state = green_setting;
            }
            if(devState.led_red_state != red_setting && setPinValue(devState.led_red, red_setting)){
                devState.led_red_state = red_setting;
            }
        }

    }
}

void cleanup(){
    //stop timer
    struct itimerval newTimer;
    newTimer.it_value.tv_sec = 0; 
    newTimer.it_value.tv_usec = 0;
    newTimer.it_interval.tv_sec = 0;
    newTimer.it_interval.tv_usec = 0;

    if(setitimer(ITIMER_REAL, &newTimer, NULL) == -1){
        perror("Failed to configure timer.");
    }

    setPinValue(devState.led_green, OFF);
    setPinValue(devState.led_red, OFF);

    /* GPIO clean up */
    close(devState.led_green);
    close(devState.led_red);
    releasePin(PIN_LED1);
    releasePin(PIN_LED2);
}
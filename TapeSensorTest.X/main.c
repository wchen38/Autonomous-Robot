/*
 * File:   main.c
 * Author: igonzalo
 *
 * Created on October 26, 2017, 3:21 PM
 */
#include <xc.h>
#include <BOARD.h>
#include <stdio.h>
#include <AD.h>
#include "IO_Ports.h"
#include "LED.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "pwm.h"
#include "RC_Servo.h"


//#define TEST_TAPE_SENSOR
#define TEST_TAPE_SENSOR_WITH_ES_FRAMEWORK
//#define TEST_BUMPER
//#define TEST_DRIVING_MOTORS
//#define TEST_SERVO
//#define TEST_DRIVING_MOTORS_HELPER_FUNCTIONS

#define TAPE_PIN_1 AD_PORTW3

#define  FRONT_LEFT_BUMPER_PIN PIN3
#define  FRONT_RIGHT_BUMPER_PIN PIN4
#define  BACK_LEFT_BUMPER_PIN PIN5
#define  BACK_RIGHT_BUMPER_PIN PIN6



#define LED_PIN PIN3
#define TAPE_HIGH_THRESHOLD 25
#define TAPE_LOW_THRESHOLD 5
#define ALL_LEDS 0xF


#ifdef TEST_TAPE_SENSOR
#include <timers.h>
#define Servo_Pin RC_PORTW08

void delay(int x) {
    int i = 0;
    while (i < x) {
        i++;
    }
}

int main() {
    BOARD_Init();
    LED_Init();

    int tape1AdcValHigh = 0;
    int tape1AdcValLow = 0;
    int diff = 0;
    ///////
    //Initialize Analog inputs
    AD_Init();
    AD_AddPins(TAPE_PIN_1);

    IO_PortsSetPortOutputs(PORTX, LED_PIN);


    LED_AddBanks(LED_BANK1);
    LED_OffBank(LED_BANK1, ALL_LEDS);
    //  IO_PortsClearPortBits(PORTX, LED_PIN);

    // IO_PortsSetPortBits(PORTX, LED_PIN);
    // int pot_val =
    while (1) {

        IO_PortsSetPortBits(PORTX, LED_PIN);
        delay(8000);
        tape1AdcValHigh = AD_ReadADPin(TAPE_PIN_1);
        //printf("tape1AdcVal = %d\r\n", tape1AdcVal);
        delay(100000);
        IO_PortsClearPortBits(PORTX, LED_PIN);
        delay(8000);
        tape1AdcValLow = AD_ReadADPin(TAPE_PIN_1);
        diff = tape1AdcValLow - tape1AdcValHigh;
        printf("tape1AdcVal diff = %d\r\n", diff);
        delay(100000);

        if (diff < TAPE_HIGH_THRESHOLD && diff > TAPE_LOW_THRESHOLD) {
            LED_SetBank(LED_BANK1, ALL_LEDS);
        } else {
            LED_OffBank(LED_BANK1, ALL_LEDS);
        }





    }
}
#endif

#ifdef TEST_TAPE_SENSOR_WITH_ES_FRAMEWORK

int main() {

    ES_Return_t ErrorType;

    BOARD_Init();



    printf("Starting ES Framework Template\r\n");
    printf("using the 2nd Generation Events & Services Framework\r\n");


    // Your hardware initialization function calls go here

    // now initialize the Events and Services Framework and start it running
    ErrorType = ES_Initialize();
    if (ErrorType == Success) {
        ErrorType = ES_Run();

    }
    //if we got to here, there was an error
    switch (ErrorType) {
        case FailedPointer:
            printf("Failed on NULL pointer");
            break;
        case FailedInit:
            printf("Failed Initialization");
            break;
        default:
            printf("Other Failure: %d", ErrorType);
            break;
    }
    for (;;)
        ;
}
#endif


#ifdef TEST_BUMPER
#define  FRONT_LEFT_BUMPER_PIN PIN3
#define  FRONT_RIGHT_BUMPER_PIN PIN4
#define  BACK_LEFT_BUMPER_PIN PIN5
#define  BACK_RIGHT_BUMPER_PIN PIN6

#define  SHIFT_AMOUNT 3

#define BUMPER_PORT PORTY

#define ALL_BUMPER_PINS (FRONT_LEFT_BUMPER_PIN | FRONT_RIGHT_BUMPER_PIN | BACK_LEFT_BUMPER_PIN |  BACK_RIGHT_BUMPER_PIN)

void delay(int x) {
    int i = 0;
    while (i < x) {
        i++;
    }
}

int main() {
    BOARD_Init();
    IO_PortsSetPortInputs(BUMPER_PORT, FRONT_LEFT_BUMPER_PIN);
    IO_PortsSetPortInputs(BUMPER_PORT, FRONT_RIGHT_BUMPER_PIN);
    IO_PortsSetPortInputs(BUMPER_PORT, BACK_LEFT_BUMPER_PIN);
    IO_PortsSetPortInputs(BUMPER_PORT, BACK_RIGHT_BUMPER_PIN);

    while (1) {
        printf("Status:%d \r\n", ((IO_PortsReadPort(BUMPER_PORT)) & ALL_BUMPER_PINS) >> SHIFT_AMOUNT);
        delay(1000000);
    }
    for (;;)
        ;
}
#endif

#ifdef TEST_DRIVING_MOTORS



#define DRIVING_MOTOR_PORT  PORTY
#define ENABLE_A PWM_PORTY12  
#define DIRECTION_A PIN11


#define ENABLE_B PWM_PORTY10 
#define DIRECTION_B PIN9

void delay(int x) {
    int i = 0;
    while (i < x) {
        i++;
    }
}

int main() {
    BOARD_Init();
    PWM_Init();

    IO_PortsSetPortOutputs(DRIVING_MOTOR_PORT, DIRECTION_A | DIRECTION_B);
    PWM_AddPins(ENABLE_A | ENABLE_B);

    //IO_PortsSetPortOutput(BUMPER_PORT,  FRONT_LEFT_BUMPER_PIN);
    //IO_PortsSetPortOutput(BUMPER_PORT,   FRONT_RIGHT_BUMPER_PIN);
    //IO_PortsSetPortInputs(BUMPER_PORT,  BACK_LEFT_BUMPER_PIN);
    //  IO_PortsSetPortInputs(BUMPER_PORT,  BACK_RIGHT_BUMPER_PIN);
    PWM_SetFrequency(PWM_500HZ);
    PWM_SetDutyCycle(ENABLE_A, 1000);
     PWM_SetDutyCycle(ENABLE_B, 950);
    IO_PortsWritePort(DRIVING_MOTOR_PORT, DIRECTION_A );
 IO_PortsClearPortBits(DRIVING_MOTOR_PORT,  DIRECTION_B);

    for (;;) {


    }

}

#endif

#ifdef TEST_SERVO
#define SERVO_PIN RC_PORTX03

void delay(int x) {
    int i = 0;
    while (i < x) {
        i++;
    }
}

int main() {
    BOARD_Init();
    RC_Init();
    RC_AddPins(SERVO_PIN);

  
 RC_SetPulseTime(SERVO_PIN, MINPULSE);
   delay(1000000);
   RC_SetPulseTime(SERVO_PIN,MAXPULSE);
   delay(100000);

    for (;;) {
        


    }

}
#endif



#ifdef TEST_DRIVING_MOTORS_HELPER_FUNCTIONS

#include "motors.h"


void delay(int x) {
    int i = 0;
    while (i < x) {
        i++;
    }
}

int main() {
    BOARD_Init();
    motors_init();
     forwards();
    
    
    for (;;) {


    }

}

#endif
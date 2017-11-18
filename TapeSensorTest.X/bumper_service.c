/*
 * File: TemplateService.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a simple service to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "BOARD.h"
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "bumper_service.h"
#include "IO_Ports.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define DEBUG

#define  FRONT_LEFT_BUMPER_PIN PIN3
#define  FRONT_RIGHT_BUMPER_PIN PIN4
#define  BACK_LEFT_BUMPER_PIN PIN5
#define  BACK_RIGHT_BUMPER_PIN PIN6

#define  REN_LEFT_PIN PIN7
#define  REN_CENTER_PIN PIN8
#define  REN_RIGHT_PIN PIN9

#define  SHIFT_AMOUNT 3

#define BUMPER_PORT PORTZ

#define ALL_BUMPER_PINS (FRONT_LEFT_BUMPER_PIN | FRONT_RIGHT_BUMPER_PIN | BACK_LEFT_BUMPER_PIN |  BACK_RIGHT_BUMPER_PIN)


#define TIMER_BUMPER_TICKS 10 //100Hz (More than enough))

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */
uint8_t CheckBumpers(void);
/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/

/* You will need MyPriority and maybe a state variable; you may need others
 * as well. */
typedef union {

    struct {
        unsigned front_left_bumper : 1;
        unsigned front_right_bumper : 1;
        unsigned back_left_bumper : 1;
        unsigned back_right_bumper : 1;
        unsigned ren_left : 1;
        unsigned ren_center : 1;
        unsigned ren_right : 1;
        unsigned bit7 : 1;
        unsigned bit8 : 1;
        unsigned bit9 : 1;
        unsigned bit10 : 1;
        unsigned bit11 : 1;
        unsigned : 4;
    };
    uint16_t value;
} Bumper_Bank_t;

Bumper_Bank_t all_bumpers;

static uint8_t MyPriority;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitBumperService(uint8_t Priority) {
    ES_Event ThisEvent;

    MyPriority = Priority;

    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.

    // post the initial transition event

    ES_Timer_InitTimer(BUMPER_SENSOR_TIMER, TIMER_BUMPER_TICKS);

    //Initialize Bumper
  
    IO_PortsSetPortInputs(BUMPER_PORT,ALL_BUMPER_PINS );
   



    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostBumperService(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunBumperService(ES_Event ThisEvent) {
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    /********************************************
     in here you write your service code
     *******************************************/
    static ES_EventTyp_t lastEvent = BATTERY_DISCONNECTED;
    ES_EventTyp_t curEvent;


    switch (ThisEvent.EventType) {
        case ES_INIT:
            // No hardware initialization or single time setups, those
            // go in the init function above.
            //
            // This section is used to reset service for some reason
            break;

        case ES_TIMEOUT:
            ES_Timer_InitTimer(BUMPER_SENSOR_TIMER, TIMER_BUMPER_TICKS);
           
            
            
            //printf("Status2:%d \r\n", ((IO_PortsReadPort(BUMPER_PORT)) & ALL_BUMPER_PINS) >> SHIFT_AMOUNT);
             all_bumpers.value = ((IO_PortsReadPort(BUMPER_PORT)) & ALL_BUMPER_PINS) >> SHIFT_AMOUNT;
           //   printf("value2=%d\r\n", all_bumpers.value);
              //printf("digital=%d\r\n", IO_PortsReadPort(BUMPER_PORT));
              CheckBumpers();
            break;
    }

    return ReturnEvent;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/
#define MAX_HISTORY_SIZE 5


#ifdef DEBUG

void PostFSMService(ES_Event thisEvent) {
    printf("Event Generated:%d, Param:%d\r\n", thisEvent.EventType, thisEvent.EventParam);
}
#endif

int checkBumper(int* flag, int counter, int param) {
    int returnVal = FALSE;
    if ((*flag == FALSE) && (counter > MAX_HISTORY_SIZE - 2)) {
        *flag = TRUE;
        ES_Event thisEvent;
        thisEvent.EventType = BUMPER_PRESSED;
        //thisEvent.EventParam = param;
        thisEvent.EventParam = all_bumpers.value;
        returnVal = TRUE;
        PostFSMService(thisEvent);

    } else if ((*flag == TRUE) &&(counter < (-MAX_HISTORY_SIZE) + 2)) {
        *flag = FALSE;
        ES_Event thisEvent;
        thisEvent.EventType = BUMPER_RELEASED;
      // thisEvent.EventParam = param;
        thisEvent.EventParam = all_bumpers.value;
        returnVal = TRUE;
        PostFSMService(thisEvent);

    }
    return returnVal;
}
#define BUMPER_TRIPPED 1
#define BUMPER_NOT_TRIPPED 0

#define Front_Right 1
#define Front_Left 2
#define Rear_Right 4
#define Rear_Left 8

uint8_t CheckBumpers(void) {
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    static int Front_Right_Pressed_Flag = FALSE;
    static int Front_Left_Pressed_Flag = FALSE;
    static int Rear_Right_Pressed_Flag = FALSE;
    static int Rear_Left_Pressed_Flag = FALSE;

    static int Front_Right_Pressed_Counter = 0;
    static int Front_Left_Pressed_Counter = 0;
    static int Rear_Right_Pressed_Counter = 0;
    static int Rear_Left_Pressed_Counter = 0;

    if (ReadFrontRightBumper() == BUMPER_TRIPPED) {
        if (Front_Right_Pressed_Counter < MAX_HISTORY_SIZE - 1) {
            Front_Right_Pressed_Counter++;
        }
    } else if (ReadFrontRightBumper() == BUMPER_NOT_TRIPPED) {
        if (Front_Right_Pressed_Counter > (-MAX_HISTORY_SIZE) + 1) {
            Front_Right_Pressed_Counter--;
        }
    }

    if (ReadFrontLeftBumper() == BUMPER_TRIPPED) {
        if (Front_Left_Pressed_Counter < MAX_HISTORY_SIZE - 1) {
            Front_Left_Pressed_Counter++;
        }
    } else if (ReadFrontLeftBumper() == BUMPER_NOT_TRIPPED) {
        if (Front_Left_Pressed_Counter > (-MAX_HISTORY_SIZE) + 1) {
            Front_Left_Pressed_Counter--;
        }
    }

    if (ReadRearRightBumper() == BUMPER_TRIPPED) {
        if (Rear_Right_Pressed_Counter < MAX_HISTORY_SIZE - 1) {
            Rear_Right_Pressed_Counter++;
        }
    } else if (ReadRearRightBumper() == BUMPER_NOT_TRIPPED) {
        if (Rear_Right_Pressed_Counter > (-MAX_HISTORY_SIZE) + 1) {
            Rear_Right_Pressed_Counter--;
        }
    }

    if (ReadRearLeftBumper() == BUMPER_TRIPPED) {
        if (Rear_Left_Pressed_Counter < MAX_HISTORY_SIZE - 1) {
            Rear_Left_Pressed_Counter++;
        }
    } else if (ReadRearLeftBumper() == BUMPER_NOT_TRIPPED) {
        if (Rear_Left_Pressed_Counter > (-MAX_HISTORY_SIZE) + 1) {
            Rear_Left_Pressed_Counter--;
        }
    }


    checkBumper(&Front_Right_Pressed_Flag, Front_Right_Pressed_Counter, Front_Right);
    checkBumper(&Front_Left_Pressed_Flag, Front_Left_Pressed_Counter, Front_Left);
    checkBumper(&Rear_Right_Pressed_Flag, Rear_Right_Pressed_Counter, Rear_Right);
    checkBumper(&Rear_Left_Pressed_Flag, Rear_Left_Pressed_Counter, Rear_Left);



    return (returnVal);
}

int ReadFrontRightBumper() {
    return all_bumpers.front_right_bumper;
}

int ReadFrontLeftBumper() {
    return all_bumpers.front_left_bumper;
}

int ReadRearRightBumper() {
    return all_bumpers.back_right_bumper;
}

int ReadRearLeftBumper() {
    return all_bumpers.back_left_bumper;
}
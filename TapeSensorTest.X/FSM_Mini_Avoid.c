/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "TopHSM.h"
#include "FSM_Mini_Avoid.h"
#include "motors.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define STOP_TIME 100
#define REVERSE_TIME 300
#define TANK_RIGHT_TIME 900
#define TANK_LEFT_TIME 900
#define FORWARDS1_TIME 1500

typedef enum {
    InitPSubState,
    Stop1State,
    ReverseState,
    Stop2State,
    TankRightState,
    Stop3State,
    Forward1State,
    Stop4State,
    TankLeftState,
    Stop5State,



} TemplateSubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"Stop1State",
	"ReverseState",
	"Stop2State",
	"TankRightState",
	"Stop3State",
	"Forward1State",
	"Stop4State",
	"TankLeftState",
	"Stop5State",
};




/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static TemplateSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitFSMMiniAvoid(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunFSMMiniAvoid(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunFSMMiniAvoid(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    TemplateSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                nextState = Stop1State;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case Stop1State: // in the first state, replace this with correct names
            switch (ThisEvent.EventType) {


                case ES_ENTRY:
                    ES_Timer_InitTimer(MINI_AVOID_TIMER, STOP_TIME);
                    stop();
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MINI_AVOID_TIMER) {
                        nextState = ReverseState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMERACTIVE:
                case ES_TIMERSTOPPED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;


                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;


        case ReverseState:
            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(MINI_AVOID_TIMER, REVERSE_TIME);
                    reverse();
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;


                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MINI_AVOID_TIMER) {
                        nextState = Stop2State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMERACTIVE:
                case ES_TIMERSTOPPED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }

            break;

        case Stop2State:
            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(MINI_AVOID_TIMER, STOP_TIME);
                    stop();
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;


                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MINI_AVOID_TIMER) {
                        nextState = TankRightState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMERACTIVE:
                case ES_TIMERSTOPPED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }
            break;
        case TankRightState:

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(MINI_AVOID_TIMER, TANK_RIGHT_TIME);
                    tank_turn_right();
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;


                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MINI_AVOID_TIMER) {
                        nextState = Stop3State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMERACTIVE:
                case ES_TIMERSTOPPED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }


            break;
        case Stop3State:

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(MINI_AVOID_TIMER, STOP_TIME);
                    stop();
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MINI_AVOID_TIMER) {
                        nextState = Forward1State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMERACTIVE:
                case ES_TIMERSTOPPED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }


            break;
        case Forward1State:

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(MINI_AVOID_TIMER, FORWARDS1_TIME);
                    forwards();
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case TAPE_DETECTED:
                    switch (ThisEvent.EventParam) {
                        case RIGHT_TAPE_SENSOR:
                        case FRONT_TAPE_SENSOR:
                        case LEFT_TAPE_SENSOR:
                            ThisEvent.EventType = OBSTACLE_AVOIDED;
                            break;
                    }
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MINI_AVOID_TIMER) {
                        nextState = Stop4State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMERACTIVE:
                case ES_TIMERSTOPPED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }


            break;
        case Stop4State:

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(MINI_AVOID_TIMER, STOP_TIME);
                    stop();
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MINI_AVOID_TIMER) {
                        nextState = TankLeftState;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMERACTIVE:
                case ES_TIMERSTOPPED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }



            break;
        case TankLeftState:

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(MINI_AVOID_TIMER, TANK_LEFT_TIME);
                    tank_turn_left();
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;


                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MINI_AVOID_TIMER) {
                        nextState = Stop5State;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMERACTIVE:
                case ES_TIMERSTOPPED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }

            break;
        case Stop5State:

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(MINI_AVOID_TIMER, STOP_TIME);
                    stop();
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MINI_AVOID_TIMER) {
                        ThisEvent.EventType = OBSTACLE_AVOIDED;
                    }
                    break;

                case ES_TIMERACTIVE:
                case ES_TIMERSTOPPED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }

            break;


        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunFSMMiniAvoid(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunFSMMiniAvoid(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

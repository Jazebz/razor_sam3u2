/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE


CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */
bool buttonOnePressed = FALSE;
bool buttonTwoPressed = FALSE;
int playerX = 64;
int playerY = 48;
int velocity = 0;
bool isJumping = FALSE;
const int gravity = 1;         // Gravity force (pulls down)
const int jumpStrength = -8;   // Initial jump force (negative = moves up)
const int playerHeight = 12;
const int constGround = 60 - playerHeight;
int groundLevel = constGround; 
int health = 4;


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */
extern bool isMainScreen;


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  LedOn(RED0);
  LedOff(GREEN0);
  LedOff(BLUE0);
  LedOn(RED1);
  LedOff(GREEN1);
  LedOff(BLUE1);
  LedOn(RED2);
  LedOff(GREEN2);
  LedOff(BLUE2);
  LedOn(RED3);
  LedOff(GREEN3);
  LedOff(BLUE3);
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp1SM_Idle(void)
{
  static u16 tickSpeed = 0;
  static u16 moveTick = 0;
  const u16 jumpFrame = 75;
  buttonOnePressed = IsButtonPressed(BUTTON0);
  buttonTwoPressed = IsButtonPressed(BUTTON1);
  if (playerX >= 100) {
    groundLevel = 150;
  }
  else {
    groundLevel = constGround;
  }

  if (moveTick >= jumpFrame) {
    moveTick = 0;
    if (WasButtonPressed(BUTTON1) && !isJumping) {
        velocity = jumpStrength;   // Apply jump force (moves up)
        isJumping = TRUE;
    }

    // Apply gravity while in the air
    if (isJumping) {
        velocity += gravity;  // Increase downward speed
        playerY += velocity;  // Update player Y position

        // Stop falling when hitting the platform
        if (playerY >= groundLevel) {
            playerY = groundLevel;  // Reset to platform level
            velocity = 0;           // Stop movement
            isJumping = FALSE;
            ButtonAcknowledge(BUTTON1);
        }
    }
    if (buttonOnePressed) {
      if (playerX == 128){
        playerX = 0;
      }
      else {
        playerX += 2;
      }
    }
    velocity += gravity;  // Apply downward force
    playerY += velocity;
    if (playerY >= 75) {
      playerX = 48;     // Respawn at X = 0
      playerY = 64;     // Respawn at Y = 0
      health--;  // Reduce health
      isMainScreen = TRUE;
      buttonOnePressed = TRUE;
      if (health == 4) {
        LedOn(RED0);
        LedOn(RED1);
        LedOn(RED2);
        LedOn(RED3);
      }
      else if (health == 3) {
        LedOn(RED0);
        LedOn(RED1);
        LedOn(RED2);
        LedOff(RED3);
      }
      else if (health == 2) {
        LedOn(RED0);
        LedOn(RED1);
        LedOff(RED2);
        LedOff(RED3);
      }
      else if (health == 1) {
        LedOn(RED0);
        LedOff(RED1);
        LedOff(RED2);
        LedOff(RED3);
      }
      else {
        health == 4;
        LedOn(RED0);
        LedOn(RED1);
        LedOn(RED2);
        LedOn(RED3);
      }
    }
    if (playerY >= groundLevel) {
      playerY = groundLevel;
      velocity = 0;
      isJumping = FALSE;
    }
    if (health == 1) {
      LedToggle(RED0);
    }


    
  }
  
  moveTick ++;
  //tickSpeed ++;


} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

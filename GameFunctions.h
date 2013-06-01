#include "GameData.h"
#ifndef __GameFunctions_H
#define __GameFunctions_H

/*******************************
Defines
*******************************/
#define ROTATION_SPEED F_NUM_UP(6)
#define FLYING_SPEED F_NUM_UP(50)
/*******************************
Variables
*******************************/

//Game camera
camera cam;

//controls
u16 buttons;

/*******************************
Functions
*******************************/
/*********************************************************
System control functions
*********************************************************/
void screenControl();
void handleInput();
void vbInit();
void timeHnd(void);
/********************************************************/

/*********************************************************
Object functions
*********************************************************/
void initObjects();
void setObjectRelative(object* o, object* parent);
void setObjectRelativeCamera(object* o);
void moveObject(object* o);
/********************************************************/


#endif

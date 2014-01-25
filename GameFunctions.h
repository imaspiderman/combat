#include "GameData.h"
#ifndef __GameFunctions_H
#define __GameFunctions_H
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
void doMoveEnemy(object* o);
void doGroundEffects();
/********************************************************/


#endif

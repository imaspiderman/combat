#include "GameData.h"
#include "GameTypes.h"

#ifndef __GameFunctions_H
#define __GameFunctions_H
/*******************************
Fixed point math defines
scale factor of 128
*******************************/
#define F_NUM_UP(X) ((X)<<3)
#define F_NUM_DN(X) (((X)>>3) + ((X>>2)&0x01)) //Rounding included
#define F_MUL(X,Y) (((X)*(Y))>>3)
#define F_ADD(X,Y) ((X)+(Y))
#define F_SUB(X,Y) ((X)-(Y))
#define F_DIV(X,Y) (F_NUM_UP(X)/(Y))
#define F_COSINE(X) (cosine[X])
#define F_SINE(X) (sine[X])
/*******************************
Defines
*******************************/
#define SCREEN_HEIGHT 224
#define SCREEN_WIDTH 384
#define FAR_Z F_NUM_UP(4096)
#define _CacheEnable asm("mov 2,r15 \n ldsr r15,sr24":::"r15");
#define _CacheDisable asm("ldsr r0,sr24");
#define PARALLAX_MAX 20
#define PARALLAX_SHIFT 8
#define ROTATION_SPEED F_NUM_UP(6)
#define FLYING_SPEED F_NUM_UP(50)
/*******************************
Variables
*******************************/

u32* currentFrameBuffer=(u32*)0x00008000;
u32* nextFrameBuffer=(u32*)0x00000000;

//Game camera
camera cam;

//controls
u16 buttons;

/*******************************
Functions
*******************************/

/********************************************************
Core Drawing Functions
*********************************************************/
void inline drawPoint(s32 x, s32 y, u8 color, s32 p);
void drawLine(vector3d* v1, vector3d* v2, u8 color, object* o);
void drawObject(object* o);
void renderVector3d(object* obj, vector3d* v, vector3d* o);
/********************************************************/

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
void inline initObject(object* o);
void setObjectRelative(object* o, object* parent);
void setObjectRelativeCamera(object* o);
void moveObject(object* o);
/********************************************************/


#endif

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

#define MAX_GAME_OBJECTS 128
object gameObjects[MAX_GAME_OBJECTS];
u8 gameObjectsIdx;

#define MAX_LASERS 5
object laserObjects[MAX_LASERS];

//controls
u8 dPadPressed = 0;
u16 buttons;

//random number
u16 randomNumber = 0;

/***************************************************
Matrix Definitions I'm using a left handed system
which is what Direct3D uses.
http://msdn.microsoft.com/en-us/library/windows/desktop/bb206269%28v=vs.85%29.aspx
***************************************************/
matrix3d m_translate3d;
matrix3d m_scale3d;
matrix3d m_rotate3d_x;
matrix3d m_rotate3d_y;
matrix3d m_rotate3d_z;
matrix3d m_camera3d;
matrix3d m_world3d;
matrix3d m_project3d;

/*******************************
Functions
*******************************/

/********************************************************
Core Drawing Functions
*********************************************************/
void inline drawPoint(s32 x, s32 y, u8 color, s32 p);
void drawLine(vector3d* v1, vector3d* v2, u8 color, object* o);
void drawObject(object* o);
/********************************************************/

/*********************************************************
3d Calculations/Functions
*********************************************************/
s32 dotProduct(vector3d* v1, vector3d* v2);
s32 isqrt(s32 num);
void crossProduct(vector3d* v1, vector3d* v2, vector3d* n);
void matrix3dxVertex(vector3d* v,matrix3d m,vector3d* o);
void matrix3dxMatrix3d(matrix3d m1, matrix3d m2, matrix3d n);
void rotateMatrixX(matrix3d m, s32 angle);
void rotateMatrixY(matrix3d m, s32 angle);
void rotateMatrixZ(matrix3d m, s32 angle);
void translateMatrix(matrix3d m, s32 x, s32 y, s32 z);
void translateCameraMatrix(matrix3d m, object* target);
void scaleMatrix(matrix3d m,s32 sx, s32 sy, s32 sz);
void worldMatrix(matrix3d m, object* o, s32 sx, s32 sy, s32 sz);
void copyMatrix(matrix3d m1, matrix3d m2);
void normalizeVector(vector3d* v, vector3d* n);
void subtractVector(vector3d* vStart, vector3d* vEnd, vector3d* n);
void addVector(vector3d* v1, vector3d* v2, vector3d* n);
void multiplyVector(vector3d* v1, vector3d* v2, vector3d* n);
u8 isEqualVector(vector3d* v1, vector3d* v2);
void projectionMatrix(matrix3d m);
/********************************************************/

/*********************************************************
System control functions
*********************************************************/
void screenControl();
void handleInput();
void vbInit();
/********************************************************/

/*********************************************************
Game functions
*********************************************************/
void initObjects();
void doShot(object* o);
void inline initObject(object* o);
void moveObject(object* o);
u8 inline isMoving(object* o);
void visualEffects(object* o);
void DoSound(u16 timerCount);
void timeHnd(void);
void initMusic();
void setObjectRelative(object* o, object* parent);
void setObjectRelativeCamera(object* o);
u8 shotTest(object* o);
u8 detectCollision(object* o1, object* o2);
void setCollisionCube(object* o, vector3d* v);
/********************************************************/


#endif

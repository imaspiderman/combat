#ifndef __G3d_H
#define __G3d_H
/*******************************
Fixed point math defines
*******************************/
#define FIXED_SHIFT 3
#define F_NUM_UP(X) ((X)<<FIXED_SHIFT)
#define F_NUM_DN(X) (((X)>>FIXED_SHIFT) + ((X>>(FIXED_SHIFT-1))&0x01)) //Rounding included
#define F_MUL(X,Y) F_NUM_DN(((X)*(Y)))
#define F_ADD(X,Y) ((X)+(Y))
#define F_SUB(X,Y) ((X)-(Y))
#define F_DIV(X,Y) (F_NUM_UP(X)/(Y))
//#define F_DIV(X,Y) ((X)/F_NUM_DN(Y))
#define FIXED_SHIFT_PRECISION 8
#define F_PRECISION_UP(X) ((X)<<FIXED_SHIFT_PRECISION)
#define F_PRECISION_DN(X) (((X)>>FIXED_SHIFT_PRECISION) + ((X>>(FIXED_SHIFT_PRECISION-1))&0x01)) //Rounding included
#define F_PRECISION_MUL(X,Y) F_PRECISION_DN(((X)*(Y)))
#define F_PRECISION_DIV(X,Y) (F_PRECISION_UP(X)/(Y))

//Definitions
#define SCREEN_HEIGHT 224
#define SCREEN_WIDTH 384
#define FAR_Z 100000
#define _CacheEnable asm("mov 2,r15 \n ldsr r15,sr24":::"r15");
#define _CacheDisable asm("ldsr r0,sr24");
#define PARALLAX_MAX 20
#define PARALLAX_SHIFT 8


/**********************
Type definitions used in
the game functions
**********************/
typedef struct{
	s32 x; //fixed point
	s32 y; //fixed point
	s32 z; //fixed point
	s32 sx; //screen x
	s32 sy; //screen y
} vector3d;//represents a vector or coordinate.

typedef struct{
	s32 vertexSize;//Size of vertex array portion
	s32 lineSize;//Size of line data portion
	s32 faceSize;//Number of points per "face". Since we use only lines for wireframe this should always be 2
	const s32 data[];//Distinct vertices and line point index data
} objectData;

typedef struct{
	u8 reset;
	s32 minX;
	s32 minY;
	s32 minZ;
	s32 maxX;
	s32 maxY;
	s32 maxZ;
}collisionCube; //This can be used for collision detection

typedef struct{
	u8 visible; //Is this object visible
	u8 detectCollision; //Do we perform collision detection
	u8 lineColor;
	collisionCube hitCube; //Cube data for collision detection
} objectProperties;

typedef struct object{
	objectProperties properties;
	struct object* parent;//Used to chain objects together
	vector3d worldPosition;//Actual position inside the world
	vector3d moveTo;//worldPosition to move the object to
	vector3d worldRotation;//Not Fixed Point (x,y,z) rotation in degrees
	vector3d rotation;//Not Fixed Point (x,y,z) incremental rotation in degrees
	vector3d worldSpeed;//Vector to move the object
	vector3d speed;//Vector for increasing or decreasing velocity
	vector3d worldScale;//Scale factor of object
	vector3d scale;//Incremental Scale factor to apply each frame
	objectData* objData;
} object;

typedef struct{
	vector3d worldPosition;
	vector3d worldSpeed;
	vector3d speed;
	vector3d moveTo;
	vector3d target;
	vector3d worldRotation;//Not Fixed Point
	vector3d rotation;//Not Fixed Point
	s32 d;
} camera;

//Vector memory area
vector3d vertexBuffer[1000];
u32 numVertices=0;

//Frame Buffers
u32* currentFrameBuffer=(u32*)0x00008000;
u32* nextFrameBuffer=(u32*)0x00000000;

/*********************
Cosine and Sine tables in degrees
multiplied by a factor of 8
Example to multiply 25 by cos(8) (degrees)
Take (25*cosine[8])>>3.
*********************/

const s32 cosine[360]={
	8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,
	8,8,8,8,8,7,7,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,6,6,6,6,
	6,6,6,6,6,6,6,5,
	5,5,5,5,5,5,5,5,
	4,4,4,4,4,4,4,4,
	4,3,3,3,3,3,3,3,
	2,2,2,2,2,2,2,2,
	1,1,1,1,1,1,1,0,
	0,0,0,-0,-0,-0,-1,-1,
	-1,-1,-1,-1,-1,-2,-2,-2,
	-2,-2,-2,-2,-2,-3,-3,-3,
	-3,-3,-3,-3,-4,-4,-4,-4,
	-4,-4,-4,-4,-4,-5,-5,-5,
	-5,-5,-5,-5,-5,-5,-6,-6,
	-6,-6,-6,-6,-6,-6,-6,-6,
	-6,-7,-7,-7,-7,-7,-7,-7,
	-7,-7,-7,-7,-7,-7,-7,-7,
	-8,-8,-8,-8,-8,-8,-8,-8,
	-8,-8,-8,-8,-8,-8,-8,-8,
	-8,-8,-8,-8,-8,-8,-8,-8,
	-8,-8,-8,-8,-8,-8,-8,-8,
	-8,-8,-8,-8,-8,-8,-8,-8,
	-8,-7,-7,-7,-7,-7,-7,-7,
	-7,-7,-7,-7,-7,-7,-7,-7,
	-6,-6,-6,-6,-6,-6,-6,-6,
	-6,-6,-6,-5,-5,-5,-5,-5,
	-5,-5,-5,-5,-4,-4,-4,-4,
	-4,-4,-4,-4,-4,-3,-3,-3,
	-3,-3,-3,-3,-2,-2,-2,-2,
	-2,-2,-2,-2,-1,-1,-1,-1,
	-1,-1,-1,-0,-0,-0,-0,0,
	0,0,1,1,1,1,1,1,
	1,2,2,2,2,2,2,2,
	2,3,3,3,3,3,3,3,
	4,4,4,4,4,4,4,4,
	4,5,5,5,5,5,5,5,
	5,5,6,6,6,6,6,6,
	6,6,6,6,6,7,7,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,8,8,8,8,
	8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8
};
const s32 sine[360]={
	0,0,0,0,1,1,1,1,
	1,1,1,2,2,2,2,2,
	2,2,2,3,3,3,3,3,
	3,3,4,4,4,4,4,4,
	4,4,4,5,5,5,5,5,
	5,5,5,5,6,6,6,6,
	6,6,6,6,6,6,6,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,8,8,
	8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,7,
	7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,6,6,
	6,6,6,6,6,6,6,6,
	6,5,5,5,5,5,5,5,
	5,5,4,4,4,4,4,4,
	4,4,4,3,3,3,3,3,
	3,3,2,2,2,2,2,2,
	2,2,1,1,1,1,1,1,
	1,0,0,0,0,-0,-0,-0,
	-1,-1,-1,-1,-1,-1,-1,-2,
	-2,-2,-2,-2,-2,-2,-2,-3,
	-3,-3,-3,-3,-3,-3,-4,-4,
	-4,-4,-4,-4,-4,-4,-4,-5,
	-5,-5,-5,-5,-5,-5,-5,-5,
	-6,-6,-6,-6,-6,-6,-6,-6,
	-6,-6,-6,-7,-7,-7,-7,-7,
	-7,-7,-7,-7,-7,-7,-7,-7,
	-7,-7,-8,-8,-8,-8,-8,-8,
	-8,-8,-8,-8,-8,-8,-8,-8,
	-8,-8,-8,-8,-8,-8,-8,-8,
	-8,-8,-8,-8,-8,-8,-8,-8,
	-8,-8,-8,-8,-8,-8,-8,-8,
	-8,-8,-8,-7,-7,-7,-7,-7,
	-7,-7,-7,-7,-7,-7,-7,-7,
	-7,-7,-6,-6,-6,-6,-6,-6,
	-6,-6,-6,-6,-6,-5,-5,-5,
	-5,-5,-5,-5,-5,-5,-4,-4,
	-4,-4,-4,-4,-4,-4,-4,-3,
	-3,-3,-3,-3,-3,-3,-2,-2,
	-2,-2,-2,-2,-2,-2,-1,-1,
	-1,-1,-1,-1,-1,-0,-0,-0
};

/*********************************************************
3d Calculations/Functions
*********************************************************/
void inline g3d_copyVector3d(vector3d* from, vector3d* to);
void inline g3d_scale(vector3d* factor, vector3d* v, vector3d* o);
void inline g3d_rotateXAxis(s32 degrees, vector3d* v, vector3d* o);
void inline g3d_rotateYAxis(s32 degrees, vector3d* v, vector3d* o);
void inline g3d_rotateZAxis(s32 degrees, vector3d* v, vector3d* o);
void inline g3d_rotateAllAxis(s32 rx, s32 ry, s32 rz, vector3d* v, vector3d* o);
void inline g3d_translate(s32 x, s32 y, s32 z, vector3d* v, vector3d* o);
void inline g3d_cameraRotateAllAxis(s32 rx, s32 ry, s32 rz, vector3d* v, vector3d* o);
void inline g3d_cameraTranslate(s32 x, s32 y, s32 z, vector3d* v, vector3d* o);
void inline g3d_initObject(object* o);
void inline g3d_moveObject(object* o);
void inline g3d_moveCamera(camera* c);
void inline g3d_calculateProjection(vector3d* o);
void inline g3d_clipZAxis(vector3d* v1, vector3d* v2);
void inline g3d_clipObject(object* o);
/********************************************************/
/********************************************************
Core Drawing Functions
*********************************************************/
void inline g3d_drawPoint(s32 x, s32 y, u8 color, s32 p);
void g3d_drawLine(vector3d* v1, vector3d* v2, u8 color);
void g3d_drawObject(object* o);
void g3d_renderVector3d(object* obj, vector3d* v, vector3d* o, u8 initHitCube);
/********************************************************/

#endif
#include <libgccvb.h>
#ifndef __GameTypes_H
#define __GameTypes_H

/**********************
Type definitions used in
the game functions
**********************/
typedef struct{
	s32 x; //fixed point
	s32 y; //fixed point
	s32 z; //fixed point
	s32 w; //fixed point
} vector3d;//represents a vector or coordinate.

typedef struct{
	s32 size;
	s32 faceSize;
	const s32 data[];
} objectData;

typedef struct{
	u8 reset;
	s32 minX;
	s32 minY;
	s32 minZ;
	s32 maxX;
	s32 maxY;
	s32 maxZ;
}collisionCube;

typedef struct{
	u8 visible;
	u8 detectCollision;
	collisionCube hitCube;
} objectProperties;

typedef struct object{
	objectProperties properties;
	struct object* parent;
	vector3d worldPosition;
	vector3d position;
	vector3d moveTo;
	vector3d worldRotation;
	vector3d rotation;
	vector3d worldRotateSpeed;
	vector3d rotateSpeed;
	vector3d worldSpeed;
	vector3d speed;
	vector3d worldScale;
	vector3d scale;
	objectData* objData;
} object;

typedef struct{
	vector3d worldPosition;
	vector3d position;
	vector3d target;
	vector3d worldRotation;
	vector3d rotation;
	s32 d;
} camera;
#endif

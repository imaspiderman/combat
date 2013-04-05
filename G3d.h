#include "GameData.h"
#include "GameTypes.h"
#ifndef __G3d_H
#define __G3d_H
/*********************************************************
3d Calculations/Functions
*********************************************************/
void inline g3d_copyVector3d(vector3d* from, vector3d* to);
void inline g3d_rotateXAxis(s32 degrees, vector3d* v, vector3d* o);
void inline g3d_rotateYAxis(s32 degrees, vector3d* v, vector3d* o);
void inline g3d_rotateZAxis(s32 degrees, vector3d* v, vector3d* o);
void inline g3d_rotateAllAxis(s32 rx, s32 ry, s32 rz, vector3d* v, vector3d* o);
void inline g3d_translate(s32 x, s32 y, s32 z, vector3d* v, vector3d* o);
void inline g3d_cameraRotateAllAxis(s32 rx, s32 ry, s32 rz, vector3d* v, vector3d* o);
void inline g3d_cameraTranslate(s32 x, s32 y, s32 z, vector3d* v, vector3d* o);
/********************************************************/
#endif
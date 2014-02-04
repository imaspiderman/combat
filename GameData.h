#include "GameFunctions.h"
#ifndef __GameData_H
#define __GameData_H

const s32 Letter_S[] = {18, //total number of vertices
	10, //total number of line end points
	2,
	//Distinct vertices
	800,800,0,-800,800,0,-800,0,0,800,0,0
	,800,-800,0,-800,-800,0
	//Vertex indexes
	,0,1,1,2,2,3,3,4,4,5};

const s32 Letter_T[] = {12, //total number of vertices
	4, //total number of line end points
	2,
	//Distinct vertices
	-800,800,0,800,800,0,0,800,0,0,-800,0
	
	//Vertex indexes
	,0,1,2,3};

const s32 Letter_A[] = {15, //total number of vertices
	6, //total number of line end points
	2,
	//Distinct vertices
	-800,-800,0,0,800,0,800,-800,0,-400,0,0
	,400,0,0
	//Vertex indexes
	,0,1,1,2,3,4};
	
const s32 Letter_R[] = {21, //total number of vertices
	10, //total number of line end points
	2,
	//Distinct vertices
	-800,800,0,-800,-800,0,800,800,0,800,0,0
	,-800,0,0,400,0,0,800,-800,0
	//Vertex indexes
	,0,1,0,2,2,3,3,4,5,6};

const s32 Letter_F[] = {15, //total number of vertices
	6, //total number of line end points
	2,
	//Distinct vertices
	-800,800,0,-800,-800,0,-800,400,0,400,800,0
	,400,400,0
	//Vertex indexes
	,0,1,0,3,2,4};
	
const s32 Letter_O[] = {12, //total number of vertices
	8, //total number of line end points
	2,
	//Distinct vertices
	-800,800,0,800,800,0,800,-800,0,-800,-800,0
	
	//Vertex indexes
	,0,1,1,2,2,3,3,0};
	
const s32 Letter_X[] = {12, //total number of vertices
	4, //total number of line end points
	2,
	//Distinct vertices
	-800,800,0,800,-800,0,-800,-800,0,800,800,0
	
	//Vertex indexes
	,0,1,2,3};

const u8 Level1[] = {
	(ROW_T_ENEMY    | 0x02),  0x88,0x80,0x00, 
	                          0x77,0x81,0x00,
	(ROW_T_OBSTACLE | 0x02),  0x20,0xF0,0x00, 
	                          0x80,0xF1,0x00,
	(ROW_T_OBSTACLE | 0x04),  0x50,0xF1,0x00, 
	                          0x80,0xA1,0x00,
							  0xA0,0x81,0x00,
							  0xC0,0xC1,0x00,
	(ROW_T_OBSTACLE | 0x04),  0xC0,0xA1,0x00, 
	                          0xA0,0xC1,0x00,
							  0x80,0x81,0x00,
							  0x50,0x51,0x00,
	(ROW_T_OBSTACLE | 0x04),  0x80,0x51,0x00, 
	                          0xA0,0xA1,0x00,
							  0xC0,0xC1,0x00,
							  0x50,0x81,0x00,
	(ROW_T_OBSTACLE | 0x04),  0x50,0x51,0x00, 
	                          0xC0,0x81,0x00,
							  0xA0,0xA1,0x00,
							  0x80,0xC1,0x00,
	(ROW_T_OBSTACLE | 0x04),  0x10,0xF1,0x00, 
	                          0x80,0xA1,0x00,
							  0xA0,0xF1,0x00,
							  0xF0,0xA1,0x00,
	(ROW_T_OBSTACLE | 0x04),  0x10,0xF1,0x00, 
	                          0xA0,0xA1,0x00,
							  0xF0,0xF1,0x00,
							  0x40,0xA1,0x00,
	0x00 //End byte
};

const s32 Fighter[] = {48, //total number of vertices
	46, //total number of line end points
	2,
	//Distinct vertices
	-800,0,0,0,0,2400,800,0,0,0,0,-800
	,0,1200,0,0,0,-800,-800,0,-800,-800,0,1200
	,-1200,2000,-1200,-1200,0,1200,-3200,-800,-2000,800,0,-800
	,800,0,1200,1200,2000,-1200,1200,0,1200,3200,-800,-2000
	
	//Vertex indexes
	,0,1,1,2,2,3,3,0,1,4,4,0,2,4,1,5
	,5,0,5,2,0,2,6,7,7,8,8,6,0,9,9,10
	,10,0,11,12,12,13,13,11,2,14,14,15,15,2};

const s32 Tree[] = {39, //total number of vertices
	32, //total number of line end points
	2,
	//Distinct vertices
	-400,0,-400,-400,0,400,400,0,400,400,0,-400
	,-400,1200,-400,-400,1200,400,400,1200,400,400,1200,-400
	,-1200,1200,-1200,-1200,1200,1200,1200,1200,1200,1200,1200,-1200
	,0,2800,0
	//Vertex indexes
	,0,1,1,2,2,3,3,0,0,4,1,5,2,6,3,7
	,8,9,9,10,10,11,11,8,8,12,9,12,10,12,11,12
	};
	
const s32 Phantron[] = {66, //total number of vertices
	78, //total number of line end points
	2,
	//Distinct vertices
	800,0,800,-800,0,800,0,0,-1200,0,800,800
	,800,0,1200,1200,1600,0,800,0,-1200,-800,0,1200
	,-1200,1600,0,-800,0,-1200,1600,-1200,400,2400,-2000,1600
	,800,-2000,1600,800,-3200,1600,2400,-3200,1600,1600,-3600,2800
	,-1600,-1200,400,-2400,-2000,1600,-800,-2000,1600,-800,-3200,1600
	,-2400,-3200,1600,-1600,-3600,2800
	//Vertex indexes
	,0,1,1,2,2,0,1,3,3,0,3,2,4,5,5,6
	,6,4,7,8,8,9,9,7,10,11,11,12,12,10,12,13
	,13,10,13,14,14,10,14,11,15,11,12,15,13,15,14,15
	,10,0,16,17,17,18,18,16,18,19,19,16,19,20,20,16
	,20,21,21,16,21,17,18,21,19,21,21,21,16,1};

const s32 Dart[] = {48, //total number of vertices
	50, //total number of line end points
	2,
	//Distinct vertices
	0,0,2400,400,800,1200,-400,800,1200,400,-800,1200
	,-400,-800,1200,400,800,-1200,-400,800,-1200,400,-800,-1200
	,-400,-800,-1200,-400,-800,0,-2000,-1600,-2400,400,-800,0
	,2000,-1600,-2400,0,800,0,0,2400,-2400,0,800,-1200
	
	//Vertex indexes
	,0,1,1,2,2,0,1,3,3,0,2,4,4,0,3,4
	,5,6,6,8,8,7,7,5,5,1,6,2,7,3,8,4
	,9,10,10,8,8,9,11,12,12,7,7,11,13,14,14,15
	,15,13};
	
const s32 GroundEffect[] = {6, //total number of vertices
	2, //total number of line end points
	2,
	//Distinct vertices
	0,0,2000,0,0,-2000
	//Vertex indexes
	,0,1};

const s32 Building[] = {24, //total number of vertices
	24, //total number of line end points
	2,
	//Distinct vertices
	-2000,0,2000,2000,0,2000,2000,0,-2000,-2000,0,-2000
	,-2000,4000,2000,2000,4000,2000,2000,4000,-2000,-2000,4000,-2000
	
	//Vertex indexes
	,0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4
	,0,4,1,5,2,6,3,7};
	
const s32 Laser[] = {12, //total number of vertices
	12, //total number of line end points
	2,
	//Distinct vertices
	-400,400,0,-400,-400,0,400,-400,0,400,400,0
	
	//Vertex indexes
	,0,1,1,2,2,3,3,0,0,2,1,3};
	
const s32 HealthRing[] = {36, //total number of vertices
	24, //total number of line end points
	2,
	//Distinct vertices
	-400,2400,0,400,2400,0,0,2000,0,-2400,400,0
	,-2400,-400,0,-2000,0,0,-400,-2400,0,400,-2400,0
	,0,-2000,0,2400,400,0,2400,-400,0,2000,0,0
	
	//Vertex indexes
	,0,1,1,2,2,0,3,4,4,5,5,3,6,7,7,8
	,8,6,9,10,10,11,11,9};

objectData* EnemyObjectDataTable[] = {
	(objectData*)Phantron,
	(objectData*)Dart
};

objectData* StaticObjectDataTable[] = {
	(objectData*)Tree,
	(objectData*)Building
};
#endif

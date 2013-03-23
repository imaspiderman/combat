#include "GameFunctions.h"
#include "GameData.h"
#include "GameTypes.h"
#include "Music.h"
#include "Font.h"

//Music stuff
#define MAX_NOTE_LENGTH 1600
midiTrack t1 = {0,(midiNote*)track_0,1,0,0xFF};
#define NUM_TRACKS 1
midiTrack trackTable[NUM_TRACKS];

char debugX[]    = {"x:    "};
char debugY[]    = {"y:    "};
char debugZ[]    = {"z:    "};
char debugMinX[] = {"min x:"};
char debugMaxX[] = {"max x:"};
char debugMinY[] = {"min y:"};
char debugMaxY[] = {"max y:"};
char debugMinZ[] = {"min z:"};
char debugMaxZ[] = {"max z:"};
char debugHit[] = {"hit occured"};

//Easy reference
object* crossH; //Cross hairs

void DoMusic(u16 timerCount){
	u8 i;
	for(i=0; i<NUM_TRACKS; i++){
		trackTable[i].currNote = 0;
	}
	musicTick = 0;
	timer_freq(1);
	timer_set(timerCount);
	timer_enable(1);
	timer_int(1);
}

void timeHnd(void){
	u8 tr,doTimer;
	timer_int(0);
	timer_enable(0);
	timer_clearstat();
	
	for(tr=0; tr<NUM_TRACKS; tr++){
		if(trackTable[tr].play == 0) continue;
		if(trackTable[tr].data[trackTable[tr].currNote].tickPosition == musicTick && 
		   trackTable[tr].data[trackTable[tr].currNote].onoff == 0x09){//Start note
		    while(trackTable[tr].data[trackTable[tr].currNote+1].tickPosition == musicTick && //Look for the last note in a cord and play that one.
			      trackTable[tr].data[trackTable[tr].currNote+1].onoff == 0x09) trackTable[tr].currNote++;
			SND_REGS[tr].SxFQH=(FREQUENCY[trackTable[tr].data[trackTable[tr].currNote].note]>>8) & 0xFF;
			SND_REGS[tr].SxFQL=FREQUENCY[trackTable[tr].data[trackTable[tr].currNote].note] & 0xFF;
			SND_REGS[tr].SxLRV=trackTable[tr].volume;
			SND_REGS[tr].SxRAM = tr;
			trackTable[tr].offNote = 0;
			SND_REGS[tr].SxINT = 0x9F;
		}
		/*some midi's don't have off notes so this sets a max length for a given note*/
		trackTable[tr].offNote++;
		if(trackTable[tr].offNote > MAX_NOTE_LENGTH){
			SND_REGS[tr].SxINT = 0x00;
		}

		while(trackTable[tr].data[trackTable[tr].currNote].tickPosition <= musicTick && 
		      trackTable[tr].data[trackTable[tr].currNote].tickPosition != 0xFFFFFFFF){
			trackTable[tr].currNote++;
		}
		
		if(trackTable[tr].data[trackTable[tr].currNote].tickPosition == 0xFFFFFFFF){
			trackTable[tr].play = 0;
			trackTable[tr].currNote = 0;
			SND_REGS[tr].SxINT=0;
		}
	}
	musicTick++;
	doTimer = 0x00;
	for(tr=0; tr<NUM_TRACKS; tr++){
		if(trackTable[tr].play == 1) doTimer = 0x01;
	}
	if(doTimer){
		timer_int(1);
		timer_enable(1);
	}
}

void intro(){
	object S,T,A,R,W,A2,R2,S2;
	s32 xspeed,yspeed,zspeed;
	
	xspeed = F_NUM_UP(10);
	yspeed = F_NUM_UP(10);
	zspeed = F_NUM_UP(10);
	
	initObject(&S);
	initObject(&T);
	initObject(&A);
	initObject(&R);
	initObject(&W);
	initObject(&A2);
	initObject(&R2);
	initObject(&S2);
	
	S.objData = (objectData*)logoLetter_S;
	S.worldPosition.z = F_NUM_UP(3000);
	S.worldSpeed.x = xspeed;
	S.worldSpeed.z = zspeed;
	S.moveTo.z = F_NUM_UP(20);
	S.moveTo.x = F_NUM_UP(-120);
	S.worldScale.x = F_NUM_UP(2);
	S.worldScale.y = F_NUM_UP(2);
	S.worldScale.z = F_NUM_UP(2);
	
	T.objData = (objectData*)logoLetter_T;
	T.worldPosition.z = F_NUM_UP(3000);
	T.worldSpeed.x = xspeed;
	T.worldSpeed.z = zspeed;
	T.moveTo.z = F_NUM_UP(20);
	T.moveTo.x = F_NUM_UP(-120);
	T.worldScale.x = F_NUM_UP(2);
	T.worldScale.y = F_NUM_UP(2);
	T.worldScale.z = F_NUM_UP(2);
	
	A.objData = (objectData*)logoLetter_A;
	A.worldPosition.z = F_NUM_UP(3000);
	A.worldSpeed.z = zspeed;
	A.moveTo.z = F_NUM_UP(20);
	A.worldScale.x = F_NUM_UP(2);
	A.worldScale.y = F_NUM_UP(2);
	A.worldScale.z = F_NUM_UP(2);
	
	R.objData = (objectData*)logoLetter_R;
	R.worldPosition.z = F_NUM_UP(3000);
	R.worldSpeed.z = zspeed;
	R.worldSpeed.x = xspeed;
	R.moveTo.z = F_NUM_UP(20);
	R.moveTo.x = F_NUM_UP(85);
	R.worldScale.x = F_NUM_UP(2);
	R.worldScale.y = F_NUM_UP(2);
	R.worldScale.z = F_NUM_UP(2);
	
	W.objData = (objectData*)logoLetter_W;
	W.worldPosition.z = F_NUM_UP(3000);
	W.worldSpeed.z = zspeed;
	W.worldSpeed.x = xspeed;
	W.worldSpeed.y = yspeed;
	W.moveTo.z = F_NUM_UP(20);
	W.moveTo.x = F_NUM_UP(-110);
	W.moveTo.y = F_NUM_UP(60);
	W.worldScale.x = F_NUM_UP(2);
	W.worldScale.y = F_NUM_UP(2);
	W.worldScale.z = F_NUM_UP(2);
	
	A2.objData = (objectData*)logoLetter_A;
	A2.worldPosition.z = F_NUM_UP(3000);
	A2.worldSpeed.x = xspeed;
	A2.worldSpeed.y = yspeed;
	A2.worldSpeed.z = zspeed;
	A2.moveTo.z = F_NUM_UP(20);
	A2.moveTo.x = F_NUM_UP(-45);
	A2.moveTo.y = F_NUM_UP(60);
	A2.worldScale.x = F_NUM_UP(2);
	A2.worldScale.y = F_NUM_UP(2);
	A2.worldScale.z = F_NUM_UP(2);
	
	R2.objData = (objectData*)logoLetter_R2;
	R2.worldPosition.z = F_NUM_UP(3000);
	R2.worldSpeed.x = xspeed;
	R2.worldSpeed.y = yspeed;
	R2.worldSpeed.z = zspeed;
	R2.moveTo.z = F_NUM_UP(20);
	R2.moveTo.x = F_NUM_UP(40);
	R2.moveTo.y = F_NUM_UP(60);
	R2.worldScale.x = F_NUM_UP(2);
	R2.worldScale.y = F_NUM_UP(2);
	R2.worldScale.z = F_NUM_UP(2);
	
	S2.objData = (objectData*)logoLetter_S2;
	S2.worldPosition.z = F_NUM_UP(3000);
	S2.worldSpeed.x = xspeed;
	S2.worldSpeed.y = yspeed;
	S2.worldSpeed.z = zspeed;
	S2.moveTo.z = F_NUM_UP(20);
	S2.moveTo.x = F_NUM_UP(80);
	S2.moveTo.y = F_NUM_UP(60);
	S2.worldScale.x = F_NUM_UP(2);
	S2.worldScale.y = F_NUM_UP(2);
	S2.worldScale.z = F_NUM_UP(2);
	
	DoMusic(52);
	while(!(vbReadPad() & K_ANY)){
		moveObject(&S);
		moveObject(&T);
		moveObject(&A);
		moveObject(&R);
		moveObject(&W);
		moveObject(&A2);
		moveObject(&R2);
		moveObject(&S2);
		
		drawObject(&S);
		drawObject(&T);
		drawObject(&A);
		drawObject(&R);
		drawObject(&W);
		drawObject(&A2);
		drawObject(&R2);
		drawObject(&S2);
		
		screenControl();
	}
}

int main(){
	u8 o,g;
	u16 position;
	object* oPtr;
	
	vbInit();
	initMusic();
	initObjects();
	intro();
	while(1){
		handleInput();
		
		for(o=0; o<gameObjectsIdx; o++){
			//Perform the actual movement of game objects
			moveObject(&gameObjects[o]);
			
			//Do random number reset
			randomNumber++;
			if(randomNumber > 255) randomNumber = 0;
			
			//Make it look like were flying forward
			if(gameObjects[o].objData == (objectData*)wallEffects){
				if(!isMoving(&gameObjects[o])){
					gameObjects[o].moveTo.z = 0; 
				}
				visualEffects(&gameObjects[o]);
			}
			
			//Set the crosshairs with the camera
			if(gameObjects[o].objData == (objectData*)crossHairs){
				setObjectRelativeCamera(&gameObjects[o]);
			}
			
			//Make enemies move around in a sudo random manner
			if(gameObjects[o].objData == (objectData*)tieFighter){
				if(!isMoving(&gameObjects[o])){
					//Set first postion
					position = ((randomNumber & (TOTAL_ENEMY_POSITIONS-1)) * 3);
					//Get and set sudo random postion
					gameObjects[o].moveTo.x = F_NUM_UP(tieFighterPositions[position]);
					gameObjects[o+1].moveTo.x = gameObjects[o].moveTo.x;
			
					gameObjects[o].moveTo.y = F_NUM_UP(tieFighterPositions[++position]);
					gameObjects[o+1].moveTo.y = gameObjects[o].moveTo.y;
					
					gameObjects[o].moveTo.z = F_NUM_UP(tieFighterPositions[++position]);
					gameObjects[o+1].moveTo.z = gameObjects[o].moveTo.z;
				}
			}
			drawObject(&gameObjects[o]);
		}
		
		//Show laser fire
		for(o=0; o<MAX_LASERS; o++){
			if(laserObjects[o].properties.visible == 0) break;
			moveObject(&laserObjects[o]);
			drawObject(&laserObjects[o]);
			//Collision detection
			if(laserObjects[o].properties.visible == 1){
				for(g=0; g<gameObjectsIdx; g++){
					if(gameObjects[g].properties.detectCollision == 1 && gameObjects[g].properties.visible == 1){
						if(detectCollision(&gameObjects[g],&laserObjects[o]) == 1){
							laserObjects[o].properties.visible = 0;
							gameObjects[g].properties.visible = 0;
							oPtr = gameObjects[g].parent;
							while(oPtr != (object*)(0x00)){
								oPtr->properties.visible = 0;
								oPtr = oPtr->parent;
							}
							vbTextOut(0,7,10,debugHit);
						}
					}
				}
			}
		}
		
		vbTextOut(0,7,4,itoa(laserObjects[0].properties.hitCube.minX,16,8));
		vbTextOut(0,7,5,itoa(laserObjects[0].properties.hitCube.maxX,16,8));
		
		screenControl();
	}
}

/*****************************************
Calculates detection between two objects
using bounding cubes.
*****************************************/
u8 detectCollision(object* o1, object* o2){
	u8 xAxis,yAxis,zAxis;
	
	if((o1->properties.hitCube.minX >= o2->properties.hitCube.minX && o1->properties.hitCube.minX <= o2->properties.hitCube.maxX) ||
	   (o1->properties.hitCube.maxX >= o2->properties.hitCube.minX && o1->properties.hitCube.maxX <= o2->properties.hitCube.maxX) ||
	   (o2->properties.hitCube.minX >= o1->properties.hitCube.minX && o2->properties.hitCube.minX <= o1->properties.hitCube.maxX) ||
	   (o2->properties.hitCube.maxX >= o1->properties.hitCube.minX && o2->properties.hitCube.maxX <= o1->properties.hitCube.maxX)
	  ) xAxis = 1;
	else return 0;
	
	if((o1->properties.hitCube.minY >= o2->properties.hitCube.minY && o1->properties.hitCube.minY <= o2->properties.hitCube.maxY) ||
	   (o1->properties.hitCube.maxY >= o2->properties.hitCube.minY && o1->properties.hitCube.maxY <= o2->properties.hitCube.maxY) ||
	   (o2->properties.hitCube.minY >= o1->properties.hitCube.minY && o2->properties.hitCube.minY <= o1->properties.hitCube.maxY) ||
	   (o2->properties.hitCube.maxY >= o1->properties.hitCube.minY && o2->properties.hitCube.maxY <= o1->properties.hitCube.maxY)
	  ) yAxis = 1;
	else return 0;
	
	if((o1->properties.hitCube.minZ >= o2->properties.hitCube.minZ && o1->properties.hitCube.minZ <= o2->properties.hitCube.maxZ) ||
	   (o1->properties.hitCube.maxZ >= o2->properties.hitCube.minZ && o1->properties.hitCube.maxZ <= o2->properties.hitCube.maxZ) ||
	   (o2->properties.hitCube.minZ >= o1->properties.hitCube.minZ && o2->properties.hitCube.minZ <= o1->properties.hitCube.maxZ) ||
	   (o2->properties.hitCube.maxZ >= o1->properties.hitCube.minZ && o2->properties.hitCube.maxZ <= o1->properties.hitCube.maxZ)
	  ) zAxis = 1;
	else return 0;
	
	return 1;
}


/*****************************************
Sets the values of a collision cube
*****************************************/
void setCollisionCube(object* o, vector3d* v){
	if(o->properties.hitCube.reset == 1){
		o->properties.hitCube.minX = v->x;
		o->properties.hitCube.minY = v->y;
		o->properties.hitCube.minZ = v->z;
		o->properties.hitCube.maxX = v->x;
		o->properties.hitCube.maxY = v->y;
		o->properties.hitCube.maxZ = v->z;
		o->properties.hitCube.reset = 0;
		vbTextOut(0,7,1,itoa(v->x,16,8));
		vbTextOut(0,7,2,itoa(v->y,16,8));
		vbTextOut(0,7,3,itoa(v->z,16,8));
	}
	
	if(v->x < o->properties.hitCube.minX) o->properties.hitCube.minX = v->x;
	if(v->y < o->properties.hitCube.minY) o->properties.hitCube.minY = v->y;
	if(v->z < o->properties.hitCube.minZ) o->properties.hitCube.minZ = v->z;
	
	if(v->x > o->properties.hitCube.maxX) o->properties.hitCube.maxX = v->x;
	if(v->y > o->properties.hitCube.maxY) o->properties.hitCube.maxY = v->y;
	if(v->z > o->properties.hitCube.maxZ) o->properties.hitCube.maxZ = v->z;
}

/*****************************************
Get the music setup and ready to go
*****************************************/
void initMusic(){
	u16 i;
	
	SSTOP = 0;
	
	for(i=0;i<32;i++)WAVEDATA1[i<<2] = TRUMPET[i];//Instrument

	SSTOP = 1;
	SND_REGS[0].SxEV0 = 0xFC;         	// No fadeout; volume is constant.
    SND_REGS[0].SxEV1 = 0x02;         	// Repeat it forever.
	SND_REGS[0].SxRAM = 0x00;
	SND_REGS[0].SxFQH = 0x00;
	SND_REGS[0].SxFQL = 0x00;
	SND_REGS[0].SxLRV = 0x33;
}

/*****************************************
Read the gamepad and seed the random number
counter
*****************************************/
void handleInput(){
	u8 o;
	u8 speed = 30;
	buttons = vbReadPad();
	if(K_LL & buttons){
		randomNumber += 1;
		cam.worldPosition.x-=F_NUM_UP(speed);
		if(cam.worldPosition.x <= F_NUM_UP(-300)) cam.worldPosition.x=F_NUM_UP(-300);
	}
	if(K_LR & buttons){
		randomNumber += 2;
		cam.worldPosition.x+=F_NUM_UP(speed);
		if(cam.worldPosition.x >= F_NUM_UP(300)) cam.worldPosition.x=F_NUM_UP(300);
	}
	if(K_LD & buttons){
		randomNumber += 3;
		cam.worldPosition.y-=F_NUM_UP(speed);
		if(cam.worldPosition.y <= F_NUM_UP(-180)) cam.worldPosition.y=F_NUM_UP(-180);
	}
	if(K_LU & buttons){
		randomNumber += 4;
		cam.worldPosition.y+=F_NUM_UP(speed);
		if(cam.worldPosition.y >= F_NUM_UP(580)) cam.worldPosition.y=F_NUM_UP(580);
	}
	if(K_A & buttons){
		while(o<MAX_LASERS && laserObjects[o].properties.visible ==1){
			o++;
		}
		if(o<MAX_LASERS && laserObjects[o].properties.visible == 0){
			setObjectRelative(&laserObjects[o],crossH);
			laserObjects[o].moveTo.x = laserObjects[o].worldPosition.x;
			laserObjects[o].moveTo.y = laserObjects[o].worldPosition.y;
			laserObjects[o].moveTo.z = FAR_Z;
			laserObjects[o].speed.z = F_NUM_UP(50);
			laserObjects[o].properties.visible = 1;
		}
	}
}

/******************************
This gives us the flying forward effect
******************************/
void visualEffects(object* o){
	if(o->worldPosition.z <= 0) {
		o->worldPosition.z = F_NUM_UP(3000);
		o->moveTo.z = F_NUM_UP(3000);
	}
}

/********************************
This will move objects around in our 3d world
********************************/
void moveObject(object* o){
	vector3d normal,difference;
	s32 speed;
	//Calculate the vector from the move to position to current world position.
	//Then normalize the vector
	if(!isEqualVector(&o->worldPosition,&o->moveTo)){
		subtractVector(&o->worldPosition,&o->moveTo,&difference);
		normalizeVector(&difference,&normal);
	}
	//If the move to value is different then will calculate a speed based on the
	//objects max speed in the given direction.
	if(o->worldPosition.x != o->moveTo.x){
		speed = F_MUL(normal.x,o->worldSpeed.x);
		if(speed != 0 && ((speed<0)?((~speed)+1):(speed))<((difference.x<0)?((~difference.x)+1):(difference.x))){
			o->worldPosition.x += speed;
		}else{
			o->worldPosition.x = o->moveTo.x;
		}
	}
	if(o->worldPosition.y != o->moveTo.y){
		speed = F_MUL(normal.y,o->worldSpeed.y);
		if(speed != 0 && ((speed<0)?((~speed)+1):(speed))<((difference.y<0)?((~difference.y)+1):(difference.y))){
			o->worldPosition.y += speed;
		}else{
			o->worldPosition.y = o->moveTo.y;
		}
	}
	if(o->worldPosition.z != o->moveTo.z){
		speed = F_MUL(normal.z,o->worldSpeed.z);
		if(speed != 0 && ((speed<0)?((~speed)+1):(speed))<((difference.z<0)?((~difference.z)+1):(difference.z))){
			o->worldPosition.z += speed;
		}else{
			o->worldPosition.z = o->moveTo.z;
		}
	}
	
	//Clip Far Z
	if(o->worldPosition.z >= FAR_Z) o->properties.visible = 0;
	
	//Rotation
	o->worldRotation.x += o->worldRotateSpeed.x;
	if(o->worldRotation.x > F_NUM_UP(359)) o->worldRotation.x = F_SUB(o->worldRotation.x,F_NUM_UP(359));
	if(o->worldRotation.x < F_NUM_UP(-359)) o->worldRotation.x = F_ADD(o->worldRotation.x,F_NUM_UP(359));
	
	o->worldRotation.y += o->worldRotateSpeed.y;
	if(o->worldRotation.y > F_NUM_UP(359)) o->worldRotation.y = F_SUB(o->worldRotation.y,F_NUM_UP(359));
	if(o->worldRotation.y < F_NUM_UP(-359)) o->worldRotation.y = F_ADD(o->worldRotation.y,F_NUM_UP(359));
	
	o->worldRotation.z += o->worldRotateSpeed.z;
	if(o->worldRotation.z > F_NUM_UP(359)) o->worldRotation.z = F_SUB(o->worldRotation.z,F_NUM_UP(359));
	if(o->worldRotation.z < F_NUM_UP(-359)) o->worldRotation.z = F_ADD(o->worldRotation.z,F_NUM_UP(359));
	
}

/*********************
See if object is currently moving
*********************/
u8 inline isMoving(object* o){
	u8 r;
	r=1;
	if(isEqualVector(&o->worldPosition,&o->moveTo)){
		r=0;
	}
	return r;
}
/*********************
Compare two vectors
*********************/
u8 isEqualVector(vector3d* v1, vector3d* v2){
	u8 r;
	r=0;
	if(v1->x == v2->x &&
	   v1->y == v2->y &&
	   v1->z == v2->z){
		r = 1;
	}
	return r;
}
/*********************
Subtracts two vectors
*********************/
void subtractVector(vector3d* vStart, vector3d* vEnd, vector3d* n){
	n->x = F_SUB(vEnd->x,vStart->x);
	n->y = F_SUB(vEnd->y,vStart->y);
	n->z = F_SUB(vEnd->z,vStart->z);
}

/**********************
Add two vectors together
**********************/
void addVector(vector3d* v1, vector3d* v2, vector3d* n){
	n->x = F_ADD(v1->x,v2->x);
	n->y = F_ADD(v1->y,v2->y);
	n->z = F_ADD(v1->z,v2->z);
}

/************************
Multiply two vectors together
************************/
void multiplyVector(vector3d* v1, vector3d* v2, vector3d* n){
	n->x = F_MUL(v1->x,v2->x);
	n->y = F_MUL(v1->y,v2->y);
	n->z = F_MUL(v1->z,v2->z);
}

/**************************************
Copies a matrix from m1 to m2
**************************************/
void copyMatrix(matrix3d m1, matrix3d m2){
	u8 r,c;
	
	for(r=0;r<4;r++){
		for(c=0;c<4;c++){
			m2[r][c]=m1[r][c];
		}
	}
}

/***************************
This will calculate an objects
world coordinates based on relative information
***************************/
void setObjectRelative(object* o, object* parent){
	addVector(&o->position,&parent->worldPosition,&o->worldPosition);//Sets the position
	addVector(&o->rotation,&parent->worldRotation,&o->worldRotation);//Sets rotation
	addVector(&o->rotateSpeed,&parent->worldRotateSpeed,&o->worldRotateSpeed);//Sets the rotation speed
	addVector(&o->speed,&parent->worldSpeed,&o->worldSpeed);//Sets the overall speed
	multiplyVector(&o->scale,&parent->worldScale,&o->worldScale);//Sets the scale relative to other object
	
	o->properties.visible = parent->properties.visible;
}

/******************************
This will set objects relative to
the camera
******************************/
void setObjectRelativeCamera(object* o){
	addVector(&o->position,&cam.worldPosition,&o->worldPosition);
	o->worldPosition.y = o->worldPosition.y * -1;
}

/**************************
This procedure actually draws an
object
**************************/
void drawObject(object* o){
	s32 size;
	vector3d v1,v2;
	vector3d vt;
	u8 verts,i;
	s32 v,firstV;
	
	//Reset hit cubes
	if(o->properties.detectCollision == 1){
		o->properties.hitCube.reset = 1;
	}
	
	//If there is a parent object set this one relative to its parent
	if(o->parent != (object*)0x00) setObjectRelative(o,o->parent);
	
	if(o->properties.visible == 0) return;
	
	size=o->objData->size;//total elements in array
	verts=o->objData->faceSize;//total vertices per section
	
	v=0;//first vertex
	worldMatrix(m_world3d,o,o->worldScale.x,o->worldScale.y,o->worldScale.z);

	while(v < size){
		firstV = v;
		v1.x = F_NUM_UP(o->objData->data[v]);
		v1.y = F_NUM_UP(o->objData->data[++v]);
		v1.z = F_NUM_UP(o->objData->data[++v]);
		v1.w = F_NUM_UP(1);
		
		matrix3dxVertex(&v1,m_world3d,&vt);
		
		v1.x = vt.x;
		v1.y = vt.y;
		v1.z = vt.z;

		for(i=1; i<verts; i++){			
			v2.x = F_NUM_UP(o->objData->data[++v]);
			v2.y = F_NUM_UP(o->objData->data[++v]);
			v2.z = F_NUM_UP(o->objData->data[++v]);
			v2.w = F_NUM_UP(1);
			
			matrix3dxVertex(&v2,m_world3d,&vt);
			
			v2.x = vt.x;
			v2.y = vt.y;
			v2.z = vt.z;
			
			drawLine(&v1,&v2,3,o);
			
			v1.x = v2.x;
			v1.y = v2.y;
			v1.z = v2.z;
		}
		//This causes a final line to be drawn back to the starting vertex
		if(verts>2){
			v2.x = F_NUM_UP(o->objData->data[firstV]);
			v2.y = F_NUM_UP(o->objData->data[firstV+1]);
			v2.z = F_NUM_UP(o->objData->data[firstV+2]);
			
			matrix3dxVertex(&v2,m_world3d,&vt);
			
			v2.x = vt.x;
			v2.y = vt.y;
			v2.z = vt.z;
			
			drawLine(&v1,&v2,3,o);
		}
		v++;
	}
}

/*********************************
This initializes an object type
*********************************/
void inline initObject(object* o){
	o->worldPosition.x = 0;
	o->worldPosition.y = 0;
	o->worldPosition.z = 0;
	o->position.x = 0;
	o->position.y = 0;
	o->position.z = 0;
	o->moveTo.x = 0;
	o->moveTo.y = 0;
	o->moveTo.z = 0;
	o->worldRotation.x = 0;
	o->worldRotation.y = 0;
	o->worldRotation.z = 0;
	o->rotation.x = 0;
	o->rotation.y = 0;
	o->rotation.z = 0;
	o->worldRotateSpeed.x = 0;
	o->worldRotateSpeed.y = 0;
	o->worldRotateSpeed.z = 0;
	o->rotateSpeed.x = 0;
	o->rotateSpeed.y = 0;
	o->rotateSpeed.z = 0;
	o->worldSpeed.x = 0;
	o->worldSpeed.y = 0;
	o->worldSpeed.z = 0;
	o->speed.x = 0;
	o->speed.y = 0;
	o->speed.z = 0;
	//The scale is a multiplication factor so it is not fixed point
	o->worldScale.x = F_NUM_UP(1);
	o->worldScale.y = F_NUM_UP(1);
	o->worldScale.z = F_NUM_UP(1);
	o->scale.x = F_NUM_UP(1);
	o->scale.y = F_NUM_UP(1);
	o->scale.z = F_NUM_UP(1);
	o->parent = (object*)0x00;
	
	o->properties.visible = 1;
	o->properties.detectCollision = 0;
}

void initObjects(){
	u16 i;
	
	for(i=0;i<MAX_GAME_OBJECTS;i++){
		initObject(&gameObjects[i]);
	}
	for(i=0;i<MAX_LASERS;i++){
		initObject(&laserObjects[i]);
		laserObjects[i].objData = (objectData*)lasers;
		laserObjects[i].properties.visible = 0;
		laserObjects[i].properties.detectCollision = 1;
	}
	
	cam.worldPosition.x = 0;
	cam.worldPosition.y = 0;
	cam.worldPosition.z = 0;
	cam.d = F_NUM_UP(128);
	
	gameObjectsIdx=0;
	//Death Star Walls
	gameObjects[gameObjectsIdx].objData = (objectData*)deathStarWalls;
	gameObjectsIdx++;
	//tie fighter
	gameObjects[gameObjectsIdx].worldPosition.x = F_NUM_UP(-150);
	gameObjects[gameObjectsIdx].moveTo.x = F_NUM_UP(-150);
	gameObjects[gameObjectsIdx].worldPosition.z = F_NUM_UP(3000);
	gameObjects[gameObjectsIdx].moveTo.z = F_NUM_UP(3000);
	gameObjects[gameObjectsIdx].worldRotation.y = F_NUM_UP(180);
	gameObjects[gameObjectsIdx].worldSpeed.x = F_NUM_UP(5);
	gameObjects[gameObjectsIdx].worldSpeed.y = F_NUM_UP(5);
	gameObjects[gameObjectsIdx].worldSpeed.z = F_NUM_UP(10);
	gameObjects[gameObjectsIdx].objData = (objectData*)tieFighter;
	gameObjectsIdx++;
	//tie fighter wings
	gameObjects[gameObjectsIdx].parent = (object*)&gameObjects[gameObjectsIdx-1];
	gameObjects[gameObjectsIdx].objData = (objectData*)tieFighterWings;
	gameObjects[gameObjectsIdx].properties.detectCollision = 1;
	gameObjectsIdx++;
	//tie fighter
	gameObjects[gameObjectsIdx].worldPosition.x = F_NUM_UP(-150);
	gameObjects[gameObjectsIdx].moveTo.x = F_NUM_UP(-150);
	gameObjects[gameObjectsIdx].worldPosition.z = F_NUM_UP(3000);
	gameObjects[gameObjectsIdx].moveTo.z = F_NUM_UP(3000);
	gameObjects[gameObjectsIdx].worldRotation.y = F_NUM_UP(180);
	gameObjects[gameObjectsIdx].worldSpeed.x = F_NUM_UP(5);
	gameObjects[gameObjectsIdx].worldSpeed.y = F_NUM_UP(5);
	gameObjects[gameObjectsIdx].worldSpeed.z = F_NUM_UP(10);
	gameObjects[gameObjectsIdx].objData = (objectData*)tieFighter;
	gameObjectsIdx++;
	//tie fighter wings
	gameObjects[gameObjectsIdx].parent = (object*)&gameObjects[gameObjectsIdx-1];
	gameObjects[gameObjectsIdx].objData = (objectData*)tieFighterWings;
	gameObjects[gameObjectsIdx].properties.detectCollision = 1;
	gameObjectsIdx++;
	//tie fighter
	gameObjects[gameObjectsIdx].worldPosition.x = F_NUM_UP(150);
	gameObjects[gameObjectsIdx].moveTo.x = F_NUM_UP(150);
	gameObjects[gameObjectsIdx].worldPosition.z = F_NUM_UP(3000);
	gameObjects[gameObjectsIdx].moveTo.z = F_NUM_UP(3000);
	gameObjects[gameObjectsIdx].worldRotation.y = F_NUM_UP(180);
	gameObjects[gameObjectsIdx].worldSpeed.x = F_NUM_UP(5);
	gameObjects[gameObjectsIdx].worldSpeed.y = F_NUM_UP(5);
	gameObjects[gameObjectsIdx].worldSpeed.z = F_NUM_UP(10);
	gameObjects[gameObjectsIdx].objData = (objectData*)tieFighter;
	gameObjectsIdx++;
	//tie fighter wings
	gameObjects[gameObjectsIdx].parent = (object*)&gameObjects[gameObjectsIdx-1];
	gameObjects[gameObjectsIdx].objData = (objectData*)tieFighterWings;
	gameObjects[gameObjectsIdx].properties.detectCollision = 1;
	gameObjectsIdx++;
	//Wall Effects
	gameObjects[gameObjectsIdx].worldPosition.z = F_NUM_UP(3000);
	gameObjects[gameObjectsIdx].moveTo.z = F_NUM_UP(3000);
	gameObjects[gameObjectsIdx].worldSpeed.z = FLYING_SPEED;
	gameObjects[gameObjectsIdx].objData = (objectData*)wallEffects;
	gameObjectsIdx++;
	//Cross Hairs
	gameObjects[gameObjectsIdx].objData = (objectData*)crossHairs;
	crossH = &gameObjects[gameObjectsIdx];
	gameObjectsIdx++;
}

/********************************
Matrix multiplication function
********************************/
void matrix3dxVertex(vector3d* v, matrix3d m, vector3d* o){	
	o->x = F_ADD(F_ADD(F_ADD(
	       F_MUL(v->x,m[0][0]),
	       F_MUL(v->y,m[1][0])),
	       F_MUL(v->z,m[2][0])),
	       F_MUL(v->w,m[3][0]));
	      
	o->y = F_ADD(F_ADD(F_ADD(
	       F_MUL(v->x,m[0][1]),
	       F_MUL(v->y,m[1][1])),
	       F_MUL(v->z,m[2][1])),
	       F_MUL(v->w,m[3][1]));
	      
	o->z = F_ADD(F_ADD(F_ADD(
	       F_MUL(v->x,m[0][2]),
	       F_MUL(v->y,m[1][2])),
	       F_MUL(v->z,m[2][2])),
	       F_MUL(v->w,m[3][2]));
	       
	o->w = F_ADD(F_ADD(F_ADD(
	       F_MUL(v->x,m[0][3]),
	       F_MUL(v->y,m[1][3])),
	       F_MUL(v->z,m[2][3])),
	       F_MUL(v->w,m[3][3]));
}

/************************************
Multiply matrices together to produce
single transform matrix.
************************************/
void matrix3dxMatrix3d(matrix3d m1, matrix3d m2, matrix3d n){
	u8 i,j,k;
	i=j=k=0;
	
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			n[i][j]=0;
			for(k=0;k<4;k++){
				n[i][j]+=F_MUL(m1[i][k],m2[k][j]);
			}
		}
	}
}

/**********************************************
This is a combination of all necessary matrices.
**********************************************/
void worldMatrix(matrix3d m, object* o, s32 sx, s32 sy, s32 sz){
	s32 ax,ay,az;
	ax = F_NUM_DN((o->worldRotation.x<0)?(F_NUM_UP(360)+o->worldRotation.x):(o->worldRotation.x));
	ay = F_NUM_DN((o->worldRotation.y<0)?(F_NUM_UP(360)+o->worldRotation.y):(o->worldRotation.y));
	az = F_NUM_DN((o->worldRotation.z<0)?(F_NUM_UP(360)+o->worldRotation.z):(o->worldRotation.z));
	
	if(ax>359)ax=0;
	if(ay>359)ay=0;
	if(az>359)az=0;
	
	m[0][0]=F_MUL(F_MUL(sx,F_COSINE(ay)),F_COSINE(az));
	m[0][1]=F_MUL(F_MUL(sx,F_COSINE(ay)),F_SINE(az));
	m[0][2]=F_MUL(sx,-(F_SINE(ay)));
	m[0][3]=0;
	
	m[1][0]=F_ADD(F_MUL(F_MUL(F_MUL(sy,F_SINE(ax)),F_SINE(ay)),F_COSINE(az)),
	        F_MUL(F_MUL(sy,F_COSINE(ax)),-F_SINE(az)));
	m[1][1]=F_ADD(F_MUL(F_MUL(sy,F_COSINE(ax)),F_COSINE(az)),
	        F_MUL(F_MUL(F_MUL(sy,F_SINE(ax)),F_SINE(ay)),F_SINE(az)));
	m[1][2]=F_MUL(F_MUL(sy,F_SINE(ax)),F_COSINE(ay));
	m[1][3]=0;
	
	m[2][0]=F_ADD(F_MUL(F_MUL(F_MUL(sz,F_COSINE(ax)),F_COSINE(az)),F_SINE(ay)),
	        F_MUL(F_MUL(sz,-F_SINE(ax)),-F_SINE(az)));
	m[2][1]=F_ADD(F_MUL(F_MUL(sz,-F_SINE(ax)),F_COSINE(az)),
	        F_MUL(F_MUL(F_MUL(sz,F_COSINE(ax)),F_SINE(ay)),F_SINE(az)));
	m[2][2]=F_MUL(F_MUL(sz,F_COSINE(ax)),F_COSINE(ay));
	m[2][3]=0;
	
	m[3][0]=F_SUB(o->worldPosition.x,cam.worldPosition.x);
	m[3][1]=F_ADD(o->worldPosition.y,cam.worldPosition.y);
	m[3][2]=F_SUB(o->worldPosition.z,cam.worldPosition.z);
	m[3][3]=F_NUM_UP(1);
}

/*********************************
Fills the camera translation matrix.
Same as regular translate except all
axis scaled by -1.
*********************************/
void translateCameraMatrix(matrix3d m, object* target){
	vector3d look, right, up;
	up.x = 0;
	up.y = F_NUM_UP(1);
	up.z = 0;
	
	right.x=F_NUM_UP(1);
	right.y=0;
	right.z=0;
	
	look.x = 0;
	look.y = 0;
	look.z = F_NUM_UP(1);
	
	m[0][0]=(right.x);
	m[0][1]=(right.y);
	m[0][2]=(right.z);
	m[0][3]=F_NUM_UP(1);
	
	m[1][0]=(up.x);
	m[1][1]=(up.y);
	m[1][1]=(up.z);
	m[1][1]=F_NUM_UP(1);
	
	m[2][0]=(look.x);
	m[2][1]=(look.y);
	m[2][2]=(look.z);
	m[2][3]=F_NUM_UP(1);
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
	
	
}

/********************************
Fills the scale matrix
********************************/
void scaleMatrix(matrix3d m,s32 sx, s32 sy, s32 sz){
	m[0][0]=sx;
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=sy;
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=sz;
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
}

void projectionMatrix(matrix3d m){
	m[0][0]=F_DIV((cam.d<<1),F_NUM_UP(SCREEN_WIDTH));
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_DIV((cam.d<<1),F_NUM_UP(SCREEN_HEIGHT));
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=F_DIV(F_NUM_UP(FAR_Z),F_SUB(F_NUM_UP(FAR_Z),cam.d));
	m[2][3]=F_DIV((F_MUL(cam.d,F_NUM_UP(FAR_Z))),(F_SUB(F_NUM_UP(FAR_Z),cam.d)));
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=F_NUM_UP(1);
	m[3][3]=0;
}

/*******************************
Fills the rotate matrix for z axis
angle is in degrees.
*******************************/
void rotateMatrixZ(matrix3d m,s32 angle){
	angle = F_NUM_DN((angle<0)?(F_NUM_UP(360)+angle):(angle));
	m[0][0]=F_COSINE(angle);
	m[0][1]=F_SINE(angle);
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=-(F_SINE(angle));
	m[1][1]=F_COSINE(angle);
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=F_NUM_UP(1);
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
}

/*******************************
Fills the rotate matrix for y axis
angle is in degrees.
*******************************/
void rotateMatrixY(matrix3d m,s32 angle){
	angle = F_NUM_DN((angle<0)?(F_NUM_UP(360)+angle):(angle));
	m[0][0]=F_COSINE(angle);
	m[0][1]=0;
	m[0][2]=-(F_SINE(angle));
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_NUM_UP(1);
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=F_SINE(angle);
	m[2][1]=0;
	m[2][2]=F_COSINE(angle);
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
}

/*******************************
Fills the rotate matrix for x axis
angle is in degrees.
*******************************/
void rotateMatrixX(matrix3d m,s32 angle){
	angle = F_NUM_DN((angle<0)?(F_NUM_UP(360)+angle):(angle));
	m[0][0]=F_NUM_UP(1);
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_COSINE(angle);
	m[1][2]=F_SINE(angle);
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=-(F_SINE(angle));
	m[2][2]=F_COSINE(angle);
	m[2][3]=0;
	
	m[3][0]=0;
	m[3][1]=0;
	m[3][2]=0;
	m[3][3]=F_NUM_UP(1);
}

/*********************************
Fills the translation matrix
*********************************/
void translateMatrix(matrix3d m, s32 x, s32 y, s32 z){
	m[0][0]=F_NUM_UP(1);
	m[0][1]=0;
	m[0][2]=0;
	m[0][3]=0;
	
	m[1][0]=0;
	m[1][1]=F_NUM_UP(1);
	m[1][2]=0;
	m[1][3]=0;
	
	m[2][0]=0;
	m[2][1]=0;
	m[2][2]=F_NUM_UP(1);
	m[2][3]=0;
	
	m[3][0]=x;
	m[3][1]=y;
	m[3][2]=z;
	m[3][3]=F_NUM_UP(1);
	
	
}

void vbInit(){
	vbDisplayOn ();
	vbSetColTable ();
	
	VIP_REGS[BRTA]  = 32;
	VIP_REGS[BRTB]  = 64;
	VIP_REGS[BRTC]  = 32;
	
	HW_REGS[WCR] = 1;
	
	VIP_REGS[FRMCYC] = 0;
	
	tim_vector = (u32)timeHnd;
	
	trackTable[0] = t1;
	
	//load font
	copymem((u8*)CharSeg3, PVB_FONT, 0x2000);
	vbTextOut(0,0,1,debugX);
	vbTextOut(0,0,2,debugY);
	vbTextOut(0,0,3,debugZ);
	vbTextOut(0,0,4,debugMinX);
	vbTextOut(0,0,5,debugMaxX);
	vbTextOut(0,0,6,debugMinY);
	vbTextOut(0,0,7,debugMaxY);
	vbTextOut(0,0,8,debugMinZ);
	vbTextOut(0,0,9,debugMaxZ);

	//world info
	WA[31].head = WRLD_ON;
	WA[31].w = 200;
	WA[31].h = 200;
	WA[30].head = WRLD_END;
}
/*******************************
Draws a pixel onto the screen
*******************************/
void inline drawPoint(s32 x, s32 y, u8 color, s32 p){

	if(y<0 || y>SCREEN_HEIGHT) return;
	if(x<0 || x>SCREEN_WIDTH) return;
	
	s32 loffset,roffset;
	u8 yleft;
	
	//Put a cap on parallax
	if(p>PARALLAX_MAX) p=PARALLAX_MAX;
	
	loffset = (((x-p)<<4) + (y>>4));
	roffset = (loffset + (p<<5));
	
	if(loffset>0x1800 || loffset<0) return;
	if(roffset>0x1800 || roffset<0) return;
	
	color &= 0x03;
	
	yleft = (y&0x0F)<<1;

	currentFrameBuffer[loffset] |= (color<<yleft);
	((u32*)(currentFrameBuffer+0x4000))[roffset] |= (color<<yleft);
}

/*******************************
My Line Algorithm (Brezenham based)
*******************************/
void drawLine(vector3d* v1, vector3d* v2, u8 color, object* o){
	s32 vx,vy,vz,vx2,vy2;
	s32 dx, dy, dz;
	s32 sx,sy,sz,p,pixels,err;
	
	//Set the collision Cubes
	setCollisionCube(o,v1);
	setCollisionCube(o,v2);

	//z clipping(clips whole line should improve in future)
	if(v1->z<=(F_NUM_DN(cam.worldPosition.z))) return;
	if(v2->z<=(F_NUM_DN(cam.worldPosition.z))) return;
	
	//Scale everything back to integers and apply projection
	vx=F_NUM_DN(F_ADD(F_DIV(F_MUL(v1->x,cam.d),F_ADD(cam.d,v1->z)),F_NUM_UP(SCREEN_WIDTH>>1)));
	vy=F_NUM_DN(F_ADD(F_DIV(F_MUL(v1->y,cam.d),F_ADD(cam.d,v1->z)),F_NUM_UP(SCREEN_HEIGHT>>1)));

	vx2=F_NUM_DN(F_ADD(F_DIV(F_MUL(v2->x,cam.d),F_ADD(cam.d,v2->z)),F_NUM_UP(SCREEN_WIDTH>>1)));
	vy2=F_NUM_DN(F_ADD(F_DIV(F_MUL(v2->y,cam.d),F_ADD(cam.d,v2->z)),F_NUM_UP(SCREEN_HEIGHT>>1)));
	
	dx=(~(vx - vx2)+1);
	dy=(~(vy - vy2)+1);
	dz=(~(F_NUM_DN(F_SUB(v1->z,v2->z))+1));
	
	sx=(dx<0)?(-1):(1);
	sy=(dy<0)?(-1):(1);
	sz=(dz<0)?(-1):(1);
	
	if(dx<0) dx=(~dx)+1;
	if(dy<0) dy=(~dy)+1;
	if(dz<0) dz=(~dz)+1;

	vz=F_NUM_DN(v1->z);
	
	pixels=((dx>dy)?(dx):(dy))+1;
	
	CACHE_ENABLE;
	if(dy<dx){
		err=(dx>>1);
		sz=(sz)*(F_NUM_UP(dz)/((dx==0)?(1):(dx)));
		vz=F_NUM_UP(vz);
		for(p=0;p<pixels;p++){
			drawPoint(vx,vy,color,(F_NUM_DN(vz)>>PARALLAX_SHIFT));
			err+=dy;
			if(err>dx){
				vy+=sy;
				err-=dx;
			}
			vz+=sz;
			vx+=sx;
		}
	}else{
		err=(dy>>1);
		sz=(sz)*(F_NUM_UP(dz)/((dy==0)?(1):(dy)));
		vz=F_NUM_UP(vz);
		for(p=0;p<pixels;p++){
			drawPoint(vx,vy,color,(F_NUM_DN(vz)>>PARALLAX_SHIFT));
			err+=dx;
			if(err>dy){
				vx+=sx;
				err-=dy;
			}
			vz+=sz;
			vy+=sy;
		}
	}
	CACHE_DISABLE;
}

/*******************************
Dot Product function for determining
the angle between two vectors.
*******************************/
s32 dotProduct(vector3d* v1, vector3d* v2){
	s32 r;
	r = F_ADD(F_ADD(F_MUL(v1->x,v2->x),F_MUL(v1->y,v2->y)),F_MUL(v1->z,v2->z));
	return r;
}

/*******************************
Cross Product function
*******************************/
void crossProduct(vector3d* v1,vector3d* v2, vector3d* n){
	n->x = F_SUB(F_MUL(v1->y,v2->z),F_MUL(v1->z,v2->y));
	n->y = F_SUB(F_MUL(v1->z,v2->x),F_MUL(v1->x,v2->z));
	n->z = F_SUB(F_MUL(v1->x,v2->y),F_MUL(v1->y,v2->x));
}

/*******************************
Controls the timing of the screen
refresh. Borrowed from the Hunter game
written by DanB.
*******************************/
void screenControl(void){	
	u32* t;
	
	static u16 pgflip = XPBSY1;
	pgflip = ~pgflip & 0x000C;
	
	VIP_REGS[XPCTRL] = (VIP_REGS[XPSTTS] | XPEN);
	while(!(VIP_REGS[XPSTTS] & pgflip));
	VIP_REGS[XPCTRL] = (VIP_REGS[XPSTTS] & ~XPEN);//Disable drawing
	
	t = currentFrameBuffer;
	currentFrameBuffer = nextFrameBuffer;
	nextFrameBuffer = t;

}

/***********************************
This function normalizes or creates
a unit vector.
***********************************/
void normalizeVector(vector3d* v, vector3d* n){
	s32 length;
	length=isqrt(F_ADD(F_ADD(F_MUL(v->x,v->x),F_MUL(v->y,v->y)),F_MUL(v->z,v->z)));
	n->x = F_DIV(v->x,length);
	n->y = F_DIV(v->y,length);
	n->z = F_DIV(v->z,length);
}

/*************************************
Square root function based on integers
This came from wikipedia
*************************************/
s32 isqrt(s32 num) {
    s32 res = 0;
    s32 bit = 1 << 30; // The second-to-top bit is set: 1L<<30 for long
 
    // "bit" starts at the highest power of four <= the argument.
    while (bit > num)
        bit >>= 2;
 
    while (bit != 0) {
        if (num >= res + bit) {
            num -= res + bit;
            res = (res >> 1) + bit;
        }
        else res >>= 1;
        
        bit >>= 2;
    }
    return res;
}

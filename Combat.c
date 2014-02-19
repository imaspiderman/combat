#include <libgccvb.h>
#include "G3d.h"
#include "GameFunctions.h"
#include "GameData.h"
#include "Font.h"
//Timer for FPS
volatile u8 frameSkip = 0;
volatile u8 fireLaser = 0;//1=straight 2=left 4=right 8=up 16=down
volatile u8 laserFrame = 0;
volatile u8 frameSkipCount = 0;
volatile u32 tick = 0;
volatile u32 tickMax = 0;
volatile u32 tickStart = 0;
volatile u32 tickEnd = 0;

//Sound variables
u16 volatile Channel1Pos = 0;
u16 volatile Channel1Max = 0;
u8  volatile Channel1Play = 0;// 1 = play once, 2 = repeat
u8  volatile Channel1Nibble = 0;
u8* volatile Channel1Data = (u8*)0;

u16 volatile Channel2Pos = 0;
u16 volatile Channel2Max = 0;
u8  volatile Channel2Play = 0;// 1 = play once, 2 = repeat
u8  volatile Channel2Nibble = 0;
u8* volatile Channel2Data = (u8*)0;

u16 volatile Channel3Pos = 0;
u16 volatile Channel3Max = 0;
u8  volatile Channel3Play = 0;// 1 = play once, 2 = repeat
u8  volatile Channel3Nibble = 0;
u8* volatile Channel3Data = (u8*)0;

s32 movePath[15] = {
	F_NUM_UP(-1000), F_NUM_UP(1000), F_NUM_UP(8000),
	F_NUM_UP(-1000), F_NUM_UP(1000), F_NUM_UP(5000),
	F_NUM_UP(1000),  F_NUM_UP(1000), F_NUM_UP(4000),
	F_NUM_UP(2000),  F_NUM_UP(1000), F_NUM_UP(2000),
	F_NUM_UP(0),     F_NUM_UP(1000), F_NUM_UP(3000)
};
u8 movePathIdx = 0;

map level1;
u8 mapRowIdx = 1;
u8 mapEnd = 0;

#define GAME_AREA_WIDTH F_NUM_UP(8000)
#define GAME_AREA_HEIGHT F_NUM_UP(2000)

#define ENEMY_TABLE_MAX 16
object enemyTable[ENEMY_TABLE_MAX];
#define LASER_TABLE_MAX 4
object laserTable[LASER_TABLE_MAX];
#define OBJECT_TABLE_MAX 16
object objectTable[OBJECT_TABLE_MAX];

#define GROUND_SPEED F_NUM_UP(196)
#define FRAME_SKIP_MAX 8
#define LASER_FRAME_DELAY 16

#define CELL_POSX(X) (((GAME_AREA_WIDTH >> 4) * (((X) & 0xF0) >> 4)) - (GAME_AREA_WIDTH>>1))
#define CELL_POSY(Y) ((GAME_AREA_HEIGHT >> 4) * ((Y) & 0x0F))
#define CELL_POSZ(Z) ((FAR_Z >> 4) * (((Z) & 0xF0) >> 4))

#include "G3d.c"

int main(){
	object objFighter;
	object objGroundEffect;
	u32 i, j, collision;
	
	vbInit();
	initObjects();
	
	cam.worldPosition.y             = F_NUM_UP(1000);
	cam.moveTo.z                    = cam.worldPosition.z;
	cam.moveTo.y                    = cam.worldPosition.y;
	cam.moveTo.x                    = cam.worldPosition.x;
	
	intro1();
	while((buttons = vbReadPad()) & K_A);//Wait for A button to be released
	intro3();
	while((buttons = vbReadPad()) & K_A);//Wait for A button to be released
	
	level1.mapData = (u8*)Level1;

	g3d_initObject(&objFighter,      (objectData*)Fighter);
	g3d_initObject(&objGroundEffect, (objectData*)GroundEffect);
	
	initEnemyTable();
	initObjectTable();
	
	for(i=0; i<LASER_TABLE_MAX; i++){
		g3d_initObject(&laserTable[i],       (objectData*)Laser);
		laserTable[i].properties.visible           = 0;
		laserTable[i].properties.detectCollision   = 1;
		laserTable[i].properties.lineColor         = 2;
		laserTable[i].properties.hitCube.width     = 800;
		laserTable[i].properties.hitCube.height    = 800;
		laserTable[i].properties.hitCube.depth     = 800;
	}
	
	objGroundEffect.worldPosition.x = 0 - (GAME_AREA_WIDTH>>1);
	objGroundEffect.worldSpeed.z    = GROUND_SPEED;
	
	objFighter.worldPosition.z            = F_NUM_UP(2500);
	objFighter.worldPosition.y            = (GAME_AREA_HEIGHT>>1);
	objFighter.worldSpeed.x               = F_NUM_UP(100);
	objFighter.worldSpeed.y               = F_NUM_UP(100);
	objFighter.worldSpeed.z               = GROUND_SPEED;
	objFighter.worldScale.x               = 4;
	objFighter.worldScale.y               = 4;
	objFighter.worldScale.z               = 4;
	objFighter.properties.detectCollision = 1;
	objFighter.properties.hitCube.width   = 3200;
	objFighter.properties.hitCube.height  = 1000;
	objFighter.properties.hitCube.depth   = 2200;
	
	initEnemyTable();
	initObjectTable();
	doReadMapRow(&level1, mapRowIdx);
	//Start Engine Sound
	SND_REGS[5].SxLRV = 0x11;
	SND_REGS[5].SxFQL = 0xFF;
	SND_REGS[5].SxFQH = 0x03;
	SND_REGS[5].SxEV0 = 0xF8;
	SND_REGS[5].SxEV1 = 0x01;
	SND_REGS[5].SxINT = 0x80;
	while(1){
		handleInput(&objFighter);
		g3d_moveCamera(&cam);
		
		//Keep the fighter in front of the camera
		objFighter.moveTo.x = cam.worldPosition.x;
		objFighter.moveTo.y = cam.worldPosition.y;
		objFighter.moveTo.z = cam.worldPosition.z + cam.d + F_NUM_UP(500);
		
		//Move the enemies around
		for(i=0; i<ENEMY_TABLE_MAX; i++){
			if(enemyTable[i].objData != (objectData*)0x00) doMoveEnemy(&enemyTable[i]);
		}
		//Move the fighter around
		g3d_moveObject(&objFighter);		
		//Move obstacles
		for(i=0; i<OBJECT_TABLE_MAX; i++){
			if(objectTable[i].objData != (objectData*)0x00){
				g3d_moveObject(&objectTable[i]);
				//Check collision
				collision = 0;
				g3d_detectCollision(&objFighter.worldPosition, &objFighter.properties.hitCube, &objectTable[i].worldPosition, &objectTable[i].properties.hitCube, &collision);
				if(collision == 1) {
					Channel3Play = 1;
					Channel3Max = HIT_SOUND_SIZE;
					Channel3Pos = 0;
					Channel3Data = (u8*)HitSound;
				}
				if(objectTable[i].worldPosition.z == 0) objectTable[i].objData = (objectData*)0x00;
			}
		}
		//Handle laser fighters
		for(i=0; i<LASER_TABLE_MAX; i++){
			doLaserFire(&objFighter, &laserTable[i]);
			if(laserTable[i].properties.state == 1){
				for(j=0; j<ENEMY_TABLE_MAX; j++){
					if(enemyTable[j].objData != (objectData*)0x00){
						collision = 0;
						g3d_detectCollision(&laserTable[i].worldPosition, &laserTable[i].properties.hitCube, &enemyTable[j].worldPosition, &enemyTable[j].properties.hitCube, &collision);
						if(collision == 1) {
							Channel3Play = 1;
							Channel3Max = HIT_SOUND_SIZE;
							Channel3Pos = 0;
							Channel3Data = (u8*)HitSound;
						}
					}
				}
			}
		}
		
		/********************************
		Perform some simple frame skipping
		*********************************/
		if(tick > 266 && frameSkipCount < FRAME_SKIP_MAX){
			frameSkip = 1;
			frameSkipCount++;
		}
		else{
			frameSkip = 0;
			frameSkipCount = 0;
		}
		
		/*********************************
		Add some scenery
		**********************************/
		doGroundEffects(&objGroundEffect);
		/*********************************
		Check for frame skip and draw the
		objects
		**********************************/
		if(!frameSkip){ //Simple frame skipping
			tickStart = tick;
			for(i=0; i<ENEMY_TABLE_MAX; i++){
				if(enemyTable[i].objData != (objectData*)0x00) g3d_drawObject(&enemyTable[i]);
			}
			g3d_drawObject(&objFighter);
			for(i=0; i<OBJECT_TABLE_MAX; i++){
				if(objectTable[i].objData != (objectData*)0x00) g3d_drawObject(&objectTable[i]);
			}
			for(i=0; i<LASER_TABLE_MAX; i++){
				g3d_drawObject(&laserTable[i]);
			}
			tickEnd = tick;
			if((tickEnd - tickStart) > tickMax) tickMax = tickEnd - tickStart;
			vbTextOut(0,5,0,itoa(tickMax,10,10));
			screenControl();
		}		
		
		//Throttle the frame rate
		while(tick < 266);//about 30 fps = 266
		if(tick <= 266)tick = 0;
		else tick -= 266;
		
		i=0;
		while(objectTable[i].objData == (objectData*)0x00 && i < OBJECT_TABLE_MAX) i++;
		if(i == OBJECT_TABLE_MAX && mapEnd == 0){
			mapRowIdx++;
			doReadMapRow(&level1, mapRowIdx);
		}
	}
}

void intro1(){
	u8 run;
	run = 1;
	while(run){
		vbTextOut(0,1,1,"The following is a demo of the wireframe  ");
		vbTextOut(0,1,2,"library I have been toying with. I make   ");
		vbTextOut(0,1,3,"no promises that the contents of this     ");
		vbTextOut(0,1,4,"demo will reach a final game state. My    ");
		vbTextOut(0,1,5,"goal was to learn some new things and     ");
		vbTextOut(0,1,6,"hone my programming skills as well as     ");
		vbTextOut(0,1,7,"provide some open source code for the     ");
		vbTextOut(0,1,8,"community to use as they see fit. However ");
		vbTextOut(0,1,9,"if I don't have anything better to do....");
		vbTextOut(0,1,12,"Press A to Continue");
		
		buttons = vbReadPad();
		tick = 0;
		if(K_A & buttons) run = 0;
		screenControl();
	}
	
	vbTextOut(0,1,1,"                                          ");
	vbTextOut(0,1,2,"                                          ");
	vbTextOut(0,1,3,"                                          ");
	vbTextOut(0,1,4,"                                          ");
	vbTextOut(0,1,5,"                                          ");
	vbTextOut(0,1,6,"                                          ");
	vbTextOut(0,1,7,"                                          ");
	vbTextOut(0,1,8,"                                          ");
	vbTextOut(0,1,9,"                                          ");
	vbTextOut(0,1,12,"                                         ");
}
void intro3(){
	u16 i;
	
	//load CHARS
	for (i=0; i<(213 << 4); i++){
		((u8*)CharSeg0)[i] = ((u8*)ArwingPicture)[i];
	}
	//load BGMAP
	for(i=0; i<(400<<2); i++){
		((u16*)BGMap(0))[i] = ((u16*)ArwingPictureBGM)[i];
	}
	//Play sound
	Channel1Play = 1;
	Channel1Max = ONLY_HOPE_SOUND_SIZE;
	Channel1Pos = 0;
	Channel1Data = (u8*)onlyHopeSound;
	
	while(!(K_A & buttons)){
		buttons = vbReadPad();
		tick = 0;
		screenControl();
	}
	
	//clear mem
	//load BGMAP
	for(i=0; i<(400<<2); i++){
		((u16*)BGMap(0))[i] = 0x0000;
	}
}

void intro2(){
	u8 run;
	run = 1;
	object objBuilding;
	g3d_initObject(&objBuilding, (objectData*)Building);
	objBuilding.worldPosition.z = F_NUM_UP(4000);
	objBuilding.worldScale.x = F_NUM_UP(1);
	objBuilding.worldScale.y = F_NUM_UP(1);
	objBuilding.worldScale.z = F_NUM_UP(1);
	while(run){			
		g3d_drawObject(&objBuilding);
		buttons = vbReadPad();
		tick = 0;
		if(K_A & buttons) run = 0;
		screenControl();
	}
}

void initEnemyTable(){
	u8 i;
	for(i=0; i<ENEMY_TABLE_MAX; i++) enemyTable[i].objData = (objectData*)0x00;	
}

void initObjectTable(){
	u8 i;
	for(i=0; i<OBJECT_TABLE_MAX; i++) objectTable[i].objData = (objectData*)0x00;
}

/****************************************
Purpose of this function will be to read the
map and setup a row or "wave" of objects
*****************************************/
void doReadMapRow(map* m, u32 mapRow){
	u32 idx;
	u8 rowType, loop, i, objIdx, b1, b2, b3;
	idx = 0;
	loop = 0;
	
	//Get to the map row
	for(i=0; i<mapRow; i++){
		if(m->mapData[idx] == 0x00){
			mapEnd = 1;
			return;
		}
		rowType = m->mapData[idx] & 0xF0;
		loop = m->mapData[idx] & 0x0F;
		
		if((i+1) < mapRow) idx += (loop<<1) + loop + 1;
	}
	idx++;//Move to first object byte
	//Loop through map objects
	for(i=0; i<loop; i++){
		b1 = m->mapData[idx]; // x and y
		b2 = m->mapData[idx+1];// z and obj num
		b3 = m->mapData[idx+2];// path and unused
		
		if(rowType == ROW_T_ENEMY){			
			objIdx = 0;
			while(enemyTable[objIdx].objData != (objectData*)0x00 && objIdx < ENEMY_TABLE_MAX) objIdx++;
			g3d_initObject(&enemyTable[objIdx], EnemyObjectDataTable[b2 & 0x0F]);
			enemyTable[objIdx].worldPosition.x = CELL_POSX(b1);
			enemyTable[objIdx].worldPosition.y = CELL_POSY(b1);
			enemyTable[objIdx].worldPosition.z = CELL_POSZ(b2) + FAR_Z;
			enemyTable[objIdx].worldSpeed.x = F_NUM_UP(80);
			enemyTable[objIdx].worldSpeed.y = F_NUM_UP(80);
			enemyTable[objIdx].worldSpeed.z = F_NUM_UP(80);
			enemyTable[objIdx].properties.hitCube.width = 2000;
			enemyTable[objIdx].properties.hitCube.height = 3000;
			enemyTable[objIdx].properties.hitCube.depth = 2000;
			enemyTable[objIdx].properties.detectCollision = 1;
		}
		
		if(rowType == ROW_T_OBSTACLE){
			objIdx = 0;
			while(objectTable[objIdx].objData != (objectData*)0x00 && objIdx < OBJECT_TABLE_MAX) objIdx++;
			g3d_initObject(&objectTable[objIdx], StaticObjectDataTable[b2 & 0x0F]);
			objectTable[objIdx].moveTo.x = objectTable[objIdx].worldPosition.x = CELL_POSX(b1);
			objectTable[objIdx].moveTo.y = objectTable[objIdx].worldPosition.y = CELL_POSY(b1);
			objectTable[objIdx].worldPosition.z = CELL_POSZ(b2) + FAR_Z;
			objectTable[objIdx].worldSpeed.x = F_NUM_UP(80);
			objectTable[objIdx].worldSpeed.y = F_NUM_UP(80);
			objectTable[objIdx].worldSpeed.z = GROUND_SPEED;
			objectTable[objIdx].properties.detectCollision = 1;
			objectTable[objIdx].worldScale.y = F_NUM_UP(4);
			
			objectTable[objIdx].properties.hitCube.width = 4000;
			objectTable[objIdx].properties.hitCube.height = 4000*objectTable[objIdx].worldScale.y;
			objectTable[objIdx].properties.hitCube.depth = 4000;
		}
		
		idx += 3;
	}
}

/***********************************
Handles the players laser fire
************************************/
void doLaserFire(object* o, object* laser){
	//Laser states: 0=Ready, 1=Moving, 2=End
	if(laser->properties.state == 0 && laserFrame == 0 && fireLaser > 0){
		g3d_copyVector3d(&o->worldPosition, &laser->worldPosition);
		g3d_copyVector3d(&o->worldRotation, &laser->worldRotation);
		g3d_copyVector3d(&laser->worldPosition, &laser->moveTo);
		laser->properties.visible = 1;
		laserFrame = LASER_FRAME_DELAY;
		laser->worldSpeed.z = F_NUM_UP(400);
		laser->worldSpeed.x = 0;
		laser->worldSpeed.y = 0;
		
		if((fireLaser & 0x02) == 2){
			laser->worldSpeed.x = -(laser->worldSpeed.z >> 2);
			laser->worldSpeed.z = laser->worldSpeed.z - (laser->worldSpeed.z >> 2);
		}
		if((fireLaser & 0x04) == 4){
			laser->worldSpeed.x = laser->worldSpeed.z >> 2;
			laser->worldSpeed.z = laser->worldSpeed.z - (laser->worldSpeed.z >> 2);
		}
		
		laser->properties.state = 1;
		fireLaser = 0;
	}
	if(laser->properties.state == 1){
		laser->worldPosition.x += laser->worldSpeed.x;
		laser->worldPosition.z += laser->worldSpeed.z;
		laser->properties.lineColor = (laser->properties.lineColor & 0x03) + 1;
		laser->worldRotation.z += 8;
		if(laser->worldRotation.z > 360) laser->worldRotation.z -= 360;
		if(laser->worldPosition.z >= FAR_Z){
			laser->properties.state = 2;
		}
	}
	if(laser->properties.state == 2){
		laser->properties.visible = 0;
		laser->properties.state = 0;
	}
	if(laserFrame > 0)laserFrame--;	
}

void doMoveEnemy(object* o){
	//enemy states: 0=Ready, 1=Moving, 2=Waiting
	if(movePathIdx > 14) movePathIdx = 0;
	
	if((o->properties.state & 0x0F) == 0){
		o->moveTo.x = movePath[movePathIdx];
		o->moveTo.y = movePath[movePathIdx+1];
		o->moveTo.z = movePath[movePathIdx+2];
		movePathIdx += 3;
		o->properties.state = 0x01;
	}
	if((o->properties.state & 0x0F) == 1){
		if(o->moveTo.x == o->worldPosition.x &&
		   o->moveTo.y == o->worldPosition.y &&
		   o->moveTo.z == o->worldPosition.z){
		    o->properties.state = 0xF2;
			o->worldRotation.y = 180;
		}
		else{
			o->worldRotation.y = 180;
			if(o->moveTo.x > o->worldPosition.x) o->worldRotation.y = 90;
			if(o->moveTo.x < o->worldPosition.x) o->worldRotation.y = 270;
			if(o->moveTo.z > o->worldPosition.z) o->worldRotation.y = 0;
			g3d_moveObject(o);
		}
	}
	if((o->properties.state & 0x0F) == 2){
		if((o->properties.state & 0xF0) == 0){
			o->properties.state = 0;
		}else{
			o->properties.state -= 0x10;
		}
	}
}

void doGroundEffects(object* objGroundEffect){
	u8 i;
	
	if(objGroundEffect->moveTo.z == objGroundEffect->worldPosition.z) objGroundEffect->moveTo.z = objGroundEffect->worldPosition.z - objGroundEffect->worldSpeed.z;
	if(objGroundEffect->worldPosition.z < 0)                          objGroundEffect->worldPosition.z = (FAR_Z>>1);
	g3d_moveObject(objGroundEffect);
	//Draw a bunch of ground marks for depth and movement effect
	for(i=0; i<8; i++){			
		if(!frameSkip) g3d_drawObject(objGroundEffect);
		objGroundEffect->worldPosition.z += (FAR_Z>>1);
		if(!frameSkip) g3d_drawObject(objGroundEffect);
		objGroundEffect->worldPosition.x += (GAME_AREA_WIDTH>>3);
		objGroundEffect->worldPosition.z -= (FAR_Z>>1);
	}
	objGroundEffect->worldPosition.x -= GAME_AREA_WIDTH;
}

void timeHnd(void){
	timer_int(0);
	timer_enable(0);
	timer_clearstat();
	
	u8 soundValue;

	tick++;
	
	if(Channel1Play){
		if(Channel1Nibble == 0){
			soundValue = ((Channel1Data[Channel1Pos] >> 4) & 0x0F);
			Channel1Nibble = 1;
		}
		else{
			soundValue = ((Channel1Data[Channel1Pos]) & 0x0F);
			Channel1Nibble = 0;
			Channel1Pos++;
		}
		SND_REGS[0].SxLRV = soundValue | (soundValue << 4);
		SND_REGS[0].SxINT = 0x80;
		
		if(Channel1Pos >= Channel1Max){
			Channel1Pos = 0;
			Channel1Nibble = 0;
			if(Channel1Play == 1){
				SND_REGS[0].SxINT = 0x00;
				Channel1Play = 0;
			}
		}
	}
	
	if(Channel2Play){
		if(Channel2Nibble == 0){
			soundValue = ((Channel2Data[Channel2Pos] >> 4) & 0x0F);
			Channel2Nibble = 1;
		}
		else{
			soundValue = ((Channel2Data[Channel2Pos]) & 0x0F);
			Channel2Nibble = 0;
			Channel2Pos++;
		}
		SND_REGS[1].SxLRV = soundValue | (soundValue << 4);
		SND_REGS[1].SxINT = 0x80;
		
		if(Channel2Pos >= Channel2Max){
			Channel2Pos = 0;
			Channel2Nibble = 0;
			if(Channel2Play == 1){
				SND_REGS[1].SxINT = 0x00;
				Channel2Play = 0;
			}
		}
	}
	
	if(Channel3Play){
		if(Channel3Nibble == 0){
			soundValue = ((Channel3Data[Channel3Pos] >> 4) & 0x0F);
			Channel3Nibble = 1;
		}
		else{
			soundValue = ((Channel3Data[Channel3Pos]) & 0x0F);
			Channel3Nibble = 0;
			Channel3Pos++;
		}
		SND_REGS[2].SxLRV = soundValue | (soundValue << 4);
		SND_REGS[2].SxINT = 0x80;
		
		if(Channel3Pos >= Channel3Max){
			Channel3Pos = 0;
			Channel3Nibble = 0;
			if(Channel3Play == 1){
				SND_REGS[2].SxINT = 0x00;
				Channel3Play = 0;
			}
		}
	}
	
	timer_int(1);
	timer_enable(1);
}
/*****************************************
Read the gamepad and seed the random number
counter
*****************************************/
void handleInput(object* o){
	buttons = vbReadPad();
	o->worldRotation.z = 0;
	o->worldRotation.x = 0;
	if(K_LL & buttons){
		o->worldRotation.z = -24;
		if(cam.worldPosition.x == cam.moveTo.x) cam.moveTo.x -= cam.worldSpeed.x;
		if(cam.moveTo.x < (~(GAME_AREA_WIDTH>>1) + 1))      cam.moveTo.x = (~(GAME_AREA_WIDTH>>1) + 1);
	}
	if(K_LR & buttons){
		o->worldRotation.z = 24;
		if(cam.worldPosition.x == cam.moveTo.x) cam.moveTo.x += cam.worldSpeed.x;
		if(cam.moveTo.x > (GAME_AREA_WIDTH>>1))       cam.moveTo.x = (GAME_AREA_WIDTH>>1);
	}
	if(K_LD & buttons){
		o->worldRotation.x = -24;
		if(cam.worldPosition.y == cam.moveTo.y) cam.moveTo.y += cam.worldSpeed.y;
		if(cam.moveTo.y > GAME_AREA_HEIGHT)       cam.moveTo.y = GAME_AREA_HEIGHT;
	}
	if(K_LU & buttons){
		o->worldRotation.x = 24;
		if(cam.worldPosition.y == cam.moveTo.y) cam.moveTo.y -= cam.worldSpeed.y;
		if(cam.moveTo.y < 0)                    cam.moveTo.y = 0;
	}
	
	if(K_A & buttons){
		if(fireLaser == 0){
			Channel1Play = 1;
			Channel1Max = LASER_SOUND_SIZE;
			Channel1Pos = 0;
			Channel1Data = (u8*)LaserSound;
			
			fireLaser |= 1;
			if(K_LL & buttons) fireLaser |= 2;
			if(K_LR & buttons) fireLaser |= 4;
			if(K_LU & buttons) fireLaser |= 8;
			if(K_LD & buttons) fireLaser |= 16;
		}
	}
	
	if(K_RU & buttons){
		//cam.worldRotation.x -= 8;
	}
	if(K_RD & buttons){
		//cam.worldRotation.x += 8;
	}
	if(K_RL & buttons){
		//cam.worldRotation.y -= 8;
	}
	if(K_RR & buttons){
		//cam.worldRotation.y += 8;
	}
	
	if(K_RT & buttons){
		o->worldRotation.z = 90;		
	}
	if(K_LT & buttons){
		o->worldRotation.z = -90;
	}
}

void initObjects(){
	cam.worldPosition.x = 0;
	cam.worldPosition.y = 0;
	cam.worldPosition.z = 0;
	cam.moveTo.x        = 0;
	cam.moveTo.y        = 0;
	cam.moveTo.z        = 0;
	cam.worldRotation.x = 0;
	cam.worldRotation.y = 0;
	cam.worldRotation.z = 0;
	cam.worldSpeed.x    = F_NUM_UP(100);
	cam.worldSpeed.y    = F_NUM_UP(50);
	cam.worldSpeed.z    = F_NUM_UP(100);
	cam.d               = F_NUM_UP(256);	
}

void vbInit(){
	u8 i;

	vbDisplayOn ();
	vbSetColTable ();
	
	VIP_REGS[BRTA]  = 32;
	VIP_REGS[BRTB]  = 64;
	VIP_REGS[BRTC]  = 32;
	
	HW_REGS[WCR] = 1;
	
	VIP_REGS[FRMCYC] = 0;
	
	tim_vector = (u32)timeHnd;
	timer_freq(1);
	timer_set(5);//about 8000hz
	//timer_set(10);
	timer_enable(1);
	timer_int(1);
	
	//load font
	copymem((u8*)CharSeg3, PVB_FONT, 0x2000);
	
	WA[31].head = WRLD_ON;
	WA[31].w = 380;
	WA[31].h = 224;
	WA[30].head = WRLD_END;
	
	//init sound
	for(i=0; i<32; i++)WAVEDATA1[i<<2] = 0x3F;
	SSTOP = 1;
	
	SND_REGS[0].SxEV0 = 0xFC;         	// No fadeout; volume is constant.
    SND_REGS[0].SxEV1 = 0x00;         	// Repeat it forever.
	SND_REGS[0].SxRAM = 0x00;
	SND_REGS[0].SxFQH = 0x00;
	SND_REGS[0].SxFQL = 0x00;
	SND_REGS[0].SxLRV = 0x00;
	SND_REGS[0].SxINT = 0x00;//Start sound (That's how I usually start it)
	
	SND_REGS[1].SxEV0 = 0xFC;         	// No fadeout; volume is constant.
    SND_REGS[1].SxEV1 = 0x00;         	// Repeat it forever.
	SND_REGS[1].SxRAM = 0x00;
	SND_REGS[1].SxFQH = 0x00;
	SND_REGS[1].SxFQL = 0x00;
	SND_REGS[1].SxLRV = 0x00;
	SND_REGS[1].SxINT = 0x00;//Start sound (That's how I usually start it)
	
	SND_REGS[2].SxEV0 = 0xFC;         	// No fadeout; volume is constant.
    SND_REGS[2].SxEV1 = 0x00;         	// Repeat it forever.
	SND_REGS[2].SxRAM = 0x00;
	SND_REGS[2].SxFQH = 0x00;
	SND_REGS[2].SxFQL = 0x00;
	SND_REGS[2].SxLRV = 0x00;
	SND_REGS[2].SxINT = 0x00;//Start sound (That's how I usually start it)
	
	SSTOP = 0;
	
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
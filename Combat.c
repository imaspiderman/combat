#include <libgccvb.h>
#include "G3d.h"
#include "GameFunctions.h"
#include "GameData.h"
#include "Font.h"
//Timer for FPS
volatile u8 frameSkip = 0;
volatile u8 fireLaser = 0;//1=straight 2=left 4=right 8=up 16=down
volatile u8 laserFrame = 0;
#define FRAME_SKIP_MAX 5
#define LASER_FRAME_DELAY 10
volatile u8 frameSkipCount = 0;
volatile u32 tick = 0;
volatile u32 tickStart = 0;
volatile u32 tickEnd = 0;

s32 movePath[15] = {
	F_NUM_UP(-1000), F_NUM_UP(1000), F_NUM_UP(8000),
	F_NUM_UP(-1000), F_NUM_UP(1000), F_NUM_UP(5000),
	F_NUM_UP(1000),  F_NUM_UP(1000), F_NUM_UP(4000),
	F_NUM_UP(2000),  F_NUM_UP(1000), F_NUM_UP(2000),
	F_NUM_UP(0),     F_NUM_UP(1000), F_NUM_UP(3000)
};
u8 movePathIdx = 0;

object* enemyTable[10];
u8      enemyTableCount = 0;

object* laserTable[5];
u8      laserTableCount = 0;

#define GROUND_SPEED F_NUM_UP(200)

#include "G3d.c"

int main(){
	object objFighter;
	object objPhantron1;
	object objPhantron2;
	object objPhantron3;
	object objGroundEffect;
	object objTree;
	object objBuilding;
	object objLaser1;
	object objLaser2;
	object objLaser3;
	object objLaser4;
	object objLaser5;
	u8 i;
	
	vbInit();
	initObjects();

	g3d_initObject(&objFighter,      (objectData*)Fighter);
	g3d_initObject(&objPhantron1,    (objectData*)Phantron);
	g3d_initObject(&objPhantron2,    (objectData*)Phantron);
	g3d_initObject(&objPhantron3,    (objectData*)Phantron);
	g3d_initObject(&objGroundEffect, (objectData*)GroundEffect);
	g3d_initObject(&objTree,         (objectData*)Tree);
	g3d_initObject(&objBuilding,     (objectData*)Building);
	g3d_initObject(&objLaser1,       (objectData*)Laser);
	g3d_initObject(&objLaser2,       (objectData*)Laser);
	g3d_initObject(&objLaser3,       (objectData*)Laser);
	g3d_initObject(&objLaser4,       (objectData*)Laser);
	g3d_initObject(&objLaser5,       (objectData*)Laser);
	
	objGroundEffect.worldPosition.x = F_NUM_UP(-5000);
	objGroundEffect.worldSpeed.z    = GROUND_SPEED;
	
	objTree.worldPosition.x         = F_NUM_UP(-4500);
	objTree.worldPosition.z         = FAR_Z - F_NUM_UP(2000);
	objTree.worldSpeed.z            = GROUND_SPEED;
	
	objBuilding.worldPosition.x     = F_NUM_UP(-4000);
	objBuilding.worldPosition.z     = FAR_Z - F_NUM_UP(3000);
	objBuilding.worldSpeed.z        = GROUND_SPEED;
	
	objPhantron1.moveTo.z           = objPhantron1.worldPosition.z = FAR_Z;
	objPhantron1.moveTo.y           = objPhantron1.worldPosition.y = F_NUM_UP(1000);
	objPhantron1.moveTo.x           = objPhantron1.worldPosition.x = 0;

	objPhantron1.worldRotation.y    = 180;
	objPhantron1.worldSpeed.x       = F_NUM_UP(80);
	objPhantron1.worldSpeed.y       = F_NUM_UP(80);
	objPhantron1.worldSpeed.z       = F_NUM_UP(80);
	
	objPhantron2.moveTo.z           = objPhantron2.worldPosition.z = FAR_Z;
	objPhantron2.moveTo.y           = objPhantron2.worldPosition.y = F_NUM_UP(1000);
	objPhantron2.moveTo.x           = objPhantron2.worldPosition.x = 0;

	objPhantron2.worldRotation.y    = 180;
	objPhantron2.worldSpeed.x       = F_NUM_UP(80);
	objPhantron2.worldSpeed.y       = F_NUM_UP(80);
	objPhantron2.worldSpeed.z       = F_NUM_UP(80);
	
	objPhantron3.moveTo.z           = objPhantron3.worldPosition.z = FAR_Z;
	objPhantron3.moveTo.y           = objPhantron3.worldPosition.y = F_NUM_UP(1000);
	objPhantron3.moveTo.x           = objPhantron3.worldPosition.x = 0;

	objPhantron3.worldRotation.y    = 180;
	objPhantron3.worldSpeed.x       = F_NUM_UP(80);
	objPhantron3.worldSpeed.y       = F_NUM_UP(80);
	objPhantron3.worldSpeed.z       = F_NUM_UP(80);
	
	objFighter.worldPosition.z      = F_NUM_UP(5000);
	objFighter.worldPosition.y      = F_NUM_UP(3000);
	objFighter.worldSpeed.x         = F_NUM_UP(100);
	objFighter.worldSpeed.y         = F_NUM_UP(50);
	objFighter.worldSpeed.z         = GROUND_SPEED;
	objFighter.worldScale.x         = 4;
	objFighter.worldScale.y         = 4;
	objFighter.worldScale.z         = 4;
	
	objLaser1.properties.visible           = 0;
	objLaser1.properties.detectCollision   = 1;
	objLaser1.properties.lineColor         = 2;
	objLaser2.properties.visible           = 0;
	objLaser2.properties.detectCollision   = 1;
	objLaser2.properties.lineColor         = 2;
	objLaser3.properties.visible           = 0;
	objLaser3.properties.detectCollision   = 1;
	objLaser3.properties.lineColor         = 2;
	objLaser4.properties.visible           = 0;
	objLaser4.properties.detectCollision   = 1;
	objLaser4.properties.lineColor         = 2;
	objLaser5.properties.visible           = 0;
	objLaser5.properties.detectCollision   = 1;
	objLaser5.properties.lineColor         = 2;
	
	cam.worldPosition.y             = F_NUM_UP(1000);
	cam.moveTo.z                    = cam.worldPosition.z;
	cam.moveTo.y                    = cam.worldPosition.y;
	cam.moveTo.x                    = cam.worldPosition.x;
	
	enemyTable[0]                   = &objPhantron1;
	enemyTable[1]                   = &objPhantron2;
	enemyTable[2]                   = &objPhantron3;
	enemyTableCount                 = 3;
	
	laserTable[0]                   = &objLaser1;
	laserTable[1]                   = &objLaser2;
	laserTable[2]                   = &objLaser3;
	laserTable[3]                   = &objLaser4;
	laserTable[4]                   = &objLaser5;
	laserTableCount                 = 5;
	
	while(1){
		handleInput(&objFighter);
		g3d_moveCamera(&cam);
		
		objFighter.moveTo.x = cam.worldPosition.x;
		objFighter.moveTo.y = cam.worldPosition.y;
		objFighter.moveTo.z = cam.worldPosition.z + cam.d + F_NUM_UP(500);
		
		for(i=0; i<enemyTableCount; i++){
			doMoveEnemy(enemyTable[i]);
			g3d_moveObject(enemyTable[i]);
		}
		g3d_moveObject(&objFighter);
		
		for(i=0; i<laserTableCount; i++){
			doLaserFire(&objFighter, laserTable[i]);
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
		doGroundEffects(&objTree);
		doGroundEffects(&objBuilding);
		/*********************************
		Check for frame skip and draw the
		objects
		**********************************/
		if(frameSkipCount > 0){
			vbTextOut(0,5,5, "Frameskip ");
			vbTextOut(0,16,5,itoa(frameSkipCount,10,2));
		}
		if(!frameSkip){ //Simple frame skipping
			for(i=0; i<enemyTableCount; i++){
				g3d_drawObject(enemyTable[i]);
			}
			g3d_drawObject(&objFighter);
			for(i=0; i<laserTableCount; i++){
				g3d_drawObject(laserTable[i]);
			}
			screenControl();
		}		
		
		while(tick < 266);//about 30 fps
		if(tick <= 266)tick = 0;
		else tick -= 266;
	}
}

/***********************************
Handles the players laser fire
************************************/
void doLaserFire(object* o, object* laser){
	if(laser->moveTo.z == laser->worldPosition.z){
		if(fireLaser > 0 && laserFrame == 0){
			g3d_copyVector3d(&o->worldPosition, &laser->worldPosition);
			g3d_copyVector3d(&o->worldRotation, &laser->worldRotation);
			g3d_copyVector3d(&laser->worldPosition, &laser->moveTo);
			
			laser->worldSpeed.z = F_NUM_UP(400);			
			if((fireLaser & 0x02) == 2){
				laser->worldSpeed.x = laser->worldSpeed.z >> 2;
				laser->worldSpeed.z = laser->worldSpeed.z - (laser->worldSpeed.z >> 2);
				laser->moveTo.x = laser->worldPosition.x - (FAR_Z >> 2);
			}
			if((fireLaser & 0x04) == 4){
				laser->worldSpeed.x = laser->worldSpeed.z >> 2;
				laser->worldSpeed.z = laser->worldSpeed.z - (laser->worldSpeed.z >> 2);
				laser->moveTo.x = laser->worldPosition.x + (FAR_Z >> 2);
			}
			laser->moveTo.z = FAR_Z;
			laser->properties.visible = 1;
			fireLaser = 0;
			laserFrame = LASER_FRAME_DELAY;
		}else{
			laser->properties.visible = 0;
		}
	}else{
		g3d_moveObject(laser);
		if(laserFrame > 0)laserFrame--;
		laser->properties.lineColor = (laser->properties.lineColor & 0x03) + 1;
	}
}

void doMoveEnemy(object* o){
	if(movePathIdx > 14) movePathIdx = 0;
	
	if(o->moveTo.x == o->worldPosition.x &&
	   o->moveTo.y == o->worldPosition.y &&
	   o->moveTo.z == o->worldPosition.z){
		o->moveTo.x = movePath[movePathIdx];
		o->moveTo.y = movePath[movePathIdx+1];
		o->moveTo.z = movePath[movePathIdx+2];
		movePathIdx += 3;
	}
}

void doGroundEffects(object* objGroundEffect){
	u8 i;
	
	if(objGroundEffect->moveTo.z == objGroundEffect->worldPosition.z) objGroundEffect->moveTo.z = objGroundEffect->worldPosition.z - objGroundEffect->worldSpeed.z;
	if(objGroundEffect->worldPosition.z < 0)                          objGroundEffect->worldPosition.z = F_NUM_UP(5000);
	g3d_moveObject(objGroundEffect);
	//Draw a bunch of ground marks for depth and movement effect
	for(i=0; i<5; i++){			
		if(!frameSkip) g3d_drawObject(objGroundEffect);
		objGroundEffect->worldPosition.z += F_NUM_UP(5000);
		if(!frameSkip) g3d_drawObject(objGroundEffect);
		objGroundEffect->worldPosition.x += F_NUM_UP(3000);
		objGroundEffect->worldPosition.z -= F_NUM_UP(5000);
	}
	objGroundEffect->worldPosition.x -= F_NUM_UP(15000);
}

void timeHnd(void){
	timer_int(0);
	timer_enable(0);
	timer_clearstat();

	tick++;
	
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
		if(cam.moveTo.x < F_NUM_UP(-5000))      cam.moveTo.x = F_NUM_UP(-5000);
	}
	if(K_LR & buttons){
		o->worldRotation.z = 24;
		if(cam.worldPosition.x == cam.moveTo.x) cam.moveTo.x += cam.worldSpeed.x;
		if(cam.moveTo.x > F_NUM_UP(5000))       cam.moveTo.x = F_NUM_UP(5000);
	}
	if(K_LD & buttons){
		o->worldRotation.x = -24;
		if(cam.worldPosition.y == cam.moveTo.y) cam.moveTo.y += cam.worldSpeed.y;
		if(cam.moveTo.y > F_NUM_UP(2000))       cam.moveTo.y = F_NUM_UP(2000);
	}
	if(K_LU & buttons){
		o->worldRotation.x = 24;
		if(cam.worldPosition.y == cam.moveTo.y) cam.moveTo.y -= cam.worldSpeed.y;
		if(cam.moveTo.y < 0)                    cam.moveTo.y = 0;
	}
	
	if(K_A & buttons){
		if(fireLaser == 0){
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
	vbDisplayOn ();
	vbSetColTable ();
	
	VIP_REGS[BRTA]  = 32;
	VIP_REGS[BRTB]  = 64;
	VIP_REGS[BRTC]  = 32;
	
	HW_REGS[WCR] = 1;
	
	VIP_REGS[FRMCYC] = 0;
	
	tim_vector = (u32)timeHnd;
	timer_freq(1);
	timer_set(6);//about 8000hz
	//timer_set(10);
	timer_enable(1);
	timer_int(1);
	
	//load font
	copymem((u8*)CharSeg3, PVB_FONT, 0x2000);
	
	WA[31].head = WRLD_ON;
	WA[31].w = 200;
	WA[31].h = 200;
	WA[30].head = WRLD_END;
	
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
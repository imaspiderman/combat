#include <libgccvb.h>
#include "G3d.h"
#include "GameFunctions.h"
#include "GameData.h"
#include "Font.h"
#include "crossHairsChars.h"
#include "crossHairsBGMap.h"
//Timer for FPS
volatile u8 FPS = 0;
volatile u32 tick = 0;
volatile u32 tickStart = 0;
volatile u32 tickEnd = 0;
volatile u32 tickDraw = 0;
volatile u32 tickRender = 0;
volatile u32 tickProject = 0;

u8 modelnum = 0;
s32 vertexCount = 0;
object objFighter;
object objTree[5];

#include "G3d.c"

int main(){
	u8 i;
	
	vbInit();
	initObjects();
	g3d_initObject(&objFighter);
	for(i=0;i<5;i++){
		g3d_initObject(&objTree[i]);
		objTree[i].objData = (objectData*)Tree;
		objTree[i].worldPosition.x = i * F_NUM_UP(1000);
		objTree[i].worldPosition.z = i * F_NUM_UP(1000) + F_NUM_UP(1000);
		objTree[i].worldPosition.y = 0;
		objTree[i].worldScale.x = F_NUM_UP(2);
		objTree[i].worldScale.y = F_NUM_UP(2);
		objTree[i].worldScale.z = F_NUM_UP(2);
	}
	
	objFighter.objData = (objectData*)Fighter;
	objFighter.worldPosition.z = F_NUM_UP(5000);
	objFighter.worldPosition.y = F_NUM_UP(3000);
	objFighter.worldSpeed.x = F_NUM_UP(50);
	objFighter.worldSpeed.y = F_NUM_UP(50);
	objFighter.worldSpeed.z = F_NUM_UP(50);
	
	cam.worldPosition.z = F_NUM_UP(-10);
	cam.worldPosition.y = F_NUM_UP(1000);
	cam.moveTo.z = cam.worldPosition.z;
	cam.moveTo.y = cam.worldPosition.y;
	cam.moveTo.x = cam.worldPosition.x;
	
	while(1){		
		tick = 0;
		tickDraw = 0;
		tickRender = 0;
		tickProject = 0;
		
		handleInput(&objFighter);
		g3d_moveCamera(&cam);
		
		objFighter.moveTo.x = cam.worldPosition.x;
		objFighter.moveTo.y = cam.worldPosition.y;
		objFighter.moveTo.z = cam.worldPosition.z + cam.d + F_NUM_UP(500);
		
		g3d_moveObject(&objFighter);
		g3d_drawObject(&objFighter);
		for(i=0; i<5; i++){
			g3d_drawObject(&objTree[i]);
		}
	
		screenControl();
		while(tick < 266);//about 30 fps
	}
}

void timeHnd(void){
	timer_int(0);
	timer_enable(0);
	timer_clearstat();
	
	FPS = 0;
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
	}
	if(K_LR & buttons){
		o->worldRotation.z = 24;
		if(cam.worldPosition.x == cam.moveTo.x) cam.moveTo.x += cam.worldSpeed.x;
	}
	if(K_LD & buttons){
		o->worldRotation.x = -24;
		if(cam.worldPosition.y == cam.moveTo.y) cam.moveTo.y += cam.worldSpeed.y;
	}
	if(K_LU & buttons){
		o->worldRotation.x = 24;
		if(cam.worldPosition.y == cam.moveTo.y) cam.moveTo.y -= cam.worldSpeed.y;
	}
	
	if(K_RU & buttons){
		if(cam.worldPosition.z == cam.moveTo.z) cam.moveTo.z += cam.worldSpeed.z;
	}
	if(K_RD & buttons){
		if(cam.worldPosition.z == cam.moveTo.z) cam.moveTo.z -= cam.worldSpeed.z;
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
	cam.moveTo.x = 0;
	cam.moveTo.y = 0;
	cam.moveTo.z = 0;
	cam.worldRotation.x = 0;
	cam.worldRotation.y = 0;
	cam.worldRotation.z = 0;
	cam.worldSpeed.x = F_NUM_UP(50);
	cam.worldSpeed.y = F_NUM_UP(50);
	cam.worldSpeed.z = F_NUM_UP(50);
	cam.d = F_NUM_UP(256);	
}

void vbInit(){
	u16 i;
	
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
	
	/*
	//load cross hairs chars
	for(i=0; i<39*16; i++){
		((u8*)CharSeg0)[i] = crossHairsChars[i];
	}
	//load cross hairs bgmap
	for(i=0; i<128*4; i++){
		((u16*)BGMap(0))[i] = crossHairsBGMap[i];
	}

	//world info
	WA[31].head = WRLD_ON;
	WA[31].w = 64;
	WA[31].h = 64;
	WA[31].gp = 2;
	WA[31].gx = (SCREEN_WIDTH >> 1) - (WA[31].h >> 1);
	WA[31].gy = (SCREEN_HEIGHT >> 1) - (WA[31].w >> 1);
	
	WA[30].head = WRLD_END;
	*/
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
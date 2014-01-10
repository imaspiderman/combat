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

#include "G3d.c"

int main(){
	vector3d scale;
	scale.x = F_NUM_UP(10);
	scale.y = F_NUM_UP(10);
	scale.z = F_NUM_UP(10);
	
	vbInit();
	initObjects();
	g3d_initObject(&objFighter);
	objFighter.objData = (objectData*)Fighter;
	objFighter.worldRotation.x = 90;
	objFighter.worldRotation.z = 180;
	objFighter.worldScale = scale; 
	
	cam.worldPosition.z = F_NUM_UP(-400);
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
		g3d_drawObject(&objFighter);
	
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
	if(K_LL & buttons){
		WA[31].gx -= 2;
	}
	if(K_LR & buttons){
		WA[31].gx += 2;
	}
	if(K_LD & buttons){
		WA[31].gy += 2;
	}
	if(K_LU & buttons){
		WA[31].gy -= 2;
	}
	if(K_RU & buttons){
		if(cam.worldPosition.z == cam.moveTo.z) cam.moveTo.z += cam.worldSpeed.z;
	}
	if(K_RD & buttons){
		if(cam.worldPosition.z == cam.moveTo.z) cam.moveTo.z -= cam.worldSpeed.z;
	}
	if(K_RT & buttons){
		o->worldRotation.y += 8;
		if(o->worldRotation.y > 359) o->worldRotation.y = o->worldRotation.y - 360;
	}
	if(K_LT & buttons){
		o->worldRotation.y -= 8;
		if(o->worldRotation.y < -359) o->worldRotation.y = o->worldRotation.y + 360;
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
	cam.worldSpeed.x = F_NUM_UP(10);
	cam.worldSpeed.y = F_NUM_UP(10);
	cam.worldSpeed.z = F_NUM_UP(10);
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
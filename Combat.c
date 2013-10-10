#include <libgccvb.h>
#include "G3d.h"
#include "GameFunctions.h"
#include "GameData.h"
#include "Font.h"
//Timer for FPS
volatile u8 FPS = 0;
volatile u32 tick = 0;
volatile u32 tickStart = 0;
volatile u32 tickEnd = 0;

u8 modelnum = 0;
s32 vertexCount = 0;
#include "G3d.c"

int main(){
	object o_player;
	object o_enemy;
	object o_map;
	
	vbInit();
	initObjects();
	
	g3d_initObject(&o_enemy);
	g3d_initObject(&o_player);
	g3d_initObject(&o_map);
	
	o_player.objData = (objectData*)tank;
	o_player.worldPosition.x = 4000;
	o_player.worldPosition.z = 16000;
	o_player.moveTo.z = o_player.worldPosition.z;
	o_player.moveTo.x = o_player.worldPosition.x;
	o_player.worldRotation.y = 90;
	
	o_enemy.objData = (objectData*)tank;
	o_enemy.worldPosition.x = 76000;
	o_enemy.worldPosition.z = 64000;
	o_enemy.moveTo.z = o_enemy.worldPosition.z;
	o_enemy.moveTo.x = o_enemy.worldPosition.x;
	o_enemy.worldRotation.y = -90;
	
	o_map.objData = (objectData*)map;
	o_map.worldPosition.x = 0;
	o_map.worldPosition.y = 0;
	o_map.worldPosition.z = 8000;
	o_map.properties.lineColor = 1;
	
	cam.worldPosition.z = -32000;
	cam.worldPosition.y = 48000;
	cam.worldPosition.x = 40000;
	cam.moveTo.z = cam.worldPosition.z;
	cam.moveTo.y = cam.worldPosition.y;
	cam.moveTo.x = cam.worldPosition.x;
	cam.worldRotation.x = 40;
	
	while(1){		
		tick = 0;
		
		handleInput(&o_player);
		
		g3d_moveCamera(&cam);
		g3d_moveObject(&o_player);
		g3d_moveObject(&o_enemy);
		
		g3d_drawObject(&o_player);
		g3d_drawObject(&o_enemy);
		g3d_drawObject(&o_map);
		
		screenControl();
		while(tick < 266);//about 30 fps
		vbTextOut(0,5,1,itoa(tick,10,8));
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
		cam.worldRotation.x -= 8;
	}
	if(K_LR & buttons){
		cam.worldRotation.x += 8;
	}
	if(K_LD & buttons){
		cam.moveTo.y -= F_NUM_UP(50);
	}
	if(K_LU & buttons){
		cam.moveTo.y += F_NUM_UP(50);
	}
	if(K_RU & buttons){
		if(cam.worldPosition.z == cam.moveTo.z) cam.moveTo.z += cam.worldSpeed.z;
	}
	if(K_RD & buttons){
		if(cam.worldPosition.z == cam.moveTo.z) cam.moveTo.z -= cam.worldSpeed.z;
	}
	if(K_RT & buttons){
		o->worldRotation.y += 8;
	}
	if(K_LT & buttons){
		o->worldRotation.y -= 8;
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
	cam.worldSpeed.x = F_NUM_UP(100);
	cam.worldSpeed.y = F_NUM_UP(100);
	cam.worldSpeed.z = F_NUM_UP(100);
	cam.d = F_NUM_UP(256);	
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

	//world info
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
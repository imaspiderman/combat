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
#define NUM_MODELS 1
objectData* models[]={
	(objectData*)tieFighterModel
};
u8 modelnum = 0;
s32 vertexCount = 0;
#include "G3d.c"

int main(){
	object o_curr;
	
	vbInit();
	initObjects();
	g3d_initObject(&o_curr);
	o_curr.objData = models[modelnum];
	o_curr.worldPosition.z = F_NUM_UP(1000);
	o_curr.worldScale.x = F_NUM_UP(3);
	o_curr.worldScale.y = F_NUM_UP(3);
	o_curr.worldScale.z = F_NUM_UP(3);
	
	while(1){		
		tick = 0;
		handleInput(&o_curr);
		o_curr.objData = models[modelnum];
		
		moveObject(&o_curr);
		g3d_drawObject(&o_curr);
		screenControl();
		FPS++;
	}
}

void moveObject(object* o){
	o->worldRotation.x += o->rotation.x;
	o->worldRotation.y += o->rotation.y;
	o->worldRotation.z += o->rotation.z;
	if(o->worldRotation.x > 359) o->worldRotation.x = o->worldRotation.x - 359;
	if(o->worldRotation.y > 359) o->worldRotation.y = o->worldRotation.y - 359;
	if(o->worldRotation.z > 359) o->worldRotation.z = o->worldRotation.z - 359;
	if(o->worldRotation.x < -359) o->worldRotation.x = o->worldRotation.x + 359;
	if(o->worldRotation.y < -359) o->worldRotation.y = o->worldRotation.y + 359;
	if(o->worldRotation.z < -359) o->worldRotation.z = o->worldRotation.z + 359;
	
	if(cam.worldRotation.x > 359) cam.worldRotation.x = cam.worldRotation.x - 359;
	if(cam.worldRotation.y > 359) cam.worldRotation.y = cam.worldRotation.y - 359;
	if(cam.worldRotation.z > 359) cam.worldRotation.z = cam.worldRotation.z - 359;
	if(cam.worldRotation.x < -359) cam.worldRotation.x = cam.worldRotation.x + 359;
	if(cam.worldRotation.y < -359) cam.worldRotation.y = cam.worldRotation.y + 359;
	if(cam.worldRotation.z < -359) cam.worldRotation.z = cam.worldRotation.z + 359;
}

void timeHnd(void){
	timer_int(0);
	timer_enable(0);
	timer_clearstat();
	
	//vbTextOut(0,5,0,itoa(FPS,10,2));	
	
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
		cam.worldPosition.x -= F_NUM_UP(50);
	}
	if(K_LR & buttons){
		cam.worldPosition.x += F_NUM_UP(50);
	}
	if(K_LD & buttons){
		cam.worldPosition.y += F_NUM_UP(50);
	}
	if(K_LU & buttons){
		cam.worldPosition.y -= F_NUM_UP(50);
	}
	if(K_RU & buttons){
		cam.worldPosition.z += F_NUM_UP(50);
	}
	if(K_RD & buttons){
		cam.worldPosition.z -= F_NUM_UP(50);
	}
	if(K_RT & buttons){
		cam.worldRotation.z += 8;
	}
	if(K_LT & buttons){
		cam.worldRotation.z -= 8;
	}
	if(K_A & buttons){
		modelnum++;
		if(modelnum >= NUM_MODELS) modelnum = 0;
	}
}

void initObjects(){
	cam.worldPosition.x = 0;
	cam.worldPosition.y = 0;
	cam.worldPosition.z = 0;
	cam.worldRotation.x = 0;
	cam.worldRotation.y = 0;
	cam.worldRotation.z = 0;
	cam.d = F_NUM_UP(128);	
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
	//timer_set(50000);//Every 1 second
	timer_set(10);
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
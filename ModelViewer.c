#include "GameFunctions.h"
#include "GameData.h"
#include "GameTypes.h"
#include "G3d.h"
#include "Font.h"
//Timer for FPS
volatile u8 FPS = 0;

#include "G3d.c"

int main(){
	vbInit();
	while(1){
		handleInput();
		drawObject();
		screenControl();
		FPS++;
	}
}

void timeHnd(void){
	timer_int(0);
	timer_enable(0);
	timer_clearstat();
	
	vbTextOut(0,0,0,itoa(FPS,10,8));
	FPS = 0;
	
	timer_int(1);
	timer_enable(1);
}
/*****************************************
Read the gamepad and seed the random number
counter
*****************************************/
void handleInput(){
	u8 o;
	buttons = vbReadPad();
	if(K_LL & buttons){
	}
	if(K_LR & buttons){
	}
	if(K_LD & buttons){
	}
	if(K_LU & buttons){
	}
	if(K_A & buttons){
	}
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
	
	cam.worldPosition.x = 0;
	cam.worldPosition.y = 0;
	cam.worldPosition.z = 0;
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
	timer_freq(0);
	timer_set(10000);
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
	
	pixels=((dx>dy)?(dx):(dy))+1;
	
	CACHE_ENABLE;
	if(dy<dx){
		err=(dx>>1);
		sz=(sz)*(F_NUM_UP(dz)/((dx==0)?(1):(dx)));
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
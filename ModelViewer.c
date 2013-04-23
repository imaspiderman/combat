#include "GameFunctions.h"
#include "GameData.h"
#include "GameTypes.h"
#include "G3d.h"
#include "Font.h"
//Timer for FPS
volatile u8 FPS = 0;
volatile u32 tick = 0;
u32 startTick = 0;
u32 diffTick = 0;
#define NUM_MODELS 2
objectData* models[]={
	(objectData*)tieFighterModel,
	(objectData*)trench
};
u8 modelnum = 0;

s32 vertexBuffer[2048];
u32 numVertices=0;

#include "G3d.c"

int main(){
	object o_curr;
	
	vbInit();
	initObjects();
	initObject(&o_curr);
	o_curr.objData = models[modelnum];
	o_curr.worldPosition.z = F_NUM_UP(1000);
	o_curr.rotation.x = 2;
	
	while(1){		
		tick = 0;
		startTick = tick;
		handleInput(&o_curr);
		o_curr.objData = models[modelnum];
		
		startTick = tick;
		moveObject(&o_curr);
		
		startTick = tick;
		if(o_curr.worldPosition.z >= (cam.worldPosition.z))drawObject(&o_curr);
		
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
}

void timeHnd(void){
	timer_int(0);
	timer_enable(0);
	timer_clearstat();
	
	vbTextOut(0,5,0,itoa(FPS,10,2));
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
		o->worldRotation.y += 10;
	}
	if(K_LT & buttons){
		o->worldRotation.y -= 10;
	}
	if(K_A & buttons){
		modelnum++;
		if(modelnum >= NUM_MODELS) modelnum = 0;
	}
}

void renderVector3d(object* obj, vector3d* v, vector3d* o){
	vector3d t;
	g3d_rotateAllAxis(obj->worldRotation.x,obj->worldRotation.y,obj->worldRotation.z,v,&t);
	g3d_copyVector3d(&t,o);
	g3d_translate(obj->worldPosition.x,obj->worldPosition.y,obj->worldPosition.z,o,&t);
	g3d_copyVector3d(&t,o);
	g3d_cameraTranslate(cam.worldPosition.x,cam.worldPosition.y,cam.worldPosition.z,o,&t);
	g3d_copyVector3d(&t,o);
	if(cam.worldRotation.x != 0 || cam.worldRotation.y != 0 || cam.worldRotation.z != 0){
		g3d_cameraRotateAllAxis(cam.worldRotation.x,cam.worldRotation.y,cam.worldRotation.z,o,&t);
		g3d_copyVector3d(&t,o);
	}
}

/**************************
This procedure actually draws an
object
**************************/
void drawObject(object* o){
	s32 vertices,lines,v,firstV,verts,i,vertex;
	vector3d v1,v2;
	
	if(o->properties.visible == 0) return;
	
	vertices=o->objData->vertexSize;//total elements in array
	lines=o->objData->lineSize;//Total line endpoints
	verts=o->objData->faceSize;//total vertices per section

	v=0;
	while(v < vertices){
		v1.x = o->objData->data[v];
		v1.y = o->objData->data[v+1];
		v1.z = o->objData->data[v+2];
		renderVector3d(o, &v1, &v2);
		vertexBuffer[v] = v2.x;
		vertexBuffer[v+1] = v2.y;
		vertexBuffer[v+2] = v2.z;
		v+=3;
	}

	while(v < (lines+vertices)){
		firstV = v;
		vertex = ((o->objData->data[v]<<1)+o->objData->data[v]);
		v1.x = vertexBuffer[vertex];
		v1.y = (~vertexBuffer[vertex+1]) + 1;
		v1.z = vertexBuffer[vertex+2];
		v1.w = F_NUM_UP(1);

		for(i=1; i<verts; i++){
			v++;
			vertex = ((o->objData->data[v]<<1)+o->objData->data[v]);
			v2.x = vertexBuffer[vertex];
			v2.y = (~vertexBuffer[vertex+1]) + 1;
			v2.z = vertexBuffer[vertex+2];
			v2.w = F_NUM_UP(1);
			
			drawLine(&v1,&v2,3,o);
			
			v1.x = v2.x;
			v1.y = v2.y;
			v1.z = v2.z;
		}
		//This causes a final line to be drawn back to the starting vertex
		if(verts>2){
			vertex = ((o->objData->data[firstV]<<1) + o->objData->data[firstV]);
			v2.x = vertexBuffer[vertex];
			v2.y =(~vertexBuffer[vertex+1]) + 1;
			v2.z = vertexBuffer[vertex+2];
			
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
	timer_freq(1);
	timer_set(50000);
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
	vz = v1->z;
	
	CACHE_ENABLE;
	#ifndef __ASM_CODE
	if(dy<dx){
		err=(dx>>1);
		sz=(sz)*(F_NUM_UP(dz)/((dx==0)?(1):(dx)));
		for(p=0;p<pixels;p++){
			if(vz >= (cam.worldPosition.z - cam.d)) drawPoint(vx,vy,color,(F_NUM_DN(vz)>>PARALLAX_SHIFT));
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
			if(vz >= (cam.worldPosition.z - cam.d)) drawPoint(vx,vy,color,(F_NUM_DN(vz)>>PARALLAX_SHIFT));
			err+=dx;
			if(err>dy){
				vx+=sx;
				err-=dy;
			}
			vz+=sz;
			vy+=sy;
		}
	}
	#else
	//The VB only runs at 20Mhz but it has 30 registers.
	//Lets use em all and squeeze out as much processing speed as possible.
	#endif
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
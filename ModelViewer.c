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
s32 maxPixels = 0;
s32 counter = 0;
#define NUM_MODELS 1
objectData* models[]={
	(objectData*)tieFighterModel
};
u8 modelnum = 0;

vector3d vertexBuffer[1000];
u32 numVertices=0;

#include "G3d.c"

int main(){
	object o_curr;
	
	vbInit();
	initObjects();
	initObject(&o_curr);
	o_curr.objData = models[modelnum];
	o_curr.worldPosition.z = F_NUM_UP(1000);
	
	while(1){		
		tick = 0;
		counter = 0;
		startTick = tick;
		handleInput(&o_curr);
		o_curr.objData = models[modelnum];
		
		moveObject(&o_curr);
		drawObject(&o_curr);
		
		vbTextOut(0,5,3,itoa(maxPixels,10,8));
		vbTextOut(0,5,4,itoa(counter,10,8));
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

void renderVector3d(object* obj, vector3d* v, vector3d* o, u8 initHitCube){
	vector3d t;
	vector3d* tp;
	vector3d* tpt;
	tp = &t;
	//Transformations
	g3d_scale(&obj->worldScale,v,tp);
	tpt = o;
	o = tp;
	tp = tpt;
	g3d_rotateAllAxis(obj->worldRotation.x,obj->worldRotation.y,obj->worldRotation.z,o,tp);
	tpt = o;
	o = tp;
	tp = tpt;
	g3d_translate(obj->worldPosition.x,obj->worldPosition.y,obj->worldPosition.z,o,tp);
	tpt = o;
	o = tp;
	tp = tpt;
	g3d_cameraTranslate(cam.worldPosition.x,cam.worldPosition.y,cam.worldPosition.z,o,tp);
	tpt = o;
	o = tp;
	tp = tpt;
	if(cam.worldRotation.x != 0 || cam.worldRotation.y != 0 || cam.worldRotation.z != 0){
		g3d_cameraRotateAllAxis(cam.worldRotation.x,cam.worldRotation.y,cam.worldRotation.z,o,tp);
		tpt = o;
		o = tp;
		tp = tpt;
	}
	//Projection calculation
	o->sx=F_NUM_DN(F_ADD(F_DIV(F_MUL(o->x,cam.d),F_ADD(cam.d,o->z)),F_NUM_UP(SCREEN_WIDTH>>1)));
	o->sy=F_NUM_DN(F_ADD(F_DIV(F_MUL(o->y,cam.d),F_ADD(cam.d,o->z)),F_NUM_UP(SCREEN_HEIGHT>>1)));
	
	//Collision cube
	if(initHitCube == 0){
		if(o->x < obj->properties.hitCube.minX) obj->properties.hitCube.minX = o->x;
		if(o->x > obj->properties.hitCube.maxX) obj->properties.hitCube.maxX = o->x;
		if(o->y < obj->properties.hitCube.minY) obj->properties.hitCube.minY = o->y;
		if(o->y > obj->properties.hitCube.maxY) obj->properties.hitCube.maxY = o->y;
		if(o->z < obj->properties.hitCube.minZ) obj->properties.hitCube.minZ = o->z;
		if(o->z > obj->properties.hitCube.maxZ) obj->properties.hitCube.maxZ = o->z;
	}else{
		obj->properties.hitCube.minX = obj->properties.hitCube.maxX = o->x;
		obj->properties.hitCube.minY = obj->properties.hitCube.maxY = o->y;
		obj->properties.hitCube.minZ = obj->properties.hitCube.maxZ = o->z;
	}
}

/**************************
This procedure actually draws an
object
**************************/
void drawObject(object* o){
	s32 vertices,lines,v,firstV,verts,i;
	vector3d v1,v2;
	vector3d* v1p;
	vector3d* v2p;
	vector3d* vtp;
	v1p = &v1;
	v2p = &v2;
	if(o->properties.visible == 0) return;
	//Check if object is in view by rendering its center point to the screen
	//and checking bounds
	
	vertices=o->objData->vertexSize;//total elements in array
	lines=o->objData->lineSize;//Total line endpoints
	verts=o->objData->faceSize;//total vertices per section

	v=0;
	i=0;
	//Load and render all distinct vertices into the vertex buffer;
	//This will render all object vertices based on the objects position,rotation etc..
	startTick = tick;
	while(v < vertices){
		v1.x = o->objData->data[v];
		v1.y = o->objData->data[v+1];
		v1.z = o->objData->data[v+2];
		
		renderVector3d(o, &v1, &v2, ((v==0)?(1):(0)));
		vertexBuffer[i++] = v2;
		v+=3;
	}
	vbTextOut(0,5,1,itoa((tick-startTick),10,8));

	//This reads the "faces" section of the data and draws lines between points.
	//We'll use the vertex buffer's already rendered vertices
	startTick = tick;
	while(v < (lines+vertices)){
		firstV = v;
		v1p = &vertexBuffer[o->objData->data[v]];

		for(i=1; i<verts; i++){
			v++;
			v2p = &vertexBuffer[o->objData->data[v]];
			drawLine(v1p,v2p,3,o);
			vtp = v2p;
			v1p = v2p;
			v2p = vtp;
			
		}
		v++;
	}
	vbTextOut(0,5,2,itoa((tick-startTick),10,8));
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
	o->worldSpeed.x = 0;
	o->worldSpeed.y = 0;
	o->worldSpeed.z = 0;
	o->speed.x = 0;
	o->speed.y = 0;
	o->speed.z = 0;
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
	timer_set(100);//Every 1 second
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

	vx = v1->sx;
	vy = v1->sy;
	vx2 = v2->sx;
	vy2 = v2->sy;
	
	//Simple screen clipping
	if((vx > SCREEN_WIDTH && vx2 > SCREEN_WIDTH) ||
	   (vx < 0 && vx2 < 0) ||
	   (vy > SCREEN_HEIGHT && vy2 > SCREEN_HEIGHT) ||
	   (vy < 0 && vy2 < 0) ||
	   (v1->z <= (cam.worldPosition.z + cam.d) && v2->z <= (cam.worldPosition.z + cam.d))) return;

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
	
	if(pixels > maxPixels) {
		maxPixels = pixels;
	}
	
	CACHE_ENABLE;
	#ifndef __ASM_CODE
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
			counter++;
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
			counter++;
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
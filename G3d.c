/****************************
Copies one vector to another
*****************************/
void inline g3d_copyVector3d(vector3d* from, vector3d* to){
	to->x = from->x;
	to->y = from->y;
	to->z = from->z;
	to->sx = from->sx;
	to->sy = from->sy;
}

/****************************
Scales a vector by a given fixed point vector
*****************************/
void inline g3d_scale(vector3d* factor, vector3d* v, vector3d* o){
	if(factor->x == 1 && factor->y == 1 && factor->z == 1){
		g3d_copyVector3d(v,o);
		return;
	}
	o->x = F_MUL(v->x,factor->x);
	o->y = F_MUL(v->y,factor->y);
	o->z = F_MUL(v->z,factor->z);
}

/***********************************
Rotates a point around the X axis
************************************/
void inline g3d_rotateXAxis(s32 degrees, vector3d* v, vector3d* o){
#ifndef __ASM_CODE
	o->x = v->x;
	o->z = (F_MUL(v->z, cosine[degrees])) + (F_MUL(sine[degrees], v->y));
	o->y = (F_MUL(v->z, -sine[degrees])) + (F_MUL(cosine[degrees], v->y));
#else
	o->x = v->x;
	asm(
	"ld.w %[vx],r6\n"
	"ld.w %[vy],r7\n"
	"ld.w %[vz],r8\n"
	"ld.w %[deg],r9\n"
	"shl 2,r9\n"
	"add r9,%[cos]\n"
	"add r9,%[sin]\n"
	"ld.w 0x0[%[cos]],r10\n"
	"ld.w 0x0[%[sin]],r11\n"
	//F_MUL(v->z, cosine[degrees])
	"mov r8,r12\n"
	"mul r10,r12\n"
	"sar %[fixShift],r12\n"
	//+ (F_MUL(sine[degrees],v->y))
	"mov r7,r13\n"
	"mul r11,r13\n"
	"sar %[fixShift],r13\n"
	"add r12,r13\n"
	"st.w r13, 0x08[%[o]]\n"
	//F_MUL(v->z, -sine[degrees])
	"not r11,r11\n"
	"addi 1,r11,r11\n"
	"mov r8,r12\n"
	"mul r11,r12\n"
	"sar %[fixShift],r12\n"
	//+ F_MUL(cosine[degrees], v->y)
	"mov r10,r13\n"
	"mul r7,r13\n"
	"sar %[fixShift],r13\n"
	"add r12,r13\n"
	"st.w r13,0x04[%[o]]\n"
	:/*output*/
	:[vx] "m" (v->x),[vy] "m" (v->y),[vz] "m" (v->z),
	 [o] "r" ((vector3d*)o),[cos] "r" (cosine),[sin] "r" (sine),
	 [deg] "m" (degrees),[fixShift] "i" (FIXED_SHIFT)
	:"r6","r7","r8","r9","r10","r11","r12","r13"
	);
#endif
}

/**********************************
Rotates a point around the Y axis
***********************************/
void inline g3d_rotateYAxis(s32 degrees, vector3d* v, vector3d* o){
	o->y = v->y;
	o->x = (F_MUL(v->x, cosine[degrees])) + (F_MUL(sine[degrees], v->z));
	o->z = (F_MUL(v->x, -sine[degrees])) + (F_MUL(cosine[degrees], v->z));
}

/**********************************
Rotates a point around the Z axis
***********************************/
void inline g3d_rotateZAxis(s32 degrees, vector3d* v, vector3d* o){
	o->z = v->z;
	o->x = (F_MUL(v->x, cosine[degrees])) + (F_MUL(sine[degrees], v->y));
	o->y = (F_MUL(v->x, -sine[degrees])) + (F_MUL(cosine[degrees], v->y));
}

/***********************************************
This will rotate a point around all 3 axis.
It performs checks on the rotation values to make sure the
values are not zero before performing the rotation calculations.
rx,ry,and rz are degrees and are NOT fixed point
Make sure the rotation values are between -360 and 360
************************************************/
void inline g3d_rotateAllAxis(s32 rx, s32 ry, s32 rz, vector3d* v, vector3d* o){
	vector3d t;
	
	if(rx==0 && ry==0 && rz==0){
		g3d_copyVector3d(v,o);
		return;
	}
	
	if(rx<0) rx=359+rx;
	if(ry<0) ry=359+ry;
	if(rz<0) rz=359+rz;
	
	g3d_copyVector3d(v,&t);
	if(rx != 0){
		g3d_rotateXAxis(rx,&t,o);
		g3d_copyVector3d(o,&t);
	}
	if(ry != 0){
		g3d_rotateYAxis(ry,&t,o);
		g3d_copyVector3d(o,&t);
	}
	if(rz != 0){
		g3d_rotateZAxis(rz,&t,o);
		g3d_copyVector3d(o,&t);
	}
	g3d_copyVector3d(&t,o);
}

/*******************************
This translates or moves a point
********************************/
void inline g3d_translate(s32 x, s32 y, s32 z, vector3d* v, vector3d* o){
	o->x = v->x + x;
	o->y = v->y + y;
	o->z = v->z + z;
}

/********************************
This performs the rotations for the camera.
It just rotates a point in the opposite direction
of the cameras rotation angles.
*********************************/
void inline g3d_cameraRotateAllAxis(s32 rx, s32 ry, s32 rz, vector3d* v, vector3d* o){
	rx = (~rx)+1;
	ry = (~ry)+1;
	rz = (~rz)+1;
	g3d_rotateAllAxis(rx,ry,rz,v,o);
}

/**********************************
This performs the camera translate or move by
calculating the difference between the camera's position
and the points position.
***********************************/
void inline g3d_cameraTranslate(s32 x, s32 y, s32 z, vector3d* v, vector3d* o){
	o->x = v->x - x;
	o->y = v->y - y;
	o->z = v->z - z;
}

/**********************************
This performs all affine transformation
functions against a vector3d object
***********************************/
void g3d_renderVector3d(object* obj, vector3d* v, vector3d* o, u8 initHitCube){
	vector3d t;
	//Transformations
	g3d_scale(&obj->worldScale,v,&t);
	g3d_copyVector3d(&t,o);
	
	g3d_rotateAllAxis(obj->worldRotation.x,obj->worldRotation.y,obj->worldRotation.z,o,&t);
	g3d_copyVector3d(&t,o);
	
	g3d_translate(obj->worldPosition.x,obj->worldPosition.y,obj->worldPosition.z,o,&t);
	g3d_copyVector3d(&t,o);
	
	g3d_cameraTranslate(cam.worldPosition.x,cam.worldPosition.y,cam.worldPosition.z,o,&t);
	g3d_copyVector3d(&t,o);
	
	if(cam.worldRotation.x != 0 || cam.worldRotation.y != 0 || cam.worldRotation.z != 0){			
		g3d_cameraRotateAllAxis(cam.worldRotation.x,cam.worldRotation.y,cam.worldRotation.z,o,&t);
		g3d_copyVector3d(&t,o);
	}
	
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

void g3d_calculateProjection(vector3d* o){
	o->sx=F_NUM_DN(F_ADD(F_DIV(F_MUL(o->x,cam.d),F_ADD(cam.d,o->z)),F_NUM_UP(SCREEN_WIDTH>>1)));
	o->sy=F_NUM_DN(F_ADD(F_DIV(F_MUL(o->y,cam.d),F_ADD(cam.d,o->z)),F_NUM_UP(SCREEN_HEIGHT>>1)));
	o->sy = SCREEN_HEIGHT - o->sy;//flip y axis	
}

/**************************
This procedure actually draws an
object
**************************/
void g3d_drawObject(object* o){
	s32 vertices,lines,v,verts,i;
	vector3d v1;
	vector3d v2;
	vector3d* v1p;
	vector3d* v2p;
	vector3d* vtp;
	v1p = &v1;
	v2p = &v2;
	if(o->properties.visible == 0) return;
	
	vertices=o->objData->vertexSize;//total elements in array
	lines=o->objData->lineSize;//Total line endpoints
	verts=o->objData->faceSize;//total vertices per section

	v=0;
	i=0;
	//Load and render all distinct vertices into the vertex buffer;
	//This will render all object vertices based on the objects position,rotation etc..
	CACHE_ENABLE;
	while(v < vertices){
		v1.x = o->objData->data[v];
		v1.y = o->objData->data[v+1];
		v1.z = o->objData->data[v+2];
		
		g3d_renderVector3d(o, &v1, &v2, ((v==0)?(1):(0)));
		
		vertexBuffer[i] = v2;
		i++;
		v+=3;
	}
	CACHE_DISABLE;

	//This reads the "faces" section of the data and draws lines between points.
	//We'll use the vertex buffer's already rendered vertices
	while(v < (lines+vertices)){
		v1p = &vertexBuffer[o->objData->data[v]];		
		
		for(i=1; i<verts; i++){
			v++;
			v2p = &vertexBuffer[o->objData->data[v]];
			
			//Simple screen z axis clipping
			if((v1p->z > cam.d) || (v2p->z > cam.d)){
				g3d_calculateProjection(v1p);
				g3d_calculateProjection(v2p);
				g3d_drawLine(v1p,v2p,o->properties.lineColor);
			}
			vtp = v2p;
			v1p = v2p;
			v2p = vtp;
			
		}
		v++;
	}
}

/************************************
Clips a line to the screen and determines
if the line should be drawn or not.
This will use the Cohen Sutherland clipping
algorithm.
*************************************/
u8 g3d_clipLine(s32* x1, s32* y1, s32* x2, s32* y2, s32 V_MAX, s32 V_MIN, s32 H_MAX, s32 H_MIN){
	u8 regionV1,regionV2;
	s32 x,y,slope,b;
	regionV1 = 0;
	regionV2 = 0;
	if(*y1 < V_MIN) regionV1 |= 1; //top
	else if (*y1 > V_MAX) regionV1 |= 2; //bottom
	else if (*x1 > H_MAX) regionV1 |= 4; //right
	else if (*x1 < H_MIN) regionV1 |= 8; //left
	
	if(*y2 < V_MIN) regionV2 |= 1; //top
	else if (*y2 > V_MAX) regionV2 |= 2; //bottom
	else if (*x2 > H_MAX) regionV2 |= 4; //right
	else if (*x2 < H_MIN) regionV2 |= 8; //left
	
	if((regionV1 & regionV2) > 0) return (u8)0;
	//Clip v1
	//formulas for line y = mx + b
	//where m = slope and b = y intercept
	if(*x2 - *x1 == 0) slope = 0;
	else slope = F_PRECISION_DIV((F_PRECISION_UP(*y2) - F_PRECISION_UP(*y1)), (F_PRECISION_UP(*x2) - F_PRECISION_UP(*x1)));
	b = -((F_PRECISION_MUL(slope, F_PRECISION_UP(*x1))) - F_PRECISION_UP(*y1));
	
	if(regionV1 > 0){
		if(regionV1 & 1){//top
			y = V_MIN;
			x = (slope == 0)?(*x1):(F_PRECISION_DN(F_PRECISION_DIV(-b, slope)));
		}
		if(regionV1 & 2){//bottom
			y = V_MAX;
			x = (slope == 0)?(*x1):(F_PRECISION_DN(F_PRECISION_DIV(F_PRECISION_UP(y)-b, slope)));
		}
		if(regionV1 & 4){//right
			x = H_MAX;
			y = (slope == 0)?(*y1):(F_PRECISION_DN((F_PRECISION_MUL(slope,F_PRECISION_UP(x)) + b)));
		}
		if(regionV1 & 8){//left
			x = H_MIN;
			y = F_PRECISION_DN(b);
		}
		*x1 = x;
		*y1 = y;		
	}
	
	if(regionV2 > 0){
		if(regionV2 & 1){//top
			y = V_MIN;
			x = (slope == 0)?(*x2):(F_PRECISION_DN(F_PRECISION_DIV(-b, slope)));
		}
		if(regionV2 & 2){//bottom
			y = V_MAX;
			x = (slope == 0)?(*x2):(F_PRECISION_DN(F_PRECISION_DIV(F_PRECISION_UP(y)-b, slope)));
		}
		if(regionV2 & 4){//right
			x = H_MAX;
			y = (slope == 0)?(*y2):(F_PRECISION_DN((F_PRECISION_MUL(slope,F_PRECISION_UP(x)) + b)));
		}
		if(regionV2 & 8){//left
			x = H_MIN;
			y = F_PRECISION_DN(b);
		}
		*x2 = x;
		*y2 = y;
	}
	
	return (u8)1;
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
void /*__attribute__((section(".data")))*/ g3d_drawLine(vector3d* v1, vector3d* v2, u8 color){
	s32 vx,vy,vz,vx2,vy2;
	s32 dx, dy, dz;
	s32 sx,sy,sz,pixels,err;
	u8 doDraw;
	#ifdef __ASM_CODE
	s32 loffset,roffset;
	u8 yleft;
	#else
	s32 p;
	#endif
	
	//Screen x,y clipping
	vx = v1->sx;
	vy = v1->sy;
	vx2 = v2->sx;
	vy2 = v2->sy;
	doDraw = g3d_clipLine(&vx,&vy,&vx2,&vy2,SCREEN_HEIGHT,0,SCREEN_WIDTH,0);
	if(!doDraw) return;
	
	dx=(~(vx - vx2)+1);
	dy=(~(vy - vy2)+1);
	dz=(~(F_NUM_DN(F_SUB(v1->z,v2->z))+1));
	//dz=(~(F_NUM_DN(F_SUB(vz,vz2))+1));
	
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
	#else
	//The VB only runs at 20Mhz but it has 30 registers.
	//Lets use em all and squeeze out as much processing speed as possible.
	asm volatile(
	//Setup all the registers with initial values
	//r28,r29 are scratch for our purposes
	"ld.w %[dy],r6\n"
	"ld.w %[dx],r7\n"
	"ld.w %[err],r8\n"
	"ld.w %[sz],r9\n"
	"ld.w %[dz],r10\n"
	"ld.w %[pixels],r11\n"
	"ld.w %[vy],r12\n"
	"ld.w %[sy],r13\n"
	"ld.w %[vz],r14\n"
	"ld.w %[vx],r15\n"
	"ld.w %[sx],r16\n"
	"ld.w %[loffset],r17\n"
	"ld.w %[roffset],r18\n"
	"ld.b %[yleft],r19\n"
	"movea %[screenH],r0,r21\n"
	"movea %[screenW],r0,r22\n"
	"movea %[parallaxM],r0,r23\n"
	"ld.w %[fbLeft],r24\n"
	"movea %[fbRightOff],r0,r25\n"
	"shl 0x02,r25\n"
	//if(dy<dx){
		"cmp r6,r7\n"
		"ble _dxltdy\n"
	//err=(dx>>1);
		"mov r7,r8\n"
		"sar 0x01,r8\n"
	//sz=(sz)*(F_NUM_UP(dz)/((dx==0)?(1):(dx)));
		"mov r7,r28\n"
		"cmp r0,r7\n"
		"bne _nextLine1\n"
		"mov 0x01,r28\n"
		"_nextLine1:\n"
		"mov r10,r29\n"
		"shl %[fixedShift],r29\n"
		"div r28,r29\n"
		"mul r29,r9\n"
	//for(p=0;p<pixels;p++){
		"_lineLoop1Top:\n"
		"cmp r0,r11\n"
		"ble _lineLoop1End\n"
		"addi -1,r11,r11\n"
	//drawPoint(vx,vy,color,(F_NUM_DN(vz)>>PARALLAX_SHIFT));
	//******************************************************
		"ld.b %[color],r27\n"
		"mov r14,r26\n"
		"sar %[fixedShift],r26\n"
		"sar %[parallaxShift],r26\n"
	//if(y<0 || y>SCREEN_HEIGHT) return;
		"cmp r0,r12\n"
		"blt _endDrawPoint1\n"
		"cmp r12,r21\n"
		"blt _endDrawPoint1\n"
	//if(x<0 || x>SCREEN_WIDTH) return;
		"cmp r0,r15\n"
		"blt _endDrawPoint1\n"
		"cmp r15,r22\n"
		"blt _endDrawPoint1\n"
	//if(p>PARALLAX_MAX) p=PARALLAX_MAX;
		"cmp r26,r23\n"
		"bgt _nextPoint1\n"
		"mov r23,r26\n"
		"_nextPoint1:\n"
	//loffset = (((x-p)<<4) + (y>>4));
		"mov r12,r17\n"
		"sar 0x04,r17\n"
		"mov r15,r28\n"
		"sub r26,r28\n"
		"shl 0x04,r28\n"
		"add r28,r17\n"
	//roffset = (loffset + (p<<5));	
		"mov r26,r18\n"
		"shl 0x05,r18\n"
		"add r17,r18\n"
	//if(loffset>0x1800 || loffset<0) return;
		"movea 0x1800,r0,r28\n"
		"cmp r28,r17\n"
		"bge _endDrawPoint1\n"
		"cmp r0,r17\n"
		"blt _endDrawPoint1\n"
	//if(roffset>0x1800 || roffset<0) return;
		"movea 0x1800,r0,r28\n"
		"cmp r28,r18\n"
		"bge _endDrawPoint1\n"
		"cmp r0,r18\n"
		"blt _endDrawPoint1\n"
	//color &= 0x03;
		"andi 0x03,r27,r27\n"
	//yleft = (y&0x0F)<<1;
		"andi 0x0F,r12,r19\n"
		"shl 0x01,r19\n"
	//currentFrameBuffer[loffset] |= (color<<yleft);
		"mov r17,r28\n"
		"shl 0x02,r28\n"
		"add r24,r28\n"
		//"add r20,r28\n"
		"ld.w 0x0[r28],r29\n"
		"shl r19,r27\n"
		"or r27,r29\n"
		"st.w r29,0x0[r28]\n"
	//((u32*)(currentFrameBuffer+0x4000))[roffset] |= (color<<yleft);
		//"movhi 0x01,r28,r28\n"
		//"add r18,r28\n"
		//"st.w r29,0x0[r28]\n"

		"mov r18,r28\n"
		"shl 0x02,r28\n"
		"add r25,r28\n"
		"add r24,r28\n"
		"ld.w 0x0[r28],r29\n"
		"or r27,r29\n"
		"st.w r29,0x0[r28]\n"

		"_endDrawPoint1:\n"
	//******************************************************
	//err+=dy;
		"add r6,r8\n"
	//if(err>dx){
		"cmp r8,r7\n"
		"bgt _nextLine2\n"
	//vy+=sy;
		"add r13,r12\n"
	//err-=dx;
		"sub r7,r8\n"
	//}
		"_nextLine2:\n"		
	//vz+=sz;
		"add r9,r14\n"
	//vx+=sx;
		"add r16,r15\n"
	//}
		"jr _lineLoop1Top\n"
		"_lineLoop1End:\n"
	//else{
	"_dxltdy:\n"
	//err=(dy>>1);
		"mov r6,r8\n"
		"sar 0x01,r8\n"
	//sz=(sz)*(F_NUM_UP(dz)/((dy==0)?(1):(dy)));
		"mov r6,r28\n"
		"cmp r0,r6\n"
		"bne _nextLine3\n"
		"mov 0x01,r28\n"
		"_nextLine3:\n"
		"mov r10,r29\n"
		"shl %[fixedShift],r29\n"
		"div r28,r29\n"
		"mul r29,r9\n"
	//for(p=0;p<pixels;p++){
		"_lineLoop2Top:\n"
		"cmp r0,r11\n"
		"ble _lineLoop2End\n"
		"addi -1,r11,r11\n"
	//drawPoint(vx,vy,color,(F_NUM_DN(vz)>>PARALLAX_SHIFT));
	//******************************************************
		"ld.b %[color],r27\n"
		"mov r14,r26\n"
		"sar %[fixedShift],r26\n"
		"sar %[parallaxShift],r26\n"
	//if(y<0 || y>SCREEN_HEIGHT) return;
		"cmp r0,r12\n"
		"blt _endDrawPoint2\n"
		"cmp r12,r21\n"
		"blt _endDrawPoint2\n"
	//if(x<0 || x>SCREEN_WIDTH) return;
		"cmp r0,r15\n"
		"blt _endDrawPoint2\n"
		"cmp r15,r22\n"
		"blt _endDrawPoint2\n"
	//if(p>PARALLAX_MAX) p=PARALLAX_MAX;
		"cmp r26,r23\n"
		"bgt _nextPoint2\n"
		"mov r23,r26\n"
		"_nextPoint2:\n"
	//loffset = (((x-p)<<4) + (y>>4));
		"mov r12,r17\n"
		"sar 0x04,r17\n"
		"mov r15,r28\n"
		"sub r26,r28\n"
		"shl 0x04,r28\n"
		"add r28,r17\n"
	//roffset = (loffset + (p<<5));	
		"mov r26,r18\n"
		"shl 0x05,r18\n"
		"add r17,r18\n"
	//if(loffset>0x1800 || loffset<0) return;
		"movea 0x1800,r0,r28\n"
		"cmp r28,r17\n"
		"bge _endDrawPoint2\n"
		"cmp r0,r17\n"
		"blt _endDrawPoint2\n"
	//if(roffset>0x1800 || roffset<0) return;
		"movea 0x1800,r0,r28\n"
		"cmp r28,r18\n"
		"bge _endDrawPoint2\n"
		"cmp r0,r18\n"
		"blt _endDrawPoint2\n"
	//color &= 0x03;
		"andi 0x03,r27,r27\n"
	//yleft = (y&0x0F)<<1;
		"andi 0x0F,r12,r19\n"
		"shl 0x01,r19\n"
	//currentFrameBuffer[loffset] |= (color<<yleft);
		"mov r17,r28\n"
		"shl 0x02,r28\n"
		"add r24,r28\n"
		//"add r20,r28\n"
		"ld.w 0x0[r28],r29\n"
		"shl r19,r27\n"
		"or r27,r29\n"
		"st.w r29,0x0[r28]\n"
	//((u32*)(currentFrameBuffer+0x4000))[roffset] |= (color<<yleft);
		//"movhi 0x01,r28,r28\n"
		//"add r18,r28\n"
		//"st.w r29,0x0[r28]\n"

		"mov r18,r28\n"
		"shl 0x02,r28\n"
		"add r25,r28\n"
		"add r24,r28\n"
		"ld.w 0x0[r28],r29\n"
		"or r27,r29\n"
		"st.w r29,0x0[r28]\n"

		"_endDrawPoint2:\n"
	//******************************************************
	//err+=dx;
		"add r7,r8\n"
	//if(err>dy){
		"cmp r8,r6\n"
		"bgt _nextLine4\n"
	//vx+=sx;
		"add r16,r15\n"
	//err-=dy;
		"sub r6,r8\n"
	//}
		"_nextLine4:\n"
	//vz+=sz;
		"add r9,r14\n"
	//vy+=sy;
		"add r13,r12\n"
	//}
		"jr _lineLoop2Top\n"
		"_lineLoop2End:\n"
	"_end:\n"
	:/*no output*/
	:[dy] "m" (dy), [dx] "m" (dx), [err] "m" (err),
	 [sz] "m" (sz), [dz] "m" (dz), [pixels] "m" (pixels),
	 [vy] "m" (vy), [sy] "m" (sy), [vz] "m" (vz),
	 [vx] "m" (vx), [sx] "m" (sx), [fixedShift] "i" (FIXED_SHIFT),
	 [loffset] "m" (loffset), [roffset] "m" (roffset), [yleft] "m" (yleft),
	 [screenH] "i" (SCREEN_HEIGHT), [screenW] "i" (SCREEN_WIDTH), [parallaxM] "i" (PARALLAX_MAX),
	 [fbLeft] "m" (currentFrameBuffer), [fbRightOff] "i" (0x4000), [color] "m" (color),
	 [parallaxShift] "i" (PARALLAX_SHIFT)
	 :"r6","r7","r8","r9","r10","r11","r12","r13","r14","r15","r16","r17","r18","r19","r21","r22","r23","r24","r25","r26","r27","r28","r29"
	);
	#endif
	CACHE_DISABLE;
	
}


/*********************************
This initializes an object type
*********************************/
void inline g3d_initObject(object* o){
	o->worldPosition.x = 0;
	o->worldPosition.y = 0;
	o->worldPosition.z = 0;
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
	o->scale.x = 0;
	o->scale.y = 0;
	o->scale.z = 0;
	o->parent = (object*)0x00;
	
	o->properties.visible = 1;
	o->properties.detectCollision = 0;
	o->properties.lineColor = 3;
}

void inline g3d_moveObject(object* o){
	//Increment attributes	
	if(o->rotation.x != 0) o->worldRotation.x += o->rotation.x;
	if(o->rotation.y != 0) o->worldRotation.y += o->rotation.y;
	if(o->rotation.z != 0) o->worldRotation.z += o->rotation.z;
	
	if(o->speed.x != 0) o->worldSpeed.x += o->speed.x;
	if(o->speed.y != 0) o->worldSpeed.y += o->speed.y;
	if(o->speed.z != 0) o->worldSpeed.z += o->speed.z;
	
	if(o->scale.x != 0) o->worldScale.x += o->scale.x;
	if(o->scale.y != 0) o->worldScale.y += o->scale.y;
	if(o->scale.z != 0) o->worldScale.z += o->scale.z;
	
	//Check rotation angles
	while(o->worldRotation.x > 359) o->worldRotation.x -= 360;
	while(o->worldRotation.y > 359) o->worldRotation.y -= 360;
	while(o->worldRotation.z > 359) o->worldRotation.z -= 360;
	while(o->worldRotation.x < -359) o->worldRotation.x += 360;
	while(o->worldRotation.y < -359) o->worldRotation.y += 360;
	while(o->worldRotation.z < -359) o->worldRotation.z += 360;
	
	//Move the object based on moveto and speed
	if(o->moveTo.x != o->worldPosition.x){
		vbTextOut(0,5,1,"X not equal");
		if(o->worldPosition.x < o->moveTo.x){
			o->worldPosition.x += o->worldSpeed.x;
			if(o->worldPosition.x > o->moveTo.x) o->worldPosition.x = o->moveTo.x;
		}else{
			o->worldPosition.x -= o->worldSpeed.x;
			if(o->worldPosition.x < o->moveTo.x) o->worldPosition.x = o->moveTo.x;
		}
	}
	if(o->moveTo.y != o->worldPosition.y){
		vbTextOut(0,5,2,"Y not equal");
		if(o->worldPosition.y < o->moveTo.y){
			o->worldPosition.y += o->worldSpeed.y;
			if(o->worldPosition.y > o->moveTo.y) o->worldPosition.y = o->moveTo.y;
		}else{
			o->worldPosition.y -= o->worldSpeed.y;
			if(o->worldPosition.y < o->moveTo.y) o->worldPosition.y = o->moveTo.y;
		}
	}
	if(o->moveTo.z != o->worldPosition.z){
		vbTextOut(0,5,3,"Z not equal");
		if(o->worldPosition.z < o->moveTo.z){
			o->worldPosition.z += o->worldSpeed.z;
			if(o->worldPosition.z > o->moveTo.z) o->worldPosition.z = o->moveTo.z;
		}else{
			o->worldPosition.z -= o->worldSpeed.z;
			if(o->worldPosition.z < o->moveTo.z) o->worldPosition.z = o->moveTo.z;
		}
	}	
}

void inline g3d_moveCamera(camera* c){
	//Do increments
	if(c->rotation.x != 0) c->worldRotation.x += c->rotation.x;
	if(c->rotation.y != 0) c->worldRotation.y += c->rotation.y;
	if(c->rotation.z != 0) c->worldRotation.z += c->rotation.z;
	
	if(c->speed.x != 0) c->worldSpeed.x += c->speed.x;
	if(c->speed.y != 0) c->worldSpeed.y += c->speed.y;
	if(c->speed.z != 0) c->worldSpeed.z += c->speed.z;
	
	//Check rotation angles
	while(c->worldRotation.x > 359) c->worldRotation.x -= 360;
	while(c->worldRotation.y > 359) c->worldRotation.y -= 360;
	while(c->worldRotation.z > 359) c->worldRotation.z -= 360;
	while(c->worldRotation.x < -359) c->worldRotation.x += 360;
	while(c->worldRotation.y < -359) c->worldRotation.y += 360;
	while(c->worldRotation.z < -359) c->worldRotation.z += 360;
	//Move camera based on moveto and speed
	if(c->moveTo.x != c->worldPosition.x){
		if(c->worldPosition.x < c->moveTo.x){
			c->worldPosition.x += c->worldSpeed.x;
			if(c->worldPosition.x > c->moveTo.x) c->worldPosition.x = c->moveTo.x;
		}else{
			c->worldPosition.x -= c->worldSpeed.x;
			if(c->worldPosition.x < c->moveTo.x) c->worldPosition.x = c->moveTo.x;
		}
	}
	if(c->moveTo.y != c->worldPosition.y){
		if(c->worldPosition.y < c->moveTo.y){
			c->worldPosition.y += c->worldSpeed.y;
			if(c->worldPosition.y > c->moveTo.y) c->worldPosition.y = c->moveTo.y;
		}else{
			c->worldPosition.y -= c->worldSpeed.y;
			if(c->worldPosition.y < c->moveTo.y) c->worldPosition.y = c->moveTo.y;
		}
	}
	if(c->moveTo.z != c->worldPosition.z){
		if(c->worldPosition.z < c->moveTo.z){
			c->worldPosition.z += c->worldSpeed.z;
			if(c->worldPosition.z > c->moveTo.z) c->worldPosition.z = c->moveTo.z;
		}else{
			c->worldPosition.z -= c->worldSpeed.z;
			if(c->worldPosition.z < c->moveTo.z) c->worldPosition.z = c->moveTo.z;
		}
	}
}
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
	o->x = v->x;
	o->z = (F_MUL(v->z, cosine[degrees])) + (F_MUL(sine[degrees], v->y));
	o->y = (F_MUL(v->z, -sine[degrees])) + (F_MUL(cosine[degrees], v->y));	
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
	tickStart = tick;
	while(v < vertices){
		v1.x = o->objData->data[v];
		v1.y = o->objData->data[v+1];
		v1.z = o->objData->data[v+2];
		
		g3d_renderVector3d(o, &v1, &v2, ((v==0)?(1):(0)));
		
		vertexBuffer[i] = v2;
		i++;
		v+=3;
	}
	tickEnd = tick;
	vbTextOut(0,5,0,"Render");
	vbTextOut(0,21,0,itoa((tickEnd - tickStart),10,2));

	//This reads the "faces" section of the data and draws lines between points.
	//We'll use the vertex buffer's already rendered vertices
	tickStart = tick;
	while(v < (lines+vertices)){
		v1p = &vertexBuffer[o->objData->data[v]];		
		
		for(i=1; i<verts; i++){
			v++;
			v2p = &vertexBuffer[o->objData->data[v]];
			g3d_drawLine(v1p,v2p,3,o);
			vtp = v2p;
			v1p = v2p;
			v2p = vtp;
			
		}
		v++;
	}
	tickEnd = tick;
	vbTextOut(0,5,1,"Draw Lines");
	vbTextOut(0,21,1,itoa((tickEnd - tickStart),10,2));
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
void g3d_drawLine(vector3d* v1, vector3d* v2, u8 color, object* o){
	s32 vx,vy,vz,vx2,vy2;
	s32 dx, dy, dz;
	s32 sx,sy,sz,pixels,err;
	#ifdef __ASM_CODE
	s32 loffset,roffset;
	u8 yleft;
	#else
	s32 p;
	#endif

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
	//2B12 - draw function
	asm volatile(
	//Setup all the registers with initial values
	//r5,r4 are scratch for our purposes
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
	"ld.w %[fbLeft],r20\n"
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
		"mov r7,r5\n"
		"cmp r0,r7\n"
		"bne _nextLine1\n"
		"mov 0x01,r5\n"
		"_nextLine1:\n"
		"mov r10,r4\n"
		"shl %[fixedShift],r4\n"
		"div r5,r4\n"
		"mul r4,r9\n"
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
		"mov r15,r5\n"
		"sub r26,r5\n"
		"shl 0x04,r5\n"
		"add r5,r17\n"
	//roffset = (loffset + (p<<5));	
		"mov r26,r18\n"
		"shl 0x05,r18\n"
		"add r17,r18\n"
	//if(loffset>0x1800 || loffset<0) return;
		"movea 0x1800,r0,r5\n"
		"cmp r5,r17\n"
		"bge _endDrawPoint1\n"
		"cmp r0,r17\n"
		"blt _endDrawPoint1\n"
	//if(roffset>0x1800 || roffset<0) return;
		"movea 0x1800,r0,r5\n"
		"cmp r5,r18\n"
		"bge _endDrawPoint1\n"
		"cmp r0,r18\n"
		"blt _endDrawPoint1\n"
	//color &= 0x03;
		"andi 0x03,r27,r27\n"
	//yleft = (y&0x0F)<<1;
		"andi 0x0F,r12,r19\n"
		"shl 0x01,r19\n"
	//currentFrameBuffer[loffset] |= (color<<yleft);
		"mov r17,r5\n"
		"shl 0x02,r5\n"
		"add r24,r5\n"
		"ld.w 0x0[r5],r4\n"
		"shl r19,r27\n"
		"or r27,r4\n"
		"st.w r4,0x0[r5]\n"
	//((u32*)(currentFrameBuffer+0x4000))[roffset] |= (color<<yleft);
		//"movhi 0x01,r5,r5\n"
		//"add r18,r5\n"
		//"st.w r4,0x0[r5]\n"
	
		"mov r18,r5\n"
		"shl 0x02,r5\n"
		"add r25,r5\n"
		"add r24,r5\n"
		"ld.w 0x0[r5],r4\n"
		"or r27,r4\n"
		"st.w r4,0x0[r5]\n"
		
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
		"mov r6,r5\n"
		"cmp r0,r6\n"
		"bne _nextLine3\n"
		"mov 0x01,r5\n"
		"_nextLine3:\n"
		"mov r10,r4\n"
		"shl %[fixedShift],r4\n"
		"div r5,r4\n"
		"mul r4,r9\n"
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
		"mov r15,r5\n"
		"sub r26,r5\n"
		"shl 0x04,r5\n"
		"add r5,r17\n"
	//roffset = (loffset + (p<<5));	
		"mov r26,r18\n"
		"shl 0x05,r18\n"
		"add r17,r18\n"
	//if(loffset>0x1800 || loffset<0) return;
		"movea 0x1800,r0,r5\n"
		"cmp r5,r17\n"
		"bge _endDrawPoint2\n"
		"cmp r0,r17\n"
		"blt _endDrawPoint2\n"
	//if(roffset>0x1800 || roffset<0) return;
		"movea 0x1800,r0,r5\n"
		"cmp r5,r18\n"
		"bge _endDrawPoint2\n"
		"cmp r0,r18\n"
		"blt _endDrawPoint2\n"
	//color &= 0x03;
		"andi 0x03,r27,r27\n"
	//yleft = (y&0x0F)<<1;
		"andi 0x0F,r12,r19\n"
		"shl 0x01,r19\n"
	//currentFrameBuffer[loffset] |= (color<<yleft);
		"mov r17,r5\n"
		"shl 0x02,r5\n"
		"add r24,r5\n"
		"ld.w 0x0[r5],r4\n"
		"shl r19,r27\n"
		"or r27,r4\n"
		"st.w r4,0x0[r5]\n"
	//((u32*)(currentFrameBuffer+0x4000))[roffset] |= (color<<yleft);
		//"movhi 0x01,r5,r5\n"
		//"add r18,r5\n"
		//"st.w r4,0x0[r5]\n"
	
		"mov r18,r5\n"
		"shl 0x02,r5\n"
		"add r25,r5\n"
		"add r24,r5\n"
		"ld.w 0x0[r5],r4\n"
		"or r27,r4\n"
		"st.w r4,0x0[r5]\n"
	
		
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
	 :"r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","r15","r16","r17","r18","r19","r20","r21","r22","r23","r24","r25","r26","r27"
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
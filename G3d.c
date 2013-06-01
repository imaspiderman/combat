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
void g3d_drawObject(object* o){
	s32 vertices,lines,v,verts,i;
	vector3d v1,v2;
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
	while(v < vertices){
		v1.x = o->objData->data[v];
		v1.y = o->objData->data[v+1];
		v1.z = o->objData->data[v+2];
		
		g3d_renderVector3d(o, &v1, &v2, ((v==0)?(1):(0)));
		vertexBuffer[i++] = v2;
		v+=3;
	}

	//This reads the "faces" section of the data and draws lines between points.
	//We'll use the vertex buffer's already rendered vertices
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
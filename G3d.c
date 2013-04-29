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
void inline g3d_copyVector(vector3d* from, vector3d* to){
	to->x = from->x;
	to->y = from->y;
	to->z = from->z;
}

void inline g3d_rotateXAxis(s32 degrees, vector3d* v, vector3d* o){
	o->x = v->x;
	o->z = (v->z * cosine[degrees]) + (sine[degrees] * v->y);
	o->y = (v->z * -sine[degrees]) + (cosine[degrees] * v->y);
}

void inline g3d_rotateYAxis(s32 degrees, vector3d* v, vector3d* o){
	o->y = v->y;
	o->x = (v->x * cosine[degrees]) + (sine[degrees] * v->z);
	o->z = (v->x * -sine[degrees]) + (cosine[degrees] * v->z);
}

void inline g3d_rotateZAxis(s32 degrees, vector3d* v, vector3d* o){
	o->z = v->z;
	o->x = (v->x * cosine[degrees]) + (sine[degrees] * v->y);
	o->y = (v->x * -sine[degrees]) + (cosine[degrees] * v->y);
}

//Make sure the rotation values are between -360 and 360
void inline g3d_rotateAllAxis(s32 rx, s32 ry, s32 rz, vector3d* v, vector3d* o){
	vector3d t;
	
	if(rx<0) rx=360+rx;
	if(ry<0) ry=360+ry;
	if(rz<0) rz=360+rz;
	
	g3d_rotateXAxis(rx,v,&t);
	g3d_rotateYAxis(ry,&t,o);
	copyVector(o,&t);
	g3d_rotateZAxis(rz,&t,o);
	
}

void inline g3d_translate(s32 x, s32 y, s32 z, vector3d* v, vector3d* o){
	o->x = v->x + x;
	o->y = v->y + y;
	o->z = v->z + z;
}

void inline g3d_cameraRotateAllAxis(s32 rx, s32 ry, s32 rz, vector3d* v, vector3d* o){
	rx = (~rx)+1;
	ry = (~ry)+1;
	rz = (~rz)+1;
	g3d_rotateAllAxis(rx,ry,rz,v,o);
}

void inline g3d_cameraTranslate(s32 x, s32 y, s32 z, vector3d* v, vector3d* o){
	o->x = v->x - x;
	o->y = v->y - y;
	o->z = v->z - z;
}
#include "Collision.h"

bool Collision_SphereToSphere(Vector3 posA,Vector3 posB,float radA,float radB){

	float distance =
		(posB.x - posA.x) * (posB.x - posA.x)
		+ (posB.y - posA.y) * (posB.y - posA.y)
		+ (posB.z - posA.z) * (posB.z - posA.z);

	float radian = 
		(radA + radB) * (radA + radB);

	if (distance <= radian) {
		return true;
	}

	return false;
}
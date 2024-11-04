#pragma once
#include "Vector3.h"

class Plane {
public:
	Plane(void) {};
	Plane(const Vector3& normal, float distance, bool normalise = false);
	~Plane(void) {};

	void setNormal(const Vector3& normal) { this->normal = normal; }
	Vector3 getNormal() const { return normal; }

	void setDistance(float dist) { distance = dist; }
	float getDistance() const { return distance; }

	bool sphereInPlane(const Vector3& position, float radius) const;

protected:
	Vector3 normal;
	float distance;
};
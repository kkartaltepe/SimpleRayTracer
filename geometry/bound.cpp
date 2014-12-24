#ifndef BOXBOUND_HPP
#define BOXBOUND_HPP

#include <vector>
#include <glm/glm.hpp>
#include <stdio.h>

#include "ray.hpp"

class Bound {
public:
	glm::vec3 min, max;
	
	static Bound combine(Bound bound1, Bound bound2) {
		Bound newBounds = bound1;

		if(bound2.max.x > newBounds.max.x) //Grow maxs
			newBounds.max.x = bound2.max.x;
		if(bound2.max.y > newBounds.max.y)
			newBounds.max.y = bound2.max.y;
		if(bound2.max.z > newBounds.max.z)
			newBounds.max.z = bound2.max.z;

		if(bound2.min.x < newBounds.min.x) //Grow mins
			newBounds.min.x = bound2.min.x;
		if(bound2.min.y < newBounds.min.y)
			newBounds.min.y = bound2.min.y;
		if(bound2.min.z < newBounds.min.z)
			newBounds.min.z = bound2.min.z;

		return newBounds;
	}

	static Bound combine(std::vector<Bound> bounds) {
		Bound newBounds = bounds.back();
		bounds.pop_back();

		for(std::vector<Bound>::iterator iter = bounds.begin(); iter != bounds.end(); iter++ ) {
			if(iter->max.x > newBounds.max.x) //Grow maxs
				newBounds.max.x = iter->max.x;
			if(iter->max.y > newBounds.max.y)
				newBounds.max.y = iter->max.y;
			if(iter->max.z > newBounds.max.z)
				newBounds.max.z = iter->max.z;

			if(iter->min.x < newBounds.min.x) //Grow mins
				newBounds.min.x = iter->min.x;
			if(iter->min.y < newBounds.min.y)
				newBounds.min.y = iter->min.y;
			if(iter->min.z < newBounds.min.z)
				newBounds.min.z = iter->min.z;
		}
		return newBounds;
	}

	// Informative improvment from http://www.cs.utah.edu/~awilliam/box/box.pdf
	// Improved with min/max possibly faster than branching? (not too much)
	bool intersect(Ray ray, float mint, float maxt) {
		float tmin,tmax,txmin,txmax,tymin,tymax,tzmin,tzmax;

		txmin = (min.x - ray.origin.x) * ray.invDirection.x;
		txmax = (max.x - ray.origin.x) * ray.invDirection.x;
		tmin = fmin(txmin, txmax);
		tmax = fmax(txmin, txmax);

		tymin = (min.y - ray.origin.y) * ray.invDirection.y;
		tymax = (max.y - ray.origin.y) * ray.invDirection.y;
		tmin = fmax(tmin, fmin(tymin, tymax));
		tmax = fmin(tmax, fmax(tymin, tymax));

		tzmin = (min.z - ray.origin.z) * ray.invDirection.z;
		tzmax = (max.z - ray.origin.z) * ray.invDirection.z;
		tmin = fmax(tmin, fmin(tzmin, tzmax));
		tmax = fmin(tmax, fmax(tzmin, tzmax));

		// tmin and tmax are the interval, hopefully infront of the ray;	
		return tmin < maxt && tmax > mint && tmax > tmin;
	}

	/**
	 * Return 0 for x, 1 for y, 2 for z
	 */
	int longestAxis() {
		float xdiff, ydiff, zdiff;
		int longestAxis = 0;
		xdiff = fabs(max.x - min.x);
		ydiff = fabs(max.y - min.y);
		zdiff = fabs(max.z - min.z);

		if(ydiff > xdiff && ydiff > zdiff)
			longestAxis = 1;
		if(zdiff > xdiff && zdiff > ydiff)
			longestAxis = 2;
		return longestAxis;

	}

};


#endif /* end of include guard: BOXBOUND_HPP */
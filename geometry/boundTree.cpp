#ifndef BVH_HPP
#define BVH_HPP

#include <algorithm>

#include "bound.cpp"
#include "ray.hpp"
#include "triangle.cpp"
#include "circle.cpp"

struct AxisAlignedCenterComparitor {
	int axis; // 0 1 2, x y z
 	bool operator() (Triangle* t1, Triangle* t2) {
 		return t1->plane.center[axis] < t2->plane.center[axis];
	}
};
// http://en.wikipedia.org/wiki/Z-order_curve#Efficiently_building_quadtrees
struct ZOrderComparitor {
 	bool operator() (Triangle* t1, Triangle* t2) {
 		int mostSigDim = 0;
 		int mostExpDiff = 0;
 		int mostMantDiff = 0;

 		int expt1,expt2;
 		int mant1, mant2;
 		// mantissa = frexp(float, &exponent); // http://en.cppreference.com/w/cpp/numeric/math/frexp

 		for(float dim = 0; dim < 3; dim++) {
 			mant1 = (int)(frexp(t1->plane.center[dim], &expt1)*(2 << 30)); // Only use 30 bits of accuracy
 			mant2 = (int)(frexp(t2->plane.center[dim], &expt2)*(2 << 30));
 			// float mant1f = frexp(t1->plane.center[dim], &expt1);
 			// float mant2f = frexp(t2->plane.center[dim], &expt2);
 			// printf("Comparing %f*2^%d to %f*2^%d\n", mant1f, expt1, mant2f, expt2);
 			int expDiff = expt1 ^ expt2;
 			int mantDiff = mant1 ^ mant2;

 			if(expDiff != 0 && lessMsb(mostExpDiff, expDiff)) {
 				mostSigDim = dim;
 				mostExpDiff = expDiff;
 			} else if (expDiff == 0 && mostExpDiff == 0 && lessMsb(mostMantDiff, mantDiff)) {
 				mostSigDim = dim;
 				mostExpDiff = 0;
 				mostMantDiff = mantDiff;
 			}
 		}
 		return t1->plane.center[mostSigDim] < t2->plane.center[mostSigDim];
	}

	bool lessMsb(int a, int b) {
		return a < b && a < (a ^ b);
	}
};

class BoundTree { // Is a node itself.
public:
	Bound bound;
	std::vector<BoundTree> children;

	// Empty unless this is a leaf

	std::vector<Triangle*> triangleLeaves;
	std::vector<Circle*> circleLeaf;


	BoundTree() { }

	BoundTree(std::vector<Circle*> circles, std::vector<Triangle*> triangles, int trianglesPerLeaf) {
		// triangleLeaf = NULL;
		for(std::vector<Triangle*>::iterator iter = triangles.begin(); iter != triangles.end(); iter++) { // expand bounds
			bound = Bound::combine(bound, (*iter)->bound);
		}
		if (triangles.size() <= trianglesPerLeaf) {
			triangleLeaves = triangles;
			return;
		}

		// struct AxisAlignedCenterComparitor comparitor;
		// comparitor.axis = bound.longestAxis();
		struct ZOrderComparitor comparitor;
		std::sort(triangles.begin(), triangles.end(), comparitor); // sort along ZOrder curve;

		//partition into left and fill out left
		std::vector<Triangle*> left;
		int childSize = triangles.size()/2;
		while(triangles.size() > childSize) {
			left.push_back(triangles.back());
			triangles.pop_back();
		}
		children.push_back(BoundTree(circles, left, trianglesPerLeaf));
		children.push_back(BoundTree(circles, triangles, trianglesPerLeaf)); //Right children are all left in triangles
	}

	// BoundTree(Triangle* triangle) {
	// 	bound = triangle->bound;
	// 	triangleLeaf = triangle;
	// }

	Intersection intersect(Ray ray, float mint, float maxt) {
		if(bound.intersect(ray, mint, maxt)) {
			if(children.size() == 0) {
				Intersection closest;
				for(std::vector<Triangle*>::iterator iter = triangleLeaves.begin(); iter != triangleLeaves.end(); iter++) {
					Intersection childInters = (*iter)->intersect(ray);
					if(childInters.didHit()) {
						if(childInters.distanceTraveled < closest.distanceTraveled || !closest.didHit())
							closest = childInters;
					}
				}
				return closest;
			} else {
				Intersection closest;
				for(std::vector<BoundTree>::iterator iter = children.begin(); iter != children.end(); iter++) {
					Intersection childInters = iter->intersect(ray, mint, maxt);
					if(childInters.didHit()) {
						if(childInters.distanceTraveled < closest.distanceTraveled || !closest.didHit())
							closest = childInters;
					}
				}
				return closest;
			}
				
		} 
		return Intersection();
	}

	int size() {
		if(children.size() > 0) {
			int sum = 0;
			for(std::vector<BoundTree>::iterator iter = children.begin(); iter != children.end(); iter++)
				sum += iter->size();
			return sum;
		} else {
			return 1;
		}
	}

	int depth() {
		if(children.size() > 0) {
			int maxChildDepth = 0;
			for(std::vector<BoundTree>::iterator iter = children.begin(); iter != children.end(); iter++)
				maxChildDepth = fmax(iter->depth(), maxChildDepth);
			return maxChildDepth+1;
		} else {
			return 1;
		}
	}

	int sizeAtDepth(int targetDepth) {
		if(targetDepth == 0)
			return 1;

		if(children.size() > 0) {
			int depthAtTarget = 0;
			for(std::vector<BoundTree>::iterator iter = children.begin(); iter != children.end(); iter++)
				depthAtTarget += iter->sizeAtDepth(targetDepth-1);
			return depthAtTarget;
		} else {
			return 0;
		}
	}
};


#endif
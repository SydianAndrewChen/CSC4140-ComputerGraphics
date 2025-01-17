#include "bvh.h"

#include "CGL/CGL.h"
#include "triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL {
namespace SceneObjects {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  primitives = std::vector<Primitive *>(_primitives);
  root = construct_bvh(primitives.begin(), primitives.end(), max_leaf_size);
}

BVHAccel::~BVHAccel() {
  if (root)
    delete root;
  primitives.clear();
}

BBox BVHAccel::get_bbox() const { return root->bb; }

void BVHAccel::draw(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->draw(c, alpha);
    }
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->drawOutline(c, alpha);
    }
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}

BVHNode *BVHAccel::construct_bvh(std::vector<Primitive *>::iterator start,
                                 std::vector<Primitive *>::iterator end,
                                 size_t max_leaf_size) {

  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  BBox bbox;
  int size = 0;

  for (auto p = start; p != end; p++, size++) {
    BBox bb = (*p)->get_bbox();
    bbox.expand(bb);
  }

  BVHNode *node = new BVHNode(bbox);
  node->start = start;
  node->end   = end;
  if (size <= max_leaf_size) return node;

  int best_axis = 0;
  for (int i = 1; i < 3; i++) {
    if (bbox.extent[i] > bbox.extent[best_axis]) {
      best_axis = i;
    }
  }
  
  std::vector<Primitive *>::iterator mid = start;

  // SAH (Still has bug in the dragon scene!)
  // std::sort(start, end, [best_axis](const Primitive * const p0, const Primitive * const p1){
  //     return p0->get_bbox().centroid()[best_axis] < p1->get_bbox().centroid()[best_axis]; 
  // });
  // int bucket_num = 4;
  // double best_threshold;
  // double best_cost = 1e20;
  // for (double threshold = bbox.min[best_axis]; threshold < bbox.max[best_axis] - EPS_F; threshold+=bbox.extent[best_axis] / bucket_num){
  //   mid = std::find_if(mid, end, [best_axis, threshold](const Primitive * p){
  //     return p->get_bbox().centroid()[best_axis] >= threshold;
  //   });

  //   auto NA = mid - start;
  //   auto NB = end - mid;

  //   BBox bboxA;
  //   for (auto p = start; p != mid; ++p){
  //     bboxA.expand((*p)->get_bbox());
  //   }

  //   BBox bboxB;
  //   for (auto p = mid; p != end; ++p){
  //     bboxB.expand((*p)->get_bbox());
  //   }

  //   auto cost = (NA * bboxA.surface_area() + NB * bboxB.surface_area()) / bbox.surface_area();
  //   if (cost < best_cost){
  //     best_threshold = threshold;
  //     best_cost = cost;
  //   }
  // }

  // Common heuristic
  double best_threshold = 0.0;
  for (auto p = start; p != end; p++) {
    best_threshold += (*p)->get_bbox().centroid()[best_axis];
  }
  best_threshold /= size;

  mid = std::partition(start, end, [best_axis, best_threshold](const Primitive * const p0){
    return p0->get_bbox().centroid()[best_axis] < best_threshold; 
  });

  if (start == mid) mid++;
  else if (end == mid) mid--;

  node->l = construct_bvh(start, mid, max_leaf_size);
  node->r = construct_bvh(mid, end, max_leaf_size);
  return node;
}

bool BVHAccel::has_intersection(const Ray &ray, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.
  double t0 = ray.min_t;
  double t1 = ray.max_t;
  if (!node->bb.intersect(ray, t0, t1) || t1 < ray.min_t || t0 > ray.max_t) return false;
  if (!node->isLeaf()) {
    bool hit_left = has_intersection(ray, node->l);
    bool hit_right = has_intersection(ray, node->r);
    return hit_left || hit_right;
  } 
  
  bool hit = false;
  for (auto p = node->start; p != node->end; ++p) {
    total_isects++;
    hit = (*p)->has_intersection(ray) || hit;
  }
  return hit;

}

bool BVHAccel::intersect(const Ray &ray, Intersection *i, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // bool hit = false;
  // for (auto p = primitives.begin(); p != primitives.end(); ++p) {
  //   total_isects++;
  //   hit = (*p)->intersect(ray, i) || hit;
  // }
  // return hit;


  double t0 = ray.min_t;
  double t1 = ray.max_t;
  if (!node->bb.intersect(ray, t0, t1) || t1 < ray.min_t || t0 > ray.max_t) return false;
  if (!node->isLeaf()) {
    bool hit_left = intersect(ray, i, node->l);
    bool hit_right = intersect(ray, i, node->r);
    return hit_left || hit_right;
  } 
  
  bool hit = false;
  for (auto p = node->start; p != node->end; ++p) {
    total_isects++;
    hit = (*p)->intersect(ray, i) || hit;
  }
  return hit;
}

} // namespace SceneObjects
} // namespace CGL

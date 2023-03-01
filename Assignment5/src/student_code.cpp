#include "student_code.h"
#include "mutablePriorityQueue.h"

using namespace std;

namespace CGL
{

  template<typename T>
  T lerp(const T & p1, const T & p2, float t) {
    return t * p1 + (1 - t) * p2;
  }

  /**
   * Evaluates one step of the de Casteljau's algorithm using the given points and
   * the scalar parameter t (class member).
   *
   * @param points A vector of points in 2D
   * @return A vector containing intermediate points or the final interpolated vector
   */
  std::vector<Vector2D> BezierCurve::evaluateStep(std::vector<Vector2D> const &points)
  { 
    // TODO Task 1.
    std::vector<Vector2D> lerped_points(points.size()-1);
    for (int i = 0; i < points.size()-1; i++)
    {
      lerped_points[i] = lerp(points[i], points[i+1], t);
    }

    return lerped_points;
  }

  /**
   * Evaluates one step of the de Casteljau's algorithm using the given points and
   * the scalar parameter t (function parameter).
   *
   * @param points    A vector of points in 3D
   * @param t         Scalar interpolation parameter
   * @return A vector containing intermediate points or the final interpolated vector
   */
  std::vector<Vector3D> BezierPatch::evaluateStep(std::vector<Vector3D> const &points, double t) const
  {
    std::vector<Vector3D> lerped_points(points.size()-1);
    for (int i = 0; i < points.size()-1; i++)
    {
      lerped_points[i] = lerp(points[i], points[i+1], t);
    }
    
    return lerped_points;
  }

  /**
   * Fully evaluates de Casteljau's algorithm for a vector of points at scalar parameter t
   *
   * @param points    A vector of points in 3D
   * @param t         Scalar interpolation parameter
   * @return Final interpolated vector
   */
  Vector3D BezierPatch::evaluate1D(std::vector<Vector3D> const &points, double t) const
  {
    // TODO Task 2.
    std::vector<Vector3D> tmp_points(points);
    for (int j = points.size(); j > 0; j--){
      for (int i = 0; i < j-1; ++i){
        tmp_points[i] = lerp(tmp_points[i], tmp_points[i+1], t);
      }
    }
    return tmp_points[0];
  }

  /**
   * Evaluates the Bezier patch at parameter (u, v)
   *
   * @param u         Scalar interpolation parameter
   * @param v         Scalar interpolation parameter (along the other axis)
   * @return Final interpolated vector
   */
  Vector3D BezierPatch::evaluate(double u, double v) const 
  {  
    // TODO Task 2.
    std::vector<Vector3D> row_points(controlPoints.size());

    for (int i = 0; i < controlPoints.size(); i++){
      row_points[i] = evaluate1D(controlPoints[i], u);
    }
  
    return evaluate1D(row_points, v);
  }

  Vector3D Vertex::normal( void ) const
  {
    // TODO Task 3.
    // Returns an approximate unit normal at this vertex, computed by
    // taking the area-weighted average of the normals of neighboring
    // triangles, then normalizing.

    // Became smoother, but why not so bright???
    
    Vector3D normal;

    HalfedgeCIter start_half_edge = halfedge();
    auto h = start_half_edge;
    do {
      auto p0 = h->vertex();
      auto p1 = h->next()->vertex();
      auto p2 = h->next()->next()->vertex();
      normal += cross(p1->position - p0->position, p2->position - p0->position);
      h = h->twin()->next();
    }
    while(h != start_half_edge);


    return normal.unit();
  }

  EdgeIter HalfedgeMesh::flipEdge( EdgeIter e0 )
  {
    // TODO Task 4.
    // This method should flip the given edge and return an iterator to the flipped edge.

    /* Assume The initla state to be:
     *                    c
     *                  / | \
     *                 a  |  d
     *                  \ | /
     *                    b
     * We need to do:
     *  Find vertex a & d
     *  Create a new edge with two new halfedges
     *  Create two new faces
     *  Delete the connectiveness of old edges
     *  Add new faces to the new geoetries
     */
    auto bc = e0->halfedge();
    auto cb = bc->twin();

    auto ba = cb->next();
    auto cd = bc->next();

    auto ac = cb->next()->next();
    auto db = bc->next()->next();
    
    auto b = bc->vertex();
    auto c = cb->vertex();
    auto a = ac->vertex();
    auto d = db->vertex();

    auto cba = cb->face(); 
    auto bcd = bc->face();

    auto dac = cba; 
    auto adb = bcd;

    auto da = cb;
    auto ad = bc;
    auto eflip = e0;

    da->setNeighbors(ac, ad, d, eflip, dac);
    ad->setNeighbors(db, da, a, eflip, adb);

    cd->setNeighbors(da, cd->twin(), c, cd->edge(), dac);
    ba->setNeighbors(ad, ba->twin(), b, ba->edge(), adb);

    ac->setNeighbors(cd, ac->twin(), a, ac->edge(), dac);
    db->setNeighbors(ba, db->twin(), d, db->edge(), adb);

    a->halfedge() = ac;
    c->halfedge() = cd;
    d->halfedge() = db;
    b->halfedge() = ba;

    dac->halfedge() = da;
    adb->halfedge() = ad;
    
    eflip->halfedge() = da;


    return eflip;
  }

  VertexIter HalfedgeMesh::splitEdge( EdgeIter e0 )
  {
    // TODO Task 5.
    // This method should split the given edge and return an iterator to the newly inserted vertex.
    // The halfedge of this vertex should point along the edge that was split, rather than the new edges.

    /* Assume The initla state to be:
     *                    c
     *                  / | \
     *                 a  |  d
     *                  \ | /
     *                    b
     * We need to do:
     *                    c
     *                  / | \
     *                 a--e--d
     *                  \ | /
     *                    b
     *  Find all vertices
     *  Find all halfedges
     *  New one vertex
     *  New two faces
     *  New three edges
     *  New six halfedges
     */
    
    auto bc = e0->halfedge();
    auto cb = bc->twin();

    auto ba = cb->next();
    auto cd = bc->next();

    auto ac = cb->next()->next();
    auto db = bc->next()->next();
    
    auto b = bc->vertex();
    auto c = cb->vertex();
    auto a = ac->vertex();
    auto d = db->vertex();

    auto cba = cb->face(); 
    auto bcd = bc->face();

    auto e = newVertex();

    auto ecd = bcd;
    auto cea = cba;
    auto eba = newFace();
    auto dbe = newFace();

    auto E_eb = bc->edge();
    auto E_ec = newEdge();
    auto E_ea = newEdge();
    auto E_ed = newEdge();

    auto ec = bc;
    auto eb = cb;
    auto be = newHalfedge();
    auto ce = newHalfedge();
    auto ea = newHalfedge();
    auto ae = newHalfedge();
    auto ed = newHalfedge();
    auto de = newHalfedge();

    // Vertex udpate
    e->position = (c->position + b->position) / 2.;

    // Edge update
    // E_ec, E_eb, E_ea. E_ed

    E_ea->halfedge() = ea;
    E_eb->halfedge() = eb;
    E_ec->halfedge() = ec;
    E_ed->halfedge() = ed;

    // Face update
    // ecd, cea, eba, dbe
    ecd->halfedge() = ec;
    cea->halfedge() = ea;
    eba->halfedge() = eb;
    dbe->halfedge() = ed;

    // Half-edge update
    // ea/ae, eb/be, ec/ce, ed/de
    // cd, db, ba, ac

    ea->setNeighbors(ac, ae, e, E_ea, cea);
    ae->setNeighbors(eb, ea, a, E_ea, eba);

    eb->setNeighbors(ba, be, e, E_eb, eba);
    be->setNeighbors(ed, eb, b, E_eb, dbe);

    ec->setNeighbors(cd, ce, e, E_ec, ecd);
    ce->setNeighbors(ea, ec, c, E_ec, cea);

    ed->setNeighbors(db, de, e, E_ed, dbe);
    de->setNeighbors(ec, ed, d, E_ed, ecd);

    cd->setNeighbors(de, cd->twin(), c, cd->edge(), ecd);
    db->setNeighbors(be, db->twin(), d, db->edge(), dbe);
    ba->setNeighbors(ae, ba->twin(), b, ba->edge(), eba);
    ac->setNeighbors(ce, ac->twin(), a, ac->edge(), cea);

    return e;
  }



  void MeshResampler::upsample( HalfedgeMesh& mesh )
  {
    // TODO Task 6.
    // This routine should increase the number of triangles in the mesh using Loop subdivision.
    // One possible solution is to break up the method as listed below.

    // 1. Compute new positions for all the vertices in the input mesh, using the Loop subdivision rule,
    // and store them in Vertex::newPosition. At this point, we also want to mark each vertex as being
    // a vertex of the original mesh.
    
    // 2. Compute the updated vertex positions associated with edges, and store it in Edge::newPosition.
    
    // 3. Split every edge in the mesh, in any order. For future reference, we're also going to store some
    // information about which subdivide edges come from splitting an edge in the original mesh, and which edges
    // are new, by setting the flat Edge::isNew. Note that in this loop, we only want to iterate over edges of
    // the original mesh---otherwise, we'll end up splitting edges that we just split (and the loop will never end!)
    
    // 4. Flip any new edge that connects an old and new vertex.

    // 5. Copy the new vertex positions into final Vertex::position.

  }
}

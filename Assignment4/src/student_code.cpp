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

    auto bd = cb->next();
    auto ca = bc->next();

    auto dc = cb->next()->next();
    auto ab = bc->next()->next();
    
    auto b = bc->vertex();
    auto c = cb->vertex();
    auto a = ab->vertex();
    auto d = dc->vertex();

    auto cbd = cb->face(); 
    auto bca = bc->face();

    auto adc = cbd; 
    auto dab = bca;

    auto ad = cb;
    auto da = bc;
    auto eflip = e0;

    da->setNeighbors(ab, ad, d, eflip, dab);
    ad->setNeighbors(dc, da, a, eflip, adc);

    dc->setNeighbors(ca, dc->twin(), d, dc->edge(), adc);
    ab->setNeighbors(bd, ab->twin(), a, ab->edge(), dab);

    ca->setNeighbors(ad, ca->twin(), c, ca->edge(), adc);
    bd->setNeighbors(da, bd->twin(), b, bd->edge(), dab);

    a->halfedge() = ab;
    c->halfedge() = ca;
    d->halfedge() = dc;
    b->halfedge() = bd;

    dab->halfedge() = da;
    adc->halfedge() = ad;
    
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
     *  New:
     *    New 1 vertex
     *    New 2 faces
     *    New 3 edges
     *    New 6 halfedges
     *  Update:
     *    Update 5 vertices
     *    Update 4 faces
     *    Update 4 edges
     *    Update 12 half-edges
     */
    
    auto bc = e0->halfedge();
    auto cb = bc->twin();

    auto bd = cb->next();
    auto ca = bc->next();

    auto dc = cb->next()->next();
    auto ab = bc->next()->next();
    
    auto a = ab->vertex();
    auto b = bc->vertex();
    auto c = cb->vertex();
    auto d = dc->vertex();

    auto cbd = cb->face(); 
    auto bca = bc->face();

    auto e = newVertex();

    auto edc = cbd;
    auto eca = bca;
    auto eab = newFace();
    auto ebd = newFace();

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
    e->halfedge() = ea;
    a->halfedge() = ab;
    b->halfedge() = bd;
    c->halfedge() = dc;
    d->halfedge() = ca;
    e->isNew = true;

    // Edge update
    // E_ec, E_eb, E_ea. E_ed

    E_ea->halfedge()  = ea;
    E_eb->halfedge()  = eb;
    E_ec->halfedge()  = ec;
    E_ed->halfedge()  = ed;

    E_ea->isNew       = true;
    E_eb->isNew       = true;
    E_ec->isNew       = true;
    E_ed->isNew       = true;

    // Face update
    // edc, eca, eab, ebd
    eca->halfedge() = ec;
    eab->halfedge() = ea;
    ebd->halfedge() = eb;
    edc->halfedge() = ed;

    // Half-edge update
    // ea/ae, eb/be, ec/ce, ed/de
    // ab, bd, dc, ca

    ea->setNeighbors(ab, ae, e, E_ea, eab);
    ae->setNeighbors(ec, ea, a, E_ea, eca);

    eb->setNeighbors(bd, be, e, E_eb, ebd);
    be->setNeighbors(ea, eb, b, E_eb, eab);

    ec->setNeighbors(ca, ce, e, E_ec, eca);
    ce->setNeighbors(ed, ec, c, E_ec, edc);

    ed->setNeighbors(dc, de, e, E_ed, edc);
    de->setNeighbors(eb, ed, d, E_ed, ebd);

    ab->setNeighbors(be, ab->twin(), a, ab->edge(), eab);
    bd->setNeighbors(de, bd->twin(), b, bd->edge(), ebd);
    ca->setNeighbors(ae, ca->twin(), c, ca->edge(), eca);
    dc->setNeighbors(ce, dc->twin(), d, dc->edge(), edc);

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
      for (auto v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++){
      int n = v->degree();
      double u = (n == 3) ? 3.0 / 16:3.0 / (8 * n);

      auto h = v->halfedge();
      Vector3D neighbor_sum;
      do {
        neighbor_sum += h->next()->vertex()->position;
        h = h->twin()->next();
      }while(h != v->halfedge());

      v->newPosition = (1 - u * n) * v->position + u * neighbor_sum;
    }

    // 2. Compute the updated vertex positions associated with edges, and store it in Edge::newPosition.
    for (auto e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++) {
      auto h = e->halfedge();
      auto a = h->vertex();
      auto b = h->twin()->vertex();
      auto c = h->next()->next()->vertex();
      auto d = h->twin()->next()->next()->vertex();

      e->newPosition = 0.375 * (a->position + b->position) + 0.125 * (c->position + d->position);
    }

    // 3. Split every edge in the mesh, in any order. For future reference, we're also going to store some
    // information about which subdivide edges come from splitting an edge in the original mesh, and which edges
    // are new, by setting the flat Edge::isNew. Note that in this loop, we only want to iterate over edges of
    // the original mesh---otherwise, we'll end up splitting edges that we just split (and the loop will never end!)
    std::vector<EdgeIter> edges_flipped;
    for (auto e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++){
      if (!e->isNew){
        auto v = mesh.splitEdge(e);
        v->newPosition = e->newPosition;
        edges_flipped.push_back(v->halfedge()->edge());
        edges_flipped.push_back(v->halfedge()->twin()->next()->twin()->next()->edge());
      }
    }
    // 4. Flip any new edge that connects an old and new vertex.
    for (auto e:edges_flipped){
      if (e->halfedge()->vertex()->isNew ^ e->halfedge()->twin()->vertex()->isNew){
        mesh.flipEdge(e);
      }
    }
    // 5. Copy the new vertex positions into final Vertex::position.
    for (VertexIter v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++) {
        v->position = v->newPosition;
        v->isNew = false;
    }

    for (EdgeIter e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++) {
      e->isNew = false;
    }
  }
}

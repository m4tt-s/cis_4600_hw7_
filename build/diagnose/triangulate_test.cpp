#include "mesh.h"
#include <iostream>
#include <stdexcept>
void check(bool b) { if (!b) throw std::runtime_error("Topology check failed"); }
int main() {
 for (int n : {3, 4, 5, 8}) {
  Mesh m(nullptr);
  for (int i=0;i<n;++i) {
   m.vertices.push_back(std::make_unique<Vertex>(glm::vec3(i,0,0)));
   m.halfEdges.push_back(std::make_unique<HalfEdge>());
  }
  m.faces.push_back(std::make_unique<Face>(glm::vec3(.2f,.4f,.6f)));
  Face* f=m.faces[0].get();
  for(int i=0;i<n;++i) {
   auto e=m.halfEdges[i].get();
   e->next=m.halfEdges[(i+1)%n].get();
   e->vertex=m.vertices[i].get(); e->face=f; e->vertex->halfEdge=e;
  }
  f->halfEdge=m.halfEdges[0].get();
  auto anchor=f->halfEdge;
  check(m.triangulateFace(f)==(n>3));
  check(m.faces.size()==size_t(n-2));
  check(m.halfEdges.size()==size_t(n+2*(n-3)));
  check(m.vertices.size()==size_t(n));
  check(f->halfEdge==anchor);
  size_t visited=0;
  for(auto& owner:m.faces) {
   auto start=owner->halfEdge; auto e=start;
   for(int j=0;j<3;++j) {
    check(e && e->face==owner.get());
    if(e->sym) {
     check(e->sym->sym==e);
     check(e->vertex==e->sym->next->next->vertex);
     check(e->next->next->vertex==e->sym->vertex);
    }
    e=e->next; ++visited;
   }
   check(e==start); check(owner->color==f->color);
   check(!m.triangulateFace(owner.get()));
  }
  check(visited==m.halfEdges.size());
  for(auto& v:m.vertices) check(v->halfEdge->vertex==v.get());
  check(!m.triangulateFace(nullptr));
 }
 Mesh m(nullptr); Face f; check(!m.triangulateFace(&f));
 std::cout << "PASS: triangle, quad, pentagon, octagon; counts, loops, face colors, diagonal twins, representatives, no-op guards\n";
}

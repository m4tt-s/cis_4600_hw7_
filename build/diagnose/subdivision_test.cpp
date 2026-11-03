#include "mesh.h"
#include <QTemporaryDir>
#include <QFile>
#include <iostream>
#include <stdexcept>
#include <chrono>
void check(bool b) { if(!b) throw std::runtime_error("Subdivision check failed"); }
void topology(Mesh& m) {
 for(auto& f:m.faces) {
  auto e=f->halfEdge;
  for(int i=0;i<4;++i) {
   check(e && e->face==f.get() && e->sym && e->sym->sym==e);
   check(e->vertex==e->sym->next->next->next->vertex);
   e=e->next;
  }
  check(e==f->halfEdge);
 }
 for(auto& v:m.vertices) check(v->halfEdge && v->halfEdge->vertex==v.get());
 check(m.vertices.size()-m.halfEdges.size()/2+m.faces.size()==2);
}
bool near(glm::vec3 a,glm::vec3 b,float eps=.00006f) {return glm::length(a-b)<eps;}
int main() {
 QTemporaryDir dir; QFile file(dir.filePath("cube.obj")); check(file.open(QIODevice::WriteOnly));
     QByteArray cube = "v -1 -1 -1\nv 1 -1 -1\nv 1 1 -1\nv -1 1 -1\nv -1 -1 1\nv 1 -1 1\nv 1 1 1\nv -1 1 1\nf 1/1/1 4/1/1 3/1/1 2/1/1\nf 5 6 7 8\nf 1 2 6 5\nf 4 8 7 3\nf 1 5 8 4\nf -7 -6 -2 -3\n";
 file.write(cube); file.close(); Mesh m(nullptr); QString error; check(m.loadOBJ(file.fileName(),error));
 for(auto& v:m.vertices) v->position*=.5f;
 check(m.subdivideCatmullClark()); topology(m);
 check(m.vertices.size()==26 && m.faces.size()==24 && m.halfEdges.size()==96);
 for(int i=0;i<8;++i) check(near(glm::abs(m.vertices[i]->position),glm::vec3(11.f/36.f)));
 int centers=0,edges=0;
 for(auto& v:m.vertices) {
  auto p=glm::abs(v->position);
  if(near(p,{.5,0,0}) || near(p,{0,.5,0}) || near(p,{0,0,.5})) ++centers;
  if(near(p,{.375,.375,0}) || near(p,{.375,0,.375}) || near(p,{0,.375,.375})) ++edges;
 }
 check(centers==6 && edges==12);
 check(m.subdivideCatmullClark()); topology(m);
 check(m.vertices.size()==98 && m.faces.size()==96 && m.halfEdges.size()==384);
 for(int i=0;i<8;++i) check(near(glm::abs(m.vertices[i]->position),glm::vec3(.2697f),.0001f));
 int matches=0;
 for(auto& v:m.vertices) { auto p=glm::abs(v->position);
 if(near(p,{.1615,.3099,.3099},.0001f)||near(p,{.3099,.1615,.3099},.0001f)||near(p,{.3099,.3099,.1615},.0001f)) ++matches;
 }
 check(matches==24);
 auto begin=std::chrono::steady_clock::now();
 for(int i=0;i<3;++i) {check(m.subdivideCatmullClark()); topology(m);}
 std::cout << "PASS: five iterations, supplied cube coordinates, counts, quad loops, sym endpoints, representatives. Extra three iterations: " << std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count() << "s\n";
 auto old=m.vertices[0].get(); auto e=m.halfEdges[0].get(); auto sym=e->sym; e->sym=nullptr;
 check(!m.subdivideCatmullClark() && m.vertices[0].get()==old); e->sym=sym;
 Mesh empty(nullptr); check(!empty.subdivideCatmullClark());
}

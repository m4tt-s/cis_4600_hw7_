#include "mainwindow.h"
#include "mygl.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QDoubleSpinBox>
#include <QListWidget>
#include <QTemporaryDir>
#include <QFile>
#include <iostream>
#include <stdexcept>
void check(bool b) { if(!b) throw std::runtime_error("Attribute test failed"); }
int main(int argc,char** argv) {
 QSurfaceFormat fmt; fmt.setVersion(3,2); fmt.setProfile(QSurfaceFormat::CoreProfile); QSurfaceFormat::setDefaultFormat(fmt);
 QApplication app(argc,argv); MainWindow w; w.show();
 for(int i=0;i<20;++i) app.processEvents();
 auto gl=w.findChild<MyGL*>("mygl"); check(gl && gl->isValid());
 auto x=w.findChild<QDoubleSpinBox*>("vertPosXSpinBox");
 auto red=w.findChild<QDoubleSpinBox*>("faceRedSpinBox");
 check(!x->isEnabled() && !red->isEnabled());
 QTemporaryDir dir; QFile file(dir.filePath("cube.obj")); check(file.open(QIODevice::WriteOnly));
     QByteArray cube = "v -1 -1 -1\nv 1 -1 -1\nv 1 1 -1\nv -1 1 -1\nv -1 -1 1\nv 1 -1 1\nv 1 1 1\nv -1 1 1\nf 1/1/1 4/1/1 3/1/1 2/1/1\nf 5 6 7 8\nf 1 2 6 5\nf 4 8 7 3\nf 1 5 8 4\nf -7 -6 -2 -3\n";
 file.write(cube); file.close(); QString error; check(gl->loadOBJ(file.fileName(),error));
 auto verts=w.findChild<QListWidget*>("vertsListWidget");
 auto faces=w.findChild<QListWidget*>("facesListWidget");
 auto v=gl->mesh().vertices[0].get(); auto f=gl->mesh().faces[0].get();
 auto originalColor=f->color; verts->setCurrentRow(0); faces->setCurrentRow(0);
 check(x->isEnabled() && red->isEnabled()); check(x->value()==v->position.x); check(f->color==originalColor);
 x->setValue(-1.25); check(v->position.x==-1.25f);
 red->setValue(.25); check(f->color.r==.25f);
 gl->makeCurrent(); auto& mesh=const_cast<Mesh&>(gl->mesh());
 mesh.bindBuffer(POSITION); glm::vec3 position; gl->glGetBufferSubData(GL_ARRAY_BUFFER,0,sizeof(position),&position);
 check(position==f->halfEdge->vertex->position);
 mesh.bindBuffer(COLOR); glm::vec3 color; gl->glGetBufferSubData(GL_ARRAY_BUFFER,0,sizeof(color),&color); check(color==f->color);
 gl->doneCurrent(); app.processEvents();
 gl->selectVertex(nullptr); gl->selectFace(nullptr); check(!x->isEnabled() && !red->isEnabled());
 std::cout << "PASS: selection synchronization, disabled controls, widget edits, GPU position/color readback\n";
}

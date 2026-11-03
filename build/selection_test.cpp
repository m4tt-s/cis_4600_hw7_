#include "mainwindow.h"
#include "mygl.h"
#include <QApplication>
#include <QTemporaryDir>
#include <QFile>
#include <QThread>
#include <QSurfaceFormat>
#include <QKeyEvent>
#include <QShortcut>
#include <iostream>
#include <stdexcept>

void check(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
int main(int argc, char** argv) {
    QSurfaceFormat format;
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    QApplication app(argc, argv);
    QTemporaryDir dir;
    QString path = dir.filePath("cube.obj");
    QFile file(path);
    check(file.open(QIODevice::WriteOnly), "fixture");
    file.write("v -1 -1 -1\nv 1 -1 -1\nv 1 1 -1\nv -1 1 -1\nv -1 -1 1\nv 1 -1 1\nv 1 1 1\nv -1 1 1\nf 1 4 3 2\nf 5 6 7 8\nf 1 2 6 5\nf 4 8 7 3\nf 1 5 8 4\nf 2 3 7 6\n");
    file.close();
    {
        MainWindow window;
        window.setAttribute(Qt::WA_DontShowOnScreen);
        window.show();
        auto gl = window.findChild<MyGL*>("mygl");
        auto vertices = window.findChild<QListWidget*>("vertsListWidget");
        auto edges = window.findChild<QListWidget*>("halfEdgesListWidget");
        auto faces = window.findChild<QListWidget*>("facesListWidget");
        for (int i = 0; i < 20; ++i) { app.processEvents(); QThread::msleep(10); }
        check(gl->isValid(), "OpenGL context");
        QString error;
        check(gl->loadOBJ(path, error), "import");
        check(vertices->count() == 8 && edges->count() == 24 && faces->count() == 6, "list population");
        auto press = [&](int key, Qt::KeyboardModifiers modifiers = Qt::NoModifier) {
            QKeyEvent event(QEvent::KeyPress, key, modifiers);
            QApplication::sendEvent(gl, &event);
        };
        for (int key : {Qt::Key_N, Qt::Key_M, Qt::Key_F, Qt::Key_V, Qt::Key_H}) press(key);
        press(Qt::Key_H, Qt::ShiftModifier);
        check(edges->currentItem() == nullptr, "null shortcuts");
        auto first = gl->mesh().halfEdges[0].get();
        edges->setCurrentItem(first);
        press(Qt::Key_N);
        check(edges->currentItem() == first->next, "NEXT");
        press(Qt::Key_M);
        auto opposite = first->next->sym;
        check(edges->currentItem() == opposite, "SYM");
        press(Qt::Key_F);
        check(faces->currentItem() == opposite->face, "FACE");
        press(Qt::Key_V);
        check(vertices->currentItem() == opposite->vertex, "VERTEX");
        press(Qt::Key_H);
        check(edges->currentItem() == opposite->vertex->halfEdge, "vertex HALF-EDGE");
        press(Qt::Key_H, Qt::ShiftModifier);
        check(edges->currentItem() == opposite->face->halfEdge, "face HALF-EDGE");
        auto current = dynamic_cast<HalfEdge*>(edges->currentItem());
        for (auto shortcut : window.findChildren<QShortcut*>()) {
            if (shortcut->key() == QKeySequence(Qt::Key_N)) {
                QMetaObject::invokeMethod(shortcut, "activated", Qt::DirectConnection);
            }
        }
        check(edges->currentItem() == current->next, "window shortcut forwarding");
        gl->selectVertex(nullptr);
        gl->selectHalfEdge(nullptr);
        gl->selectFace(nullptr);
        auto baseline = gl->grabFramebuffer();
        check(!baseline.isNull(), "framebuffer");
        vertices->setCurrentRow(0);
        check(dynamic_cast<Vertex*>(vertices->currentItem()) == gl->mesh().vertices[0].get(), "vertex item identity");
        auto point = gl->grabFramebuffer();
        check(point != baseline, "vertex highlight");
        edges->setCurrentRow(0);
        auto edge = gl->grabFramebuffer();
        check(edge != point, "edge highlight");
        faces->setCurrentRow(0);
        auto face = gl->grabFramebuffer();
        check(face != edge, "face highlight");
        face.save("selection-test.png");
        gl->makeCurrent();
        check(gl->glIsEnabled(GL_DEPTH_TEST), "depth test restored");
        gl->doneCurrent();
        check(!gl->loadOBJ(dir.filePath("missing.obj"), error), "invalid import");
        check(vertices->count() == 8 && faces->count() == 6, "lists restored on failure");
        gl->grabFramebuffer();
        check(gl->loadOBJ(path, error), "second import");
        check(edges->count() == 24 && vertices->currentItem() == nullptr, "replacement resets selection");
        edges->setCurrentRow(1);
        gl->grabFramebuffer();
    }
    std::cout << "PASS: list identity, all three rendered highlights, depth restoration, failed import, replacement, destruction\n";
}


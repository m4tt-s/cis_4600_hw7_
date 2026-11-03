#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"
#include "mesh.h"
#include "componentdisplay.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QTimer>


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    QTimer timer;
    float currTime;

    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    Mesh m_mesh;
    VertexDisplay m_vertDisplay;
    HalfEdgeDisplay m_edgeDisplay;
    FaceDisplay m_faceDisplay;
    bool m_selectionDirty;
    Vertex* m_selectedVertex;
    HalfEdge* m_selectedHalfEdge;
    Face* m_selectedFace;
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_camera;
    // A variable used to track the mouse's previous position when
    // clicking and dragging on the GL viewport. Used to move the camera
    // in the scene.
    glm::vec2 m_mousePosPrev;
    bool refreshMeshBuffers(QString& error);
    void refreshTopology();


public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    const Mesh& mesh() const;
    void selectVertex(Vertex* vertex);
    void selectHalfEdge(HalfEdge* edge);
    void selectFace(Face* face);
    bool loadOBJ(const QString& path, QString& error);
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

signals:
    void meshAboutToChange();
    void meshChanged();
    void vertexSelected(Vertex* vertex);
    void halfEdgeSelected(HalfEdge* edge);
    void faceSelected(Face* face);
    void attributeEditFailed(const QString& error);

public slots:
    void setVertexPosition(int axis, double value);
    void setFaceColor(int channel, double value);
    // Connect the topology buttons to these selection-aware entry points.
    void splitSelectedHalfEdge();
    void triangulateSelectedFace();
    void subdivideMesh();
    void tick();
};


#endif // MYGL_H




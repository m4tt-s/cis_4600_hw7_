#include "mygl.h"
#include <la.h>

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      timer(), currTime(0.),
      m_geomSquare(this), m_mesh(this),
      m_vertDisplay(this), m_edgeDisplay(this), m_faceDisplay(this),
      m_selectionDirty(true),
      m_selectedVertex(nullptr), m_selectedHalfEdge(nullptr), m_selectedFace(nullptr),
      m_progLambert(this), m_progFlat(this),
      vao(),
      m_camera(width(), height()),
      m_mousePosPrev()
{
    setFocusPolicy(Qt::StrongFocus);

    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Create the instances of Cylinder and Sphere.
    m_geomSquare.initializeAndBufferGeometryData();

    // Create and set up the diffuse shader
    m_progLambert.createAndCompileShaderProgram(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.createAndCompileShaderProgram(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_camera.recomputeAspectRatio(w, h);

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)
    glm::mat4 viewproj = m_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 viewproj = m_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);
    m_progLambert.setUnifVec3("u_CamPos", m_camera.eye);
    m_progFlat.setUnifMat4("u_Model", glm::mat4(1.f));

    if (!m_mesh.faces.empty()) {
        m_progLambert.setUnifMat4("u_Model", glm::mat4(1.f));
        m_progLambert.setUnifMat4("u_ModelInvTr", glm::mat4(1.f));
        m_progLambert.draw(m_mesh);
        if (m_selectionDirty) {
            m_vertDisplay.initializeAndBufferGeometryData();
            m_edgeDisplay.initializeAndBufferGeometryData();
            m_faceDisplay.initializeAndBufferGeometryData();
            m_selectionDirty = false;
        }
        glDisable(GL_DEPTH_TEST);
        if (m_faceDisplay.getIndexBufferLength() > 0) m_progFlat.draw(m_faceDisplay);
        if (m_edgeDisplay.getIndexBufferLength() > 0) m_progFlat.draw(m_edgeDisplay);
        if (m_vertDisplay.getIndexBufferLength() > 0) m_progFlat.draw(m_vertDisplay);
        glEnable(GL_DEPTH_TEST);
        return;
    }

    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
    //Send the geometry's transformation matrix to the shader
    m_progLambert.setUnifMat4("u_Model", model);
    m_progLambert.setUnifMat4("u_ModelInvTr", glm::inverse(glm::transpose(model)));
    //Draw the example sphere using our lambert shader
    m_progLambert.draw(m_geomSquare);

    //Now do the same to render the cylinder
    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
    model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
    m_progLambert.setUnifMat4("u_Model", model);
    m_progLambert.setUnifMat4("u_ModelInvTr", glm::inverse(glm::transpose(model)));
    m_progLambert.draw(m_geomSquare);
}

void MyGL::keyPressEvent(QKeyEvent *e) {
    if (e->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) {
        OpenGLContext::keyPressEvent(e);
        return;
    }
    switch (e->key()) {
    case Qt::Key_N:
        if (m_selectedHalfEdge && m_selectedHalfEdge->next) selectHalfEdge(m_selectedHalfEdge->next);
        break;
    case Qt::Key_M:
        if (m_selectedHalfEdge && m_selectedHalfEdge->sym) selectHalfEdge(m_selectedHalfEdge->sym);
        break;
    case Qt::Key_F:
        if (m_selectedHalfEdge && m_selectedHalfEdge->face) selectFace(m_selectedHalfEdge->face);
        break;
    case Qt::Key_V:
        if (m_selectedHalfEdge && m_selectedHalfEdge->vertex) selectVertex(m_selectedHalfEdge->vertex);
        break;
    case Qt::Key_H:
        if (e->modifiers() & Qt::ShiftModifier) {
            if (m_selectedFace && m_selectedFace->halfEdge) selectHalfEdge(m_selectedFace->halfEdge);
        } else if (m_selectedVertex && m_selectedVertex->halfEdge) {
            selectHalfEdge(m_selectedVertex->halfEdge);
        }
        break;
    default:
        OpenGLContext::keyPressEvent(e);
        return;
    }
    e->accept();
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    if(e->buttons() & (Qt::LeftButton | Qt::RightButton))
    {
        m_mousePosPrev = glm::vec2(e->pos().x(), e->pos().y());
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    glm::vec2 pos(e->pos().x(), e->pos().y());
    if(e->buttons() & Qt::LeftButton)
    {
        // Rotation
        glm::vec2 diff = 0.2f * (pos - m_mousePosPrev);
        m_mousePosPrev = pos;
        m_camera.RotateAboutGlobalUp(-diff.x);
        m_camera.RotateAboutLocalRight(-diff.y);
    }
    else if(e->buttons() & Qt::RightButton)
    {
        // Panning
        glm::vec2 diff = 0.05f * (pos - m_mousePosPrev);
        m_mousePosPrev = pos;
        m_camera.PanAlongRight(-diff.x);
        m_camera.PanAlongUp(diff.y);
    }
}

void MyGL::wheelEvent(QWheelEvent *e) {
    m_camera.Zoom(e->angleDelta().y() * 0.001f);
}

void MyGL::tick() {
    ++currTime;
    update();
}

bool MyGL::refreshMeshBuffers(QString& error) {
    makeCurrent();
    glBindVertexArray(vao);
    try {
        // Rebuild positions, normals, colors, and indices consumed by GLSL.
        m_mesh.initializeAndBufferGeometryData();
    } catch (const std::exception& e) {
        doneCurrent();
        error = QString::fromUtf8(e.what());
        return false;
    }
    doneCurrent();
    m_selectionDirty = true;
    update();
    return true;
}

void MyGL::setVertexPosition(int axis, double value) {
    if (!m_selectedVertex || !isValid() || axis < 0 || axis > 2 ||
        !std::isfinite(value) || !std::isfinite(float(value))) return;
    float& coordinate = m_selectedVertex->position[axis];
    const float previous = coordinate;
    coordinate = float(value);
    QString error;
    if (!refreshMeshBuffers(error)) {
        // Geometry is validated before any buffers are uploaded.
        coordinate = previous;
        emit vertexSelected(m_selectedVertex);
        emit attributeEditFailed(error);
    }
}

void MyGL::setFaceColor(int channel, double value) {
    if (!m_selectedFace || !isValid() || channel < 0 || channel > 2 ||
        !std::isfinite(value) || value < 0.0 || value > 1.0) return;
    float& component = m_selectedFace->color[channel];
    const float previous = component;
    component = float(value);
    QString error;
    if (!refreshMeshBuffers(error)) {
        component = previous;
        emit faceSelected(m_selectedFace);
        emit attributeEditFailed(error);
    }
}

void MyGL::refreshTopology() {
    // Repopulating the lists can emit selection changes, so save pointers first.
    Vertex* vertex = m_selectedVertex;
    HalfEdge* edge = m_selectedHalfEdge;
    Face* face = m_selectedFace;
    emit meshAboutToChange();
    emit meshChanged();
    selectVertex(vertex);
    selectHalfEdge(edge);
    selectFace(face);
    QString error;
    if (!refreshMeshBuffers(error)) emit attributeEditFailed(error);
}

void MyGL::splitSelectedHalfEdge() {
    if (!m_selectedHalfEdge || !isValid()) return;
    if (m_mesh.splitHalfEdge(m_selectedHalfEdge)) refreshTopology();
}

void MyGL::triangulateSelectedFace() {
    if (!m_selectedFace || !isValid()) return;
    if (m_mesh.triangulateFace(m_selectedFace)) refreshTopology();
}
void MyGL::subdivideMesh() {
    if (!isValid() || m_mesh.faces.empty()) return;

    // Replacement topology will destroy old QListWidgetItems. Detach them
    // and clear selection/display pointers BEFORE committing the operation.
    emit meshAboutToChange();
    selectVertex(nullptr);
    selectHalfEdge(nullptr);
    selectFace(nullptr);
    const bool changed = m_mesh.subdivideCatmullClark();
    emit meshChanged();
    if (changed) {
        QString error;
        if (!refreshMeshBuffers(error)) emit attributeEditFailed(error);
    }
}

bool MyGL::loadOBJ(const QString& path, QString& error) {
    if (!isValid()) {
        error = "The OpenGL viewport is not ready.";
        return false;
    }
    emit meshAboutToChange();
    selectVertex(nullptr);
    selectHalfEdge(nullptr);
    selectFace(nullptr);
    if (!m_mesh.loadOBJ(path, error)) {
        emit meshChanged();
        return false;
    }
    makeCurrent();
    glBindVertexArray(vao);
    m_mesh.initializeAndBufferGeometryData();
    doneCurrent();
    glm::vec3 low = m_mesh.vertices.front()->position;
    glm::vec3 high = low;
    for (const auto& vertex : m_mesh.vertices) {
        low = glm::min(low, vertex->position);
        high = glm::max(high, vertex->position);
    }
    float radius = glm::max(glm::length(high - low) * 0.5f, 0.001f);
    m_camera = Camera(width(), glm::max(height(), 1));
    m_camera.target = (low + high) * 0.5f;
    float halfFov = glm::radians(m_camera.fovy) * 0.5f;
    float horizontal = std::atan(std::tan(halfFov) * m_camera.aspect);
    float distance = 1.1f * radius / std::sin(glm::min(halfFov, horizontal));
    m_camera.eye = m_camera.target + glm::vec3(0.f, 0.f, distance);
    m_camera.nearClip = radius * 0.001f;
    m_camera.farClip = distance + radius * 4.f;
    emit meshChanged();
    update();
    return true;
}

const Mesh& MyGL::mesh() const { return m_mesh; }

void MyGL::selectVertex(Vertex* vertex) {
    m_selectedVertex = vertex;
    emit vertexSelected(vertex);
    m_vertDisplay.updateVertex(vertex);
    m_selectionDirty = true;
    update();
}

void MyGL::selectHalfEdge(HalfEdge* edge) {
    m_selectedHalfEdge = edge;
    emit halfEdgeSelected(edge);
    m_edgeDisplay.updateHalfEdge(edge);
    m_selectionDirty = true;
    update();
}

void MyGL::selectFace(Face* face) {
    m_selectedFace = face;
    emit faceSelected(face);
    m_faceDisplay.updateFace(face);
    m_selectionDirty = true;
    update();
}



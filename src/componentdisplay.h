#pragma once

#include "drawable.h"
#include "vertex.h"
#include "halfedge.h"
#include "face.h"

class VertexDisplay : public Drawable {
    Vertex* representedVertex;
public:
    explicit VertexDisplay(OpenGLContext* context);
    void updateVertex(Vertex* vertex);
    void initializeAndBufferGeometryData() override;
    GLenum drawMode() override;
};

class HalfEdgeDisplay : public Drawable {
    HalfEdge* representedHalfEdge;
public:
    explicit HalfEdgeDisplay(OpenGLContext* context);
    void updateHalfEdge(HalfEdge* edge);
    void initializeAndBufferGeometryData() override;
    GLenum drawMode() override;
};

class FaceDisplay : public Drawable {
    Face* representedFace;
public:
    explicit FaceDisplay(OpenGLContext* context);
    void updateFace(Face* face);
    void initializeAndBufferGeometryData() override;
    GLenum drawMode() override;
};

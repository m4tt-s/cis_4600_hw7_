#pragma once

#include "drawable.h"
#include "face.h"
#include "halfedge.h"
#include "vertex.h"

#include <QString>
#include <memory>
#include <vector>

class Mesh : public Drawable
{
public:
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<HalfEdge>> halfEdges;
    std::vector<std::unique_ptr<Vertex>> vertices;

    explicit Mesh(OpenGLContext* context);
    ~Mesh() override;

    bool loadOBJ(const QString& path, QString& error);
    Vertex* splitHalfEdge(HalfEdge* edge);
    bool triangulateFace(Face* face);
    // One subdivision iteration; true only when new topology is committed.
    bool subdivideCatmullClark();
    void initializeAndBufferGeometryData() override;

};


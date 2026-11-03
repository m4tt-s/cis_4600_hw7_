#include "vertex.h"
#include "halfedge.h"

int Vertex::nextId = 0;

Vertex::Vertex(const glm::vec3& position) :
    position(position),  halfEdge(nullptr)
{
    id = nextId;
    nextId ++;
    setText(QString("Vertex %1").arg(id));
}

Vertex::Vertex() : Vertex(glm::vec3(0.f)) {}

Vertex::~Vertex() = default;

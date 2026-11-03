#include "face.h"

int Face::nextId = 0;

Face::Face() : Face(glm::vec3(1.f))
{}

Face::Face(const glm::vec3& color) :
    halfEdge(nullptr), color(color), id(nextId++)
{
    setText(QString("Face %1").arg(id));
}

Face::~Face() = default;



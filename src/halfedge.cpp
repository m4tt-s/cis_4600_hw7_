#include "halfedge.h"
#include "face.h"
#include "vertex.h"

int HalfEdge::nextId = 0;

HalfEdge::HalfEdge() :
    next(nullptr), sym(nullptr), face(nullptr), vertex(nullptr)
{   id = nextId;
    nextId++;
    setText(QString("HalfEdge %1").arg(id));
}

HalfEdge::~HalfEdge() = default;


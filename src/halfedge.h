#pragma once

#include <QListWidgetItem>

class Face;
class Vertex;

class HalfEdge : public QListWidgetItem
{
public:
    HalfEdge* next;
    HalfEdge* sym;
    Face* face;
    Vertex* vertex;
    int id;

    HalfEdge();
    ~HalfEdge();

private:
    static int nextId;
};



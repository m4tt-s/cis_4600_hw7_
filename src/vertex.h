#pragma once

#include <QListWidgetItem>

#include <la.h>

class HalfEdge;

class Vertex : public QListWidgetItem
{
public:
    glm::vec3 position;
    HalfEdge* halfEdge;
    int id;

    Vertex();
    explicit Vertex(const glm::vec3& position);
    ~Vertex();

private:
    static int nextId;
};



#pragma once

#include <QListWidgetItem>

#include <la.h>

class HalfEdge;

class Face : public QListWidgetItem
{
public:
    HalfEdge* halfEdge;
    glm::vec3 color;
    int id;

    Face();
    explicit Face(const glm::vec3& color);
    ~Face();

private:
    static int nextId;
};



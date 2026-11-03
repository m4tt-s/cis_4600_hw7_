#include "componentdisplay.h"

namespace {
void upload(Drawable& drawable, const std::vector<glm::vec3>& positions,
            const std::vector<glm::vec3>& colors, const std::vector<GLuint>& indices) {
    for (BufferType type : {POSITION, COLOR, INDEX}) {
        if (!drawable.hasBuffer(type)) drawable.generateBuffer(type);
        drawable.bindBuffer(type);
        if (type == POSITION) drawable.bufferData(type, positions);
        else if (type == COLOR) drawable.bufferData(type, colors);
        else drawable.bufferData(type, indices);
    }
}
}

VertexDisplay::VertexDisplay(OpenGLContext* context)
    : Drawable(context), representedVertex(nullptr) {}

void VertexDisplay::updateVertex(Vertex* vertex) {
    representedVertex = vertex;
}

GLenum VertexDisplay::drawMode() { return GL_POINTS; }

void VertexDisplay::initializeAndBufferGeometryData() {
    indexBufferLength = 0;
    if (!representedVertex) return;
    upload(*this, {representedVertex->position}, {glm::vec3(1.f)}, {0});
    indexBufferLength = 1;
}

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext* context)
    : Drawable(context), representedHalfEdge(nullptr) {}

void HalfEdgeDisplay::updateHalfEdge(HalfEdge* edge) {
    representedHalfEdge = edge;
}

GLenum HalfEdgeDisplay::drawMode() { return GL_LINES; }

void HalfEdgeDisplay::initializeAndBufferGeometryData() {
    indexBufferLength = 0;
    if (!representedHalfEdge || !representedHalfEdge->vertex) return;
    HalfEdge* previous = representedHalfEdge;
    if (representedHalfEdge->sym) {
        previous = representedHalfEdge->sym;
    } else {
        while (previous->next != representedHalfEdge) previous = previous->next;
    }
    upload(*this,
           {previous->vertex->position, representedHalfEdge->vertex->position},
           {glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 0.f)}, {0, 1});
    indexBufferLength = 2;
}

FaceDisplay::FaceDisplay(OpenGLContext* context)
    : Drawable(context), representedFace(nullptr) {}

void FaceDisplay::updateFace(Face* face) {
    representedFace = face;
}

GLenum FaceDisplay::drawMode() { return GL_LINES; }

void FaceDisplay::initializeAndBufferGeometryData() {
    indexBufferLength = 0;
    if (!representedFace || !representedFace->halfEdge) return;
    std::vector<glm::vec3> positions;
    std::vector<GLuint> indices;
    HalfEdge* start = representedFace->halfEdge;
    HalfEdge* edge = start;
    do {
        positions.push_back(edge->vertex->position);
        edge = edge->next;
    } while (edge != start);
    for (size_t i = 0; i < positions.size(); ++i) {
        indices.push_back(static_cast<GLuint>(i));
        indices.push_back(static_cast<GLuint>((i + 1) % positions.size()));
    }
    std::vector<glm::vec3> colors(positions.size(), glm::vec3(1.f) - representedFace->color);
    upload(*this, positions, colors, indices);
    indexBufferLength = static_cast<int>(indices.size());
}

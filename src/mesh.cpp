#include "mesh.h"
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <cstdint>

bool Mesh::subdivideCatmullClark() {
    if (faces.empty()) return false;

    std::unordered_set<HalfEdge*> ownedEdges;
    std::unordered_set<Vertex*> ownedVertices;
    for (const auto& edge : halfEdges) ownedEdges.insert(edge.get());
    for (const auto& vertex : vertices) ownedVertices.insert(vertex.get());
    std::unordered_set<HalfEdge*> visited;
    std::unordered_map<Vertex*, size_t> incidentCounts;
    std::unordered_map<Face*, glm::vec3> faceCentroids;
    std::unordered_map<HalfEdge*, HalfEdge*> previous;
    for (const auto& face : faces) {
        HalfEdge* start = face->halfEdge;
        HalfEdge* edge = start;
        glm::vec3 sum(0.0f);
        size_t count = 0;
        do {
            if (!ownedEdges.contains(edge) || !visited.insert(edge).second ||
                edge->face != face.get() || !ownedVertices.contains(edge->vertex) ||
                !ownedEdges.contains(edge->next)) return false;
            sum += edge->vertex->position;
            ++incidentCounts[edge->vertex];
            previous[edge->next] = edge;
            ++count;
            edge = edge->next;
        } while (edge != start);
        if (count < 3) return false;
        faceCentroids[face.get()] = sum / static_cast<float>(count);
    }
    if (visited.size() != halfEdges.size()) return false;

    std::unordered_map<HalfEdge*, glm::vec3> edgePoints;
    for (const auto& owner : halfEdges) {
        HalfEdge* edge = owner.get();
        HalfEdge* sym = edge->sym;
        if (!ownedEdges.contains(sym) || sym == edge || sym->sym != edge ||
            previous.at(edge)->vertex != sym->vertex ||
            previous.at(sym)->vertex != edge->vertex) return false;
        if (edgePoints.contains(edge)) continue;
        glm::vec3 point = (edge->vertex->position + sym->vertex->position +
                          faceCentroids.at(edge->face) + faceCentroids.at(sym->face)) / 4.0f;
        edgePoints[edge] = point;
        edgePoints[sym] = point;
    }

    std::unordered_map<Vertex*, glm::vec3> updatedPositions;
    for (const auto& owner : vertices) {
        Vertex* vertex = owner.get();
        HalfEdge* start = vertex->halfEdge;
        HalfEdge* edge = start;
        glm::vec3 sumEdges(0.0f), sumFaces(0.0f);
        size_t count = 0;
        do {
            if (!ownedEdges.contains(edge) || edge->vertex != vertex ||
                ++count > incidentCounts[vertex]) return false;
            sumEdges += edgePoints.at(edge);
            sumFaces += faceCentroids.at(edge->face);
            edge = edge->next->sym;
        } while (edge != start);
        if (count != incidentCounts[vertex]) return false;
        float n = static_cast<float>(count);
        updatedPositions[vertex] = ((n - 2.0f) / n) * vertex->position +
                                   (sumEdges + sumFaces) / (n * n);
    }

    std::vector<std::unique_ptr<Vertex>> newVertices;
    std::vector<std::unique_ptr<Face>> newFaces;
    std::vector<std::unique_ptr<HalfEdge>> newEdges;
    newVertices.reserve(vertices.size() + halfEdges.size() / 2 + faces.size());
    newFaces.reserve(halfEdges.size());
    newEdges.reserve(4 * halfEdges.size());
    auto addVertex = [&](const glm::vec3& position) {
        newVertices.push_back(std::make_unique<Vertex>(position));
        return newVertices.back().get();
    };
    std::unordered_map<Vertex*, Vertex*> vertexCopies;
    std::unordered_map<Face*, Vertex*> faceVertices;
    std::unordered_map<HalfEdge*, Vertex*> edgeVertices;
    for (const auto& vertex : vertices)
        vertexCopies[vertex.get()] = addVertex(updatedPositions.at(vertex.get()));
    for (const auto& face : faces)
        faceVertices[face.get()] = addVertex(faceCentroids.at(face.get()));
    for (const auto& edge : halfEdges) {
        if (edgeVertices.contains(edge.get())) continue;
        Vertex* point = addVertex(edgePoints.at(edge.get()));
        edgeVertices[edge.get()] = point;
        edgeVertices[edge->sym] = point;
    }

    std::unordered_map<Vertex*, std::unordered_map<Vertex*, HalfEdge*>> directedEdges;
    for (const auto& face : faces) {
        HalfEdge* start = face->halfEdge;
        HalfEdge* edge = start;
        do {
            Vertex* corners[4] = {vertexCopies.at(edge->vertex),
                                  edgeVertices.at(edge->next),
                                  faceVertices.at(face.get()), edgeVertices.at(edge)};
            auto quad = std::make_unique<Face>(face->color);
            HalfEdge* loop[4];
            for (int i = 0; i < 4; ++i) {
                newEdges.push_back(std::make_unique<HalfEdge>());
                loop[i] = newEdges.back().get();
                loop[i]->vertex = corners[(i + 1) % 4];
                loop[i]->face = quad.get();
                loop[i]->vertex->halfEdge = loop[i];
                if (!directedEdges[corners[i]].emplace(corners[(i + 1) % 4], loop[i]).second)
                    return false;
            }
            for (int i = 0; i < 4; ++i) loop[i]->next = loop[(i + 1) % 4];
            quad->halfEdge = loop[0];
            newFaces.push_back(std::move(quad));
            edge = edge->next;
        } while (edge != start);
    }
    for (const auto& [from, outgoing] : directedEdges) {
        for (const auto& [to, edge] : outgoing) {
            auto reverseFrom = directedEdges.find(to);
            if (reverseFrom == directedEdges.end()) return false;
            auto reverse = reverseFrom->second.find(from);
            if (reverse == reverseFrom->second.end()) return false;
            edge->sym = reverse->second;
        }
    }
    vertices.swap(newVertices);
    faces.swap(newFaces);
    halfEdges.swap(newEdges);
    return true;
}

Vertex* Mesh::splitHalfEdge(HalfEdge* edge) {
    if (!edge || !edge->sym || edge->sym->sym != edge ||
        !edge->next || !edge->sym->next ||
        !edge->vertex || !edge->sym->vertex) return nullptr;

    HalfEdge* originalSym = edge->sym;
    Vertex* originalEndpoint = edge->vertex;
    Vertex* originalStartpoint = originalSym->vertex;
    const glm::vec3 midpoint =
        (originalEndpoint->position + originalStartpoint->position) * 0.5f;

    auto midpointOwner = std::make_unique<Vertex>(midpoint);
    auto newEdgeOwner = std::make_unique<HalfEdge>();
    auto newSymEdgeOwner = std::make_unique<HalfEdge>();
    Vertex* midpointVertex = midpointOwner.get();
    HalfEdge* newEdge = newEdgeOwner.get();
    HalfEdge* newSymEdge = newSymEdgeOwner.get();

    vertices.reserve(vertices.size() + 1);
    halfEdges.reserve(halfEdges.size() + 2);

    newEdge->next = edge->next;
    newEdge->vertex = originalEndpoint;
    newEdge->face = edge->face;
    newSymEdge->next = originalSym->next;
    newSymEdge->vertex = originalStartpoint;
    newSymEdge->face = originalSym->face;

    edge->next = newEdge;
    originalSym->next = newSymEdge;
    edge->vertex = midpointVertex;
    originalSym->vertex = midpointVertex;

    edge->sym = newSymEdge;
    newSymEdge->sym = edge;
    originalSym->sym = newEdge;
    newEdge->sym = originalSym;

    midpointVertex->halfEdge = edge;
    if (originalEndpoint->halfEdge == edge) originalEndpoint->halfEdge = newEdge;
    if (originalStartpoint->halfEdge == originalSym) originalStartpoint->halfEdge = newSymEdge;

    vertices.push_back(std::move(midpointOwner));
    halfEdges.push_back(std::move(newEdgeOwner));
    halfEdges.push_back(std::move(newSymEdgeOwner));
    return midpointVertex;
}

bool Mesh::triangulateFace(Face* face) {
    if (!face || !face->halfEdge) return false;

    std::unordered_set<HalfEdge*> boundary;
    HalfEdge* edge = face->halfEdge;
    do {
        if (!edge || !edge->vertex || edge->face != face ||
            !boundary.insert(edge).second) return false;
        edge = edge->next;
    } while (edge != face->halfEdge);
    if (boundary.size() <= 3) return false;

    const size_t cuts = boundary.size() - 3;
    std::vector<std::unique_ptr<Face>> newFaces;
    std::vector<std::unique_ptr<HalfEdge>> diagonals;
    newFaces.reserve(cuts);
    diagonals.reserve(2 * cuts);
    for (size_t i = 0; i < cuts; ++i) {
        newFaces.push_back(std::make_unique<Face>(face->color));
        diagonals.push_back(std::make_unique<HalfEdge>());
        diagonals.push_back(std::make_unique<HalfEdge>());
    }
    faces.reserve(faces.size() + cuts);
    halfEdges.reserve(halfEdges.size() + 2 * cuts);

    for (size_t i = 0; i < cuts; ++i) {
        HalfEdge* first = edge->next;
        HalfEdge* second = first->next;
        HalfEdge* after = second->next;
        HalfEdge* closing = diagonals[2 * i].get();
        HalfEdge* remaining = diagonals[2 * i + 1].get();
        Face* triangle = newFaces[i].get();

        closing->vertex = edge->vertex;
        remaining->vertex = second->vertex;
        closing->sym = remaining;
        remaining->sym = closing;

        second->next = closing;
        closing->next = first;
        first->face = second->face = closing->face = triangle;
        triangle->halfEdge = first;

        edge->next = remaining;
        remaining->next = after;
        remaining->face = face;

        faces.push_back(std::move(newFaces[i]));
        halfEdges.push_back(std::move(diagonals[2 * i]));
        halfEdges.push_back(std::move(diagonals[2 * i + 1]));
    }
    return true;
}

namespace {
std::vector<GLuint> triangulate(const std::vector<glm::vec3>& points, glm::vec3& normal) {
    if (points.size() < 3) {
        throw std::runtime_error("A face must have at least three vertices.");
    }
    glm::dvec3 sum(0.0);
    const glm::dvec3 origin(points[0]);
    for (size_t i = 0; i < points.size(); ++i) {
        sum += glm::cross(glm::dvec3(points[i]) - origin,
                          glm::dvec3(points[(i + 1) % points.size()]) - origin);
    }
    if (glm::length(sum) == 0.0) {
        throw std::runtime_error("A face has zero area.");
    }
    normal = glm::vec3(glm::normalize(sum));
    int axis = 0;
    if (std::abs(sum.y) > std::abs(sum[axis])) axis = 1;
    if (std::abs(sum.z) > std::abs(sum[axis])) axis = 2;
    std::vector<glm::dvec2> projected;
    for (const auto& p : points) {
        projected.emplace_back(double(p[(axis + 1) % 3]) - origin[(axis + 1) % 3],
                               double(p[(axis + 2) % 3]) - origin[(axis + 2) % 3]);
    }
    const double winding = sum[axis] > 0.0 ? 1.0 : -1.0;
    auto turn = [&](GLuint a, GLuint b, GLuint c) {
        auto u = projected[b] - projected[a];
        auto v = projected[c] - projected[a];
        return winding * (u.x * v.y - u.y * v.x);
    };
    std::vector<GLuint> remaining(points.size());
    std::iota(remaining.begin(), remaining.end(), 0u);
    std::vector<GLuint> result;
    while (remaining.size() > 3) {
        bool clipped = false;
        for (size_t i = 0; i < remaining.size(); ++i) {
            GLuint a = remaining[(i + remaining.size() - 1) % remaining.size()];
            GLuint b = remaining[i];
            GLuint c = remaining[(i + 1) % remaining.size()];
            if (turn(a, b, c) <= 0.0) continue;
            bool contains = false;
            for (GLuint p : remaining) {
                if (p == a || p == b || p == c) continue;
                if (turn(a, b, p) >= 0.0 && turn(b, c, p) >= 0.0 && turn(c, a, p) >= 0.0) {
                    contains = true;
                    break;
                }
            }
            if (contains) continue;
            result.insert(result.end(), {a, b, c});
            remaining.erase(remaining.begin() + i);
            clipped = true;
            break;
        }
        if (!clipped) {
            throw std::runtime_error("Could not triangulate a face.");
        }
    }
    if (turn(remaining[0], remaining[1], remaining[2]) <= 0.0) {
        throw std::runtime_error("A face with a degenerate triangle.");
    }
    result.insert(result.end(), remaining.begin(), remaining.end());
    return result;
}
}

Mesh::Mesh(OpenGLContext* context)
    : Drawable(context), faces(), halfEdges(), vertices()
{}

Mesh::~Mesh() = default;

bool Mesh::loadOBJ(const QString& path, QString& error) {
    error.clear();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error = file.errorString();
        return false;
    }
    std::vector<std::unique_ptr<Vertex>> newVertices;
    std::vector<std::unique_ptr<Face>> newFaces;
    std::vector<std::unique_ptr<HalfEdge>> newEdges;
    std::unordered_map<std::uint64_t, HalfEdge*> edges;
    auto key = [](size_t a, size_t b) {
        return (std::uint64_t(a) << 32) | std::uint64_t(b);
    };
    QTextStream input(&file);
    int lineNumber = 0;
    try {
        while (!input.atEnd()) {
            QString line = input.readLine();
            ++lineNumber;
            std::istringstream stream(line.section('#', 0, 0).toStdString());
            std::string type;
            stream >> type;
            if (type == "v") {
                float x, y, z;
                if (!(stream >> x >> y >> z) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z)) {
                    throw std::runtime_error("Invalid vertex position.");
                }
                newVertices.push_back(std::make_unique<Vertex>(glm::vec3(x, y, z)));
            } else if (type == "f") {
                std::vector<size_t> boundary;
                std::unordered_set<size_t> used;
                std::vector<glm::vec3> points;
                std::string token;
                while (stream >> token) {
                    std::string position = token.substr(0, token.find('/'));
                    size_t consumed = 0;
                    long long index = std::stoll(position, &consumed);
                    if (consumed != position.size() || index == 0) {
                        throw std::runtime_error("Invalid face index.");
                    }
                    index = index > 0 ? index - 1 : static_cast<long long>(newVertices.size()) + index;
                    if (index < 0 || static_cast<size_t>(index) >= newVertices.size()) {
                        throw std::runtime_error("Face index is out of range.");
                    }
                    if (!used.insert(static_cast<size_t>(index)).second) {
                        throw std::runtime_error("A face repeats a vertex.");
                    }
                    boundary.push_back(static_cast<size_t>(index));
                    points.push_back(newVertices[index]->position);
                }
                glm::vec3 normal;
                triangulate(points, normal);
                auto random = QRandomGenerator::global();
                auto face = std::make_unique<Face>(glm::vec3(random->generateDouble(), random->generateDouble(), random->generateDouble()));
                size_t first = newEdges.size();
                for (size_t i = 0; i < boundary.size(); ++i) {
                    size_t a = boundary[i];
                    size_t b = boundary[(i + 1) % boundary.size()];
                    auto edge = std::make_unique<HalfEdge>();
                    edge->face = face.get();
                    edge->vertex = newVertices[b].get();
                    if (!edges.emplace(key(a, b), edge.get()).second) {
                        throw std::runtime_error("Duplicate directed edge: inconsistent winding or non-manifold mesh.");
                    }
                    auto opposite = edges.find(key(b, a));
                    if (opposite != edges.end()) {
                        edge->sym = opposite->second;
                        opposite->second->sym = edge.get();
                    }
                    edge->vertex->halfEdge = edge.get();
                    newEdges.push_back(std::move(edge));
                }
                for (size_t i = 0; i < boundary.size(); ++i) {
                    newEdges[first + i]->next = newEdges[first + (i + 1) % boundary.size()].get();
                }
                face->halfEdge = newEdges[first].get();
                newFaces.push_back(std::move(face));
            }
        }
        if (newFaces.empty()) throw std::runtime_error("The OBJ contains no faces.");
        for (const auto& edge : newEdges) {
            if (!edge->sym) throw std::runtime_error("The mesh is not closed: an edge has no symmetrical partner.");
        }
    } catch (const std::exception& exception) {
        error = QString("Line %1: %2").arg(lineNumber).arg(QString::fromUtf8(exception.what()));
        return false;
    }
    vertices.swap(newVertices);
    faces.swap(newFaces);
    halfEdges.swap(newEdges);
    return true;
}

void Mesh::initializeAndBufferGeometryData() {
    std::vector<glm::vec3> positions, normals, colors;
    std::vector<GLuint> indices;
    for (const auto& face : faces) {
        HalfEdge* start = face->halfEdge;
        if (!start) continue;
        std::vector<glm::vec3> points;
        HalfEdge* current = start;
        do {
            points.push_back(current->vertex->position);
            current = current->next;
        } while (current != start);
        glm::vec3 normal;
        auto triangles = triangulate(points, normal);
        GLuint base = static_cast<GLuint>(positions.size());
        positions.insert(positions.end(), points.begin(), points.end());
        normals.insert(normals.end(), points.size(), normal);
        colors.insert(colors.end(), points.size(), face->color);
        for (GLuint index : triangles) indices.push_back(base + index);
    }
    indexBufferLength = static_cast<int>(indices.size());
    for (BufferType type : {POSITION, NORMAL, COLOR, INDEX}) {
        if (!hasBuffer(type)) generateBuffer(type);
        bindBuffer(type);
        switch (type) {
        case POSITION: bufferData(type, positions); break;
        case NORMAL: bufferData(type, normals); break;
        case COLOR: bufferData(type, colors); break;
        case INDEX: bufferData(type, indices); break;
        }
    }
}

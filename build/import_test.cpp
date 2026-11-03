#include "mesh.h"
#include <QTemporaryDir>
#include <QFile>
#include <iostream>
#include <stdexcept>

void check(bool condition) {
    if (!condition) throw std::runtime_error("Test failed");
}
int main() {
    QTemporaryDir dir;
    QString path = dir.filePath("test.obj");
    auto write = [&](const QByteArray& data) {
        QFile file(path);
        check(file.open(QIODevice::WriteOnly));
        file.write(data);
    };
    QString error;
    Mesh mesh(nullptr);
    QByteArray cube = "v -1 -1 -1\nv 1 -1 -1\nv 1 1 -1\nv -1 1 -1\nv -1 -1 1\nv 1 -1 1\nv 1 1 1\nv -1 1 1\nf 1/1/1 4/1/1 3/1/1 2/1/1\nf 5 6 7 8\nf 1 2 6 5\nf 4 8 7 3\nf 1 5 8 4\nf -7 -6 -2 -3\n";
    write(cube);
    check(mesh.loadOBJ(path, error));
    check(mesh.vertices.size() == 8 && mesh.faces.size() == 6 && mesh.halfEdges.size() == 24);
    for (const auto& edge : mesh.halfEdges) {
        check(edge->sym && edge->sym->sym == edge.get());
        check(edge->next && edge->face && edge->vertex);
        check(edge->next->next->next->next == edge.get());
        check(edge->vertex == edge->sym->next->next->next->vertex);
    }
    for (const auto& vertex : mesh.vertices) check(vertex->halfEdge->vertex == vertex.get());
    auto original = mesh.faces.front().get();
    write("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
    check(!mesh.loadOBJ(path, error) && mesh.faces.front().get() == original);
    write("v 0 0 0\nf 1 2 3\n");
    check(!mesh.loadOBJ(path, error) && mesh.faces.front().get() == original);
    write(cube);
    check(mesh.loadOBJ(path, error) && mesh.faces.size() == 6);
    QByteArray prism;
    const float xy[6][2] = {{0,0},{2,0},{2,1},{1,1},{1,2},{0,2}};
    for (int z = 0; z < 2; ++z) for (auto& p : xy)
        prism += "v " + QByteArray::number(p[0]) + " " + QByteArray::number(p[1]) + " " + QByteArray::number(z) + "\n";
    prism += "f 6 5 4 3 2 1\nf 7 8 9 10 11 12\n";
    for (int i = 0; i < 6; ++i) {
        int j = (i + 1) % 6;
        prism += "f " + QByteArray::number(i+1) + " " + QByteArray::number(j+1) + " " + QByteArray::number(j+7) + " " + QByteArray::number(i+7) + "\n";
    }
    write(prism);
    check(mesh.loadOBJ(path, error));
    check(mesh.faces.size() == 8 && mesh.halfEdges.size() == 36);
    std::cout << "PASS: cube topology, SYM pairs, negative/slash indices, replacement, rejected invalid files, concave n-gons\n";
}

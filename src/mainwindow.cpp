#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QShortcut>
#include <QKeyEvent>
#include <array>
#include <limits>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->subdivideButton, &QPushButton::clicked, ui->mygl, &MyGL::subdivideMesh);
    connect(ui->mygl, &MyGL::meshChanged, this, [this]() {
        ui->subdivideButton->setEnabled(!ui->mygl->mesh().faces.empty());
    });
    connect(ui->splitEdgeButton, &QPushButton::clicked,
            ui->mygl, &MyGL::splitSelectedHalfEdge);
    connect(ui->triangulateFaceButton, &QPushButton::clicked,
            ui->mygl, &MyGL::triangulateSelectedFace);
    connect(ui->mygl, &MyGL::halfEdgeSelected, this, [this](HalfEdge* edge) {
        ui->splitEdgeButton->setEnabled(edge && edge->sym);
    });
    connect(ui->mygl, &MyGL::faceSelected, this, [this](Face* face) {
        ui->triangulateFaceButton->setEnabled(face && face->halfEdge &&
            face->halfEdge->next->next->next != face->halfEdge);
    });
    const std::array<QDoubleSpinBox*, 3> positionBoxes{
        ui->vertPosXSpinBox, ui->vertPosYSpinBox, ui->vertPosZSpinBox};
    const std::array<QDoubleSpinBox*, 3> colorBoxes{
        ui->faceRedSpinBox, ui->faceGreenSpinBox, ui->faceBlueSpinBox};
    for (int axis = 0; axis < 3; ++axis) {
        auto positionBox = positionBoxes[axis];
        positionBox->setDecimals(4);
        positionBox->setRange(-std::numeric_limits<float>::max(),
                             std::numeric_limits<float>::max());
        positionBox->setSingleStep(0.1);
        positionBox->setKeyboardTracking(false);
        positionBox->setEnabled(false);
        connect(positionBox, &QDoubleSpinBox::valueChanged, this,
                [this, axis](double value) { ui->mygl->setVertexPosition(axis, value); });
        auto colorBox = colorBoxes[axis];
        colorBox->setRange(0.0, 1.0);
        colorBox->setKeyboardTracking(false);
        colorBox->setEnabled(false);
        connect(colorBox, &QDoubleSpinBox::valueChanged, this,
                [this, axis](double value) { ui->mygl->setFaceColor(axis, value); });
    }
    connect(ui->mygl, &MyGL::vertexSelected, this, [positionBoxes](Vertex* vertex) {
        for (int axis = 0; axis < 3; ++axis) {
            QSignalBlocker blocker(positionBoxes[axis]);
            positionBoxes[axis]->setEnabled(vertex != nullptr);
            positionBoxes[axis]->setValue(vertex ? vertex->position[axis] : 0.0);
        }
    });
    connect(ui->mygl, &MyGL::faceSelected, this, [colorBoxes](Face* face) {
        for (int channel = 0; channel < 3; ++channel) {
            QSignalBlocker blocker(colorBoxes[channel]);
            colorBoxes[channel]->setEnabled(face != nullptr);
            colorBoxes[channel]->setValue(face ? face->color[channel] : 0.0);
        }
    });
    connect(ui->mygl, &MyGL::attributeEditFailed, this, [this](const QString& error) {
        QMessageBox::warning(this, "Cannot Apply Edit", error);
    });
    for (QListWidget* list : {ui->vertsListWidget, ui->halfEdgesListWidget, ui->facesListWidget}) {
        list->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    connect(ui->vertsListWidget, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem* item, QListWidgetItem*) {
        ui->mygl->selectVertex(dynamic_cast<Vertex*>(item));
    });
    connect(ui->halfEdgesListWidget, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem* item, QListWidgetItem*) {
        ui->mygl->selectHalfEdge(dynamic_cast<HalfEdge*>(item));
    });
    connect(ui->facesListWidget, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem* item, QListWidgetItem*) {
        ui->mygl->selectFace(dynamic_cast<Face*>(item));
    });
    auto synchronize = [](QListWidget* list, QListWidgetItem* item) {
        QSignalBlocker blocker(list);
        list->setCurrentItem(item);
        if (item) list->scrollToItem(item);
        else list->clearSelection();
    };
    connect(ui->mygl, &MyGL::vertexSelected, this, [this, synchronize](Vertex* vertex) {
        synchronize(ui->vertsListWidget, vertex);
    });
    connect(ui->mygl, &MyGL::halfEdgeSelected, this, [this, synchronize](HalfEdge* edge) {
        synchronize(ui->halfEdgesListWidget, edge);
    });
    connect(ui->mygl, &MyGL::faceSelected, this, [this, synchronize](Face* face) {
        synchronize(ui->facesListWidget, face);
    });
    for (int key : {Qt::Key_N, Qt::Key_M, Qt::Key_F, Qt::Key_V, Qt::Key_H}) {
        auto shortcut = new QShortcut(QKeySequence(key), this);
        connect(shortcut, &QShortcut::activated, this, [this, key]() {
            QKeyEvent event(QEvent::KeyPress, key, Qt::NoModifier);
            QApplication::sendEvent(ui->mygl, &event);
        });
    }
    auto faceEdgeShortcut = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_H), this);
    connect(faceEdgeShortcut, &QShortcut::activated, this, [this]() {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_H, Qt::ShiftModifier);
        QApplication::sendEvent(ui->mygl, &event);
    });
    connect(ui->mygl, &MyGL::meshAboutToChange, this, &MainWindow::detachMeshItems);
    connect(ui->mygl, &MyGL::meshChanged, this, &MainWindow::populateMeshItems);
    ui->mygl->setFocus();
}

MainWindow::~MainWindow()
{
    detachMeshItems();
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}
void MainWindow::on_importOBJButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(
        this,
        "Import OBJ",
        QString(),
        "OBJ Files (*.obj)"
        );

    if (path.isEmpty()) {
        return;
    }

    QString error;
    if (!ui->mygl->loadOBJ(path, error)) {
        QMessageBox::warning(this, "OBJ Import Failed", error);
    }
    ui->mygl->setFocus();
}

void MainWindow::detachMeshItems() {
    for (QListWidget* list : {ui->vertsListWidget, ui->halfEdgesListWidget, ui->facesListWidget}) {
        QSignalBlocker blocker(list);
        while (list->count() > 0) list->takeItem(list->count() - 1);
    }
}

void MainWindow::populateMeshItems() {
    const Mesh& mesh = ui->mygl->mesh();
    for (const auto& vertex : mesh.vertices) ui->vertsListWidget->addItem(vertex.get());
    for (const auto& edge : mesh.halfEdges) ui->halfEdgesListWidget->addItem(edge.get());
    for (const auto& face : mesh.faces) ui->facesListWidget->addItem(face.get());
}


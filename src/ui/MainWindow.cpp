#include "MainWindow.h"
#include "../persistence/SvgExporter.h"
#include "../persistence/ProjectSerializer.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QStyle>

namespace VectorForge {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    applyDarkTheme();
    setWindowTitle("VectorForge — 2D Vector Design & Generative Art Studio");
    resize(1440, 900);
}

void MainWindow::setupUi() {
    m_canvasView = new CanvasView(&m_document, &m_commandStack, this);
    setCentralWidget(m_canvasView);

    setupMenus();
    setupToolbars();
    setupDocks();

    connect(&m_commandStack, &CommandStack::stackChanged, this, [this]() {
        m_actionUndo->setEnabled(m_commandStack.canUndo());
        m_actionRedo->setEnabled(m_commandStack.canRedo());
        if (m_commandStack.canUndo()) {
            m_actionUndo->setText(QString("Undo %1").arg(QString::fromStdString(m_commandStack.undoDescription())));
        } else {
            m_actionUndo->setText("Undo");
        }
    });
}

void MainWindow::setupMenus() {
    QMenuBar* mb = menuBar();

    // File Menu
    QMenu* fileMenu = mb->addMenu("&File");
    fileMenu->addAction("&New Project", this, &MainWindow::newProject, QKeySequence::New);
    fileMenu->addAction("&Open Project...", this, &MainWindow::openProject, QKeySequence::Open);
    fileMenu->addAction("&Save Project...", this, &MainWindow::saveProject, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction("Export to &SVG...", this, &MainWindow::exportSvg, QKeySequence(Qt::CTRL | Qt::Key_E));
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", qApp, &QApplication::quit, QKeySequence::Quit);

    // Edit Menu
    QMenu* editMenu = mb->addMenu("&Edit");
    m_actionUndo = editMenu->addAction("&Undo", [this]() { m_commandStack.undo(); }, QKeySequence::Undo);
    m_actionRedo = editMenu->addAction("&Redo", [this]() { m_commandStack.redo(); }, QKeySequence::Redo);
    m_actionUndo->setEnabled(false);
    m_actionRedo->setEnabled(false);

    editMenu->addSeparator();
    editMenu->addAction("&Group Shapes", this, &MainWindow::groupSelected, QKeySequence(Qt::CTRL | Qt::Key_G));
    editMenu->addAction("&Ungroup Shapes", this, &MainWindow::ungroupSelected, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_G));

    // View Menu
    QMenu* viewMenu = mb->addMenu("&View");
    viewMenu->addAction("Reset Zoom", m_canvasView, &CanvasView::resetZoom);
    viewMenu->addSeparator();

    auto snapAct = viewMenu->addAction("Snap to Grid", [this](bool checked) {
        m_canvasView->setSnapToGrid(checked);
    });
    snapAct->setCheckable(true);
    snapAct->setChecked(true);

    auto debugAct = viewMenu->addAction("Show Quadtree Spatial Bounds", [this](bool checked) {
        m_canvasView->setShowQuadtreeDebug(checked);
    });
    debugAct->setCheckable(true);
    debugAct->setChecked(false);
}

void MainWindow::setupToolbars() {
    auto toolBar = addToolBar("Tools");
    toolBar->setMovable(false);

    auto group = new QActionGroup(this);
    group->setExclusive(true);

    auto addToolAction = [&](const QString& text, ToolType tool, bool checked = false) {
        auto act = toolBar->addAction(text, [this, tool]() {
            m_canvasView->setCurrentTool(tool);
        });
        act->setCheckable(true);
        act->setChecked(checked);
        group->addAction(act);
    };

    addToolAction("🖐️ Select", ToolType::Select, true);
    toolBar->addSeparator();
    addToolAction("🟦 Rectangle", ToolType::Rectangle);
    addToolAction("⚪ Circle", ToolType::Circle);
    addToolAction("⬢ Polygon", ToolType::Polygon);
    addToolAction("⭐ Star", ToolType::Star);
    addToolAction("📏 Line", ToolType::Line);
    addToolAction("〰️ Bezier", ToolType::Bezier);
}

void MainWindow::setupDocks() {
    // Right Inspector Dock
    auto dockInspector = new QDockWidget("Property Inspector", this);
    m_propertyInspector = new PropertyInspector(&m_document, &m_commandStack, dockInspector);
    dockInspector->setWidget(m_propertyInspector);
    addDockWidget(Qt::RightDockWidgetArea, dockInspector);

    // Right Generative Dock
    auto dockGenerative = new QDockWidget("Generative Art Studio", this);
    m_generativeDock = new GenerativeDock(&m_document, &m_commandStack, dockGenerative);
    dockGenerative->setWidget(m_generativeDock);
    addDockWidget(Qt::RightDockWidgetArea, dockGenerative);

    // Right Layer Dock
    auto dockLayer = new QDockWidget("Layer Manager", this);
    m_layerPanel = new LayerPanel(&m_document, &m_commandStack, dockLayer);
    dockLayer->setWidget(m_layerPanel);
    addDockWidget(Qt::RightDockWidgetArea, dockLayer);
}

void MainWindow::newProject() {
    m_document.fromJson(QJsonObject());
    m_commandStack.clear();
    m_canvasView->update();
}

void MainWindow::openProject() {
    QString path = QFileDialog::getOpenFileName(this, "Open VectorForge Project", "", "VectorForge Project (*.vfg *.json)");
    if (!path.isEmpty()) {
        if (ProjectSerializer::loadFromFile(m_document, path.toStdString())) {
            m_commandStack.clear();
            m_canvasView->update();
        } else {
            QMessageBox::critical(this, "Error", "Failed to load VectorForge project file.");
        }
    }
}

void MainWindow::saveProject() {
    QString path = QFileDialog::getSaveFileName(this, "Save VectorForge Project", "artwork.vfg", "VectorForge Project (*.vfg)");
    if (!path.isEmpty()) {
        if (!ProjectSerializer::saveToFile(m_document, path.toStdString())) {
            QMessageBox::critical(this, "Error", "Failed to save project file.");
        }
    }
}

void MainWindow::exportSvg() {
    QString path = QFileDialog::getSaveFileName(this, "Export SVG Vector", "artwork.svg", "Scalable Vector Graphics (*.svg)");
    if (!path.isEmpty()) {
        if (SvgExporter::exportToSvg(m_document, path)) {
            QMessageBox::information(this, "Export Success", "Exported vector artwork to SVG successfully!");
        } else {
            QMessageBox::critical(this, "Error", "Failed to export SVG file.");
        }
    }
}

void MainWindow::groupSelected() {
    if (!m_document.selection().empty()) {
        auto cmd = std::make_unique<GroupShapesCommand>(&m_document, m_document.selection());
        m_commandStack.pushAndExecute(std::move(cmd));
        m_canvasView->update();
    }
}

void MainWindow::ungroupSelected() {
    if (!m_document.selection().empty()) {
        auto group = std::dynamic_pointer_cast<GroupShape>(m_document.selection().front());
        if (group) {
            auto cmd = std::make_unique<UngroupShapesCommand>(&m_document, group);
            m_commandStack.pushAndExecute(std::move(cmd));
            m_canvasView->update();
        }
    }
}

void MainWindow::applyDarkTheme() {
    qApp->setStyle("Fusion");
    QString darkStyle = R"(
        QMainWindow { background-color: #16181E; }
        QWidget { background-color: #1A1D24; color: #E0E4ED; font-family: 'Segoe UI', Arial, sans-serif; font-size: 13px; }
        QDockWidget { titlebar-close-icon: url(); titlebar-normal-icon: url(); font-weight: bold; }
        QDockWidget::title { background: #222630; text-align: left; padding-left: 8px; padding-top: 4px; border-bottom: 1px solid #303644; color: #00E6FF; }
        QToolBar { background: #20232B; border-bottom: 1px solid #2D323E; spacing: 4px; padding: 4px; }
        QToolButton { background: transparent; color: #D0D4DF; border: 1px solid transparent; border-radius: 4px; padding: 6px 10px; font-weight: bold; }
        QToolButton:hover { background: #2D3342; border: 1px solid #00E6FF; }
        QToolButton:checked { background: #00E6FF; color: #101216; border: 1px solid #00E6FF; }
        QMenuBar { background-color: #1A1D24; color: #E0E4ED; border-bottom: 1px solid #2D323E; }
        QMenuBar::item:selected { background: #2D3342; color: #00E6FF; }
        QMenu { background-color: #20232B; color: #E0E4ED; border: 1px solid #303644; }
        QMenu::item:selected { background-color: #00E6FF; color: #101216; }
        QPushButton { background-color: #2A2F3D; border: 1px solid #3A4254; color: #E0E4ED; padding: 5px 12px; border-radius: 4px; }
        QPushButton:hover { background-color: #363D4F; border-color: #00E6FF; }
        QGroupBox { border: 1px solid #303644; border-radius: 6px; margin-top: 12px; font-weight: bold; color: #00E6FF; }
        QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }
        QSpinBox, QDoubleSpinBox, QComboBox { background-color: #242834; border: 1px solid #363D4E; color: #E0E4ED; padding: 4px; border-radius: 4px; }
        QTreeWidget { background-color: #1E212A; border: 1px solid #303644; color: #E0E4ED; }
        QHeaderView::section { background-color: #262B36; color: #00E6FF; border: none; padding: 4px; }
    )";
    qApp->setStyleSheet(darkStyle);
}

} // namespace VectorForge

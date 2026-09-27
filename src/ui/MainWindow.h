#ifndef VECTORFORGE_MAINWINDOW_H
#define VECTORFORGE_MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QActionGroup>
#include <QDockWidget>
#include "../model/Document.h"
#include "../command/CommandStack.h"
#include "CanvasView.h"
#include "PropertyInspector.h"
#include "GenerativeDock.h"
#include "LayerPanel.h"

namespace VectorForge {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void newProject();
    void openProject();
    void saveProject();
    void exportSvg();
    void groupSelected();
    void ungroupSelected();

private:
    void setupUi();
    void setupMenus();
    void setupToolbars();
    void setupDocks();
    void applyDarkTheme();

    Document m_document;
    CommandStack m_commandStack;

    CanvasView* m_canvasView;
    PropertyInspector* m_propertyInspector;
    GenerativeDock* m_generativeDock;
    LayerPanel* m_layerPanel;

    QAction* m_actionUndo;
    QAction* m_actionRedo;
};

} // namespace VectorForge

#endif // VECTORFORGE_MAINWINDOW_H

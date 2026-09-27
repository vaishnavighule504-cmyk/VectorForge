#ifndef VECTORFORGE_LAYERPANEL_H
#define VECTORFORGE_LAYERPANEL_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include "../model/Document.h"
#include "../command/CommandStack.h"
#include "../command/LayerCommands.h"

namespace VectorForge {

class LayerPanel : public QWidget {
    Q_OBJECT
public:
    explicit LayerPanel(Document* doc, CommandStack* cmdStack, QWidget* parent = nullptr);
    ~LayerPanel() override = default;

public slots:
    void refreshLayers();

private:
    void setupUi();
    void populateTreeItem(QTreeWidgetItem* parentItem, const std::shared_ptr<Layer>& layer);

    Document* m_doc;
    CommandStack* m_cmdStack;

    QTreeWidget* m_treeWidget;
    QPushButton* m_btnAddLayer;
    QPushButton* m_btnRemoveLayer;
    QPushButton* m_btnMoveUp;
    QPushButton* m_btnMoveDown;

    bool m_isRefreshing{false};
};

} // namespace VectorForge

#endif // VECTORFORGE_LAYERPANEL_H

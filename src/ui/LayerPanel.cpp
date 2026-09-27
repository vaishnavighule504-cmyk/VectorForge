#include "LayerPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVariant>

namespace VectorForge {

LayerPanel::LayerPanel(Document* doc, CommandStack* cmdStack, QWidget* parent)
    : QWidget(parent), m_doc(doc), m_cmdStack(cmdStack) {
    setupUi();
    if (m_doc) {
        connect(m_doc, &Document::layerStructureChanged, this, &LayerPanel::refreshLayers);
        connect(m_doc, &Document::documentChanged, this, &LayerPanel::refreshLayers);
    }
    refreshLayers();
}

void LayerPanel::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabels({"Layer Name", "Vis", "Lock"});
    m_treeWidget->setColumnWidth(0, 150);
    m_treeWidget->setColumnWidth(1, 40);
    m_treeWidget->setColumnWidth(2, 40);
    m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeWidget->setAnimated(true);

    mainLayout->addWidget(m_treeWidget);

    auto btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);

    m_btnAddLayer = new QPushButton("+ Layer", this);
    m_btnRemoveLayer = new QPushButton("- Layer", this);
    m_btnMoveUp = new QPushButton("⬆", this);
    m_btnMoveDown = new QPushButton("⬇", this);

    btnLayout->addWidget(m_btnAddLayer);
    btnLayout->addWidget(m_btnRemoveLayer);
    btnLayout->addWidget(m_btnMoveUp);
    btnLayout->addWidget(m_btnMoveDown);

    mainLayout->addLayout(btnLayout);

    connect(m_btnAddLayer, &QPushButton::clicked, this, [this]() {
        if (m_doc && m_cmdStack) {
            m_cmdStack->pushAndExecute(std::make_unique<AddLayerCommand>(m_doc, ""));
        }
    });

    connect(m_btnRemoveLayer, &QPushButton::clicked, this, [this]() {
        if (m_doc && m_cmdStack) {
            int currentIdx = static_cast<int>(m_doc->activeLayerIndex());
            m_cmdStack->pushAndExecute(std::make_unique<RemoveLayerCommand>(m_doc, currentIdx));
        }
    });

    connect(m_btnMoveUp, &QPushButton::clicked, this, [this]() {
        if (m_doc && m_cmdStack) {
            size_t idx = m_doc->activeLayerIndex();
            if (idx > 0) {
                m_cmdStack->pushAndExecute(std::make_unique<MoveLayerCommand>(m_doc, idx, MoveLayerCommand::Up));
            }
        }
    });

    connect(m_btnMoveDown, &QPushButton::clicked, this, [this]() {
        if (m_doc && m_cmdStack) {
            size_t idx = m_doc->activeLayerIndex();
            if (idx + 1 < m_doc->rootLayers().size()) {
                m_cmdStack->pushAndExecute(std::make_unique<MoveLayerCommand>(m_doc, idx, MoveLayerCommand::Down));
            }
        }
    });

    connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this, [this]() {
        if (m_isRefreshing) return;
        auto selected = m_treeWidget->selectedItems();
        if (!selected.isEmpty() && m_doc) {
            QVariant ptrVar = selected.first()->data(0, Qt::UserRole);
            Layer* targetLayer = reinterpret_cast<Layer*>(ptrVar.value<uintptr_t>());
            if (targetLayer) {
                const auto& layers = m_doc->rootLayers();
                for (size_t i = 0; i < layers.size(); ++i) {
                    if (layers[i].get() == targetLayer) {
                        m_doc->setActiveLayerIndex(i);
                        break;
                    }
                }
            }
        }
    });

    connect(m_treeWidget, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item, int column) {
        if (m_isRefreshing || !m_doc || !m_cmdStack) return;

        QVariant ptrVar = item->data(0, Qt::UserRole);
        Layer* targetLayerPtr = reinterpret_cast<Layer*>(ptrVar.value<uintptr_t>());
        if (!targetLayerPtr) return;

        // Find matching shared_ptr<Layer>
        std::shared_ptr<Layer> targetLayer = nullptr;
        for (const auto& l : m_doc->rootLayers()) {
            if (l.get() == targetLayerPtr) {
                targetLayer = l;
                break;
            }
        }

        if (targetLayer) {
            if (column == 1) { // Vis
                bool isVis = (item->checkState(1) == Qt::Checked);
                if (isVis != targetLayer->isVisible()) {
                    m_cmdStack->pushAndExecute(std::make_unique<ToggleLayerVisibilityCommand>(m_doc, targetLayer, isVis));
                }
            } else if (column == 2) { // Lock
                bool isLocked = (item->checkState(2) == Qt::Checked);
                if (isLocked != targetLayer->isLocked()) {
                    m_cmdStack->pushAndExecute(std::make_unique<ToggleLayerLockCommand>(m_doc, targetLayer, isLocked));
                }
            }
        }
    });
}

void LayerPanel::populateTreeItem(QTreeWidgetItem* parentItem, const std::shared_ptr<Layer>& layer) {
    if (!layer) return;

    QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
    item->setText(0, QString::fromStdString(layer->name()));
    item->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<uintptr_t>(layer.get())));

    item->setCheckState(1, layer->isVisible() ? Qt::Checked : Qt::Unchecked);
    item->setText(1, layer->isVisible() ? "👁" : "🙈");

    item->setCheckState(2, layer->isLocked() ? Qt::Checked : Qt::Unchecked);
    item->setText(2, layer->isLocked() ? "🔒" : "🔓");

    for (const auto& child : layer->children()) {
        populateTreeItem(item, child);
    }
}

void LayerPanel::refreshLayers() {
    if (!m_doc) return;

    m_isRefreshing = true;
    m_treeWidget->clear();

    const auto& layers = m_doc->rootLayers();
    size_t activeIdx = m_doc->activeLayerIndex();

    for (size_t i = 0; i < layers.size(); ++i) {
        const auto& layer = layers[i];
        if (layer) {
            QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_treeWidget);
            rootItem->setText(0, QString::fromStdString(layer->name()));
            rootItem->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<uintptr_t>(layer.get())));

            rootItem->setCheckState(1, layer->isVisible() ? Qt::Checked : Qt::Unchecked);
            rootItem->setText(1, layer->isVisible() ? "👁" : "🙈");

            rootItem->setCheckState(2, layer->isLocked() ? Qt::Checked : Qt::Unchecked);
            rootItem->setText(2, layer->isLocked() ? "🔒" : "🔓");

            for (const auto& child : layer->children()) {
                populateTreeItem(rootItem, child);
            }

            if (i == activeIdx) {
                m_treeWidget->setCurrentItem(rootItem);
            }
        }
    }

    m_treeWidget->expandAll();
    m_isRefreshing = false;
}

} // namespace VectorForge

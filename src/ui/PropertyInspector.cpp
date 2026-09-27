#include "PropertyInspector.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QColorDialog>

namespace VectorForge {

PropertyInspector::PropertyInspector(Document* doc, CommandStack* cmdStack, QWidget* parent)
    : QWidget(parent), m_doc(doc), m_cmdStack(cmdStack) {
    setupUi();

    if (m_doc) {
        connect(m_doc, &Document::selectionChanged, this, &PropertyInspector::updateFromSelection);
        connect(m_doc, &Document::documentChanged, this, &PropertyInspector::updateFromSelection);
    }
}

void PropertyInspector::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    // Appearance Group Box
    auto appearanceBox = new QGroupBox("Appearance & Style", this);
    auto appLayout = new QFormLayout(appearanceBox);

    m_btnStrokeColor = new QPushButton("Pick Stroke", this);
    m_btnFillColor = new QPushButton("Pick Fill", this);
    m_spinStrokeWidth = new QDoubleSpinBox(this);
    m_spinStrokeWidth->setRange(0.0, 50.0);
    m_spinStrokeWidth->setSingleStep(0.5);

    m_sliderOpacity = new QSlider(Qt::Horizontal, this);
    m_sliderOpacity->setRange(0, 100);
    m_sliderOpacity->setValue(100);

    m_comboStrokeStyle = new QComboBox(this);
    m_comboStrokeStyle->addItem("Solid Line", static_cast<int>(Qt::SolidLine));
    m_comboStrokeStyle->addItem("Dash Line", static_cast<int>(Qt::DashLine));
    m_comboStrokeStyle->addItem("Dot Line", static_cast<int>(Qt::DotLine));

    appLayout->addRow("Stroke Color:", m_btnStrokeColor);
    appLayout->addRow("Fill Color:", m_btnFillColor);
    appLayout->addRow("Stroke Width:", m_spinStrokeWidth);
    appLayout->addRow("Opacity:", m_sliderOpacity);
    appLayout->addRow("Line Style:", m_comboStrokeStyle);

    mainLayout->addWidget(appearanceBox);

    // Connect Color Dialogs
    connect(m_btnStrokeColor, &QPushButton::clicked, this, [this]() {
        QColor col = QColorDialog::getColor(m_currentStroke, this, "Select Stroke Color", QColorDialog::ShowAlphaChannel);
        if (col.isValid()) {
            m_currentStroke = col;
            applyStyleChanges();
        }
    });

    connect(m_btnFillColor, &QPushButton::clicked, this, [this]() {
        QColor col = QColorDialog::getColor(m_currentFill, this, "Select Fill Color", QColorDialog::ShowAlphaChannel);
        if (col.isValid()) {
            m_currentFill = col;
            applyStyleChanges();
        }
    });

    connect(m_spinStrokeWidth, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double) {
        if (!m_isUpdatingUi) applyStyleChanges();
    });

    connect(m_sliderOpacity, &QSlider::valueChanged, this, [this](int) {
        if (!m_isUpdatingUi) applyStyleChanges();
    });

    connect(m_comboStrokeStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        if (!m_isUpdatingUi) applyStyleChanges();
    });

    mainLayout->addStretch();
}

void PropertyInspector::updateFromSelection() {
    if (!m_doc || m_isUpdatingUi) return;

    m_isUpdatingUi = true;

    if (!m_doc->selection().empty()) {
        auto shape = m_doc->selection().front();
        if (shape) {
            m_currentStroke = shape->strokeColor();
            m_currentFill = shape->fillColor();
            m_spinStrokeWidth->setValue(shape->strokeWidth());
            m_sliderOpacity->setValue(static_cast<int>(shape->opacity() * 100.0));

            m_btnStrokeColor->setStyleSheet(QString("background-color: %1; color: white; border: 1px solid #555;").arg(m_currentStroke.name()));
            m_btnFillColor->setStyleSheet(QString("background-color: %1; color: white; border: 1px solid #555;").arg(m_currentFill.name(QColor::HexArgb)));
        }
    }

    m_isUpdatingUi = false;
}

void PropertyInspector::applyStyleChanges() {
    if (!m_doc || m_doc->selection().empty() || !m_cmdStack || m_isUpdatingUi) return;

    std::vector<StyleEntry> entries;
    for (const auto& shape : m_doc->selection()) {
        if (shape) {
            StyleState oldS = {shape->fillColor(), shape->strokeColor(), shape->strokeWidth(), shape->strokeStyle(), shape->opacity()};
            StyleState newS = {m_currentFill, m_currentStroke, m_spinStrokeWidth->value(), static_cast<Qt::PenStyle>(m_comboStrokeStyle->currentData().toInt()), m_sliderOpacity->value() / 100.0};
            entries.push_back({shape, oldS, newS});
        }
    }

    auto cmd = std::make_unique<StyleChangeCommand>(m_doc, entries);
    m_cmdStack->pushAndExecute(std::move(cmd));
}

} // namespace VectorForge

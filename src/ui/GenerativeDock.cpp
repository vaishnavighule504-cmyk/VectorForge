#include "GenerativeDock.h"
#include "../command/ShapeCommands.h"
#include <QColorDialog>
#include <QLabel>

namespace VectorForge {

GenerativeDock::GenerativeDock(Document* doc, CommandStack* cmdStack, QWidget* parent)
    : QWidget(parent), m_doc(doc), m_cmdStack(cmdStack) {
    setupUi();
}

void GenerativeDock::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    auto headerLabel = new QLabel("🎨 Generative Art Studio", this);
    headerLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #00E6FF;");
    mainLayout->addWidget(headerLabel);

    m_comboAlgorithms = new QComboBox(this);
    for (const auto& name : m_engine.algorithmNames()) {
        m_comboAlgorithms->addItem(QString::fromStdString(name));
    }
    mainLayout->addWidget(m_comboAlgorithms);

    m_paramGroupBox = new QGroupBox("Parametric Controls", this);
    m_paramLayout = new QFormLayout(m_paramGroupBox);
    mainLayout->addWidget(m_paramGroupBox);

    m_btnGenerate = new QPushButton("✨ Generate Vector Art", this);
    m_btnGenerate->setStyleSheet("background-color: #00E6FF; color: black; font-weight: bold; padding: 8px; border-radius: 4px;");
    mainLayout->addWidget(m_btnGenerate);

    mainLayout->addStretch();

    connect(m_comboAlgorithms, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GenerativeDock::onAlgorithmChanged);
    connect(m_btnGenerate, &QPushButton::clicked, this, &GenerativeDock::generateArt);

    if (m_comboAlgorithms->count() > 0) {
        onAlgorithmChanged(0);
    }
}

void GenerativeDock::onAlgorithmChanged(int index) {
    QLayoutItem* child;
    while ((child = m_paramLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }
    m_bindings.clear();

    std::string name = m_comboAlgorithms->itemText(index).toStdString();
    auto algo = m_engine.findAlgorithm(name);
    if (algo) {
        buildParameterControls(algo->parameters());
    }
}

void GenerativeDock::buildParameterControls(const std::vector<ParameterInfo>& params) {
    for (const auto& info : params) {
        QWidget* control = nullptr;

        if (info.type == ParamType::Int) {
            auto spin = new QSpinBox(this);
            spin->setRange(static_cast<int>(info.minValue), static_cast<int>(info.maxValue));
            spin->setValue(static_cast<int>(info.defaultValue));
            spin->setSingleStep(static_cast<int>(info.step));
            control = spin;
        } else if (info.type == ParamType::Double) {
            auto spin = new QDoubleSpinBox(this);
            spin->setRange(info.minValue, info.maxValue);
            spin->setValue(info.defaultValue);
            spin->setSingleStep(info.step);
            control = spin;
        } else if (info.type == ParamType::Color) {
            auto btn = new QPushButton("Pick Color", this);
            QColor current = info.defaultColor;
            btn->setStyleSheet(QString("background-color: %1; color: white;").arg(current.name()));
            connect(btn, &QPushButton::clicked, this, [btn, current]() mutable {
                QColor c = QColorDialog::getColor(current, btn, "Select Color");
                if (c.isValid()) {
                    current = c;
                    btn->setStyleSheet(QString("background-color: %1; color: white;").arg(c.name()));
                    btn->setProperty("selectedColor", c);
                }
            });
            btn->setProperty("selectedColor", current);
            control = btn;
        }

        if (control) {
            m_paramLayout->addRow(QString::fromStdString(info.label) + ":", control);
            m_bindings.push_back({info, control});
        }
    }
}

void GenerativeDock::generateArt() {
    if (!m_doc || !m_cmdStack) return;

    std::string algoName = m_comboAlgorithms->currentText().toStdString();
    ParameterMap paramMap;

    for (const auto& binding : m_bindings) {
        if (binding.info.type == ParamType::Int) {
            auto spin = qobject_cast<QSpinBox*>(binding.widget);
            if (spin) paramMap[binding.info.name] = spin->value();
        } else if (binding.info.type == ParamType::Double) {
            auto spin = qobject_cast<QDoubleSpinBox*>(binding.widget);
            if (spin) paramMap[binding.info.name] = spin->value();
        } else if (binding.info.type == ParamType::Color) {
            QVariant colVar = binding.widget->property("selectedColor");
            if (colVar.isValid()) paramMap[binding.info.name] = colVar.value<QColor>();
        }
    }

    QPointF origin(m_doc->width() / 2.0, m_doc->height() / 2.0);
    auto generatedShapes = m_engine.generate(algoName, paramMap, origin);

    if (!generatedShapes.empty()) {
        auto group = std::make_shared<GroupShape>(generatedShapes);
        group->setName(algoName + " Output");
        auto cmd = std::make_unique<AddShapeCommand>(m_doc, group);
        m_cmdStack->pushAndExecute(std::move(cmd));
    }
}

} // namespace VectorForge

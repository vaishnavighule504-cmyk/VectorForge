#ifndef VECTORFORGE_GENERATIVEDOCK_H
#define VECTORFORGE_GENERATIVEDOCK_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QGroupBox>
#include <map>
#include <memory>
#include "../generative/GenerativeEngine.h"
#include "../model/Document.h"
#include "../command/CommandStack.h"

namespace VectorForge {

class GenerativeDock : public QWidget {
    Q_OBJECT
public:
    explicit GenerativeDock(Document* doc, CommandStack* cmdStack, QWidget* parent = nullptr);
    ~GenerativeDock() override = default;

private slots:
    void onAlgorithmChanged(int index);
    void generateArt();

private:
    void setupUi();
    void buildParameterControls(const std::vector<ParameterInfo>& params);

    Document* m_doc;
    CommandStack* m_cmdStack;
    GenerativeEngine m_engine;

    QComboBox* m_comboAlgorithms;
    QGroupBox* m_paramGroupBox;
    QFormLayout* m_paramLayout;
    QPushButton* m_btnGenerate;

    struct ControlBinding {
        ParameterInfo info;
        QWidget* widget;
    };
    std::vector<ControlBinding> m_bindings;
};

} // namespace VectorForge

#endif // VECTORFORGE_GENERATIVEDOCK_H

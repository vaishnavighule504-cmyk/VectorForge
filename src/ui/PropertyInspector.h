#ifndef VECTORFORGE_PROPERTYINSPECTOR_H
#define VECTORFORGE_PROPERTYINSPECTOR_H

#include <QWidget>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QComboBox>
#include <QLabel>
#include "../model/Document.h"
#include "../command/CommandStack.h"
#include "../command/ShapeCommands.h"

namespace VectorForge {

class PropertyInspector : public QWidget {
    Q_OBJECT
public:
    explicit PropertyInspector(Document* doc, CommandStack* cmdStack, QWidget* parent = nullptr);
    ~PropertyInspector() override = default;

public slots:
    void updateFromSelection();

private:
    void setupUi();
    void applyStyleChanges();

    Document* m_doc;
    CommandStack* m_cmdStack;

    QPushButton* m_btnStrokeColor;
    QPushButton* m_btnFillColor;
    QDoubleSpinBox* m_spinStrokeWidth;
    QSlider* m_sliderOpacity;
    QComboBox* m_comboStrokeStyle;

    QDoubleSpinBox* m_spinX;
    QDoubleSpinBox* m_spinY;
    QDoubleSpinBox* m_spinWidth;
    QDoubleSpinBox* m_spinHeight;

    QColor m_currentStroke{Qt::white};
    QColor m_currentFill{QColor(100, 150, 240, 100)};

    bool m_isUpdatingUi{false};
};

} // namespace VectorForge

#endif // VECTORFORGE_PROPERTYINSPECTOR_H

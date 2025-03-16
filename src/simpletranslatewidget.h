//
// Created by YubinKim on 25/03/12 수.
//

#ifndef SIMPLETRANSLATEWIDGET_H
#define SIMPLETRANSLATEWIDGET_H

#include <QWidget>

class QVBoxLayout;
class QLabel;
QT_BEGIN_NAMESPACE

namespace Ui
{
class SimpleTranslateWidget;
}

QT_END_NAMESPACE


class SimpleTranslateWidget : public QWidget
{
    Q_OBJECT

public:
    SimpleTranslateWidget(QWidget* parent = nullptr);

    ~SimpleTranslateWidget() override;

    void showTranslationPopup(const QString& translatedText);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    bool bIsDrag = false;
    QPoint dragPoint;

private:
    Ui::SimpleTranslateWidget* ui;
};


#endif //SIMPLETRANSLATEWIDGET_H

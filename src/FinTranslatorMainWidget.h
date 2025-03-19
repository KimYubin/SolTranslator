//
// Created by YubinKim on 25/03/19 수.
//

#ifndef FINTRANSLATORMAINWIDGET_H
#define FINTRANSLATORMAINWIDGET_H

#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui
{
class FinTranslatorMainWidget;
}

QT_END_NAMESPACE

class FinTranslatorMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FinTranslatorMainWidget(QWidget* parent = nullptr);
    ~FinTranslatorMainWidget() override;

private:
    Ui::FinTranslatorMainWidget* ui;
};


#endif //FINTRANSLATORMAINWIDGET_H

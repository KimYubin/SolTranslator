//
// Created by YubinKim on 25/05/13 화.
//

#ifndef COMMONOPTIONWIDGET_H
#define COMMONOPTIONWIDGET_H

#include <QWidget>

#include "IOptionWidget.h"


QT_BEGIN_NAMESPACE

namespace Ui
{
class CommonOptionWidget;
}

QT_END_NAMESPACE

class CommonOptionWidget : public IOptionWidget
{
    Q_OBJECT

public:
    explicit CommonOptionWidget(QWidget* parent = nullptr);
    ~CommonOptionWidget() override;

private:
protected:
    friend class CommonOption;
    virtual void apply() override;
    virtual void cancel() override;
    virtual void finish() override;
    
    Ui::CommonOptionWidget* ui;
};


class CommonOption : public IOptionPage
{
    Q_DISABLE_COPY_MOVE(CommonOption)

public:
    CommonOption();
    virtual ~CommonOption() override;

};


#endif //COMMONOPTIONWIDGET_H

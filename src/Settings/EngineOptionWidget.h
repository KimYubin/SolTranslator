//
// Created by YubinKim on 25/04/14 월.
//

#ifndef ENGINEOPTIONWIDGET_H
#define ENGINEOPTIONWIDGET_H
#include "IOptionWidget.h"

class FinTranslatorCore;
QT_BEGIN_NAMESPACE
namespace Ui
{
class EngineOptionWidget;
}
QT_END_NAMESPACE

class EngineOptionWidget : public IOptionWidget
{
    Q_OBJECT

public:
    explicit EngineOptionWidget(QWidget* parent = nullptr);
    ~EngineOptionWidget() override;

protected:
    friend class EngineOption;
    virtual void apply() override;
    virtual void cancel() override;
    virtual void finish() override;

    void applyTheme();

    Ui::EngineOptionWidget* ui;
};



class EngineOption : public IOptionPage
{
    Q_DISABLE_COPY_MOVE(EngineOption)

public:
    EngineOption();
    virtual ~EngineOption();

};


#endif //ENGINEOPTIONWIDGET_H

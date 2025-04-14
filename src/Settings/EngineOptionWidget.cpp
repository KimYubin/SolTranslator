//
// Created by YubinKim on 25/04/14 월.
//
#include <QString>

#include "EngineOptionWidget.h"
#include "../../ui/ui_EngineOptionWidget.h"

EngineOptionWidget::EngineOptionWidget()
{
}

EngineOptionWidget::~EngineOptionWidget() {
}

void EngineOptionWidget::apply()
{
    IOptionWidget::apply();
}

void EngineOptionWidget::cancel()
{
    IOptionWidget::cancel();
}

void EngineOptionWidget::finish()
{
    IOptionWidget::finish();
}

EngineOption::EngineOption()
{
    setDisplayName(tr(""));
    setIconPath(tr(""));
    setOptionWidgetCtor([]() { return new EngineOptionWidget(); });
}

EngineOption::~EngineOption()
{
}

const EngineOption engineOption;

// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATOR_WIDGETINSPECTOR_H
#define FINTRANSLATOR_WIDGETINSPECTOR_H

#include <QWidget>

class QLabel;

class WidgetInspector : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetInspector(QWidget* parent = nullptr);

    virtual bool eventFilter(QObject* watched, QEvent* event) override;

    void updateInfo(QObject* watched, QEvent* event);

private:
    QLabel* label;
};

#endif //FINTRANSLATOR_WIDGETINSPECTOR_H

// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_WIDGETINSPECTOR_H
#define SOLTRANSLATOR_WIDGETINSPECTOR_H

#include <QWidget>

class QLabel;

class WidgetInspector : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetInspector(QWidget* parent = nullptr);

    virtual bool eventFilter(QObject* watched, QEvent* event) override;

    void updateInfo(const QObject* watched, const QEvent* event);

private:
    QLabel* label;
};

#endif //SOLTRANSLATOR_WIDGETINSPECTOR_H

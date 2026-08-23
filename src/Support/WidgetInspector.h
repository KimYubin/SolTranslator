// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_WIDGETINSPECTOR_H
#define SOLTRANSLATOR_WIDGETINSPECTOR_H

#include <QWidget>

class QLabel;

namespace Sol
{
class WidgetInspector : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetInspector(QWidget* inParent = nullptr);

    bool eventFilter(QObject* inWatched, QEvent* inEvent) override;

    void updateInfo(const QObject* inWatched, const QEvent* inEvent);

private:
    QLabel* _label;
};

} // namespace Sol

#endif //SOLTRANSLATOR_WIDGETINSPECTOR_H

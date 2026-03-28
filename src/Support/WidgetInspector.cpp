// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.


#include "WidgetInspector.h"

#include "SolUtilibrary.h"

#include "../../external/magic_enum.hpp"

#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>


WidgetInspector::WidgetInspector(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet("background: rgba(0, 0, 0, 150); color: white; font: 10pt;");

    label       = new QLabel("Waiting...", this);
    auto layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->setContentsMargins(5, 5, 5, 5);

    resize(300, 50);

    qApp->installEventFilter(this);

    show();
}

bool WidgetInspector::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseMove)
    {
        updateInfo(watched, event);
    }
    return QWidget::eventFilter(watched, event);
}

void WidgetInspector::updateInfo(QObject* watched, QEvent* event)
{
    const QPoint globalPos = QCursor::pos();

    if (const QWidget* widget = QApplication::widgetAt(globalPos))
    {
        label->setText(QString("Class: %1\nObject: %2\nevent: %3")
                       .arg(widget->metaObject()->className())
                       .arg(widget->objectName().isEmpty() ? "<no name>" : widget->objectName())
                       .arg(Sol::enumToQStr(event->type())));
    }
    else
    {
        label->setText("No widget under cursor");
    }
}

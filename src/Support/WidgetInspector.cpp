// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.


#include "WidgetInspector.h"

#include "Utils/EnumUtils.hpp"

#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

#include <magic_enum.hpp>


WidgetInspector::WidgetInspector(QWidget* inParent) : QWidget(inParent)
{
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet("background: rgba(0, 0, 0, 150); color: white; font: 10pt;");

    _label = new QLabel("Waiting...", this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(_label);
    layout->setContentsMargins(5, 5, 5, 5);

    resize(300, 50);

    qApp->installEventFilter(this);

    show();
}

bool WidgetInspector::eventFilter(QObject* inWatched, QEvent* inEvent)
{
    if (inEvent->type() == QEvent::MouseMove)
    {
        updateInfo(inWatched, inEvent);
    }
    return QWidget::eventFilter(inWatched, inEvent);
}

void WidgetInspector::updateInfo(const QObject* inWatched, const QEvent* inEvent)
{
    const QPoint globalPos = QCursor::pos();

    if (const QWidget* widget = QApplication::widgetAt(globalPos))
    {
        _label->setText(QString("Class: %1\nObject: %2\nevent: %3")
                       .arg(widget->metaObject()->className())
                       .arg(widget->objectName().isEmpty() ? "<no name>" : widget->objectName())
                       .arg(Sol::enumToQStr(inEvent->type())));
    }
    else
    {
        _label->setText("No widget under cursor");
    }
}

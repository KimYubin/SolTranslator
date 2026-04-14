// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "LoadingSpinner.h"

#include "SolToolTip.h"

#include <QSvgRenderer>
#include <QSvgWidget>
#include <QTimer>

LoadingSpinner::LoadingSpinner(const QString& inFile, QWidget* parent)
    : ILoadingWidget(inFile, parent)
{
    _svg = new QSvgWidget(inFile, this);

    _stopTimer = new QTimer(this);
    _stopTimer->setInterval(500);
    _stopTimer->setSingleShot(true);
    _stopTimer->callOnTimeout(this, &LoadingSpinner::hideSvg);

    hideSvg();
}

void LoadingSpinner::setBubbleToolTip(const QString& inToolTip)
{
    SolTooltipFilter::setBubbleToolTip(_svg, inToolTip);
}

void LoadingSpinner::run()
{
    showSvg();
}

void LoadingSpinner::stop()
{
    _stopTimer->start();
}

void LoadingSpinner::showSvg()
{
    _svg->renderer()->setAnimationEnabled(true);
    _svg->setVisible(true);
}

void LoadingSpinner::hideSvg()
{
    _svg->renderer()->setAnimationEnabled(false);
    _svg->setVisible(false);
}

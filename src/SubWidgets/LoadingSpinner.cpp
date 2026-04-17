// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "LoadingSpinner.h"

#include "SolToolTip.h"

#include <QPainter>
#include <QPropertyAnimation>
#include <QStyleOption>
#include <QSvgRenderer>
#include <QTimer>


LoadingSpinner::LoadingSpinner(const QString& inFile, QWidget* parent)
    : ILoadingWidget(parent)
{
    _svgRenderer = new QSvgRenderer(this);
    _svgRenderer->load(inFile);
    connect(_svgRenderer, &QSvgRenderer::repaintNeeded, this, [this]() { update(); });

    _fadeOutAnim = new QPropertyAnimation(this, "opacityRatio", this);
    _fadeOutAnim->setDuration(200);
    _fadeOutAnim->setEasingCurve(QEasingCurve::OutCubic);
    _fadeOutAnim->setStartValue(1);
    _fadeOutAnim->setEndValue(0);
    connect(_fadeOutAnim, &QAbstractAnimation::finished, this, &LoadingSpinner::hideSvg);

    _fadeOutDelay = new QTimer(this);
    _fadeOutDelay->setInterval(500);
    _fadeOutDelay->setSingleShot(true);
    _fadeOutDelay->callOnTimeout(_fadeOutAnim, [this]() { _fadeOutAnim->start(); });


    ILoadingWidget::setBubbleToolTip("");

    hideSvg();
}

void LoadingSpinner::setBubbleToolTip(const QString& inToolTip)
{
    _toolTip = inToolTip;

    setToolTip(_isSvgVisible ? _toolTip : "");
}

void LoadingSpinner::run()
{
    showSvg();
}

void LoadingSpinner::stop()
{
    if (_isSvgVisible)
    {
        _fadeOutDelay->start();
    }
}

void LoadingSpinner::showEvent(QShowEvent* event)
{
    _svgRenderer->setAnimationEnabled(true);

    ILoadingWidget::showEvent(event);
}

void LoadingSpinner::hideEvent(QHideEvent* event)
{
    _svgRenderer->setAnimationEnabled(false);

    ILoadingWidget::hideEvent(event);
}

void LoadingSpinner::paintEvent(QPaintEvent* event)
{
    ILoadingWidget::paintEvent(event);

    QStyleOption opt;
    opt.initFrom(this);
    QPainter opacityPainter(this);
    opacityPainter.setOpacity(_opacityRatio);

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &opacityPainter, this);
    _svgRenderer->render(&opacityPainter);
}

void LoadingSpinner::setSvgVisibility(const bool inVisible)
{
    _isSvgVisible = inVisible;
    _opacityRatio = _isSvgVisible ? 1 : 0;
    _svgRenderer->setAnimationEnabled(_isSvgVisible);
    _fadeOutDelay->stop();
    _fadeOutAnim->stop();

    setToolTip(_isSvgVisible ? _toolTip : "");
}

void LoadingSpinner::showSvg()
{
    setSvgVisibility(true);
    update();
}

void LoadingSpinner::hideSvg()
{
    setSvgVisibility(false);
    update();
}

void LoadingSpinner::setOpacityRatio(const float inOpacityRatio)
{
    _opacityRatio = inOpacityRatio;
    update();
}

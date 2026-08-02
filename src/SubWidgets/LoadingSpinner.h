// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_LOADINGSPINNER_H
#define SOLTRANSLATOR_LOADINGSPINNER_H

#include "ILoadingWidget.h"

class QSvgRenderer;
class QTimer;
class QPropertyAnimation;

class LoadingSpinner : public ILoadingWidget
{
    Q_OBJECT
    Q_PROPERTY(float opacityRatio READ getOpacityRatio WRITE setOpacityRatio)

public:
    explicit LoadingSpinner(const QString& inFile, QWidget* parent = nullptr);

    void run() override;
    void stop() override;

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

private:
    void setSvgVisibility(const bool inVisible);
    void showSvg();
    void hideSvg();

private:
    QSvgRenderer* _svgRenderer;
    QPropertyAnimation* _fadeOutAnim;
    QTimer* _fadeOutDelay;

    bool _isSvgVisible;

    // Q_PROPERTY
    float _opacityRatio;
    float getOpacityRatio() const { return _opacityRatio; }
    void setOpacityRatio(const float inOpacityRatio);
};


#endif //SOLTRANSLATOR_LOADINGSPINNER_H

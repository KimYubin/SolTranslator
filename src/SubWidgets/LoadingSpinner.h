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
    virtual void setBubbleToolTip(const QString& inToolTip) override;

    virtual void run() override;
    virtual void stop() override;

protected:
    virtual void showEvent(QShowEvent* event) override;
    virtual void hideEvent(QHideEvent* event) override;

    virtual void paintEvent(QPaintEvent* event) override;

private:
    void setSvgVisibility(const bool inVisible);
    void showSvg();
    void hideSvg();

private:
    QSvgRenderer* _svgRenderer;
    QPropertyAnimation* _fadeOutAnim;
    QTimer* _fadeOutDelay;

    QString _toolTip;

    bool _isSvgVisible;

    // Q_PROPERTY
    float _opacityRatio;
    float getOpacityRatio() const { return _opacityRatio; }
    void setOpacityRatio(const float inOpacityRatio);
};


#endif //SOLTRANSLATOR_LOADINGSPINNER_H

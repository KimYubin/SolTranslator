// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef LOADINGBAR_H
#define LOADINGBAR_H

#include <QWidget>


class QPropertyAnimation;
class QSvgWidget;

class LoadingBar : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(float animRatio READ getAnimRatio WRITE setAnimRatio)

public:
    explicit LoadingBar(const QString &file, QWidget *parent = nullptr);

    void run();
    void stop();
private:
    QSvgWidget* svg;
    QPropertyAnimation* animation;

    float getAnimRatio() const {return _animRatio;};
    void setAnimRatio(const float inAnimRatio);
    float _animRatio;
    float _prevRatio;
    float _integralRatio;

    bool _bRunning;
};


#endif //LOADINGBAR_H

// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "SolUtilibrary.h"

#include <QApplication>
#include <QWidget>
#include <QFont>
#include <QScreen>


bool sol::isThis(const QObject* inThis, const QObject* inOther)
{
    bool bIsOtherThis = false;

    const QObject* parentObj = inOther;
    while (parentObj != nullptr)
    {
        if (parentObj == inThis)
        {
            bIsOtherThis = true;
            break;
        }
        parentObj = parentObj->parent();
    }
    return bIsOtherThis;
}

namespace
{
template <typename T>
concept HasFontFunctions = requires(T* t)
{
    { t->font() } -> std::convertible_to<QFont>;
    { t->setFont(std::declval<QFont>()) };
};

template <HasFontFunctions T>
void noHintingFontInternal(T* inOutWidget)
{
    QFont qfont = inOutWidget->font();
    qfont.setHintingPreference(QFont::PreferNoHinting);
    qfont.setStyleStrategy(QFont::PreferAntialias);
    inOutWidget->setFont(qfont);


    // 그레이스케일 적용을 위해서는 qt.conf 파일에 다음 내용을 추가해야합니다.
    //
    // [Platforms]
    // WindowsArguments = fontengine=freetype
    //
    // 또한 다음을 적용해야합니다.
    // qfont.setStyleStrategy(QFont::NoSubpixelAntialias);
}
} // anonymous namespace

void sol::noHintingFont(QWidget* inOutWidget)
{
    noHintingFontInternal(inOutWidget);
}

void sol::noHintingFont()
{
    noHintingFontInternal(qApp);
}

QRect sol::availableGeometryAt(const QPoint& inPoint)
{
    const QScreen* cursorScreen = qApp->screenAt(inPoint);
    const QScreen* targetScreen = cursorScreen ? cursorScreen : qApp->primaryScreen();

    return targetScreen ? targetScreen->availableGeometry() : QRect();
}

QRect sol::moveToInside(const QRect& outerRect, const QRect& innerRect)
{
    QRect res = innerRect;
    if (outerRect.left() > res.left())
    {
        res.moveLeft(outerRect.left());
    }
    if (outerRect.top() > res.top())
    {
        res.moveTop(outerRect.top());
    }
    if (outerRect.right() < res.right())
    {
        res.moveRight(outerRect.right());
    }
    if (outerRect.bottom() < res.bottom())
    {
        res.moveBottom(outerRect.bottom());
    }

    return res;
}

QString sol::QStrFromStdView(const std::string_view& inStrView)
{
    // string_view -> std::str -> QString
    return QString::fromStdString(std::string{inStrView});
}

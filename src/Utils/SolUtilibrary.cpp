// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolUtilibrary.h"

#include "Types/EngineId.h"
#include "Types/OptionKey.h"

#include <QApplication>
#include <QFont>
#include <QScreen>
#include <QWidget>


bool Sol::isThis(const QObject* inThis, const QObject* inOther)
{
    bool isOtherThis = false;

    const QObject* parentObj = inOther;
    while (parentObj != nullptr)
    {
        if (parentObj == inThis)
        {
            isOtherThis = true;
            break;
        }
        parentObj = parentObj->parent();
    }
    return isOtherThis;
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
    qfont.setHintingPreference(QFont::PreferDefaultHinting);
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

void Sol::noHintingFont(QWidget* inOutWidget)
{
    noHintingFontInternal(inOutWidget);
}

void Sol::noHintingFont()
{
    noHintingFontInternal(qApp);
}

QRect Sol::availableGeometryAt(const QPoint& inPoint)
{
    const QScreen* cursorScreen = qApp->screenAt(inPoint);
    const QScreen* targetScreen = cursorScreen ? cursorScreen : qApp->primaryScreen();

    return targetScreen ? targetScreen->availableGeometry() : QRect();
}

QRect Sol::moveToInside(const QRect& outerRect, const QRect& innerRect)
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

QString Sol::qStrFromStdView(const std::string_view& inStrView)
{
    // string_view -> std::str -> QString
    return QString::fromStdString(std::string{inStrView});
}

QString Sol::engineOptionKey(const EngineId& inEngineId, const OptionKey& inKey)
{
    return "Engine/" + inEngineId.toString() + "/" + inKey.toString();
}

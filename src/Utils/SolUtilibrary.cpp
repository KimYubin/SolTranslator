// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolUtilibrary.h"

#include "Types/EngineId.h"
#include "Types/OptionKey.h"

#include <QApplication>
#include <QFont>
#include <QScreen>
#include <QWidget>


namespace Sol
{
bool isThis(const QObject* inThis, const QObject* inOther)
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

void noHintingFont(QWidget* inOutWidget)
{
    noHintingFontInternal(inOutWidget);
}

void noHintingFont()
{
    noHintingFontInternal(qApp);
}

QRect availableGeometryAt(const QPoint& inPoint)
{
    const QScreen* cursorScreen = qApp->screenAt(inPoint);
    const QScreen* targetScreen = cursorScreen ? cursorScreen : qApp->primaryScreen();

    return targetScreen ? targetScreen->availableGeometry() : QRect();
}

QRect moveToInside(const QRect& inOuterRect, const QRect& inNerRect)
{
    QRect res = inNerRect;
    if (inOuterRect.left() > res.left())
    {
        res.moveLeft(inOuterRect.left());
    }
    if (inOuterRect.top() > res.top())
    {
        res.moveTop(inOuterRect.top());
    }
    if (inOuterRect.right() < res.right())
    {
        res.moveRight(inOuterRect.right());
    }
    if (inOuterRect.bottom() < res.bottom())
    {
        res.moveBottom(inOuterRect.bottom());
    }

    return res;
}

QString qStrFromStdView(const std::string_view& inStrView)
{
    // string_view -> std::str -> QString
    return QString::fromStdString(std::string{inStrView});
}

QString engineOptionKey(const EngineId& inEngineId, const OptionKey& inKey)
{
    return "Engine/" + inEngineId.toString() + "/" + inKey.toString();
}
} // namespace Sol

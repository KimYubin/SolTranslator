// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolWidgetFactory.h"

#include "SolButton.h"
#include "SolToast.h"
#include "SolTypes.h"
#include "Utils/Tr.h"

#include <QApplication>
#include <QClipboard>
#include <QPushButton>
#include <QTimer>

using Sol::i18n;

namespace
{
SolButton* createButton(QWidget* inParent
                      , const QIcon& inIcon
                      , const Qt::FocusPolicy inPolicy
                      , const QString& inToolTip
                      , const Action inAction
                      , std::move_only_function<void(void)>&& inFunc)
{
    SolButton* newButton = new SolButton(inParent);
    newButton->setIcon(inIcon);
    newButton->setFocusPolicy(inPolicy);
    newButton->setToolTipAction(inToolTip, inAction);

    return newButton;
}
} // anonymous namespace

SolButton* SolWidgetFactory::createCopyButton(QWidget* inParent
                                            , std::move_only_function<QString(void)>&& inCopyStringFunc)
{
    SolButton* copyButton = new SolButton(inParent);
    copyButton->setIcon(QIcon(":/img/copy_img"));
    copyButton->setFocusPolicy(Qt::TabFocus);
    copyButton->setToolTipAction(i18n(Tr::Copy_Translation), Action::DocCopyButton);

    connect(copyButton, &SolButton::clicked, inParent, [inParent, copyButton, func = std::move(inCopyStringFunc)]() mutable
    {
        QMetaObject::Connection connection = connect(QApplication::clipboard(), &QClipboard::dataChanged, copyButton, [copyButton]() mutable
        {
            SolToast::popToastOnWidget(i18n(Tr::Copy_Complete_Noti), copyButton, 150);
        }, Qt::SingleShotConnection);

        // connection timeout limit.
        // Prevents empty copies and unlimited waiting.
        QTimer::singleShot(500, inParent, [connection]()
        {
            disconnect(connection);
        });

        QGuiApplication::clipboard()->setText(func());
    });

    return copyButton;
}

SolButton* SolWidgetFactory::createToggleButton(QWidget* inParent
                                              , std::move_only_function<void()>&& inToggleFunc)
{
    SolButton* toggleButton = new SolButton(inParent);
    toggleButton->setIcon(QIcon(":/img/swap_text_img"));
    toggleButton->setFocusPolicy(Qt::TabFocus);
    toggleButton->setToolTipAction(i18n(Tr::Source_Target_Toggle), Action::TextToggle);

    connect(toggleButton, &QPushButton::clicked, inParent, [inParent, toggleFunc = std::move(inToggleFunc)]() mutable
    {
        toggleFunc();
    });

    return toggleButton;
}

SolButton* SolWidgetFactory::createReTranslateButton(QWidget* inParent
                                                   , std::move_only_function<void(void)>&& inTranslateFunc)
{
    SolButton* newButton = new SolButton(inParent);
    newButton->setIcon(QIcon(":/img/refresh_img"));
    newButton->setFocusPolicy(Qt::TabFocus);
    newButton->setToolTipAction(i18n(Tr::Re_Translate), Action::None);

    connect(newButton, &QPushButton::clicked, inParent, [inParent, trFunc = std::move(inTranslateFunc)]() mutable
    {
        trFunc();
    });

    return newButton;
}

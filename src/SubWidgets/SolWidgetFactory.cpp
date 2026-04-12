// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolWidgetFactory.h"

#include "SolButton.h"
#include "SolToast.h"
#include "Types/SolTypes.h"
#include "Utils/Tr.h"

#include <QApplication>
#include <QClipboard>
#include <QPushButton>
#include <QTimer>

using Sol::i18n;

SolButton* SolWidgetFactory::createButton(QWidget* inParent
                                        , const QIcon& inIcon
                                        , const Qt::FocusPolicy inPolicy
                                        , const QString& inToolTip
                                        , const Action inAction)
{
    SolButton* newButton = new SolButton(inParent);
    newButton->setIcon(inIcon);
    newButton->setFocusPolicy(inPolicy);
    newButton->setToolTipAction(inToolTip, inAction);

    return newButton;
}

SolButton* SolWidgetFactory::createCopyButton(QWidget* inParent
                                            , std::move_only_function<QString(void)>&& inCopyStringFunc)
{
    SolButton* copyButton = createButton(inParent
                                       , QIcon(":/img/copy_img")
                                       , Qt::TabFocus
                                       , i18n(Tr::Copy_Translation)
                                       , Action::CopyDoc);

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
    SolButton* toggleButton = createButton(inParent
                                         , QIcon(":/img/swap_text_img")
                                         , Qt::TabFocus
                                         , i18n(Tr::Source_Target_Toggle)
                                         , Action::SourceTargetToggle);

    connect(toggleButton, &QPushButton::clicked, inParent, [clickFunc = std::move(inToggleFunc)]() mutable
    {
        clickFunc();
    });

    return toggleButton;
}

SolButton* SolWidgetFactory::createReTranslateButton(QWidget* inParent
                                                   , std::move_only_function<void(void)>&& inTranslateFunc)
{
    SolButton* newButton = createButton(inParent
                                      , QIcon(":/img/refresh_img")
                                      , Qt::TabFocus
                                      , i18n(Tr::Re_Translate)
                                      , Action::ReTranslate);

    connect(newButton, &QPushButton::clicked, inParent, [clickFunc = std::move(inTranslateFunc)]() mutable
    {
        clickFunc();
    });

    return newButton;
}

SolButton* SolWidgetFactory::createViewInPopup(QWidget* inParent
                                             , std::move_only_function<void()>&& inFunc)
{
    SolButton* newButton = createButton(inParent
                                      , QIcon(":/img/open_new_img")
                                      , Qt::TabFocus
                                      , i18n(Tr::View_In_Popup)
                                      , Action::ViewInPopup);

    connect(newButton, &QPushButton::clicked, inParent, [clickFunc = std::move(inFunc)]() mutable
    {
        clickFunc();
    });

    return newButton;
}

SolButton* SolWidgetFactory::createDeleteTranslation(QWidget* inParent
                                                   , std::move_only_function<void()>&& inFunc)
{
    SolButton* newButton = createButton(inParent
                                      , QIcon(":/img/delete_img")
                                      , Qt::TabFocus
                                      , i18n(Tr::Delete_Translation)
                                      , Action::DeleteTranslation);

    connect(newButton, &QPushButton::clicked, inParent, [clickFunc = std::move(inFunc)]() mutable
    {
        clickFunc();
    });

    return newButton;
}

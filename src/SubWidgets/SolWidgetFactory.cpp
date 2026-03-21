// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolWidgetFactory.h"

#include <QApplication>
#include <QClipboard>
#include <QPushButton>
#include <QTimer>

#include "SolButton.h"
#include "SolToast.h"
#include "SolToolTip.h"

#include "Managers/ConfigManager.h"
#include "Utils/Tr.h"

using sol::i18n;

SolButton* SolWidgetFactory::createCopyButton(QWidget* inParent, std::move_only_function<QString()>&& inCopyStringFunc)
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

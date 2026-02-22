// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolWidgetFactory.h"

#include <QApplication>
#include <QClipboard>
#include <QPushButton>
#include <QTimer>

#include "SolToast.h"
#include "SolToolTip.h"


QPushButton* SolWidgetFactory::createCopyButton(QWidget* inParent, std::function<QString()>&& inCopyStringFunc)
{
    QPushButton* copyButton = new QPushButton(inParent);
    copyButton->setIcon(QIcon(":/img/copy_img"));
    copyButton->setShortcut(Qt::Key_C);
    copyButton->setFocusPolicy(Qt::TabFocus);

    SolTooltipFilter::setBubbleToolTip(copyButton, tr("번역 복사(<u>C<\\u>)"));

    connect(copyButton, &QPushButton::clicked, inParent, [inParent, copyButton, func = std::move(inCopyStringFunc)]()
    {
        QMetaObject::Connection connection = connect(QApplication::clipboard(), &QClipboard::dataChanged, copyButton, [copyButton]() mutable
        {
            SolToast::popToastOnWidget(tr("복사 완료!"), copyButton, 50);
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

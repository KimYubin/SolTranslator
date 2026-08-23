// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolTrayIcon.h"

#include "Managers/ConfigManager.h"

namespace Sol
{
SolTrayIcon::SolTrayIcon(QObject* inParent) : QSystemTrayIcon(inParent)
{
    QMenu* trayMenu = new QMenu();
    trayMenu->setAttribute(Qt::WA_TranslucentBackground);
    trayMenu->setWindowFlag(Qt::FramelessWindowHint);
    trayMenu->setWindowFlag(Qt::NoDropShadowWindowHint);
    trayMenu->setObjectName("trayMenu");
    setContextMenu(trayMenu);
}

SolTrayIcon::SolTrayIcon(const QIcon& inIcon, QObject* inParent)
    : SolTrayIcon(inParent)
{
    setIcon(inIcon);
}

SolTrayIcon::~SolTrayIcon()
{
    if (contextMenu())
    {
        contextMenu()->deleteLater();
    }
}

QAction* SolTrayIcon::addSeparator()
{
    return contextMenu()->addSeparator();
}

namespace
{
constexpr QChar amp = '&';
qsizetype findFirstAmp(const QString& inStr, int inFrom = 0)
{
    const int strSize = inStr.size();
    while (inFrom < strSize)
    {
        const qsizetype findIdx = inStr.indexOf(amp, inFrom);
        if (findIdx < 0)
        {
            return -1;
        }

        const bool prev = ((findIdx > 0) && (inStr[findIdx - 1] == amp));
        const bool next = ((findIdx + 1) < inStr.size() && inStr[findIdx + 1] == amp);
        if (!prev && !next)
        {
            return findIdx;
        }

        inFrom = findIdx + 1;
    }
    return -1;
}
} // anonymous namespace
void SolTrayIcon::addActionInMenu(QAction* inQAction, const Action inAction)
{
    QString actText           = inQAction->text();
    const QKeySequence keySeq = solConfig.shortcut(inAction);
    QString keyStr            = keySeq.toString();
    if (keyStr.size() != 1)
    {
        // 단일키만 허용
        return;
    }

    const qsizetype ampIdx = findFirstAmp(actText);
    if (ampIdx >= 0)
    {
        if (ampIdx + 1 < actText.size() && (actText[ampIdx + 1] == keyStr))
        {
            return;
        }

        actText.remove(ampIdx, 2);

        // 괄호에 있었다면, 빈 괄호 제거.
        if (actText.last(2) == "()" && inQAction->text().last(2) != "()")
        {
            actText.chop(2);
        }
    }

    // 동일한 키가 이미 있다면 앰퍼센트로 대응
    const int findIdx = actText.indexOf(keyStr[0]);
    if (findIdx >= 0)
    {
        actText.insert(findIdx, amp);
    }
    else
    {
        actText += "(&" + keyStr + ")";
    }

    inQAction->setText(actText);
}
} // namespace Sol

// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLTRAYICON_H
#define SOLTRANSLATOR_SOLTRAYICON_H

#include "Types/SolTypes.h"
#include "Utils/SolI18n.h"

#include <QMenu>
#include <QSystemTrayIcon>
#include <QWidget>


class SolTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

    template <typename Func>
    using ContextType = QtPrivate::ContextTypeForFunctor<Func>::ContextType;

public:
    explicit SolTrayIcon(QObject* inParent = nullptr);
    explicit SolTrayIcon(const QIcon& inIcon, QObject* inParent = nullptr);
    ~SolTrayIcon() override;

    // ~================
    // addAction
    /** Text ,Action */
    template <typename Func>
    QAction* addAction(const QString& inText
                     , const Action inAction
                     , const ContextType<Func>* inContext
                     , Func&& inSlot
                     , Qt::ConnectionType inType = Qt::AutoConnection)
    {
        QAction* result = contextMenu()->addAction(inText);
        connect(result, &QAction::triggered, inContext, std::forward<Func>(inSlot), inType);

        addActionInMenu(result, inAction);
        return result;
    }

    /** Tr ,Action */
    template <typename Func>
    QAction* addAction(const Tr inTr
                     , const Action inAction
                     , const ContextType<Func>* inContext
                     , Func&& inSlot
                     , Qt::ConnectionType inType = Qt::AutoConnection)
    {
        return addAction(Sol::i18n(inTr), inAction, inContext, std::forward<Func>(inSlot), inType);
    }

    /** Text */
    template <typename Func>
    QAction* addAction(const QString& inText
                     , const ContextType<Func>* inContext
                     , Func&& inSlot
                     , Qt::ConnectionType inType = Qt::AutoConnection)
    {
        return addAction(inText, Action::None, inContext, std::forward<Func>(inSlot), inType);
    }

    /** Tr */
    template <typename Func>
    QAction* addAction(const Tr inTr
                     , const ContextType<Func>* inContext
                     , Func&& inSlot
                     , Qt::ConnectionType inType = Qt::AutoConnection)
    {
        return addAction(Sol::i18n(inTr), inContext, std::forward<Func>(inSlot), inType);
    }

    /** Icon, Text, Action */
    template <typename Func>
    QAction* addAction(const QIcon& inIcon
                     , const QString& inText
                     , const Action inAction
                     , const ContextType<Func>* inContext
                     , Func&& inSlot
                     , Qt::ConnectionType inType = Qt::AutoConnection)
    {
        QAction* result = addAction(inText, inAction, inContext, std::forward<Func>(inSlot), inType);

        result->setIcon(inIcon);
        return result;
    }

    /** Icon, Tr, Action */
    template <typename Func>
    QAction* addAction(const QIcon& inIcon
                     , const Tr inTr
                     , const Action inAction
                     , const ContextType<Func>* inContext
                     , Func&& inSlot
                     , Qt::ConnectionType inType = Qt::AutoConnection)
    {
        return addAction(inIcon, Sol::i18n(inTr), inAction, inContext, std::forward<Func>(inSlot), inType);
    }

    /** Icon, Text */
    template <typename Func>
    QAction* addAction(const QIcon& inIcon
                     , const QString& inText
                     , const ContextType<Func>* inContext
                     , Func&& inSlot
                     , Qt::ConnectionType inType = Qt::AutoConnection)
    {
        return addAction(inIcon, inText, inContext, std::forward<Func>(inSlot), inType);;
    }

    /** Icon, Tr */
    template <typename Func>
    QAction* addAction(const QIcon& inIcon
                     , const Tr inTr
                     , const ContextType<Func>* inContext
                     , Func&& inSlot
                     , Qt::ConnectionType inType = Qt::AutoConnection)
    {
        return addAction(inIcon, Sol::i18n(inTr), inContext, std::forward<Func>(inSlot), inType);
    }

    QAction* addSeparator();

protected:
    void addActionInMenu(QAction* inQAction, const Action inAction);
};


#endif //SOLTRANSLATOR_SOLTRAYICON_H

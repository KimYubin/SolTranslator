// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLASYNC_H
#define SOLASYNC_H

#include "Types/SolTypes.h"

#include <QFuture>
#include <QObject>
#include <QtConcurrent>


class SolAsync : public QObject
{
    Q_OBJECT

public:
    /**
     * Fire & Forget, QFuture와 QFutureWatcher을 이용한 비동기 계산 후, 동기화 로직의 간소화 버전입니다.
     * 
     * @tparam Ret 비동기 함수 반환값입니다. 
     * @param inWatcherContext watcher의 수명을 관리하는 QObject 객체입니다.
     * @param inAsyncFunc worker thread에서 실행할 펑터입니다.
     * @param inMainThreadFunc main thread에서 계산 결과를 동기화하는 펑터입니다. inAsyncFunc의 반환값을 매개변수로 받아야합니다.
     */
    template <typename Ret>
    static void asyncLaunch(QObject* inWatcherContext
                          , Callback<Ret(void)>&& inAsyncFunc
                          , Callback<void(Ret)>&& inMainThreadFunc)
    {
        QFutureWatcher<Ret>* watcher = new QFutureWatcher<Ret>(inWatcherContext);
        connect(watcher, &QFutureWatcher<Ret>::finished, inWatcherContext, [watcher, mtFunc = std::move(inMainThreadFunc)]() mutable
        {
            mtFunc(watcher->future().result());
            watcher->deleteLater();
        });
        QFuture<Ret> future = QtConcurrent::run(std::move(inAsyncFunc));

        watcher->setFuture(future);
    }

    template <typename>
    static void asyncLaunch(QObject* inWatcherContext
                          , Callback<void(void)>&& inAsyncFunc
                          , Callback<void(void)>&& inMainThreadFunc)
    {
        QFutureWatcher<void>* watcher = new QFutureWatcher<void>(inWatcherContext);
        connect(watcher, &QFutureWatcher<void>::finished, inWatcherContext, [watcher, mtFunc = std::move(inMainThreadFunc)]() mutable
        {
            mtFunc();
            watcher->deleteLater();
        });
        QFuture<void> future = QtConcurrent::run(std::move(inAsyncFunc));

        watcher->setFuture(future);
    }

};


#endif //SOLASYNC_H

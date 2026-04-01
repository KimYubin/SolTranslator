// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef ASYNCMANAGER_H
#define ASYNCMANAGER_H

#include "AbstractManager.h"

#include <QFuture>
#include <QtConcurrent>


class SolTranslatorCore;

class AsyncManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit AsyncManager(SolTranslatorCore* parent);

    /**
     * Fire & Forget, QFuture와 QFutureWatcher을 이용한 비동기 계산 후, 동기화 로직의 간소화 버전입니다.
     * 
     * @tparam ret 비동기 함수 반환값입니다. 
     * @param inWatcherContext watcher의 수명을 관리하는 QObject 객체입니다.
     * @param inAsyncFunc worker thread에서 실행할 펑터입니다.
     * @param inMainThreadFunc main thread에서 계산 결과를 동기화하는 펑터입니다. inAsyncFunc의 반환값을 매개변수로 받아야합니다.
     */
    template <typename ret>
    static void asyncLaunch(QObject* inWatcherContext
                          , std::move_only_function<ret(void)>&& inAsyncFunc
                          , std::move_only_function<void(ret)>&& inMainThreadFunc)
    {
        QFutureWatcher<ret>* watcher = new QFutureWatcher<ret>(inWatcherContext);
        connect(watcher, &QFutureWatcher<ret>::finished, inWatcherContext, [watcher, mtFunc = std::move(inMainThreadFunc)] () mutable
        {
            mtFunc(watcher->future().result());
            watcher->deleteLater();
        });
        QFuture<ret> future = QtConcurrent::run(std::move(inAsyncFunc));

        watcher->setFuture(future);
    }

    template <typename>
    static void asyncLaunch(QObject* inWatcherContext
                          , std::move_only_function<void(void)>&& inAsyncFunc
                          , std::move_only_function<void(void)>&& inMainThreadFunc)
    {
        QFutureWatcher<void>* watcher = new QFutureWatcher<void>(inWatcherContext);
        connect(watcher, &QFutureWatcher<void>::finished, inWatcherContext, [watcher, mtFunc = std::move(inMainThreadFunc)] () mutable
        {
            mtFunc();
            watcher->deleteLater();
        });
        QFuture<void> future = QtConcurrent::run(std::move(inAsyncFunc));

        watcher->setFuture(future);
    }

private:
};


#endif //ASYNCMANAGER_H

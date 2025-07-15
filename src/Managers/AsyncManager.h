// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef ASYNCMANAGER_H
#define ASYNCMANAGER_H

#include "AbstractManager.h"

#include <QtConcurrent>
#include <QFuture>


class FinTranslatorCore;

class AsyncManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit AsyncManager(FinTranslatorCore* parent);

    /**
     * Fire & Forget, QFuture와 QFutureWatcher을 이용한 비동기 계산 후, 동기화 로직의 간소화 버전입니다.
     * 
     * @tparam ret 비동기 함수 반환값입니다. 
     * @param inWatcherContext watcher의 수명을 관리하는 QObject 객체입니다.
     * @param inAsyncFunc worker thread에서 실행할 펑터입니다.
     * @param inMainThreadFunc main thread에서 계산 결과를 동기화하는 펑터입니다. 계산 완료 후 반환 값을 받습니다. inAsyncFunc의 반환값을 매개변수로 받아야합니다. 
     */
    template <typename ret>
    static void asyncLaunch(QObject* inWatcherContext
                          , std::function<ret()>&& inAsyncFunc
                          , std::function<void(ret)>&& inMainThreadFunc)
    {
        QFutureWatcher<ret>* watcher = new QFutureWatcher<ret>(inWatcherContext);
        connect(watcher, &QFutureWatcher<ret>::finished, inWatcherContext, [watcher, mtFunc = std::forward<std::function<void(ret)>>(inMainThreadFunc)]
        {
            mtFunc(watcher->future().result());
            watcher->deleteLater();
        });
        QFuture<ret> future = QtConcurrent::run(std::forward<std::function<ret()>>(inAsyncFunc));

        watcher->setFuture(future);
    }
    
    template <typename>
    static void asyncLaunch(QObject* inWatcherContext
                          , std::function<void()>&& inAsyncFunc
                          , std::function<void(void)>&& inMainThreadFunc)
    {
        QFutureWatcher<void>* watcher = new QFutureWatcher<void>(inWatcherContext);
        connect(watcher, &QFutureWatcher<void>::finished, inWatcherContext, [watcher, mtFunc = std::forward<std::function<void(void)>>(inMainThreadFunc)]
        {
            watcher->deleteLater();
        });
        QFuture<void> future = QtConcurrent::run(std::forward<std::function<void()>>(inAsyncFunc));

        watcher->setFuture(future);
    }

private:
};


#endif //ASYNCMANAGER_H

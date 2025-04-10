//
// Created by YubinKim on 25/04/09 수.
//

#ifndef ASYNCMANAGER_H
#define ASYNCMANAGER_H

#include "AbstractManager.h";

#include <QtConcurrent>
#include <QFuture>


class FinTranslatorCore;

class AsyncManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit AsyncManager(FinTranslatorCore* parent);

    template <typename ret>
    static void asyncLaunch(QObject* inContext
                         , std::function<ret()>&& inAsyncFunc
                         , std::function<void(ret)>&& inMainThreadFunc)
    {
        QFutureWatcher<ret>* watcher = new QFutureWatcher<ret>(inContext);
        connect(watcher, &QFutureWatcher<ret>::finished, inContext, [watcher, mtFunc = std::forward<std::function<void(ret)>>(inMainThreadFunc)]
        {
            mtFunc(watcher->future().result());
            watcher->deleteLater();
        });
        QFuture<ret> future = QtConcurrent::run(std::forward<std::function<ret()>>(inAsyncFunc));

        watcher->setFuture(future);
    }
    template <typename ret>
    void asyncTask(std::function<ret()>&& inAsyncFunc, QObject* inReceiver, std::function<void(ret)>&& inMainThreadFunc)
    {
        QFutureWatcher<ret>* watcher = new QFutureWatcher<ret>(this);
        connect(watcher, &QFutureWatcher<ret>::finished, inReceiver, [watcher, mtFunc = std::forward<std::function<void(ret)>>(inMainThreadFunc)]
        {
            mtFunc(watcher->future().result());
            watcher->deleteLater();
        });
        QFuture<ret> future = QtConcurrent::run(std::forward<std::function<ret()>>(inAsyncFunc));

        watcher->setFuture(future);
    }

    template <typename>
    void asyncTask(std::function<void()>&& inAsyncFunc, QObject* inReceiver, std::function<void(void)>&& inMainThreadFunc)
    {
        QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
        connect(watcher, &QFutureWatcher<void>::finished, inReceiver, [watcher, mtFunc = std::forward<std::function<void(void)>>(inMainThreadFunc)]
        {
            watcher->deleteLater();
        });
        QFuture<void> future = QtConcurrent::run(std::forward<std::function<void()>>(inAsyncFunc));

        watcher->setFuture(future);
    }

private:
};


#endif //ASYNCMANAGER_H

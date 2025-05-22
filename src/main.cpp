
#include <QApplication>
#include <QFile>

#include <qdatetime.h>

#include "FinTranslatorCore.h"

// 로그 파일 스트림
QFile logFile;
QTextStream logStream;

// 메시지 핸들러 함수
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logMessage;

    switch (type) {
    case QtDebugMsg:
        logMessage = QString("[%1] Debug: %2").arg(timeStamp, msg);
        break;
    case QtInfoMsg:
        logMessage = QString("[%1] Info: %2").arg(timeStamp, msg);
        break;
    case QtWarningMsg:
        logMessage = QString("[%1] Warning: %2").arg(timeStamp, msg);
        break;
    case QtCriticalMsg:
        logMessage = QString("[%1] Critical: %2").arg(timeStamp, msg);
        break;
    case QtFatalMsg:
        logMessage = QString("[%1] Fatal: %2").arg(timeStamp, msg);
        abort();
    }

    logStream << logMessage << Qt::endl;
    logStream.flush();
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    FinTranslatorCore finTranslatorCore(app);

    // 로그 파일 열기
    logFile.setFileName(FinPaths::getLogPath());
    if (!logFile.open(QIODevice::Append | QIODevice::Text))
    {
        qCritical() << "로그 파일을 열 수 없습니다.";
        return -1;
    }
    logStream.setDevice(&logFile);

    // 메시지 핸들러 등록
    qInstallMessageHandler(customMessageHandler);

    // 예제 로그
    qInfo() << argv;
    qInfo() << "애플리케이션 시작";
    qWarning() << "경고 메시지 예시";

    return app.exec();
}

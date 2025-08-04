// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef ITRANSLATEWIDGET_H
#define ITRANSLATEWIDGET_H

#include <QPointer>
#include <QWidget>

#include "FinTypes.h"

class TranslateUnit;
class QTextCursor;
class QScrollBar;

class ITranslateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ITranslateWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    ~ITranslateWidget() override;

    virtual void streamTransText(const QString& inTranslatedText, const TextStyle inTextStyle);

    virtual void completeTransText(const QString& inTranslatedText, const TextStyle inTextStyle);

    void abortTrUnit();

    void setTrUnit(TranslateUnit* inTrUnit);

protected:
    /**
     * 번역 텍스트를 에디터에 적용합니다.
     * 에디터 크기 변경과 무관하게 스크롤바 위치를 고정합니다.
     * 
     * @param inTranslatedText 
     * @param inTextStyle 
     */
    void setTranslationWithFixedScroll(const QString& inTranslatedText, const TextStyle inTextStyle);

    /**
     * 번역을 텍스트 에디터에 적용합니다.
     * 
     * @param inTranslatedText 
     * @param inTextStyle 
     */
    virtual void applyTranslation(const QString& inTranslatedText, const TextStyle inTextStyle) = 0;

    /**
     * 스크롤바 위치를 고정하기 위해 사용할 스크롤바를 반환합니다
     * 자식 클래스에서 이를 지정합니다.
     */
    virtual QScrollBar* getVerticalScrollBar() = 0;
    virtual QScrollBar* getHorizontalScrollBar() = 0;

    /**
     * 번역 중, 커서 위치를 고정하기 위해 사용할 텍스트 커서
     */
    virtual QTextCursor getTextCursor() = 0;
    virtual void setTextCursor(const QTextCursor& cursor) = 0;

    QPointer<TranslateUnit> _trUnit;

    QString _translatedText;        // 번역문자열 보관
    TextStyle _translatedTextStyle; // 번역문자열의 스타일
    QTimer* _streamUpdateTimer;     // 연속으로 너무 빨리 업데이트 되는 것을 방지하기 위한 타이머.
};

#endif //ITRANSLATEWIDGET_H

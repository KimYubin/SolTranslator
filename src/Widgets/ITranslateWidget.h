// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef ITRANSLATEWIDGET_H
#define ITRANSLATEWIDGET_H

#include "SolTypes.h"

#include <QPointer>
#include <QWidget>

class TranslateUnit;
class QTextCursor;
class QScrollBar;

class ITranslateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ITranslateWidget(QWidget* parent = nullptr, const Qt::WindowFlags flags = Qt::WindowFlags());

    ~ITranslateWidget() override;

protected:
    virtual void executeTranslateImpl(const EngineType inEngine
                                    , const QString& inSourceText
                                    , const TextStyle inTextStyle
                                    , const LangType inSourceLang
                                    , const LangType inTargetLang
                                    , const bool inIsIgnoreCache);

public:
    virtual void streamTranslateText(const QString& inTargetText);

    virtual void completeTranslateText(const QString& inTargetText);

    void detachTrUnit() const;
    void abortTrUnit() const;

    void setTrUnit(TranslateUnit* inTrUnit);

protected:
    /**
     * 번역 텍스트를 에디터에 적용합니다.
     * 에디터 크기 변경과 무관하게 스크롤바 위치를 고정합니다.
     */
    void applyTranslationWithFixedScroll();

    /**
     * 번역을 텍스트 에디터에 적용합니다.
     */
    virtual void applyTranslation() = 0;

    /**
     * 스크롤바 위치를 고정하기 위해 사용할 스크롤바를 반환합니다
     * 자식 클래스에서 이를 지정합니다.
     */
    virtual QScrollBar* getVerticalScrollBar() const = 0;
    virtual QScrollBar* getHorizontalScrollBar() const = 0;

    /**
     * 번역 중, 커서 위치를 고정하기 위해 사용할 텍스트 커서
     */
    virtual QTextCursor getTextCursor() const = 0;
    virtual void setTextCursor(const QTextCursor& cursor) = 0;

    const QString& getSourceText() const { return _sourceText; }
    const QString& getTargetText() const { return _targetText; }
    TextStyle getTextStyle() const { return _textStyle; }

    void setSourceAndStyle(const QString& inSourceText, const TextStyle inTextStyle);

private:
    QPointer<TranslateUnit> _trUnit;

    QString _sourceText;
    QString _targetText;
    TextStyle _textStyle;
    QTimer* _streamUpdateTimer; // To prevent updates from occurring too quickly in succession.
};

#endif //ITRANSLATEWIDGET_H

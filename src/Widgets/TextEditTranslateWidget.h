//
// Created by YubinKim on 25/03/19 수.
//

#ifndef TEXTEDITTRANSLATEWIDGET_H
#define TEXTEDITTRANSLATEWIDGET_H

#include <QWidget>

#include "ITranslateWidget.h"


enum class TextStyle;
class FinTranslatorCore;
QT_BEGIN_NAMESPACE

namespace Ui
{
class TextEditTranslateWidget;
}

QT_END_NAMESPACE

class TextEditTranslateWidget : public ITranslateWidget
{
    Q_OBJECT

public:
    explicit TextEditTranslateWidget(QWidget* parent = nullptr);
    ~TextEditTranslateWidget() override;

protected:
    /** 입력된 문자열로 교체하고, 적정 사이즈로 팝업을 엽니다. */
    virtual void applyTranslation(const QString& inTranslatedText, const TextStyle inTextStyle) override;

    virtual QScrollBar* getVerticalScrollBar() override;
    virtual QScrollBar* getHorizontalScrollBar() override;

public:
    void focusTextOrigin();

private slots:
    void onTranslateClicked();

private:
    QString _prevString;
    QTimer* _updateStreamStrTimer;

    Ui::TextEditTranslateWidget* ui;
};


#endif //TEXTEDITTRANSLATEWIDGET_H

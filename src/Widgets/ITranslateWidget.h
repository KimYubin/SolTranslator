//
// Created by YubinKim on 25/06/01 일.
//

#ifndef ITRANSLATEWIDGET_H
#define ITRANSLATEWIDGET_H

#include <QWidget>

#include "../FinTypes.h"

class ITranslateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ITranslateWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    ~ITranslateWidget() override;

    virtual void streamTransText(const QString& inTranslatedText, const TextStyle inTextStyle);

    virtual void completeTransText(const QString& inTranslatedText, const TextStyle inTextStyle);

protected:
    virtual void applyTranslation(const QString& inTranslatedText, const TextStyle inTextStyle) = 0;

    
    QString _prevString;
    TextStyle _prevTextStyle;
    QTimer* _updateStreamStrTimer;
};

#endif //ITRANSLATEWIDGET_H

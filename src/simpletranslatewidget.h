//
// Created by YubinKim on 25/03/12 수.
//

#ifndef SIMPLETRANSLATEWIDGET_H
#define SIMPLETRANSLATEWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class SimpleTranslateWidget; }
QT_END_NAMESPACE



class SimpleTranslateWidget : public QWidget {
Q_OBJECT

public:
    SimpleTranslateWidget(const QString &translatedText, QWidget *parent = nullptr);

    ~SimpleTranslateWidget() override;

    void showTranslationPopup(const QString &translatedText);

private:
    Ui::SimpleTranslateWidget *ui;
    
};


#endif //SIMPLETRANSLATEWIDGET_H

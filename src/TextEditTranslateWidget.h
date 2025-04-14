//
// Created by YubinKim on 25/03/19 수.
//

#ifndef TEXTEDITTRANSLATEWIDGET_H
#define TEXTEDITTRANSLATEWIDGET_H

#include <QWidget>


class FinTranslatorCore;
QT_BEGIN_NAMESPACE

namespace Ui
{
class TextEditTranslateWidget;
}

QT_END_NAMESPACE

class TextEditTranslateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TextEditTranslateWidget(QWidget* parent = nullptr);
    ~TextEditTranslateWidget() override;

private slots:
    void on_findButton_clicked();

private:
    void loadSettings();
    void loadAPI();

private:
    Ui::TextEditTranslateWidget* ui;
};


#endif //TEXTEDITTRANSLATEWIDGET_H

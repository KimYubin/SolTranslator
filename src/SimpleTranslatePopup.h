//
// Created by YubinKim on 25/03/12 수.
//

#ifndef SIMPLETRANSLATEPOPUP_H
#define SIMPLETRANSLATEPOPUP_H

#include <QWidget>

class FinTranslatorCore;
class QVBoxLayout;
class QLabel;

QT_BEGIN_NAMESPACE

namespace Ui
{
class SimpleTranslatePopup;
}

QT_END_NAMESPACE


class SimpleTranslatePopup : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleTranslatePopup(FinTranslatorCore* inFinCore, QWidget* parent = nullptr);

    ~SimpleTranslatePopup() override;


    void showTranslationPopup(const QString& inTranslatedText);
    void addTranslationText(const QString& inTranslatedText);
    void completeText(const QString& inTranslatedText);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:

    void calculateTextEditSize();
    
    bool bIsDrag = false;
    QPoint dragPoint;


    QMargins outerMargin;
    QMargins innerMargin;
    int frameLineWidth;
    QSize minTextEditSize;
    QSize maxTextEditSize;

    
    const float widthRatio  = 0.20f;
    const float heightRatio = 0.6f;
    const float xPosRatio   = 0.85f;
    const float yPosRatio   = 0.35f;

private:
    FinTranslatorCore* finCore;

    Ui::SimpleTranslatePopup* ui;
};


#endif //SIMPLETRANSLATEPOPUP_H

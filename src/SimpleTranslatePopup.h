//
// Created by YubinKim on 25/03/12 수.
//

#ifndef SIMPLETRANSLATEPOPUP_H
#define SIMPLETRANSLATEPOPUP_H

#include <QWidget>

class QPropertyAnimation;
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

    Q_PROPERTY(QSize textEditSize READ getTextEditSize WRITE setTextEditSize)
    Q_PROPERTY(QPoint textEditPos READ getTextEditPos WRITE setTextEditPos)

public:
    explicit SimpleTranslatePopup(FinTranslatorCore* inFinCore, QWidget* parent = nullptr);

    ~SimpleTranslatePopup() override;


    void showTranslationPopup(const QString& inTranslatedText);
    void addTranslationText(const QString& inTranslatedText);
    void completeText(const QString& inTranslatedText);

    QSize getTextEditSize() const { return _textEditSize; };
    QPoint getTextEditPos() const { return _textEditPos; };

    /** 텍스트 에디트 사이즈를 기반으로 전체 Widget의 크기와 위치를 계산 및 적용합니다. */
    void setTextEditSize(const QSize& inTextEditSize);
    void setTextEditPos(const QPoint& inTextEditPos);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void animateResize();

private:

    void calculateTextEditMax();
    QSize calculateTextEditSize(int margin = 10);

    QPropertyAnimation* animation;

    QSize _textEditSize;
    QPoint _textEditPos;

    bool bIsDrag = false;
    QPoint dragPoint;


    QMargins outerMargin;
    QMargins innerMargin;

    QSize minTextEditSize;
    QSize maxTextEditSize;

    QSize _prevSize;
    const float widthRatio  = 0.20f;
    const float heightRatio = 0.6f;
    const float xPosRatio   = 0.85f;
    const float yPosRatio   = 0.35f;

private:
    FinTranslatorCore* finCore;

    Ui::SimpleTranslatePopup* ui;
};


#endif //SIMPLETRANSLATEPOPUP_H

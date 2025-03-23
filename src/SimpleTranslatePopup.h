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

public:
    explicit SimpleTranslatePopup(FinTranslatorCore* inFinCore, QWidget* parent = nullptr);

    ~SimpleTranslatePopup() override;


    void showTranslationPopup(const QString& inTranslatedText);

    QSize getTextEditSize() const { return _textEditSize; };

    /** 텍스트 에디트 사이즈를 기반으로 전체 Widget의 크기와 위치를 계산 및 적용합니다. */
    void setTextEditSize(const QSize& inTextEditSize);

private:
    void animateTextEditResize(const QSize& inNewSize);
    void calculateTextEditMax();

    /**
     * inNewText의 에디터 크기를 계산합니다. 
     */
    QSize calculateTextEditSize(const QString& inNewText);
    
protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;


    QPropertyAnimation* _animation;

    QSize _textEditSize;

    bool _bIsDrag = false;
    QPoint _dragPoint;


    QSize _innerMarginSize;
    QSize _outerMarginSize;

    QSize _minEditSize;
    QSize _maxEditSize;

    QSize _prevSize;
    int _lineBreakCount = 0;

    const float widthRatio  = 0.20f;
    const float heightRatio = 0.6f;
    const float xPosRatio   = 0.85f;
    const float yPosRatio   = 0.35f;

    FinTranslatorCore* _finCore;

private:
    Ui::SimpleTranslatePopup* ui;
};


#endif //SIMPLETRANSLATEPOPUP_H

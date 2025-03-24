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

    /** 입력된 문자열로 교체하고, 적정 사이즈로 팝업을 엽니다. */
    void showTranslationPopup(const QString& inTranslatedText);

    QSize getTextEditSize() const { return _textEditSize; };

    /** 텍스트 에디트 사이즈를 기반으로 전체 Widget의 크기와 위치를 계산 및 적용합니다. */
    void setTextEditSize(const QSize& inTextEditSize);

private:
    /** 입력된 inNewText에 적합한 에디터의 크기를 계산합니다. */
    QSize calculateTextEditSize(const QString& inNewText);

    /** 입력된 사이즈를 목표로 애니메이션을 실행합니다. */
    void animateTextEditResize(const QSize& inNewSize);

    /** text edit의 최대 규격 등을 계산합니다. */
    void calculateTextEditMax();

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;


    QPropertyAnimation* _animation;

    QSize _textEditSize;

    QSize _innerMarginSize;
    QSize _outerMarginSize;

    QSize _minEditSize;
    QSize _maxEditSize;

    QSize _prevSize;

    int _lineCount = 0;
    int _lastLineLength = 0;

    const float widthRatio  = 0.20f;
    const float heightRatio = 0.6f;
    const float xPosRatio   = 0.85f;
    const float yPosRatio   = 0.35f;

    bool _bIsDrag = false;
    QPoint _dragPoint;

private:
    FinTranslatorCore* _finCore;

    Ui::SimpleTranslatePopup* ui;
};


#endif //SIMPLETRANSLATEPOPUP_H

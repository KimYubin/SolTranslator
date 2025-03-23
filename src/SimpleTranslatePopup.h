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
    void addTranslationText(const QString& inTranslatedText);
    void completeText(const QString& inTranslatedText);

    QSize getTextEditSize() const { return _textEditSize; };

    /** 텍스트 에디트 사이즈를 기반으로 전체 Widget의 크기와 위치를 계산 및 적용합니다. */
    void setTextEditSize(const QSize& inTextEditSize);
    void setTextEditPos(const QPoint& inTextEditPos);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void animateTextEditResize(const QSize& inNewSize);

private:

    void calculateTextEditMax();

    /**
     * inNewText의 에디터 크기를 계산합니다. 
     */
    QSize calculateTextEditSize(const QString& inNewText);

    QPropertyAnimation* animation;

    QSize _textEditSize;

    bool bIsDrag = false;
    QPoint dragPoint;


    QSize innerMarginSize;
    QSize outerMarginSize;

    QSize minEditSize;
    QSize maxEditSize;

    QSize _prevSize;
    int _lineBreakCount = 0;

    const float widthRatio  = 0.20f;
    const float heightRatio = 0.6f;
    const float xPosRatio   = 0.85f;
    const float yPosRatio   = 0.35f;

private:
    FinTranslatorCore* finCore;

    Ui::SimpleTranslatePopup* ui;
};


#endif //SIMPLETRANSLATEPOPUP_H

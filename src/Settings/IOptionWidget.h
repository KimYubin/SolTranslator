// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef OPTIONWIDGET_H
#define OPTIONWIDGET_H

#include <QObject>
#include <QPointer>
#include <QScrollArea>
#include <QWidget>

#include "FinTypes.h"

class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QGroupBox;
class FinTranslatorCore;
class IOptionPage;

class IOptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IOptionWidget(QWidget* parent = nullptr);
    ~IOptionWidget() override;

protected:
    friend class IOptionPage;
    void initializeAfterCtor();
    void syncInOutScrollbar();

    virtual void apply();
    virtual void cancel();
    virtual void finish();


    /**
     * 새로운 GroupBox와 GroupBox 내부에서 사용되는 VBoxLayout을 만들어 제공합니다.
     * inParentLayout의 위치에 추가되는 동작을 포함합니다.
     * 
     * @param inGroupTitle GroupBox 타이틀에 사용할 이름 
     * @param inParentLayout GroupBox가 배치될 상위 레이아웃
     * @param inRow inParentLayout row
     * @param inColumn inParentLayout row
     * @param inAlignment inParentLayout에서 Alignment
     * @return 그룹박스와 그룹박스 내부에 배치된 VBoxLayout을 반환합니다.
     */
    std::tuple<QGroupBox*, QVBoxLayout*> newOptionGroupBox(const QString& inGroupTitle, QGridLayout* inParentLayout
                                                         , const int inRow, const int inColumn
                                                         , Qt::Alignment inAlignment = Qt::Alignment());
    std::tuple<QGroupBox*, QVBoxLayout*> newOptionGroupBox(const QString& inGroupTitle, QGridLayout* inParentLayout
                                                         , const int inRow, const int inColumn
                                                         , const int inRowSpan, const int inColumnSpan
                                                         , Qt::Alignment inAlignment = Qt::Alignment());

    /**
     * 새로운 GroupBox와 GroupBox 내부에서 사용되는 VBoxLayout을 만들어 제공합니다.
     * mainLayout에 마지막에 추가됩니다.
     * 
     * @param inGroupTitle GroupBox 타이틀에 사용할 이름 
     * @return 그룹박스와 그룹박스 내부에 배치된 VBoxLayout을 반환합니다.
     */
    std::tuple<QGroupBox*, QVBoxLayout*> addNewOptionGroupBox(const QString& inGroupTitle);

    /**
     * 새로운 GroupBox와 GroupBox 내부에서 사용되는 VBoxLayout을 만들어 제공합니다.
     * 
     * @param inGroupTitle GroupBox 타이틀에 사용할 이름 
     * @return 그룹박스와 그룹박스 내부에 배치된 VBoxLayout을 반환합니다.
     */
    std::tuple<QGroupBox*, QVBoxLayout*> generateGroupBox(const QString& inGroupTitle);

public:
    IOptionPage* getOptionPage() const;

private:
    void setOptionPage(IOptionPage* inOptionPage);
    QPointer<IOptionPage> _optionPage;

    QHBoxLayout* _outScrollLayout; // scrollArea와 외부 스크롤바가 위치할 레이아웃
    QScrollBar* _outScrollBar;     // 외부 스크롤바

    QScrollArea* _srollArea;
    QWidget* _scrollContent;
    QGridLayout* _wrapMainLayout;  // mainLayout을 AlignTop으로 위로 정렬하기 위한 랩핑용 레이아웃

protected:
    QGridLayout* _mainLayout; // 내부 위젯을 배치할 가장 안쪽 레이아웃
};



class IOptionPage : public QObject
{
    Q_OBJECT

public:
    IOptionPage();
    ~IOptionPage() override;

    static const QSet<QPointer<IOptionPage>>& allOptionsPages();
    static std::vector<IOptionPage*> sortedOptionsPages();
    static bool compareOptionsPages(const IOptionPage* inPage1, const IOptionPage* inPage2);

    QString getDisplayName() const;
    QString getIconPath() const;
    QIcon getIcon() const;

    virtual QWidget* getOptionWidget();
    virtual void apply();
    virtual void cancel();
    virtual void finish();

protected:
    void setDisplayName(const QString& inDisplayName);
    void setIconPath(const QString& inIconPath);
    void setOptionWidgetCtor(const std::function<IOptionWidget*()>& inOptionWidgetCtor);
    void setPriority(const OptionPriority inPriority);

private:
    QPointer<IOptionWidget> _optionWidget;

    QString _displayName;
    QString _iconPath;
    std::function<IOptionWidget*()> _optionWidgetCtor;
    OptionPriority _priority = OptionPriority::None; // 옵션 정렬 우선 순위

    int _optionStkId;

    Q_DISABLE_COPY_MOVE(IOptionPage)
};


#endif //OPTIONWIDGET_H

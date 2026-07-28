// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef OPTIONWIDGET_H
#define OPTIONWIDGET_H

#include "SubWidgets/SolSmoothAbstractScrollArea.hpp"
#include "Types/SolTypes.h"

#include <QObject>
#include <QPointer>
#include <QScrollArea>
#include <QWidget>

class OptionGroupBox;
class QScrollBar;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QGroupBox;
class SolTranslatorCore;
class IOptionPage;

using SolSmoothScrollArea = SolSmoothAbstractScrollArea<QScrollArea>;

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
     * Add a new OptionGroupBox at the end of _mainLayout.
     *
     * @param inGroupTitle GroupBox 타이틀에 사용할 이름
     */
    OptionGroupBox* addNewOptionGroupBox(const QString& inGroupTitle);

public:
    IOptionPage* getOptionPage() const;

private:
    void setOptionPage(IOptionPage* inOptionPage);
    QPointer<IOptionPage> _optionPage;

    QHBoxLayout* _outScrollLayout; // scrollArea와 외부 스크롤바가 위치할 레이아웃
    QScrollBar* _outScrollBar;     // 외부 스크롤바

    SolSmoothScrollArea* _srollArea;
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

    static void allOptionsFinish();
    static std::vector<QPointer<IOptionPage>> sortedOptionsPages();

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
    void setOptionWidgetCtor(Callback<IOptionWidget*(void)>&& inOptionWidgetCtor);
    void setPriority(const int inPriority);

private:
    QPointer<IOptionWidget> _optionWidget;

    QString _displayName;
    QString _iconPath;
    Callback<IOptionWidget*(void)> _optionWidgetCtor;
    int _priority;

    int _optionStkId;

    Q_DISABLE_COPY_MOVE(IOptionPage)
};


#endif //OPTIONWIDGET_H

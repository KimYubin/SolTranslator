//
// Created by YubinKim on 25/04/14 월.
//

#ifndef OPTIONWIDGET_H
#define OPTIONWIDGET_H

#include <QObject>
#include <QPointer>
#include <QWidget>

#include "../FinTypes.h"

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
    virtual void apply();
    virtual void cancel();
    virtual void finish();

public:
    IOptionPage* getOptionPage() const;;

private:
    void setOptionPage(IOptionPage* inOptionPage);
    QPointer<IOptionPage> _optionPage;
};

class IOptionPage : public QObject
{
    Q_OBJECT

public:
    IOptionPage();
    ~IOptionPage() override;

    static const std::unordered_set<IOptionPage*>& allOptionsPages();
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

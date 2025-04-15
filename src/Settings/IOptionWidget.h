//
// Created by YubinKim on 25/04/14 월.
//

#ifndef OPTIONWIDGET_H
#define OPTIONWIDGET_H

#include <QObject>
#include <QPointer>
#include <QWidget>

class FinTranslatorCore;

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

private:
};

class IOptionPage : public QObject
{
    Q_OBJECT

public:
    IOptionPage();
    ~IOptionPage() override;

    static const QList<IOptionPage*> allOptionsPages();

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

private:
    QPointer<IOptionWidget> _optionWidget;
    QString _displayName;
    QString _iconPath;
    std::function<IOptionWidget*()> _optionWidgetCtor;

    Q_DISABLE_COPY_MOVE(IOptionPage)
};


#endif //OPTIONWIDGET_H

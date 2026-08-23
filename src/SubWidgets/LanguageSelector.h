// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef LANGUAGESELECTOR_H
#define LANGUAGESELECTOR_H

#include <QFrame>
#include <QPointer>
#include <QWidget>

class QGridLayout;
class QListWidget;
class QListWidgetItem;
class QPushButton;

namespace Sol
{
enum class LangType;
class LanguageSelectorMenuPrivate;

class LanguageSelector : public QFrame
{
    Q_OBJECT

public:
    explicit LanguageSelector(QWidget* inParent
                            , QWidget* inSizeWidget
                            , QWidget* inReturnFocusWidget
                            , const LangType inLangType);

    ~LanguageSelector() override;

    void setButtonText(const LangType inLangType);

    void setButtonToolTip(const QString& inStr);

    LangType getCurrentLangType() const { return _currentLangType; }

public slots:
    void onSelectedLanguage(const LangType inLangType);

public:
signals:
    void languageSelected(const LangType inLangType);

protected:
    void closeEvent(QCloseEvent* inEvent) override;

private:
    friend class LanguageSelectorMenuPrivate;

    LanguageSelectorMenuPrivate* getMenu();

    QGridLayout* _mainLayout;
    QPushButton* _button;
    LangType _currentLangType;

    QPointer<QWidget> _sizeWidget;
    QPointer<QWidget> _returnFocusWidget;
    QPointer<LanguageSelectorMenuPrivate> _menu;
};


} // namespace Sol

#endif //LANGUAGESELECTOR_H

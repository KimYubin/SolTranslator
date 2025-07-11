//
// Created by YubinKim on 25/07/08 화.
//

#ifndef LANGUAGESELECTOR_H
#define LANGUAGESELECTOR_H

#include <QPointer>
#include <QWidget>

enum class LangType;
class QGridLayout;
class LanguageSelectorMenuPrivate;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class LanguageSelector : public QWidget
{
    Q_OBJECT

public:
    explicit LanguageSelector(QWidget* parent
                            , QWidget* inSizeWidget
                            , const LangType inLangType);
    ~LanguageSelector() override;

    void setButtonText(const LangType inlangType);

public slots:
    void onSelectedLanguage(const LangType inlangType);

public:
signals:
    void languageSelected(const LangType inlangType);

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private:
    friend class LanguageSelectorMenuPrivate;

    LanguageSelectorMenuPrivate* getMenu();

    QGridLayout* _mainLayout;
    QPushButton* _button;

    QPointer<QWidget> _sizeWidget;
    QPointer<LanguageSelectorMenuPrivate> _menu;
};






#endif //LANGUAGESELECTOR_H

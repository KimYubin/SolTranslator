//
// Created by YubinKim on 25/07/08 화.
//

#ifndef SEARCHDROPDOWN_H
#define SEARCHDROPDOWN_H

#include <QPointer>
#include <QWidget>

enum class LangType;
class QGridLayout;
class SearchDropdownMenuPrivate;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class SearchDropdown : public QWidget
{
    Q_OBJECT

public:
    explicit SearchDropdown(QWidget* parent
                          , QWidget* inSizeWidget
                          , const LangType inLangType);
    ~SearchDropdown() override;

    void setButtonText(const LangType inlangType);

public slots:
    void onSelectedLanguage(const LangType inlangType);

public:
signals:
    void languageSelected(const LangType inlangType);

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private:
    friend class SearchDropdownMenuPrivate;

    SearchDropdownMenuPrivate* getMenu();

    QGridLayout* _mainLayout;
    QPushButton* _button;

    QPointer<QWidget> _sizeWidget;
    QPointer<SearchDropdownMenuPrivate> _menu;
};






#endif //SEARCHDROPDOWN_H

//
// Created by YubinKim on 25/07/09 수.
//

#ifndef LANGUAGESELECTOR_H
#define LANGUAGESELECTOR_H


#include <QPointer>
#include <QWidget>

class QStackedWidget;
class QLineEdit;
class QGridLayout;
class LanguageSelectorMenuPrivate;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class LanguageSelector : public QWidget
{
    Q_OBJECT

public:
    explicit LanguageSelector(QWidget* parent, QWidget* inSizeWidget);

    void setButtonText(const QString& text);
private:
    friend class LanguageSelectorMenuPrivate;

    LanguageSelectorMenuPrivate* getMenu();

    void onShowMenuEvent();
    void onHideMenuEvent();

    QGridLayout* _mainLayout;
    QStackedWidget* _buttonStk;
    QPushButton* _button;
    QLineEdit* _searchLine;

    QPointer<QWidget> _sizeWidget;
    QPointer<LanguageSelectorMenuPrivate> _menu;
};




#endif //LANGUAGESELECTOR_H

//
// Created by YubinKim on 25/07/08 화.
//

#ifndef SEARCHDROPDOWN_H
#define SEARCHDROPDOWN_H

#include <QPointer>
#include <QWidget>

class QGridLayout;
class SearchDropdownMenuPrivate;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class SearchDropdown : public QWidget
{
    Q_OBJECT

public:
    explicit SearchDropdown(QWidget* parent);

    void setButtonText(const QString& text);
private:
    SearchDropdownMenuPrivate* getMenu();

    QGridLayout* _mainLayout;
    QPushButton* _button;

    QPointer<SearchDropdownMenuPrivate> _menu;
};






#endif //SEARCHDROPDOWN_H

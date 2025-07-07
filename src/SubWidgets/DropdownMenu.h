//
// Created by YubinKim on 25/06/25 수.
//

#ifndef DROPDOWNMENU_H
#define DROPDOWNMENU_H
#include <QComboBox>


class DropdownMenu : public QComboBox
{
    Q_OBJECT

public:
    explicit DropdownMenu(QWidget* parent = nullptr);


protected:
    virtual void wheelEvent(QWheelEvent *event) override;

private slots:


private:
};


#endif //DROPDOWNMENU_H

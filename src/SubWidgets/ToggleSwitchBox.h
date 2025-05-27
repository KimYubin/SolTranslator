//
// Created by YubinKim on 25/05/27 화.
//

#ifndef TOGGLESWITCHBOX_H
#define TOGGLESWITCHBOX_H
#include <QWidget>


class QGridLayout;
class QLabel;
class SwitchButton;

class ToggleSwitchBox : public QWidget
{
    Q_OBJECT

public:
    explicit ToggleSwitchBox(QWidget* parent = nullptr);
    ~ToggleSwitchBox() override;

    void setHeader(const QString& inStr);
    void setDescription(const QString& inStr);
private:
    QGridLayout* _layout;
    QLabel* _header;
    QLabel* _description;

    SwitchButton* _switchButton;


    QString _headerText;
    std::optional<QString> _descriptionText;
};


#endif //TOGGLESWITCHBOX_H

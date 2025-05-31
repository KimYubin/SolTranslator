//
// Created by YubinKim on 25/05/27 화.
//

#ifndef TOGGLESWITCHBOX_H
#define TOGGLESWITCHBOX_H
#include <QWidget>


class QGridLayout;
class QLabel;
class SwitchButton;

class SettingCard : public QWidget
{
    Q_OBJECT

public:
    enum ContentPos
    {
        Left
      , Right
    };

    explicit SettingCard(QWidget* inContent, QWidget* parent, const ContentPos contentPos = ContentPos::Right);
    ~SettingCard() override;

    /***/
    void setHeader(const QString& inStr);
    void setDescription(const QString& inStr);


private:
    QGridLayout* _layout;
    QWidget* _gridLayoutWidget;
    QLabel* _header;
    std::optional<QLabel*> _description;

    QWidget* _content;

    QString _headerText;
    std::optional<QString> _descriptionText;

    ContentPos _contentPos;
};


#endif //TOGGLESWITCHBOX_H

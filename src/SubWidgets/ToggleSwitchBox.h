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

    /***/
    void setHeader(const QString& inStr);
    void setDescription(const QString& inStr);

    void connectCheckStateChange(QObject* inContext, std::function<void(Qt::CheckState inCheckState)>&& callback);

    void setCheckable(const bool inCheckable);
    bool isCheckable() const;

    void setCheck(const bool inCheck);
    bool isCheck() const;

private:
    QGridLayout* _layout;
    QWidget* _gridLayoutWidget;
    QLabel* _header;
    std::optional<QLabel*> _description;

    SwitchButton* _switchButton;


    QString _headerText;
    std::optional<QString> _descriptionText;
};


#endif //TOGGLESWITCHBOX_H

// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TOGGLESWITCHBOX_H
#define TOGGLESWITCHBOX_H
#include <QFrame>


class QGridLayout;
class QLabel;
class SwitchButton;

class SettingCard : public QFrame
{
    Q_OBJECT

public:
    enum ContentPos
    {
        Left
      , Right
    };

    /** SettingCard는 inContent의 소유권을 갖습니다. */
    explicit SettingCard(QWidget* inContent, QWidget* parent, const ContentPos contentPos = ContentPos::Right);
    ~SettingCard() override;

    /***/
    void setHeader(const QString& inStr);
    void setDescription(const QString& inStr);

    template <std::derived_from<QWidget> T>
    T* getContent() const;

private:
    QGridLayout* _layout;
    QWidget* _gridLayoutWidget;

    QLabel* _header;                     // 기본 설명
    QWidget* _content;                   // 셋팅을 조정하는 위젯
    std::optional<QLabel*> _description; // 상세 설명. 없을 경우 공간을 차지하지 않습니다.

    QString _headerText;
    std::optional<QString> _descriptionText;

    ContentPos _contentPos;
};

template <std::derived_from<QWidget> T>
T* SettingCard::getContent() const
{
    return qobject_cast<T*>(_content);
}


#endif //TOGGLESWITCHBOX_H

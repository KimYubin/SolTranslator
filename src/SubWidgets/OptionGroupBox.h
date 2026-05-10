// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_OPTIONGROUPBOX_H
#define SOLTRANSLATOR_OPTIONGROUPBOX_H
#include <QGroupBox>


class QVBoxLayout;

class OptionGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit OptionGroupBox(const QString& inGroupTitle = "", QWidget* parent = nullptr);

    void addChild(QWidget* inChild);

    QVBoxLayout* vBoxLayout() const { return _vLayout; }

private:
    QVBoxLayout* _vLayout;
};


#endif //SOLTRANSLATOR_OPTIONGROUPBOX_H

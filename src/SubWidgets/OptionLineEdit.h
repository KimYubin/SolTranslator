// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_OPTIONLINEEDIT_H
#define SOLTRANSLATOR_OPTIONLINEEDIT_H
#include "CustomMenuTextEdit.h"
#include "Types/SolTypes.h"

class OptionLineEdit : public MenuLineEdit
{
    Q_OBJECT

public:
    explicit OptionLineEdit(QWidget* parent       = nullptr
                          , const bool inIsSecret = false);

    explicit OptionLineEdit(const QString& inContent
                          , QWidget* parent       = nullptr
                          , const bool inIsSecret = false);
    ~OptionLineEdit() override = default;

    void updatePlaceholderText(const QString& inText);
    void setDefaultTextAndText(const QString& inDefault, const QString& inText);


    void setOptionPlaceholder(const QString& inText);

    void setIsSecret(const bool inIsSecret);

    void setSaveFunctor(Callback<void(const QString&)>&& inSaveFunction);

public slots:
    void saveText();

private:
    QString _defaultText;
    Callback<void(const QString&)> _saveFunctor;

    bool _isSecret = false;
};


#endif //SOLTRANSLATOR_OPTIONLINEEDIT_H

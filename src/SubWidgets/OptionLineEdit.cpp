// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "OptionLineEdit.h"

#include "SolButton.h"
#include "Utils/SolI18n.h"

#include <QHBoxLayout>

OptionLineEdit::OptionLineEdit(QWidget* parent
                             , const bool inIsUsedSaveButton
                             , const bool inIsSecret)
    : OptionLineEdit(QString()
                   , parent
                   , inIsUsedSaveButton
                   , inIsSecret)
{}

OptionLineEdit::OptionLineEdit(const QString& inContent
                             , QWidget* parent
                             , const bool inIsUsedSaveButton
                             , const bool inIsSecret)
    : QWidget(parent)
    , _isUsedSaveButton(inIsUsedSaveButton)
{
    _lineEdit = new SolLineEdit(inContent, this);
    _hLayout  = new QHBoxLayout(this);
    _hLayout->setContentsMargins(0, 0, 0, 0);
    _hLayout->addWidget(_lineEdit);
    setIsSecret(inIsSecret);

    if (_isUsedSaveButton)
    {
        SolButton* saveButton = new SolButton(Sol::i18n(Tr::Save), this);
        _hLayout->addWidget(saveButton);
        connect(saveButton, &SolButton::clicked, this, &OptionLineEdit::saveText);
    }
    else
    {
        connect(_lineEdit, &QLineEdit::textEdited, this, &OptionLineEdit::saveText);
    }
}


void OptionLineEdit::updatePlaceholderText(const QString& inText)
{
    if (_isSecret && _defaultText.isEmpty())
    {
        setOptionPlaceholder(inText);
    }
}

/**
 * - 일반키
 *    - placeholder = 기본값
 *    - text == placeholder
 *      - setText = ""
 *    - text != placeholder
 *      - setText = text
 *
 * - 비밀키
 *    - 기본값 있음 && 키 없음
 *      - placeholder = 기본값
 *    - 기본값 있음 && 키 있음
 *      - placeholder = 마스크된 키
 *    - 기본값 없음
 *      - placeholder = 마스크된 키
 */
void OptionLineEdit::setDefaultTextAndText(const QString& inDefault, const QString& inText)
{
    _defaultText   = inDefault;
    QString phText = _defaultText;
    if (_isSecret)
    {
        if (_defaultText.isEmpty())
        {
            phText = inText;
        }
    }
    else
    {
        if (_defaultText != inText)
        {
            _lineEdit->setText(inText);
        }
    }
    setOptionPlaceholder(phText);
}

void OptionLineEdit::setOptionPlaceholder(const QString& inText)
{
    QString phText = inText;
    if (_isSecret)
    {
        if (inText.size() > 15)
        {
            phText = inText.sliced(0, 3).trimmed() + "..." + inText.last(4).trimmed();
        }
        else if (inText.size() > 0)
        {
            phText = QString{"*******"};
        }
        else if (inText.isEmpty())
        {
            phText = _defaultText;
        }
    }

    _lineEdit->setPlaceholderText(phText);
}

void OptionLineEdit::setIsSecret(const bool inIsSecret)
{
    _isSecret = inIsSecret;
    if (_isSecret)
    {
        _lineEdit->setEchoMode(QLineEdit::Password);
    }
    else
    {
        _lineEdit->setEchoMode(QLineEdit::Normal);
    }
}

void OptionLineEdit::setSaveFunctor(Callback<void(const QString&)>&& inSaveFunction)
{
    _saveFunctor = std::move(inSaveFunction);
}

void OptionLineEdit::saveText()
{
    QString inputText = _lineEdit->text();
    updatePlaceholderText(inputText);

    // Update a secret key placeholder.
    if (_isSecret && _defaultText.isEmpty())
    {
        setOptionPlaceholder(inputText);
    }

    // If it is an empty value, save the default value.
    // To prevent it from loading an empty value instead of the default value.
    // A secret key can be stored as an empty value for the purpose of removal.
    if (!_isSecret && inputText.isEmpty())
    {
        inputText = _defaultText;
    }

    if (_saveFunctor)
    {
        _saveFunctor(inputText);
    }

    if (_isSecret)
    {
        _lineEdit->setText("");
    }
}

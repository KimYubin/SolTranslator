// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "StyleManger.h"

#include <QApplication>
#include <QDir>

#include "Widgets/IFinWidget.h"
#include "FinUtilibrary.h"

#define QSS_PALETTE_COLOR(colorName) \
private: \
    Q_PROPERTY(QColor colorName READ get##colorName WRITE set##colorName) \
    void set##colorName(const QColor& inColor) { _##colorName = inColor; }; \
    QColor get##colorName() const { return _##colorName; }; \
    QColor _##colorName;


class FinPalette : public IFinWidget
{
    Q_OBJECT

public:
    explicit FinPalette(QWidget* parent = nullptr);
    ~FinPalette() override;

    void applyThemePrivate(const QString& inThemeName = "dark");

    QString applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors);

    void updatePaletteColor() const;

private:
    QSS_PALETTE_COLOR(windowColor);
    QSS_PALETTE_COLOR(windowTextColor);
    QSS_PALETTE_COLOR(baseColor);
    QSS_PALETTE_COLOR(textColor);
    QSS_PALETTE_COLOR(buttonColor);
    QSS_PALETTE_COLOR(buttonTextColor);
    QSS_PALETTE_COLOR(highlightColor);
    QSS_PALETTE_COLOR(highlightedTextColor);
    QSS_PALETTE_COLOR(linkColor);

    // focus out
    QSS_PALETTE_COLOR(inact_highlightColor);
    QSS_PALETTE_COLOR(inact_highlightedTextColor);

    // 비활성화
    QSS_PALETTE_COLOR(disableColor);
};

#include "StyleManger.moc"

FinPalette::FinPalette(QWidget* parent): IFinWidget(parent)
{
    _windowColor          = QColor(53, 53, 53);
    _windowTextColor      = Qt::white;
    _baseColor            = QColor(42, 42, 42);
    _textColor            = QColor(250, 250, 250, 237);
    _buttonColor          = QColor(53, 53, 53);
    _buttonTextColor      = QColor(250, 250, 250, 237);
    _highlightColor       = QColor(36, 91, 130, 211);
    _highlightedTextColor = QColor(250, 250, 250, 237);
    _linkColor            = QColor(107, 167, 247);

    _inact_highlightColor       = QColor(146, 146, 146, 145);
    _inact_highlightedTextColor = QColor(250, 250, 250, 237);

    _disableColor = QColor(76, 76, 76);
}

FinPalette::~FinPalette() {
}

StyleManger::StyleManger(QObject* parent) : QObject(parent)
{
}

StyleManger::~StyleManger() {
}

void StyleManger::applyTheme(const QString& inThemeName)
{
    FinPalette finManger;

    finManger.applyThemePrivate(inThemeName);
}

void FinPalette::applyThemePrivate(const QString& inThemeName)
{
    // hide 상태에서도 qss의 qproperty 항목 로드를 보장하기 위해 스타일 적용 전 호출.
    ensurePolished();

    QString rtPrefixPath = "../resource/theme/" + inThemeName;
    const QDir rtThemeDir(rtPrefixPath, {"*.qss"}, QDir::Name, QDir::Files);
    if (rtThemeDir.exists() == false)
    {
        qDebug() << "no rt theme" << rtPrefixPath;
    }
    QStringList rtFiles = rtThemeDir.entryList();


    QString qrcPrefixPath = ":/theme/" + inThemeName;
    const QDir qrcThemeDir(qrcPrefixPath, {"*.qss"}, QDir::Name, QDir::Files);
    if (qrcThemeDir.exists() == false)
    {
        qWarning() << "qrc theme path is not valid" << qrcPrefixPath;
    }
    QStringList qrcFiles = qrcThemeDir.entryList();


    // 런타임 테마가 있다면 해당 테마 우선 사용.
    QString prefixPath;
    QStringList sheetFileList;
    if (qrcFiles == rtFiles)
    {
        prefixPath    = std::move(rtPrefixPath);
        sheetFileList = std::move(rtFiles);
    }
    else
    {
        qDebug() << "rt theme list is different from the existing theme list." << rtPrefixPath;
        prefixPath    = std::move(qrcPrefixPath);
        sheetFileList = std::move(qrcFiles);
    }

    QString newStyleSheet;
    for (const QString& sheetFileName : sheetFileList)
    {
        QFile file(prefixPath + "/" + sheetFileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            newStyleSheet += stream.readAll() + "\n";
            file.close();
        }
    }
    if (newStyleSheet.isEmpty() == false)
    {
        qApp->setStyleSheet(newStyleSheet);
        updatePaletteColor();

        Fin::noHintingFont();

        QWidgetList allWidgetList = qApp->allWidgets();
        for (QWidget* childWidget : allWidgetList)
        {
            childWidget->update();
        }
    }
}

QString FinPalette::applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors)
{
    QString res = templateTheme;
    for (const auto& [colorName, colorValue] : colors)
    {
        res.replace("${" + colorName + "}", colorValue);
    }
    return res;
}

void FinPalette::updatePaletteColor() const
{
    QPalette qPalette;

    qPalette.setColor(QPalette::Window,           _windowColor);
    qPalette.setColor(QPalette::WindowText,       _windowTextColor);
    qPalette.setColor(QPalette::Base,             _baseColor);
    qPalette.setColor(QPalette::Text,             _textColor);
    qPalette.setColor(QPalette::Button,           _buttonColor);
    qPalette.setColor(QPalette::ButtonText,       _buttonTextColor);
    qPalette.setColor(QPalette::Highlight,        _highlightColor);
    qPalette.setColor(QPalette::HighlightedText,  _highlightedTextColor);
    qPalette.setColor(QPalette::Link,             _linkColor);

    qPalette.setColor(QPalette::Inactive, QPalette::Highlight, _inact_highlightColor);
    qPalette.setColor(QPalette::Inactive, QPalette::HighlightedText, _inact_highlightedTextColor);

    qPalette.setColor(QPalette::Disabled, QPalette::Window, _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::WindowText, _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::Base, _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::Text, _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::Button, _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::ButtonText, _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::Highlight, _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, _disableColor);

    QApplication::setPalette(qPalette);
}

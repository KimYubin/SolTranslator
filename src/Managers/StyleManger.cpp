// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "StyleManger.h"

#include "Utils/SolLog.h"
#include "Utils/SolUtilibrary.h"
#include "Widgets/ISolWidget.h"

#include <QApplication>
#include <QDir>

class SolPalette
{
public:
    SolPalette();
    ~SolPalette();

    QString _themeName;
};

class SolPaletteWidget : public ISolWidget
{
    Q_OBJECT

public:
    explicit SolPaletteWidget(QWidget* parent = nullptr);
    ~SolPaletteWidget() override;

    void applyThemePrivate(const QString& inThemeName = "dark");

    QString applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors);

    void updatePaletteColor() const;

private:
    SOL_QSS_COLOR(windowColor);
    SOL_QSS_COLOR(windowTextColor);
    SOL_QSS_COLOR(baseColor);
    SOL_QSS_COLOR(textColor);
    SOL_QSS_COLOR(buttonColor);
    SOL_QSS_COLOR(buttonTextColor);
    SOL_QSS_COLOR(highlightColor);
    SOL_QSS_COLOR(highlightedTextColor);
    SOL_QSS_COLOR(linkColor);

    // focus out
    SOL_QSS_COLOR(inact_highlightColor);
    SOL_QSS_COLOR(inact_highlightedTextColor);

    // 비활성화
    SOL_QSS_COLOR(disableColor);

};

#include "StyleManger.moc"

SolPaletteWidget::SolPaletteWidget(QWidget* parent): ISolWidget(parent)
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

SolPaletteWidget::~SolPaletteWidget()
{}


StyleManger::StyleManger(QObject* parent)
    : QObject(parent)
    , _solPalette(nullptr)
{}

StyleManger::~StyleManger()
{}

void StyleManger::applyTheme(const QString& inThemeName)
{
    SolPaletteWidget solPaletteWidget;

    solPaletteWidget.applyThemePrivate(inThemeName);
}

void SolPaletteWidget::applyThemePrivate(const QString& inThemeName)
{
    // hide 상태에서도 qss의 qproperty 항목 로드를 보장하기 위해 스타일 적용 전 호출.
    ensurePolished();

    QString rtPrefixPath = "../resource/theme/" + inThemeName;
    const QDir rtThemeDir(rtPrefixPath, {"*.qss"}, QDir::Name, QDir::Files);
    if (rtThemeDir.exists() == false)
    {
        solDebug << "no rt theme" << rtPrefixPath;
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
        solDebug << "rt theme list is different from the existing theme list. apply qrc version." << rtPrefixPath;
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

        Sol::noHintingFont();

        QWidgetList allWidgetList = qApp->allWidgets();
        for (QWidget* childWidget : allWidgetList)
        {
            childWidget->update();
        }
    }
}

QString SolPaletteWidget::applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors)
{
    QString res = templateTheme;
    for (const auto& [colorName, colorValue] : colors)
    {
        res.replace("${" + colorName + "}", colorValue);
    }
    return res;
}

void SolPaletteWidget::updatePaletteColor() const
{
    QPalette qPalette;

    qPalette.setColor(QPalette::Window,          _windowColor);
    qPalette.setColor(QPalette::WindowText,      _windowTextColor);
    qPalette.setColor(QPalette::Base,            _baseColor);
    qPalette.setColor(QPalette::Text,            _textColor);
    qPalette.setColor(QPalette::Button,          _buttonColor);
    qPalette.setColor(QPalette::ButtonText,      _buttonTextColor);
    qPalette.setColor(QPalette::Highlight,       _highlightColor);
    qPalette.setColor(QPalette::HighlightedText, _highlightedTextColor);
    qPalette.setColor(QPalette::Link,            _linkColor);

    qPalette.setColor(QPalette::Inactive, QPalette::Highlight,       _inact_highlightColor);
    qPalette.setColor(QPalette::Inactive, QPalette::HighlightedText, _inact_highlightedTextColor);

    qPalette.setColor(QPalette::Disabled, QPalette::Window,          _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::WindowText,      _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::Base,            _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::Text,            _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::Button,          _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::ButtonText,      _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::Highlight,       _disableColor);
    qPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, _disableColor);

    QApplication::setPalette(qPalette);
}

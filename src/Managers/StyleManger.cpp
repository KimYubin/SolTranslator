// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "StyleManger.h"

#include <QApplication>
#include <QDir>

#include "Widgets/IFinWidget.h"
#include "FinUtilibrary.h"

class FinStyleWidget  : public IFinWidget
{
    Q_OBJECT

public:
    explicit FinStyleWidget(QWidget* parent = nullptr);
    ~FinStyleWidget() override;

    void applyThemePrivate(const QString& inThemeName = "dark");

    QString applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors);

    void updatePaletteColor() const;

private:
    Q_PROPERTY(QColor windowColor READ getWindowColor WRITE setWindowColor)
    Q_PROPERTY(QColor windowTextColor READ getWindowTextColor WRITE setWindowTextColor)
    Q_PROPERTY(QColor baseColor READ getBaseColor WRITE setBaseColor)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
    Q_PROPERTY(QColor buttonColor READ getButtonColor WRITE setButtonColor)
    Q_PROPERTY(QColor buttonTextColor READ getButtonTextColor WRITE setButtonTextColor)
    Q_PROPERTY(QColor highlightColor READ getHighlightColor WRITE setHighlightColor)
    Q_PROPERTY(QColor highlightedTextColor READ getHighlightedTextColor WRITE setHighlightedTextColor)
    Q_PROPERTY(QColor linkColor READ getLinkColor WRITE setLinkColor)
    Q_PROPERTY(QColor disableColor READ getDisableColor WRITE setDisableColor)

    void setWindowColor(const QColor& inColor);
    void setWindowTextColor(const QColor& inColor);
    void setBaseColor(const QColor& inColor);
    void setTextColor(const QColor& inColor);
    void setButtonColor(const QColor& inColor);
    void setButtonTextColor(const QColor& inColor);
    void setHighlightColor(const QColor& inColor);
    void setHighlightedTextColor(const QColor& inColor);
    void setLinkColor(const QColor& inColor);
    void setDisableColor(const QColor& inColor);

    QColor getWindowColor() const;
    QColor getWindowTextColor() const;
    QColor getBaseColor() const;
    QColor getTextColor() const;
    QColor getButtonColor() const;
    QColor getButtonTextColor() const;
    QColor getHighlightColor() const;
    QColor getHighlightedTextColor() const;
    QColor getLinkColor() const;
    QColor getDisableColor() const;
    // Color

    QColor _windowColor;
    QColor _windowTextColor;
    QColor _baseColor;
    QColor _textColor;
    QColor _buttonColor;
    QColor _buttonTextColor;
    QColor _highlightColor;
    QColor _highlightedTextColor;
    QColor _linkColor;
    QColor _disableColor;
    
};

#include "StyleManger.moc"

FinStyleWidget::FinStyleWidget(QWidget* parent): IFinWidget(parent)
{
    _windowColor          = QColor(53, 53, 53);
    _windowTextColor      = Qt::white;
    _baseColor            = QColor(42, 42, 42);
    _textColor            = Qt::white;
    _buttonColor          = QColor(53, 53, 53);
    _buttonTextColor      = Qt::white;
    _highlightColor       = QColor(142, 45, 197).lighter();
    _highlightedTextColor = Qt::black;
    _linkColor            = QColor(0x6ba7f7);
    _disableColor         = QColor(76, 76, 76);
}

FinStyleWidget::~FinStyleWidget() {
}

StyleManger::StyleManger(QObject* parent) : QObject(parent)
{
}

StyleManger::~StyleManger() {
}

void StyleManger::applyTheme(const QString& inThemeName)
{
    FinStyleWidget finManger;

    finManger.applyThemePrivate(inThemeName);
}

void FinStyleWidget::applyThemePrivate(const QString& inThemeName)
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

QString FinStyleWidget::applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors)
{
    QString res = templateTheme;
    for (const auto& [colorName, colorValue] : colors)
    {
        res.replace("${" + colorName + "}", colorValue);
    }
    return res;
}

void FinStyleWidget::updatePaletteColor() const
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

void FinStyleWidget::setWindowColor(const QColor& inColor) { _windowColor = inColor; }
void FinStyleWidget::setWindowTextColor(const QColor& inColor) { _windowTextColor = inColor; }
void FinStyleWidget::setBaseColor(const QColor& inColor) { _baseColor = inColor; }
void FinStyleWidget::setTextColor(const QColor& inColor) { _textColor = inColor; }
void FinStyleWidget::setButtonColor(const QColor& inColor) { _buttonColor = inColor; }
void FinStyleWidget::setButtonTextColor(const QColor& inColor) { _buttonTextColor = inColor; }
void FinStyleWidget::setHighlightColor(const QColor& inColor) { _highlightColor = inColor; }
void FinStyleWidget::setHighlightedTextColor(const QColor& inColor) { _highlightedTextColor = inColor; }
void FinStyleWidget::setLinkColor(const QColor& inColor) { _linkColor = inColor; }
void FinStyleWidget::setDisableColor(const QColor& inColor) { _disableColor = inColor; }

QColor FinStyleWidget::getWindowColor() const { return _windowColor; }
QColor FinStyleWidget::getWindowTextColor() const { return _windowTextColor; }
QColor FinStyleWidget::getBaseColor() const { return _baseColor; }
QColor FinStyleWidget::getTextColor() const { return _textColor; }
QColor FinStyleWidget::getButtonColor() const { return _buttonColor; }
QColor FinStyleWidget::getButtonTextColor() const { return _buttonTextColor; }
QColor FinStyleWidget::getHighlightColor() const { return _highlightColor; }
QColor FinStyleWidget::getHighlightedTextColor() const { return _highlightedTextColor; }
QColor FinStyleWidget::getLinkColor() const { return _linkColor; }
QColor FinStyleWidget::getDisableColor() const { return _disableColor; }

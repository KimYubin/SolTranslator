//
// Created by YubinKim on 25/07/09 수.
//

#include "LanguageSelector.h"


#include <QApplication>
#include <QCoreApplication>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

/** LanguageSelector에서 사용하는 메뉴 */
class LanguageSelectorMenuPrivate : public QWidget
{
    Q_OBJECT

public:
    explicit LanguageSelectorMenuPrivate(LanguageSelector* parent);

    virtual QSize sizeHint() const override;

    void showMenuPopup();
    void hideMenuPopup();

    QString selectedLanguage() const;

protected:
    virtual void showEvent(QShowEvent* event) override;
    virtual void hideEvent(QHideEvent *event) override;


public:
signals:
    void itemSelected(const QString& inItem);

private:
    void filterItems(const QString& inText);
    void onItemClicked(QListWidgetItem* inItem);

    QPoint getTargetGlobalPos() const;
    QSize getTargetSize() const;

    QListWidget* _listWidget;

    QString _currentItem;
    QStringList _allDataList;

    QPointer<LanguageSelector> _parentSD;
    QPointer<QLineEdit> _parentSearchLine;
    QPointer<QWidget> _sizeWidget;
};


#include "LanguageSelector.moc"


LanguageSelector::LanguageSelector(QWidget* parent, QWidget* inSizeWidget)
    : QWidget(parent), _sizeWidget(inSizeWidget)
{
    _mainLayout = new QGridLayout(this);
    _mainLayout->setObjectName("mainLayout");
    _mainLayout->setSpacing(0);
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(_mainLayout);

    _buttonStk = new QStackedWidget(this);
    _mainLayout->addWidget(_buttonStk, 0, 0, Qt::AlignLeft | Qt::AlignCenter);

    _button = new QPushButton(this);
    _buttonStk->addWidget(_button);

    _searchLine = new QLineEdit(this);
    _searchLine->setPlaceholderText(tr("언어 검색"));
    _buttonStk->addWidget(_searchLine);


    connect(_button, &QPushButton::clicked, this, &LanguageSelector::onShowMenuEvent);
}

void LanguageSelector::setButtonText(const QString& text)
{
    _button->setText(text);
}

LanguageSelectorMenuPrivate* LanguageSelector::getMenu()
{
    if (_menu.isNull())
    {
        _menu = new LanguageSelectorMenuPrivate(this);
    }

    return _menu;
}

void LanguageSelector::onShowMenuEvent()
{
    _buttonStk->setCurrentWidget(_searchLine);

    getMenu()->showMenuPopup();
    _searchLine->setFocus(Qt::TabFocusReason);
}

void LanguageSelector::onHideMenuEvent()
{
    _searchLine->setText("");
    _buttonStk->setCurrentWidget(_button);
}


// ~=====================================
// LanguageSelectorMenuPrivate

LanguageSelectorMenuPrivate::LanguageSelectorMenuPrivate(LanguageSelector* parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
    , _parentSD(parent)
    , _parentSearchLine(parent->_searchLine)
    , _sizeWidget(parent->_sizeWidget)
{
    Q_ASSERT(parent);
    Q_ASSERT(_parentSD);
    Q_ASSERT(_parentSearchLine);
    Q_ASSERT(_sizeWidget);

    setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    _listWidget = new QListWidget(this);
    layout->addWidget(_listWidget);

    _allDataList = QStringList{
        "한국어", "영어", "중국어", "일본어", "프랑스어", "독일어", "스페인어",
    };

    _listWidget->addItems(_allDataList);

    // setText()에 반응하지 않도록, textChanged 대신 사용.
    connect(_parentSearchLine, &QLineEdit::textEdited, this, &LanguageSelectorMenuPrivate::filterItems);
    connect(_listWidget, &QListWidget::itemClicked, this, &LanguageSelectorMenuPrivate::onItemClicked);

}

QSize LanguageSelectorMenuPrivate::sizeHint() const
{
    return getTargetSize();
}

void LanguageSelectorMenuPrivate::showMenuPopup()
{
    move(getTargetGlobalPos());
    show();
}

void LanguageSelectorMenuPrivate::hideMenuPopup() 
{
    _parentSD->onHideMenuEvent();
}

QString LanguageSelectorMenuPrivate::selectedLanguage() const
{
    return _currentItem;
}

void LanguageSelectorMenuPrivate::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    // activateWindow();
    // setFocus();
}

void LanguageSelectorMenuPrivate::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    hideMenuPopup();
}

void LanguageSelectorMenuPrivate::filterItems(const QString& inText)
{
    _listWidget->clear();
    for (const QString& lang : _allDataList)
    {
        if (lang.contains(inText, Qt::CaseInsensitive))
        {
            _listWidget->addItem(lang);
        }
    }
}

void LanguageSelectorMenuPrivate::onItemClicked(QListWidgetItem* inItem)
{
    _currentItem = inItem->text();
    emit itemSelected(_currentItem);
    _parentSD->setButtonText(_currentItem);
    hide();
}

QPoint LanguageSelectorMenuPrivate::getTargetGlobalPos() const
{
    if (_sizeWidget.isNull())
    {
        qDebug() << "_sizeWidget is not valid.";
        return QPoint(0, 0);
    }

    const QPoint local = _sizeWidget->rect().topLeft();
    const QPoint pGlobalPos = _sizeWidget->mapToGlobal(local);

    return pGlobalPos;
}

QSize LanguageSelectorMenuPrivate::getTargetSize() const
{
    if (_sizeWidget.isNull())
    {
        qDebug() << "_sizeWidget is not valid.";
        return QSize(0, 0);
    }
    
    return _sizeWidget->size();
}



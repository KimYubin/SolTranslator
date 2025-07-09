//
// Created by YubinKim on 25/07/08 화.
//

#include "SearchDropdown.h"

#include <QApplication>
#include <QCoreApplication>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include <qevent.h>

/** SearchDropdown에서 사용하는 메뉴 */
class SearchDropdownMenuPrivate : public QWidget
{
    Q_OBJECT

public:
    explicit SearchDropdownMenuPrivate(SearchDropdown* searchDropdown, QWidget* parentWidget);

    virtual QSize sizeHint() const override;

    void showMenuPopup();

    QString selectedLanguage() const;

    void detectFocusInOut(QWidget* old, QWidget* now);

protected:
    virtual void focusOutEvent(QFocusEvent* event) override;
    
public:
signals:
    void itemSelected(const QString& inItem);

private:
    void filterItems(const QString& inText);
    void onItemClicked(QListWidgetItem* inItem);

    QPoint getTargetGlobalPos() const;
    QSize getTargetSize() const;

    QLineEdit* _searchLine;
    QListWidget* _listWidget;

    QString _currentItem;
    QStringList _allDataList;

    QPointer<SearchDropdown> _searchDropdown;
    QPointer<QWidget> _sizeWidget;
};


#include "SearchDropdown.moc"


SearchDropdown::SearchDropdown(QWidget* parent, QWidget* inSizeWidget)
    : QWidget(parent), _sizeWidget(inSizeWidget)
{
    // resize(300, 400);

    _mainLayout = new QGridLayout(this);
    _mainLayout->setObjectName("mainLayout");
    _mainLayout->setSpacing(0);
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(_mainLayout);

    _button = new QPushButton(this);
    _mainLayout->addWidget(_button, 0, 0, Qt::AlignLeft);


    connect(_button, &QPushButton::clicked, this, [this]()
    {
        if (getMenu()->isVisible() == false)
        {
            getMenu()->showMenuPopup();
        }
    });

    connect(this, &QObject::destroyed, this, [this]()
    {
        if (_menu)
        {
            _menu->deleteLater();
        }
    });
    
}

void SearchDropdown::setButtonText(const QString& text)
{
    _button->setText(text);
}

void SearchDropdown::closeEvent(QCloseEvent* event)
{
    if (_menu)
    {
        _menu->close();
    }

    QWidget::closeEvent(event);
}

SearchDropdownMenuPrivate* SearchDropdown::getMenu()
{
    if (_menu.isNull())
    {
        _menu = new SearchDropdownMenuPrivate(this, parentWidget());
    }

    return _menu;
}


// ~=====================================
// SearchDropdownMenuPrivate

SearchDropdownMenuPrivate::SearchDropdownMenuPrivate(SearchDropdown* searchDropdown, QWidget* parentWidget)
    : QWidget(parentWidget)
    , _searchDropdown(searchDropdown)
    , _sizeWidget(searchDropdown->_sizeWidget)
{
    Q_ASSERT(_searchDropdown);

    setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    _searchLine = new QLineEdit(this);
    layout->addWidget(_searchLine);
    _searchLine->setAttribute(Qt::WA_InputMethodEnabled, true);

    _searchLine->setPlaceholderText(tr("언어 검색"));

    _listWidget = new QListWidget(this);
    layout->addWidget(_listWidget);

    _allDataList = QStringList{
        "한국어", "영어", "중국어", "일본어", "프랑스어", "독일어", "스페인어",
    };

    _listWidget->addItems(_allDataList);

    connect(_searchLine, &QLineEdit::textChanged, this, &SearchDropdownMenuPrivate::filterItems);
    connect(_listWidget, &QListWidget::itemClicked, this, &SearchDropdownMenuPrivate::onItemClicked);

    connect(this, &SearchDropdownMenuPrivate::itemSelected, this, [this](const QString& lang)
    {
        if (_searchDropdown)
        {
            _searchDropdown->setButtonText(lang);
        }
    });

    connect(qApp, &QApplication::focusChanged, this, &SearchDropdownMenuPrivate::detectFocusInOut);
}

QSize SearchDropdownMenuPrivate::sizeHint() const
{
    return getTargetSize();
}

void SearchDropdownMenuPrivate::showMenuPopup()
{
    resize(getTargetSize());
    move(getTargetGlobalPos());
    show();
    raise();
    setFocus();
}

QString SearchDropdownMenuPrivate::selectedLanguage() const
{
    return _currentItem;
}

void SearchDropdownMenuPrivate::detectFocusInOut(QWidget* old, QWidget* now)
{
    // 위젯과 그 부모가 this인지 재귀적으로 확인합니다.
    auto isThis = [this](QWidget* inWidget)
    {
        bool bIsWidgetThis = false;
        QObject* parentObj = inWidget;
        while (parentObj != nullptr)
        {
            if (parentObj == this)
            {
                bIsWidgetThis = true;
                break;
            }
            parentObj = parentObj->parent();
        }
        return bIsWidgetThis;
    };

    if (isThis(old) && (isThis(now) == false))
    {
        hide();
    }
}

void SearchDropdownMenuPrivate::focusOutEvent(QFocusEvent* event)
{
    QWidget::focusOutEvent(event);
}

void SearchDropdownMenuPrivate::filterItems(const QString& inText)
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

void SearchDropdownMenuPrivate::onItemClicked(QListWidgetItem* inItem)
{
    _currentItem = inItem->text();
    emit itemSelected(_currentItem);
    hide();
}

QPoint SearchDropdownMenuPrivate::getTargetGlobalPos() const
{
    if (_sizeWidget.isNull())
    {
        qDebug() << "_sizeWidget is not valid.";
        return QPoint(0, 0);
    }

    const QPoint local = _sizeWidget->rect().topLeft();
    const QPoint pGlobalPos = _sizeWidget->mapToGlobal(local);
    

    return _sizeWidget->pos();
}

QSize SearchDropdownMenuPrivate::getTargetSize() const
{
    if (_sizeWidget.isNull())
    {
        qDebug() << "_sizeWidget is not valid.";
        return QSize(0, 0);
    }
    
    return _sizeWidget->size();
}



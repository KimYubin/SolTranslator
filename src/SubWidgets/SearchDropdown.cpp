//
// Created by YubinKim on 25/07/08 화.
//

#include "SearchDropdown.h"

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
    explicit SearchDropdownMenuPrivate(SearchDropdown* parent);

    virtual QSize sizeHint() const override;

    void showMenuPopup();

    QString selectedLanguage() const;

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
        if (getMenu()->isVisible())
        {
            getMenu()->hide();
            return;
        }
        getMenu()->showMenuPopup();
    });
    
}

void SearchDropdown::setButtonText(const QString& text)
{
    _button->setText(text);
}

SearchDropdownMenuPrivate* SearchDropdown::getMenu()
{
    if (_menu.isNull())
    {
        _menu = new SearchDropdownMenuPrivate(this);
    }

    return _menu;
}


// ~=====================================
// SearchDropdownMenuPrivate

SearchDropdownMenuPrivate::SearchDropdownMenuPrivate(SearchDropdown* parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
    , _sizeWidget(parent->_sizeWidget)
{
    Q_ASSERT(parent);

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

    connect(this, &SearchDropdownMenuPrivate::itemSelected, this, [this, parent](const QString& lang)
    {
        parent->setButtonText(lang);
    });
}

QSize SearchDropdownMenuPrivate::sizeHint() const
{
    return getTargetSize();
}

void SearchDropdownMenuPrivate::showMenuPopup()
{
    move(getTargetGlobalPos());
    show();
    raise();
}

QString SearchDropdownMenuPrivate::selectedLanguage() const
{
    return _currentItem;
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

    return pGlobalPos;
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



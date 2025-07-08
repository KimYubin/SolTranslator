//
// Created by YubinKim on 25/07/08 화.
//

#include "SearchDropdown.h"

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

/** SearchDropdown에서 사용하는 메뉴 */
class SearchDropdownMenuPrivate : public QWidget
{
    Q_OBJECT

public:
    explicit SearchDropdownMenuPrivate(SearchDropdown* parent, Qt::WindowFlags flags = Qt::Popup);

    void showMenuPopup();

    QString selectedLanguage() const;

public:
signals:
    void itemSelected(const QString& inItem);

private:
    void filterItems(const QString& inText);
    void onItemClicked(QListWidgetItem* inItem);

    QLineEdit* _searchLine;
    QListWidget* _listWidget;

    QString _currentItem;
    QStringList _allDataList;
};


#include "SearchDropdown.moc"


SearchDropdown::SearchDropdown(QWidget* parent)
    : QWidget(parent)
{
    resize(300, 400);

    _mainLayout = new QGridLayout(this);
    _mainLayout->setObjectName("mainLayout");
    _mainLayout->setSpacing(0);
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(_mainLayout);

    _button = new QPushButton(this);
    _mainLayout->addWidget(_button);
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

SearchDropdownMenuPrivate::SearchDropdownMenuPrivate(SearchDropdown* parent, const Qt::WindowFlags flags): QWidget(parent, flags)
{
    Q_ASSERT(parent);

    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);

    _searchLine = new QLineEdit(this);
    _searchLine->setPlaceholderText("언어 검색...");

    _listWidget = new QListWidget(this);

    layout->addWidget(_searchLine);
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

void SearchDropdownMenuPrivate::showMenuPopup()
{
    const QWidget* pWidget  = parentWidget();
    const QPoint pGlobalPos = pWidget->mapToGlobal(QPoint(0, pWidget->height()));
    move(pGlobalPos);
    // resize(pWidget->width() * 3, 300); // 적절한 크기로 조정
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


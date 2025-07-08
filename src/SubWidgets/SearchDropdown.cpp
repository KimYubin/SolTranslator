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
    void languageSelected(const QString& language);

private:
    void filterLanguages(const QString& text);
    void onItemClicked(QListWidgetItem* item);

    QLineEdit* _searchBox;
    QListWidget* _languageList;

    QString _currentSelection;
    QStringList _allLanguages;
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
    _searchBox          = new QLineEdit(this);
    _searchBox->setPlaceholderText("언어 검색...");

    _languageList = new QListWidget(this);

    layout->addWidget(_searchBox);
    layout->addWidget(_languageList);

    _allLanguages = QStringList{
        "한국어", "영어", "중국어", "일본어", "프랑스어", "독일어", "스페인어",
    };

    _languageList->addItems(_allLanguages);

    connect(_searchBox, &QLineEdit::textChanged, this, &SearchDropdownMenuPrivate::filterLanguages);
    connect(_languageList, &QListWidget::itemClicked, this, &SearchDropdownMenuPrivate::onItemClicked);

    connect(this, &SearchDropdownMenuPrivate::languageSelected, this, [this, parent](const QString& lang)
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
    return _currentSelection;
}

void SearchDropdownMenuPrivate::filterLanguages(const QString& text)
{
    _languageList->clear();
    for (const QString& lang : _allLanguages)
    {
        if (lang.contains(text, Qt::CaseInsensitive))
        {
            _languageList->addItem(lang);
        }
    }
}

void SearchDropdownMenuPrivate::onItemClicked(QListWidgetItem* item)
{
    _currentSelection = item->text();
    emit languageSelected(_currentSelection);
    hide();
}


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

#include "FinUtilibrary.h"

/** SearchDropdown에서 사용하는 메뉴 */
class SearchDropdownMenuPrivate : public QWidget
{
    Q_OBJECT

public:
    explicit SearchDropdownMenuPrivate(SearchDropdown* searchDropdown, QWidget* parentWidget);
    ~SearchDropdownMenuPrivate() override;

    virtual QSize sizeHint() const override;

    void showMenuPopup();

    QString selectedLanguage() const;

protected:
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

public:
signals:
    void itemSelected(const QString& inItem);

private:
    void filterItems(const QString& inText);
    void onItemClicked(QListWidgetItem* inItem);

    QPoint getTargetRelPos() const;
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
        else
        {
            getMenu()->close();
        }
    });

}

SearchDropdown::~SearchDropdown()
{
    if (_menu)
    {
        _menu->deleteLater();
    }
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
}

SearchDropdownMenuPrivate::~SearchDropdownMenuPrivate()
{
    qApp->removeEventFilter(this);
}

QSize SearchDropdownMenuPrivate::sizeHint() const
{
    return getTargetSize();
}

void SearchDropdownMenuPrivate::showMenuPopup()
{
    qApp->installEventFilter(this);
    resize(getTargetSize());
    move(getTargetRelPos());
    show();
    raise();
    setFocus();
}

QString SearchDropdownMenuPrivate::selectedLanguage() const
{
    return _currentItem;
}

bool SearchDropdownMenuPrivate::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress
        || event->type() == QEvent::NonClientAreaMouseButtonPress)
    {
        if (Fin::isThis(this, obj) == false)
        {
            const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            const QPoint mouseGlobalPos   = mouseEvent->globalPosition().toPoint();

            // 마우스가 메뉴 위에 있는지 확인
            const QPoint menuGlobalPos     = mapToGlobal(QPoint(0, 0));
            const QRect menuGlobalRect     = QRect(menuGlobalPos, size());
            const bool bIsMenuContainMouse = menuGlobalRect.contains(mouseGlobalPos);

            // 마우스가 드롭다운 버튼 위에 있는지 확인
            bool bIsButtonContainMouse = false;
            if (const QPushButton* dropButton = _searchDropdown ? _searchDropdown->_button : nullptr)
            {
                const QPoint buttonGlobalPos = dropButton->mapToGlobal(QPoint(0, 0));
                const QRect buttonGlobalRect = QRect(buttonGlobalPos, dropButton->size());

                bIsButtonContainMouse = buttonGlobalRect.contains(mouseGlobalPos);
            }

            // 마우스 위치가 외부라면 닫음. Popup 행동
            if (bIsMenuContainMouse == false && bIsButtonContainMouse == false)
            {
                qApp->removeEventFilter(this);
                close();
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void SearchDropdownMenuPrivate::filterItems(const QString& inText)
{
    _listWidget->clear();
    QStringList filteredList;
    for (const QString& lang : _allDataList)
    {
        if (lang.contains(inText, Qt::CaseInsensitive))
        {
            filteredList.append(lang);
        }
    }

    _listWidget->addItems(filteredList);
}

void SearchDropdownMenuPrivate::onItemClicked(QListWidgetItem* inItem)
{
    _currentItem = inItem->text();
    emit itemSelected(_currentItem);
    hide();
}

QPoint SearchDropdownMenuPrivate::getTargetRelPos() const
{
    if (_sizeWidget.isNull())
    {
        qDebug() << "_sizeWidget is not valid.";
        return QPoint(0, 0);
    }

    return _sizeWidget->pos();
}

QSize SearchDropdownMenuPrivate::getTargetSize() const
{
    if (_sizeWidget.isNull())
    {
        qDebug() << "_sizeWidget is not valid.";
        return QSize(200, 300);
    }

    return _sizeWidget->size();
}



// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "LanguageSelector.h"

#include <QApplication>
#include <QCoreApplication>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include <qevent.h>

#include "FinToolTip.h"
#include "FinTypes.h"
#include "FinUtilibrary.h"
#include "KeySelectionList.h"

#include "Managers/ConfigManager.h"

enum
{
    LangTypeRole = Qt::ItemDataRole::UserRole + 1
  , 
};

/** LanguageSelector에서 사용하는 메뉴 */
class LanguageSelectorMenuPrivate : public QWidget
{
    Q_OBJECT

public:
    explicit LanguageSelectorMenuPrivate(LanguageSelector* inLangSelector, QWidget* inParentWidget);
    ~LanguageSelectorMenuPrivate() override;

    virtual QSize sizeHint() const override;

    void showMenuPopup();
    void closeMenuPopup();

protected:
    virtual bool eventFilter(QObject* obj, QEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;

public:
signals:
    void itemSelected(const LangType inLangType);

private:
    void filterItems(const QString& inText);
    void onItemClicked(QListWidgetItem* inItem);
    void addListItem(const LangType& inLangType);

    QPoint getTargetRelPos() const;
    QSize getTargetSize() const;

    QLineEdit* _searchLine;
    KeySelectionList* _listWidget;

    std::vector<LangType> _allLangTypes;

    QPointer<LanguageSelector> _langSelector;
    QPointer<QWidget> _sizeWidget;
};


#include "LanguageSelector.moc"


LanguageSelector::LanguageSelector(QWidget* parent
                                 , QWidget* inSizeWidget
                                 , QWidget* inReturnFocusWidget
                                 , const LangType inLangType)
    : QFrame(parent)
    , _sizeWidget(inSizeWidget)
    , _returnFocusWidget(inReturnFocusWidget)
{
    setObjectName("LanguageSelector");

    _mainLayout = new QGridLayout(this);
    _mainLayout->setObjectName("mainLayout");
    _mainLayout->setSpacing(0);
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(_mainLayout);

    _button = new QPushButton(this);
    setButtonText(inLangType); // onSelectedLanguage(inLangType);

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

    setFocusPolicy(Qt::TabFocus);
    setFocusProxy(_button);
}

LanguageSelector::~LanguageSelector()
{
    if (_menu)
    {
        _menu->deleteLater();
    }
}

void LanguageSelector::setButtonText(const LangType inlangType)
{
    _button->setText(Langs::GetLocaleName(inlangType));
}

void LanguageSelector::setBubbleToolTip(const QString& inStr)
{
    FinTooltipFilter::setBubbleToolTip(_button, inStr);
}

void LanguageSelector::onSelectedLanguage(const LangType inlangType)
{
    setButtonText(inlangType);
    emit languageSelected(inlangType);
}

void LanguageSelector::closeEvent(QCloseEvent* event)
{
    if (_menu)
    {
        _menu->close();
    }

    QWidget::closeEvent(event);
}

LanguageSelectorMenuPrivate* LanguageSelector::getMenu()
{
    if (_menu.isNull())
    {
        _menu = new LanguageSelectorMenuPrivate(this, parentWidget());
    }

    return _menu;
}


// ~=====================================
// LanguageSelectorMenuPrivate

LanguageSelectorMenuPrivate::LanguageSelectorMenuPrivate(LanguageSelector* inLangSelector, QWidget* inParentWidget)
    : QWidget(inParentWidget)
    , _langSelector(inLangSelector)
    , _sizeWidget(inLangSelector->_sizeWidget)
{
    Q_ASSERT(_langSelector);

    setObjectName("LanguageSelectorMenu");

    setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    _searchLine = new QLineEdit(this);
    _searchLine->setAttribute(Qt::WA_InputMethodEnabled, true);
    _searchLine->setPlaceholderText(tr("언어 검색"));
    layout->addWidget(_searchLine);

    _listWidget = new KeySelectionList(this);
    layout->addWidget(_listWidget);

    _allLangTypes = Langs::GetLanguageList();
    for (const LangType langType : _allLangTypes)
    {
        addListItem(langType);
    }

    connect(_searchLine, &QLineEdit::textChanged, this, &LanguageSelectorMenuPrivate::filterItems);
    connect(_listWidget, &QListWidget::itemClicked, this, &LanguageSelectorMenuPrivate::onItemClicked);
    connect(_listWidget, &KeySelectionList::itemKeyPressed, this, &LanguageSelectorMenuPrivate::onItemClicked);
    connect(this, &LanguageSelectorMenuPrivate::itemSelected, _langSelector, &LanguageSelector::onSelectedLanguage);

    setTabOrder({_searchLine, _listWidget});
}

LanguageSelectorMenuPrivate::~LanguageSelectorMenuPrivate()
{
    qApp->removeEventFilter(this);
}

QSize LanguageSelectorMenuPrivate::sizeHint() const
{
    return getTargetSize();
}

void LanguageSelectorMenuPrivate::showMenuPopup()
{
    qApp->installEventFilter(this);
    resize(getTargetSize());
    move(getTargetRelPos());
    show();
    raise();
    _searchLine->setFocus();
}

void LanguageSelectorMenuPrivate::closeMenuPopup()
{
    qApp->removeEventFilter(this);
    if (_langSelector && _langSelector->_returnFocusWidget)
    {
        _langSelector->_returnFocusWidget->setFocus();
    }

    close();
}

bool LanguageSelectorMenuPrivate::eventFilter(QObject* obj, QEvent* event)
{
    const QEvent::Type eventType = event->type();
    bool bIsCloseEvent = (eventType == QEvent::NonClientAreaMouseButtonPress);

    if (eventType == QEvent::MouseButtonPress)
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
            if (const QPushButton* dropButton = _langSelector ? _langSelector->_button : nullptr)
            {
                const QPoint buttonGlobalPos = dropButton->mapToGlobal(QPoint(0, 0));
                const QRect buttonGlobalRect = QRect(buttonGlobalPos, dropButton->size());

                bIsButtonContainMouse = buttonGlobalRect.contains(mouseGlobalPos);
            }

            if (bIsMenuContainMouse == false && bIsButtonContainMouse == false)
            {
                bIsCloseEvent = true;
            }
        }
    }
    else if (eventType == QEvent::KeyPress)
    {
        const QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        const int pressedKey = keyEvent->key();
        // esc 종료
        if (pressedKey == Qt::Key_Escape)
        {
            bIsCloseEvent = true;
        }
        // 탭 순환
        else if (pressedKey == Qt::Key_Tab)
        {
            if (obj == _searchLine)
            {
                _listWidget->setFocus();
                return true;
            }
            else if (obj == _listWidget)
            {
                _searchLine->setFocus();
                return true;
            }
        }
    }

    // 클릭 위치가 외부이거나 esc키를 눌렀다면 닫음. Popup 행동
    if (bIsCloseEvent)
    {
        closeMenuPopup();
        event->accept();
    }

    return QWidget::eventFilter(obj, event);
}

void LanguageSelectorMenuPrivate::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
}

void LanguageSelectorMenuPrivate::filterItems(const QString& inText)
{
    _listWidget->clear();
    for (const LangType langType : _allLangTypes)
    {
        if (Langs::IsContainName(langType, inText))
        {
            addListItem(langType);
        }
    }
}

void LanguageSelectorMenuPrivate::onItemClicked(QListWidgetItem* inItem)
{
    const int payload = inItem->data(LangTypeRole).toInt();
    
    emit itemSelected(static_cast<LangType>(payload));

    closeMenuPopup();
}

void LanguageSelectorMenuPrivate::addListItem(const LangType& inLangType)
{
    const QString langName = Langs::GetLocaleName(inLangType);
    _listWidget->addItem(langName);
    if (QListWidgetItem* lastItem = _listWidget->item(_listWidget->count() - 1))
    {
        lastItem->setData(LangTypeRole, static_cast<int>(inLangType));
    }
}

QPoint LanguageSelectorMenuPrivate::getTargetRelPos() const
{
    if (_sizeWidget.isNull())
    {
        qDebug() << "_sizeWidget is not valid.";
        return QPoint(0, 0);
    }

    return _sizeWidget->pos();
}

QSize LanguageSelectorMenuPrivate::getTargetSize() const
{
    if (_sizeWidget.isNull())
    {
        qDebug() << "_sizeWidget is not valid.";
        return QSize(200, 300);
    }

    return _sizeWidget->size();
}



//
// Created by YubinKim on 25/03/20 목.
//

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>


class CategoryListView;
class QLabel;
class QListView;
class QCheckBox;
class FinTranslatorCore;
QT_BEGIN_NAMESPACE

namespace Ui
{
class SettingsWidget;
}

QT_END_NAMESPACE

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(FinTranslatorCore* inFinCore, QWidget* parent = nullptr);
    ~SettingsWidget() override;

private:
    void applyTheme();
    
    FinTranslatorCore* _finCore;

    Ui::SettingsWidget* ui;

    
};


#endif //SETTINGSWIDGET_H

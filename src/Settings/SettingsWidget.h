//
// Created by YubinKim on 25/03/20 목.
//

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

#include "Widgets/IFinWidget.h"


class QButtonGroup;
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

class SettingsWidget : public IFinWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget* parent = nullptr);
    ~SettingsWidget() override;

private:
    void applyTheme();

    QButtonGroup* _buttonGroup;
    Ui::SettingsWidget* ui;
};


#endif //SETTINGSWIDGET_H

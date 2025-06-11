//
// Created by YubinKim on 25/06/08 일.
//

#ifndef IFINWIDGET_H
#define IFINWIDGET_H

#include <QWidget>

class IFinWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IFinWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    ~IFinWidget() override;

};


#endif //IFINWIDGET_H

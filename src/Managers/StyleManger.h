// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef PALETTEMANGER_H
#define PALETTEMANGER_H
#include <QObject>

namespace Sol
{
class SolPalette;

class StyleManger  : public QObject
{
    Q_OBJECT

public:
    explicit StyleManger(QObject* inParent = nullptr);
    ~StyleManger() override;

    static void applyTheme(const QString& inThemeName = "dark");

private:
    SolPalette* _solPalette;
};



} // namespace Sol

#endif //PALETTEMANGER_H

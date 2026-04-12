// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolGuard.h"

#include <QPainter>

PainterPenStateGuard::PainterPenStateGuard(QPainter* inPainter)
    : SolGeneralGuard([inPainter, prvPen = inPainter->pen()]()
    {
        inPainter->setPen(prvPen);
    })
{}

PainterFontStateGuard::PainterFontStateGuard(QPainter* inPainter)
    : SolGeneralGuard([inPainter, prevFont = inPainter->font()]()
    {
        inPainter->setFont(prevFont);
    })
{}

/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <QPainter>
#include "testwidget.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TestWidget::TestWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
  : HbWidget(parent, flags)     
{
    setPreferredSize(100, 100);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TestWidget::~TestWidget()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void TestWidget::onInitialize()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void TestWidget::onShow()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void TestWidget::onHide()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void TestWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawRect(rect());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
QRectF TestWidget::boundingRect() const
{
    return rect();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
QPainterPath TestWidget::shape() const
{
    QPainterPath path;
    path.addRect(rect());
    return path;
}

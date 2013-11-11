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
* Description: Bubble container.
*
*/

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <hbstyle.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include "bubblecontainerwidget.h"

BubbleContainerWidget::BubbleContainerWidget(QGraphicsItem* item)
    : HbWidget(item), mBackground(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    createPrimitives();
    updatePrimitives();
}

BubbleContainerWidget::~BubbleContainerWidget()
{
}

void BubbleContainerWidget::createPrimitives()
{
    delete mBackground;
    mBackground = new HbFrameItem(this);
    style()->setItemName(mBackground, QLatin1String("background"));
    mBackground->setZValue(-1.0);
    mBackground->setVisible(false); // background in drawn in paint()
}

void BubbleContainerWidget::updatePrimitives()
{
    mBackground->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    mBackground->frameDrawer().setFrameGraphicsName(
        QLatin1String("qtg_fr_list_normal"));
}

void BubbleContainerWidget::mousePressEvent(
    QGraphicsSceneMouseEvent * event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    mPressed = true;
}

void BubbleContainerWidget::mouseMoveEvent(
    QGraphicsSceneMouseEvent *event)
{
    if ( !rect().contains(event->pos()) && mPressed ) {
        ungrabMouse();
        mPressed = false;
    }
}

void BubbleContainerWidget::mouseReleaseEvent(
    QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)    

    if (mPressed) {
        emit clicked();
        mPressed = false;
    }
}

void BubbleContainerWidget::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    mBackground->frameDrawer().paint(painter,boundingRect());
}

void BubbleContainerWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    mPressed = false;
}


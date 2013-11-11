/*!
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
* Description:  Collapsed call bubble handler.
*
*/

#include <QtGui>
#include <hbaction.h>

#include "bubblemanager2.h"
#include "bubblecollapsedhandler.h"
#include "bubbleheader.h"
#include "bubbleutils.h"
#include "bubbleheadingwidget.h"
#include "bubblecontainerwidget.h"

BubbleCollapsedHandler::BubbleCollapsedHandler(
    BubbleWidgetManager& widgetManager,
    BubbleWidgetManager::View view,
    BubbleWidgetManager::Container container,
    QObject* parent )
    : BubbleHandler(widgetManager,view,container,parent)
{
    mHeading =
        qobject_cast<BubbleHeadingWidget*>(
                widget(BubbleWidgetManager::Heading));
    Q_ASSERT(mHeading);

    BubbleContainerWidget* bubble
        = qobject_cast<BubbleContainerWidget*>(
                widgetManager.container(view,container));
    Q_ASSERT(bubble);

    connect(bubble,SIGNAL(clicked()),this,SLOT(handleBubbleClicked()));

    reset();
}

BubbleCollapsedHandler::~BubbleCollapsedHandler()
{
}

void BubbleCollapsedHandler::reset()
{  
    mHeader = 0;
    mHeading->reset();
}

void BubbleCollapsedHandler::readBubbleHeader(
    const BubbleHeader& header )
{
    mHeader = &header;
    mHeading->readBubbleHeader(header);
}

QGraphicsWidget* BubbleCollapsedHandler::graphicsWidgetForAction(
    HbAction* action ) const
{
    Q_UNUSED(action)
    return 0;
}

void BubbleCollapsedHandler::updateTimerDisplayNow()
{
    mHeading->updateTimerDisplayNow();
}

void BubbleCollapsedHandler::handleBubbleClicked()
{
    emit headerSelected(mHeader->bubbleId());
}

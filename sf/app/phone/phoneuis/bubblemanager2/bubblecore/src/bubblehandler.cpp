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
* Description:  Base class for bubble handlers.
*
*/

#include <QtGui>
#include "bubblehandler.h"
#include "bubbleheader.h"

BubbleHandler::BubbleHandler(
    BubbleWidgetManager& widgetManager,
    BubbleWidgetManager::View view,
    BubbleWidgetManager::Container container,
    QObject* parent )
    : QObject( parent ), mWidgetManager(widgetManager),
      mView(view), mContainer(container)
{
}

BubbleHandler::~BubbleHandler()
{
}

QGraphicsWidget* BubbleHandler::widget(
    BubbleWidgetManager::Widget widget) const
{
    return mWidgetManager.widget(mView,mContainer,widget);
}

void BubbleHandler::conferenceMemberRemoved(int bubbleId)
{
    Q_UNUSED(bubbleId);
}

void BubbleHandler::conferenceRemoved()
{
}

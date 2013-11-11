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
* Description:  Expanded call bubble handler.
*
*/

#include <QtGui>
#include <hbaction.h>
#include <hbtextitem.h>

#include "bubblemanager2.h"
#include "bubbleexpandedhandler.h"
#include "bubbleheader.h"
#include "bubbleutils.h"
#include "bubbleimagewidget.h"
#include "bubbleheadingwidget.h"
#include "bubblebutton.h"

BubbleExpandedHandler::BubbleExpandedHandler(
    BubbleWidgetManager& widgetManager,
    BubbleWidgetManager::View view,
    BubbleWidgetManager::Container container,
    QObject* parent )
    : BubbleHandler(widgetManager,view,container,parent)
{
    mHeading = qobject_cast<BubbleHeadingWidget*>(
        widget(BubbleWidgetManager::Heading));
    Q_ASSERT(mHeading);

    mImage = qobject_cast<BubbleImageWidget*>(
        widget(BubbleWidgetManager::Image));
    Q_ASSERT(mImage);

    mButtonCenter = qobject_cast<BubbleButton*>(
        widget(BubbleWidgetManager::CenterButton));
    Q_ASSERT(mButtonCenter);
    mButtonLeft = qobject_cast<BubbleButton*>(
        widget(BubbleWidgetManager::LeftButton));
    Q_ASSERT(mButtonLeft);
    mButtonRight = qobject_cast<BubbleButton*>(
        widget(BubbleWidgetManager::RightButton));
    Q_ASSERT(mButtonRight);

    reset();
}

BubbleExpandedHandler::~BubbleExpandedHandler()
{
}

void BubbleExpandedHandler::reset()
{  
    mHeader = 0;
    mButtonCenter->hide();
    mButtonCenter->setDown(false);
    mButtonCenter->disconnect();
    mButtonLeft->hide();
    mButtonLeft->setDown(false);
    mButtonLeft->disconnect();
    mButtonRight->hide();
    mButtonRight->setDown(false);
    mButtonRight->disconnect();
    mImage->hide();
    mHeading->reset();
}

void BubbleExpandedHandler::readBubbleHeader(
    const BubbleHeader& header )
{
    mHeader = &header;

    mHeading->readBubbleHeader(header);

    if (!header.callImage().isEmpty() ||
        header.showDefaultAvatar()) {
        mImage->setImageName(header.callImage());
        if (header.callFlags()&BubbleManagerIF::EmergencyCall) {
            mImage->displayEmergencyCallIcon();
        }
        mImage->show();
    } else {
        mImage->hide();
    }

    setButtons(mHeader->actions());
}

void BubbleExpandedHandler::setButtons(const QList<HbAction*>& actions)
{
    if ( actions.count()==1 && mButtonCenter ) {
        HbAction* action = actions.at(0);
        mButtonCenter->setIcon( action->icon() );
        connect(mButtonCenter, SIGNAL( clicked() ),
                action, SLOT( trigger() ) );
        connect(mButtonCenter, SIGNAL( longPress(QPointF)),
                action, SLOT( trigger() ) );
        BubbleUtils::setButtonStyleForAction(*mButtonCenter,*action);
        mButtonCenter->show();
    } else  if (actions.count()==2 && mButtonLeft && mButtonRight ) {
        // Left button
        HbAction* action1 = actions.at(0);
        mButtonLeft->setIcon(action1->icon());
        connect( mButtonLeft, SIGNAL( clicked() ),
                 action1, SLOT( trigger() ) );
        connect( mButtonLeft, SIGNAL( longPress(QPointF)),
                 action1, SLOT( trigger() ) );
        BubbleUtils::setButtonStyleForAction(*mButtonLeft,*action1);
        mButtonLeft->show();
        // Right button
        HbAction* action2 = actions.at(1);
        mButtonRight->setIcon( action2->icon() );
        connect( mButtonRight, SIGNAL( clicked() ),
                 action2, SLOT( trigger() ) );
        connect( mButtonRight, SIGNAL( longPress(QPointF)),
                 action2, SLOT( trigger() ) );
        BubbleUtils::setButtonStyleForAction(*mButtonRight,*action2);
        mButtonRight->show();
    }
}

QGraphicsWidget* BubbleExpandedHandler::graphicsWidgetForAction(
    HbAction* action ) const
{
    if ( mButtonLeft->text() == action->text() ) {
        return mButtonLeft;
    } else if ( mButtonRight->text() == action->text() ) {
        return mButtonRight;
    } else if ( mButtonCenter->text() == action->text() ) {
        return mButtonCenter;
    } else {
        return 0;
    }
}

void BubbleExpandedHandler::updateTimerDisplayNow()
{
    mHeading->updateTimerDisplayNow();
}

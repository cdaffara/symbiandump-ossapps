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
* Description:  Conference call bubble handler.
*
*/

#include <QTimer>

#include <hbaction.h>
#include <hbtextitem.h>
#include <hblabel.h>
#include <hblistview.h>

#include "bubblemanager2.h"
#include "bubbleconferencehandler.h"
#include "bubbleconferenceheader.h"
#include "bubbleutils.h"
#include "bubbleparticipantlistitem.h"
#include "bubbleparticipantlistmodel.h"
#include "bubblebutton.h"
#include "bubbleheadingwidget.h"

static const int BUBBLE_SELECTION_TIMEOUT = 3000;

BubbleConferenceHandler::BubbleConferenceHandler(
    BubbleWidgetManager& widgetManager,
    BubbleWidgetManager::View view,
    BubbleWidgetManager::Container container,
    QObject* parent )
    : BubbleHandler(widgetManager,view,container,parent)
{
    mHeading =
        qobject_cast<BubbleHeadingWidget*>(widget(BubbleWidgetManager::Heading));
    Q_ASSERT(mHeading);

    mButtonCenter =
        qobject_cast<BubbleButton*>(widget(BubbleWidgetManager::CenterButton));
    Q_ASSERT(mButtonCenter);
    mButtonLeft =
        qobject_cast<BubbleButton*>(widget(BubbleWidgetManager::LeftButton));
    Q_ASSERT(mButtonLeft);
    mButtonRight =
        qobject_cast<BubbleButton*>(widget(BubbleWidgetManager::RightButton));
    Q_ASSERT(mButtonRight);

    mList =
        qobject_cast<HbListView*>(widget(BubbleWidgetManager::ParticipantList));
    Q_ASSERT(mList);
    mModel = new BubbleParticipantListModel();
    mList->setModel(mModel);
    
    HbAbstractItemView::ItemAnimations noCreationAndRemovalAnimations = HbAbstractItemView::All;
    noCreationAndRemovalAnimations ^= HbAbstractItemView::Appear;
    noCreationAndRemovalAnimations ^= HbAbstractItemView::Disappear;
    mList->setEnabledAnimations(noCreationAndRemovalAnimations);

    mPrototype =
        qobject_cast<BubbleParticipantListItem*>(
            widgetManager.createParticipantListItem());
    Q_ASSERT(mPrototype);
    mList->setItemPrototype(mPrototype);
    connect(mList->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(handleItemSelected(QModelIndex,QModelIndex)));

    mSelectionTimer = new QTimer(this);
    connect(mSelectionTimer,SIGNAL(timeout()),SLOT(clearSelection()));
    mSelectionTimer->setSingleShot(true);

    reset();
}

BubbleConferenceHandler::~BubbleConferenceHandler()
{
}

void BubbleConferenceHandler::reset()
{  
    mHeader = 0;

    mSelectionTimer->stop();
    mPrototype->clearActions();
    mHeading->reset();
    mButtonCenter->hide();
    mButtonCenter->setDown(false);
    mButtonCenter->disconnect();
    mButtonLeft->hide();
    mButtonLeft->setDown(false);
    mButtonLeft->disconnect();
    mButtonRight->hide();
    mButtonRight->setDown(false);
    mButtonRight->disconnect();
}

void BubbleConferenceHandler::readBubbleHeader( const BubbleHeader& header )
{
    Q_ASSERT(header.isConference());
    mHeader = static_cast<const BubbleConferenceHeader*>(&header);

    // set actions to item prototype
    QList<HbAbstractViewItem*> prototypes = mList->itemPrototypes();
    QList<HbAction*> actions = mHeader->participantListActions();
    foreach (HbAction* action, actions ) {
        mPrototype->addAction(action);
    }

    // populate participant list model
    QList<BubbleHeader*> participants = mHeader->headers();
    foreach(BubbleHeader* participant, participants) {
         mModel->addParticipant(
            participant->bubbleId(),
            participant->cli(),
            participant->cliClipDirection());
    }

    mHeading->readBubbleHeader(header);

    setButtons(mHeader->actions());

    clearSelection();
}

void BubbleConferenceHandler::setButtons(const QList<HbAction*>& actions)
{
    if ( actions.count()==1 && mButtonCenter ) {
        HbAction* action = actions.at(0);
        mButtonCenter->setIcon( action->icon() );
        BubbleUtils::setButtonStyleForAction(*mButtonCenter,*action);
        connect(mButtonCenter, SIGNAL( clicked() ),
                action, SLOT( trigger() ) );
        connect(mButtonCenter, SIGNAL( longPress(QPointF)),
                action, SLOT( trigger() ) );
        mButtonCenter->show();
    } else  if (actions.count()==2 && mButtonLeft && mButtonRight ) {
        // Left button
        HbAction* action1 = actions.at(0);
        mButtonLeft->setIcon( action1->icon() );
        BubbleUtils::setButtonStyleForAction(*mButtonLeft,*action1);
        connect( mButtonLeft, SIGNAL( clicked() ),
                 action1, SLOT( trigger() ) );
        connect(mButtonLeft, SIGNAL( longPress(QPointF)),
                action1, SLOT( trigger() ) );
        mButtonLeft->show();
        // Right button
        HbAction* action2 = actions.at(1);
        mButtonRight->setIcon( action2->icon() );
        BubbleUtils::setButtonStyleForAction(*mButtonRight,*action2);
        connect( mButtonRight, SIGNAL( clicked() ),
                 action2, SLOT( trigger() ) );
        connect(mButtonRight, SIGNAL( longPress(QPointF)),
                action2, SLOT( trigger() ) );
        mButtonRight->show();
    }
}

QGraphicsWidget* BubbleConferenceHandler::graphicsWidgetForAction(
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

void BubbleConferenceHandler::updateTimerDisplayNow()
{
    mHeading->updateTimerDisplayNow();
}

void BubbleConferenceHandler::handleItemSelected(
    const QModelIndex & current,
    const QModelIndex & previous )
{
    Q_UNUSED(previous);

    if (current.isValid() && mHeader!=0) {
        // need to cast constness away
        BubbleConferenceHeader& mutableHeader =
            const_cast<BubbleConferenceHeader&>(*mHeader);
        mutableHeader.setSelectedHeader(mModel->bubbleId(current.row()));
        mSelectionTimer->stop();
        mSelectionTimer->start(BUBBLE_SELECTION_TIMEOUT);
    }
}

void BubbleConferenceHandler::clearSelection()
{
    mList->selectionModel()->clear();
}

void BubbleConferenceHandler::conferenceMemberRemoved(int bubbleId)
{
    mModel->removeParticipant(bubbleId);
}

void BubbleConferenceHandler::conferenceRemoved()
{
    mModel->reset();
    mList->reset();
}

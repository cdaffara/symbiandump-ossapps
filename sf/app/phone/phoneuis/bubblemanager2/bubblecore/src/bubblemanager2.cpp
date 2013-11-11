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
* Description:  BubbleManager widget.
*
*/

#include <QtGui>
#include <QtAlgorithms>

#include <hblabel.h>
#include <hbstyle.h>
#include <hbiconitem.h>
#include <hbaction.h>
#include <hbstackedlayout.h>
#include <hbmainwindow.h>
#include <hbstyleloader.h>

#include "bubblemanager2.h"
#include "bubbleheader.h"
#include "bubbleconferenceheader.h"
#include "bubbleutils.h"
#include "bubblewidgetmanager.h"
#include "bubbleimagemanager.h"
#include "bubblehandler.h"
#include "bubbleeffecthandler.h"


BubbleManager::BubbleManager( QGraphicsItem *parent ) :
    HbWidget( parent ),
    mIsReadyToDraw(0),
    mMuted(0),
    mMutedIcon(0),
    mSortHeaders(false),
    mInitialized(false),
    mBubbleSelectionDisabled(0)
{
    // allocate headers
    BubbleHeader* header;
    for ( int i = 0 ; i < BUBBLE_MAX_CALL_COUNT ; i++ ) {
        header = new BubbleHeader;
        header->setBubbleId( i );
        mCallHeaders.append( header );
    }

    mConferenceHeader = new BubbleConferenceHeader();
    mConferenceHeader->setBubbleId(BUBBLE_CONF_CALL_ID);

    mBubbleImageManager = new BubbleImageManager(this);

    mEffectHandler = new BubbleEffectHandler(this);

    mWidgetManager = new BubbleWidgetManager(
        *mBubbleImageManager, this);

    mMainLayout = new HbStackedLayout(this);
    mMainLayout->setContentsMargins(0.0,0.0,0.0,0.0);
    setLayout(mMainLayout);

    // preload views on boot
    mWidgetManager->view(BubbleWidgetManager::SingleCallView);
}

BubbleManager::~BubbleManager()
{
    qDeleteAll(mCallHeaders);
    mCallHeaders.clear();
    delete mConferenceHeader;
}

/**
* Before making any changes to bubble headers, call this function so
* manager can prapare for them properly.
*/
void BubbleManager::startChanges()
{
    mIsReadyToDraw++;
    if ( mIsReadyToDraw > 1 ) {
        return;
    }
    
    if (parentItem()) {
        // reset/hide on outlooks may result clearFocus() call,
        // which means that there is no focused item. move focus
        // to parent item, before reset/hide calls.
        parentItem()->setFocus();
    }

    // invalidate current widgets
    for (int i=0; i<mMainLayout->count(); i++) {
        mMainLayout->itemAt(i)->graphicsItem()->hide();
    }

    // reset active handlers
    foreach (BubbleHandler* handler, mActiveHandlers) {
        handler->reset();
    }
    mActiveHandlers.clear();
}

/**
* After the changes to bubble headers call this function so manager
* can prepare bubbles to right places and check the drawing order.
* Memory for arrays has been allocated beforehand.
*/
void BubbleManager::endChanges()
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    mIsReadyToDraw--;
    if ( mIsReadyToDraw != 0 ) {
        return;
    }

    if (!mInitialized) {
        if (mainWindow()) {
            mWidgetManager->handleOrientationChange(mainWindow()->orientation());
            mInitialized=true;
        }
    }

    // sort active headers
    if ( mActiveHeaders.count() && mSortHeaders) {
        qStableSort(mActiveHeaders.begin(),
                    mActiveHeaders.end(),
                    BubbleUtils::compareHeaders );
        mSortHeaders = false;
    }

    // select view
    QGraphicsWidget* view = selectView();

    if (view) {
        setViewData(view);
        view->show();
    }

    if (!mActiveHeaders.count()) {
        // all calls ended
        mBubbleImageManager->releasePixmaps();
        releaseNonCachedViews();
    }

    // restore mute state
    setPhoneMuted(mMuted);

    update();
}

QGraphicsWidget* BubbleManager::selectView()
{
    QGraphicsWidget* view=0;
    int bubbleCount = shownHeaderCount();

    if (bubbleCount && isConferenceExpanded()) {
        view = mWidgetManager->view(BubbleWidgetManager::ConferenceView);
    } else {
        switch(bubbleCount) {
        case 1:
            view = mWidgetManager->view(BubbleWidgetManager::SingleCallView);
            break;
        case 2:
            view = mWidgetManager->view(BubbleWidgetManager::TwoCallsView);
            break;
        case 3:
            view = mWidgetManager->view(BubbleWidgetManager::ThreeCallsView);
            break;
        default:
            break;
        }
    }

    if (view) {
        if (addToLayout(view)) {
            // connect signals
            connectSignals(view);
        }
    }

    return view;
}

bool BubbleManager::addToLayout(QGraphicsWidget* widget)
{
    Q_ASSERT(widget);

    bool added=false;
    int index = mMainLayout->indexOf(widget);
    if (index == -1) {
        mMainLayout->addItem(widget);
        added=true;
    }

    return added;
}

void BubbleManager::removeFromLayout(QGraphicsWidget* widget)
{
    mMainLayout->removeItem(widget);
}

void BubbleManager::connectSignals(QGraphicsWidget* widget)
{
    QList<BubbleHandler*>* handlers = mWidgetManager->handlers(widget);
    if (handlers) {
        foreach (BubbleHandler* handler, *handlers) {
            connect(handler,SIGNAL(headerSelected(int)),
                    SLOT(showExpanded(int)));
        }
    }
}

void BubbleManager::setViewData(QGraphicsWidget* view)
{
    Q_ASSERT(view);

    QList<BubbleHandler*>* handlers =
            mWidgetManager->handlers(view);

    if (handlers) {
        if (isConferenceExpanded()) {
            // only conference call is displayed
            Q_ASSERT(handlers->count()==1);
            BubbleHandler* handler = handlers->at(0);
            handler->readBubbleHeader(*mConferenceHeader);
            mActiveHandlers.append(handler);
        } else {
            int i=0;
            foreach (BubbleHandler* handler, *handlers) {
                findNextDrawableHeader(i);
                handler->readBubbleHeader(*mActiveHeaders.at(i));
                mActiveHandlers.append(handler);
                i++;
                if (i==mActiveHeaders.count()) {
                    break;
                }
            }
        }
    }
}

/**
* Takes a new call header in use.
* Returns bubble idenfication number.
*/
int BubbleManager::createCallHeader()
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    quint8 index = 0;
    while ( mCallHeaders[index]->isUsed( ) ) {
        index++;
        Q_ASSERT( index < mCallHeaders.size() );
    }

    BubbleHeader* header = mCallHeaders[index];

    header->setIsUsed( true );

    mActiveHeaders.insert( 0, header ); // in priority order

    return header->bubbleId();
}

/**
* Removes call header from use
*/
void BubbleManager::removeCallHeader( int bubbleId )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    for ( int i=0; i < mActiveHeaders.size(); i++ ) {
        if ( mActiveHeaders[i]->bubbleId() == bubbleId ) {
            QString image = mActiveHeaders[i]->callImage();
            mActiveHeaders[i]->reset();
            mActiveHeaders.remove( i );
            releaseImageIfNotUsed(image);
            break;
        }
    }
}

/**
* Finds header by bubble id.
*/
bool BubbleManager::findActiveHeader( int bubbleId, BubbleHeader*& header )
{
    header = 0;
    for ( int i=0; i < mActiveHeaders.size(); i++ ) {
        if ( mActiveHeaders[i]->bubbleId() == bubbleId ) {
            header = mActiveHeaders[i];
            break;
        }
    }

    return header ? true : false;
}

void BubbleManager::findNextDrawableHeader(int& index) const
{
    while (mActiveHeaders.at(index)->isInConference()) {
        index++;
    }
}

/**
* Sets call state to header.
*/
void BubbleManager::setState(
    int bubbleId,
    PhoneCallState state )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    BubbleManager::PhoneCallState oldState =
        header->callState();

    if ( shownHeaderCount()<3 &&
         !header->isInConference() &&
         (expandedBubble() != bubbleId) &&
         (oldState==OnHold && state==Active) ) {
        // send key swap -> make active call expanded
        int i = mActiveHeaders.indexOf(header);
        mActiveHeaders.remove(i);
        mActiveHeaders.insert(0, header);
    }

    header->setCallState( state );
}

/**
* Sets text label to header. For conf also.
* Text to be seen in bubble ( e.g. 'on hold' )
*/
void BubbleManager::setLabel(
    int bubbleId,
    const QString& text,
    Qt::TextElideMode clipDirection )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setText( text, clipDirection );
}

/**
* Sets caller's line identification ( name or number) to header.
* For conf also. Caller's CLI ( e.g. 'Mother' )
*/
void BubbleManager::setCli(
    int bubbleId,
    const QString& cliText,
    Qt::TextElideMode clipDirection )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setCli( cliText, clipDirection );
}

/**
* Updates caller's line identification ( name or number) to header.
* Caller's CLI ( e.g. 'Daddy' )-
*/
void BubbleManager::updateCLI(
    int bubbleId,
    const QString& cliText,
    Qt::TextElideMode clipDirection )
{
    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setCli( cliText, clipDirection );

    // ToDo: take care of redrawing
}

/**
* Number or voip adress, when phonebook name takes Cli.
*/
void BubbleManager::setSecondaryCli(
    int bubbleId,
    const QString& cliText,
    Qt::TextElideMode clipDirection )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setSecondaryCli( cliText, clipDirection );
}

/**
* Sets call time or cost text to header. For conf also.
*/
void BubbleManager::setCallTime(
    int bubbleId,
    const QString& callTime )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setTimerCost( callTime );
}

/**
* Updates call time or cost text to header.  For conf also.
* Timer or cost text ( e.g. '00:12:34' or '£01.23' ).
*/
void BubbleManager::updateCallTime(
    int bubbleId,
    const QString& callTime )
{
    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setTimerCost( callTime );

    foreach (BubbleHandler* handler,mActiveHandlers) {
        handler->updateTimerDisplayNow();
    }
}

/**
* Attach a call image to header.
*/
void BubbleManager::setCallObjectImage(
    int bubbleId,
    const QString& fileName )
{
    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setCallImage(fileName);
}

/**
* Attach the theme call image to header.
*/
void BubbleManager::setCallObjectFromTheme(
    int bubbleId )
{
    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setShowDefaultAvatar(true);
}

/**
* Sets call flags to header.
*/
void BubbleManager::setCallFlags(
    int bubbleId,
    int flags )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setCallFlags( flags );
}

/**
* Sets call flags to header.
*/
void BubbleManager::setCallFlag(
    int bubbleId,
    PhoneCallFlags flag,
    bool set )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    if ( set ) {
        header->setCallFlag( flag );
    }
    else {
        header->removeCallFlag( flag );
    }
}

/**
* Sets number type.
*/
void BubbleManager::setNumberType(
    int bubbleId,
    PhoneNumberType type )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->setNumberType( type );
}

/**
* Sets phone muted/unmuted.
* @param aIsMuted ETrue if phone is muted.
*/
void BubbleManager::setPhoneMuted(
    bool muted )
{
    if (!mMutedIcon && muted) {
        mMutedIcon = mWidgetManager->view(BubbleWidgetManager::MutedOverlay);
        Q_ASSERT(mMutedIcon);
        addToLayout(mMutedIcon);
        mMutedIcon->setZValue(10.0);
        mEffectHandler->addEffect(mMutedIcon,BubbleMutedAppear);
        mEffectHandler->addEffect(mMutedIcon,BubbleMutedDisappear);
    }

    if (mMutedIcon) {
        // run effect when mute status changes
        if (muted && !mMuted) {
            mEffectHandler->startEffect(BubbleMutedAppear);
        } else if (!muted && mMuted) {
            mEffectHandler->startEffect(BubbleMutedDisappear);
        } else {
            mEffectHandler->cancelAllEffects(mMutedIcon);
            mMutedIcon->setVisible(muted);
        }
    }

    mMuted = muted;
}

/**
* Creates a conference call based upon two calls. Bubbles must be
* created first.
*/
int BubbleManager::createConference(
    int bubble1,
    int bubble2 )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    mConferenceHeader->setIsUsed(true);
    mActiveHeaders.append(mConferenceHeader);
    addRowToConference(bubble1);
    addRowToConference(bubble2);
    return mConferenceHeader->bubbleId();
}

/**
* Splits conference call into invidual two calls. Call headers stays
* in use. Headers' state will not be changed.
*/
void BubbleManager::removeConference()
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    mConferenceHeader->reset();
    mSortHeaders = true;

    for ( int i=0; i < mActiveHeaders.size(); i++ ) {
        if ( mActiveHeaders[i]->bubbleId() == BUBBLE_CONF_CALL_ID ) {
            mActiveHeaders.remove( i );
            break;
        }
    }

    QGraphicsWidget* view =
        mWidgetManager->view(BubbleWidgetManager::ConferenceView);
    if (view) {
        QList<BubbleHandler*>* handlers = mWidgetManager->handlers(view);
        if (handlers) {
            foreach (BubbleHandler* handler, *handlers) {
                handler->conferenceRemoved();
            }
        }
    }
}

/**
* Adds new call to conference call.
*/
void BubbleManager::addRowToConference( int bubbleId )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );
    
    Q_ASSERT( !header->isInConference() );
    Q_ASSERT( !header->isConference() );

    mConferenceHeader->addHeader(header);

    // release image while in conference
    releaseImageIfNotUsed(header->callImage());
}

/**
* Takes specified call out of conference. Use RemoveConference if
* conference has only two calls in it. Header's state is same as
* before adding it to conference (if not changed inside the conf).
*/
void BubbleManager::removeRowFromConference( int bubbleId )
{
    Q_ASSERT( mIsReadyToDraw > 0 );
    mConferenceHeader->removeHeader(bubbleId);
    mSortHeaders = true;

    QGraphicsWidget* view =
        mWidgetManager->view(BubbleWidgetManager::ConferenceView);

    if (view) {
        QList<BubbleHandler*>* handlers = mWidgetManager->handlers(view);
        if (handlers) {
            foreach (BubbleHandler* handler, *handlers) {
                handler->conferenceMemberRemoved(bubbleId);
            }
        }
    }
}

/**
* Counts calls in conference call.
*/
int BubbleManager::conferenceRowCount() const
{
    return mConferenceHeader->headers().count();
}


/**
* Sets highlight to specified line in conference.
*/
void BubbleManager::setSelectionInConference( int rowNumber )
{
    Q_UNUSED(rowNumber)
}

/**
* Sets highlight to specified bubble id in conference.
*/
void BubbleManager::setSelectionIdInConference( int bubbleId )
{
    Q_UNUSED(bubbleId)
}

/**
* Gets highlighted item in conference.
*/
int BubbleManager::selectionInConference() const
{
    return -1;
}

/**
* Gets highlighted item in conference.
*/
int BubbleManager::selectionIdInConference() const
{
    return mConferenceHeader->selectedHeader();
}

/**
* Moves highligh one up if possible
*/
void BubbleManager::moveHighlightOneUpInConference()
{
// may be needed in non-touch
}

/**
* Moves highligh one down if possible
*/
void BubbleManager::moveHighlightOneDownInConference()
{
// may be needed in non-touch
}

/**
* Use this function to expand or shrink conference bubble.
*/
void BubbleManager::setExpandedConferenceCallHeader(
    bool expanded )
{
    Q_ASSERT( mIsReadyToDraw > 0 );
    if ( mConferenceHeader->isUsed() ) {
        mConferenceHeader->setExpanded(expanded);
        mSortHeaders = true;
    }
}

/**
* Query: is conference expanded?
*/
bool BubbleManager::isConferenceExpanded( ) const
{
    return mConferenceHeader->isExpanded();
}


/**
* Number of headers shown on the screen.
*/
int BubbleManager::shownHeaderCount() const
{
    QVectorIterator<BubbleHeader*> i(mActiveHeaders);
    int count = 0;
    while (i.hasNext()) {
        if ( !i.next()->isInConference() ) {
            count++;
        }
    }
    return count;
}

/**
* Set CLI used in participant list (text or phonenumber).
*/
void BubbleManager::setParticipantListCli(
    int bubbleId,
    ParticipantListCli aParticipantCli )
{
    Q_UNUSED(bubbleId)
    Q_UNUSED(aParticipantCli)
}

void BubbleManager::addAction( int bubbleId, HbAction* action )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->addAction(action);
}

void BubbleManager::clearActions( int bubbleId )
{
    Q_ASSERT( mIsReadyToDraw > 0 );

    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );     
    Q_ASSERT( header );

    header->clearActions();
}

QGraphicsWidget* BubbleManager::graphicsWidgetForAction(
    HbAction* action ) const
{
    QGraphicsWidget* widget = 0;

    foreach (BubbleHandler* handler,mActiveHandlers) {
        widget = handler->graphicsWidgetForAction(action);
        if (widget) {
            break;
        }
    }

    if (!widget) {
        // check if it's expand action
        QVectorIterator<BubbleHeader*> i(mActiveHeaders);
        bool bubbleWidget=false;
        while (i.hasNext()) {
            BubbleHeader* h = i.next();
            if ( h->expandAction() &&
                 (h->expandAction()->text()==action->text()) ) {
                bubbleWidget=true;
                break;
            }
        }

        if (bubbleWidget) {
            if ( mActiveHandlers.count()==2 ) {
                widget = mWidgetManager->container(
                    BubbleWidgetManager::TwoCallsView,
                    BubbleWidgetManager::CollapsedBubble );
            } else if ( mActiveHandlers.count()==3 ) {
                // return top most bubble
                widget = mWidgetManager->container(
                    BubbleWidgetManager::ThreeCallsView,
                    BubbleWidgetManager::CollapsedBubble2 );
            }
        }
    }

    return widget;
}

void BubbleManager::addParticipantListAction(HbAction *action)
{
    Q_ASSERT( mIsReadyToDraw > 0 );
    mConferenceHeader->addParticipantListAction(action);
}

void BubbleManager::clearParticipantListActions()
{
    Q_ASSERT( mIsReadyToDraw > 0 );
    mConferenceHeader->clearParticipantListActions();
}

int BubbleManager::expandedBubble() const
{
    // shall not be called without creating call headers first
    Q_ASSERT(mActiveHeaders.count());

    if (isConferenceExpanded()) {
        return mConferenceHeader->bubbleId();
    } else {
        int i=0;
        findNextDrawableHeader(i);
        return mActiveHeaders.at(i)->bubbleId();
    }
}

void BubbleManager::setExpandAction(int bubbleId, HbAction* action)
{
    BubbleHeader* header = 0;
    findActiveHeader( bubbleId, header );
    Q_ASSERT( header );
    header->setExpandAction(action);
}

void BubbleManager::setBubbleSelectionDisabled(bool disabled)
{
    mBubbleSelectionDisabled = disabled;
}

void BubbleManager::handleOrientationChange(
    Qt::Orientation orientation)        
{
    mWidgetManager->handleOrientationChange(orientation);
}

void BubbleManager::polishEvent()
{
    // for debugging - remove
    HbWidget::polishEvent();
}

void BubbleManager::showExpanded( int bubbleId )
{
    if ( !mBubbleSelectionDisabled && (mActiveHeaders.count() > 1) ) {
        int expanded = mActiveHeaders[0]->bubbleId();
        if ( bubbleId != expanded ) {
            startChanges();

            BubbleHeader* header = 0;

            if (bubbleId == BUBBLE_CONF_CALL_ID) {
                header = mConferenceHeader;
            } else {
                // find header
                for ( int i=0; i < mActiveHeaders.size(); i++ ) {
                    if ( mActiveHeaders[i]->bubbleId() == bubbleId ) {
                        header = mActiveHeaders[i];
                        mActiveHeaders.remove(i);
                        break;
                    }
                }

                Q_ASSERT(header);

                // set it first
                mActiveHeaders.insert(0,header);
            }

            endChanges();

            // trigger the expand action
            HbAction* action = header->expandAction();
            if (action) {
                action->trigger();
            }
        }
    }
}

void BubbleManager::releaseImageIfNotUsed(
    const QString& imageFileName)
{
    bool used = false;

    if (!imageFileName.isEmpty()) {
        foreach(BubbleHeader* header, mActiveHeaders) {
            if (!header->isInConference() &&
                header->callImage()==imageFileName) {
                used = true;
                break;
            }
        }

        if (!used) {
            mBubbleImageManager->unloadImage(imageFileName);
        }
    }
}

void BubbleManager::releaseNonCachedViews()
{
    static const int viewCount = 3;
    static const int views[viewCount] = {
        BubbleWidgetManager::TwoCallsView,
        BubbleWidgetManager::ThreeCallsView,
        BubbleWidgetManager::ConferenceView
    };

    for (int i=0; i < viewCount; i++ ) {
        BubbleWidgetManager::View viewid =
            (BubbleWidgetManager::View)views[i];

        if (mWidgetManager->isLoaded(viewid)) {
            QGraphicsWidget* view = mWidgetManager->view(viewid);

            if (view) {
                removeFromLayout(view);
                mWidgetManager->releaseView(viewid);
            }
        }
    }
}

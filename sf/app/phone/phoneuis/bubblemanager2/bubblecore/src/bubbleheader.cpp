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
* Description:  Container for call data.
*
*/

#include "bubbleheader.h"

BubbleHeader::BubbleHeader()
{
    reset();
}

BubbleHeader::~BubbleHeader()
{
}

void BubbleHeader::reset()
{
    mIsUsed = false;
    mCallState = BubbleManager::None;
    mNumberType = BubbleManager::NotSet;
    mCallFlags = 0;
    mText.clear();
    mCli.clear();
    mTimerCost.clear();
    mSecondaryCli.clear();
    mActions.clear();
    mTextClip = Qt::ElideNone;
    mSecondaryCliClip = Qt::ElideNone;
    mCliClip = Qt::ElideNone;
    mCallImage.clear();
    mIsInConference = false;
    mExpandAction = 0;
    mShowDefaultAvatar = false;
}

void BubbleHeader::setIsUsed( bool used )
{
    mIsUsed = used;
}

bool BubbleHeader::isUsed() const
{
    return mIsUsed;
}

void BubbleHeader::setBubbleId( int bubbleId )
{
    mBubbleId = bubbleId;   
}

int BubbleHeader::bubbleId() const
{
    return mBubbleId;
}

void BubbleHeader::setCallState( BubbleManager::PhoneCallState state )
{
    if (state == BubbleManager::Disconnected) {
        if ( mCallState == BubbleManager::Waiting ||
             mCallState == BubbleManager::Incoming ||
             mCallState == BubbleManager::Alerting ||
             mCallState == BubbleManager::Outgoing ||
             mCallState == BubbleManager::AlertToDisconnected ) {
            mCallState = BubbleManager::AlertToDisconnected;
            return;
            }
        }

    mCallState = state;
}

BubbleManager::PhoneCallState BubbleHeader::callState() const
{
    return mCallState;
}

void BubbleHeader::setCallFlags( int flags )
{
    mCallFlags = flags;
}

void BubbleHeader::setCallFlag( BubbleManager::PhoneCallFlags flag )
{
    if ( flag == BubbleManager::Normal ) {
        mCallFlags = 0;
        return;
    }
    
    mCallFlags |= flag;
}

void BubbleHeader::removeCallFlag( BubbleManager::PhoneCallFlags flag )
{
    if ( flag == BubbleManager::Normal ) {
        return;
    }
    
    mCallFlags &= ~flag;
}

int BubbleHeader::callFlags() const
{
    return mCallFlags;
}

void BubbleHeader::setNumberType( BubbleManager::PhoneNumberType numberType )
{
    mNumberType = numberType;
}

BubbleManager::PhoneNumberType BubbleHeader::numberType() const
{
    return mNumberType;
}

void BubbleHeader::setText( 
    const QString& text,
    Qt::TextElideMode clip )
{
    mText = text;
    mTextClip = clip;
}

const QString& BubbleHeader::text() const
{
    return mText;
}

Qt::TextElideMode BubbleHeader::textClipDirection() const
{
    return mTextClip;
}

void BubbleHeader::setCli( 
    const QString& cli, 
    Qt::TextElideMode clip )
{
    mCli = cli;
    mCliClip = clip;
}

const QString& BubbleHeader::cli() const
{
    return mCli;
}

Qt::TextElideMode BubbleHeader::cliClipDirection() const
{
    return mCliClip;
}

void BubbleHeader::setSecondaryCli( 
    const QString& cli, 
    Qt::TextElideMode clip )
{
    mSecondaryCli = cli;
    mSecondaryCliClip = clip;
}

const QString& BubbleHeader::secondaryCli() const
{
    return mSecondaryCli;
}

Qt::TextElideMode BubbleHeader::secondaryCliClipDirection() const
{
    return mSecondaryCliClip;
}

void BubbleHeader::setTimerCost( 
    const QString& timerCost )
{
    mTimerCost = timerCost;
}

const QString& BubbleHeader::timerCost() const
{
    return mTimerCost;
}

void BubbleHeader::setCallImage( const QString& image )
{
    mCallImage = image;
}

const QString& BubbleHeader::callImage() const
{
    return mCallImage;
}

void BubbleHeader::setShowDefaultAvatar(bool show)
{
    mShowDefaultAvatar = show;
}

bool BubbleHeader::showDefaultAvatar() const
{
    return mShowDefaultAvatar;
}

void BubbleHeader::addAction ( HbAction* action )
{
    mActions.append( action );
}

const QList<HbAction*>& BubbleHeader::actions() const
{
    return mActions;    
}

void BubbleHeader::clearActions()
{
    mActions.clear();
}

bool BubbleHeader::isConference() const
{
    return false;
}

void BubbleHeader::setInConference( bool inConference )
{
    mIsInConference = inConference;
}

bool BubbleHeader::isInConference() const
{
    return mIsInConference;
}

void BubbleHeader::setExpandAction( HbAction* action )
{
    mExpandAction = action;
}

HbAction* BubbleHeader::expandAction() const
{
    return mExpandAction;
}

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
* Description: Conference call header
*
*/

#include "bubbleconferenceheader.h"

BubbleConferenceHeader::BubbleConferenceHeader()
{
    reset();
}

void BubbleConferenceHeader::reset()
{
    BubbleHeader::reset();

    mExpanded = false;
    mSelection = -1;

    foreach(BubbleHeader* header,mHeaders) {
        header->setInConference(false);
    }
    mHeaders.clear();

    mParticipantListActions.clear();
}

void BubbleConferenceHeader::addHeader(BubbleHeader* header)
{
    header->setInConference(true);
    mHeaders.append(header);
}

void BubbleConferenceHeader::removeHeader(int bubbleId)
{
    for(int i=0; i<mHeaders.count(); i++) {
        if (mHeaders.at(i)->bubbleId()==bubbleId) {
            mHeaders.at(i)->setInConference(false);
            mHeaders.removeAt(i);            
            break;
        }
    }
}

bool BubbleConferenceHeader::isConference() const
{
    return true;
}

QList<BubbleHeader*> BubbleConferenceHeader::headers() const
{
    return mHeaders;
}

void BubbleConferenceHeader::addParticipantListAction( HbAction* action )
{
    mParticipantListActions.append( action );
}

const QList<HbAction*>& BubbleConferenceHeader::participantListActions() const
{
    return mParticipantListActions;
}

void BubbleConferenceHeader::clearParticipantListActions()
{
    mParticipantListActions.clear();
}

void BubbleConferenceHeader::setExpanded(bool expanded)
{
    mExpanded = expanded;
}

bool BubbleConferenceHeader::isExpanded() const
{
    return mExpanded;
}

void BubbleConferenceHeader::setSelectedHeader(int bubbleId)
{
    mSelection = bubbleId;
}

int BubbleConferenceHeader::selectedHeader() const
{
    return mSelection;
}


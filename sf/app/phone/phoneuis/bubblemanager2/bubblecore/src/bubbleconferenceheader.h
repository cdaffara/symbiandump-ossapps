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

#ifndef BUBBLECONFERENCEHEADER_H
#define BUBBLECONFERENCEHEADER_H

#include "bubbleheader.h"

class BubbleConferenceHeader : public BubbleHeader
{
public:
    BubbleConferenceHeader();

    virtual void reset();

    void addHeader(BubbleHeader* header);
    void removeHeader(int bubbleId);

    QList<BubbleHeader*> headers() const;

    // header type
    virtual bool isConference() const;

    // participant list actions
    void addParticipantListAction( HbAction* action );
    const QList<HbAction*>& participantListActions() const;
    void clearParticipantListActions();

    // expanded
    void setExpanded(bool expanded);
    bool isExpanded() const;

    // selection in list
    void setSelectedHeader(int bubbleId);
    int selectedHeader() const;

private:
    QList<BubbleHeader*> mHeaders;
    QList<HbAction*> mParticipantListActions;
    bool mExpanded;
    int mSelection;
};

#endif // BUBBLECONFERENCEHEADER_H

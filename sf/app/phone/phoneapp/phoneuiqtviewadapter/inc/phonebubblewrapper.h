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
* Description:  Bubble wrapper.
*
*/

#ifndef PHONEBUBBLEWRAPPER_H
#define PHONEBUBBLEWRAPPER_H

#include <exception> // must be before e32base.h so uncaught_exception gets defined
#include <e32base.h>
#include <QObject>
#include <QMap>

class BubbleManagerIF;
class HbAction;

class PhoneBubbleWrapper : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    PhoneBubbleWrapper (BubbleManagerIF& bubble, QObject *parent = 0);
    
    /**
     * Destructor
     */
    virtual ~PhoneBubbleWrapper ();
    
    void updateCallState (int callId, int newState);
    
    int callId (int state);
    
    int createCallHeader (int callId);

    void removeCallHeader (int callId);
    
    void setState (int callId, int bubble, int callState);
    
    void setLabel (int bubble, const TDesC &text);
    
    void setCli (int bubble, const TDesC &cliText, Qt::TextElideMode elide);
    
    void setSecondaryCli (int bubble, const TDesC &cliText, Qt::TextElideMode elide);
    
    void setCallType (int bubble, int callType);

    BubbleManagerIF& bubbleManager ();
    
    int bubbleId (int callId);
    
    void setDivert (int bubble, bool enabled);
    
    void setCiphering(int bubble, bool indicatorAllowed, bool enabled);

    int activeCallCount();    
    
    QMap<int, int> callStates() const;
    
    QMap<int, int> bubbles() const;
    
    int createConferenceBubble(int callId, int callState,
            const TDesC &labelText, const TDesC &cliText);
    
    void setConferenceCallId(int callId);
    
    QList<int> conferenceCallList() const;
    
    void removeConferenceCallId(int callId);
    
    bool conferenceCallId(int callId) const;
    
    void removeCallFromConference(int callId);
    
    void removeConferenceBubble();
    
    int callIdByBubbleId(int bubbleId);
    
    void addToConferenceBubble();
    
    void setServiceId(int callId, int serviceId);
    
    int serviceIdByCallId(int callId) const;
    
    QMap<int,int> serviceIds() const;
    
private:

    int callIdForConference();
    
    bool getCallIdsForConference(int &first, int &second);
    
private:

    BubbleManagerIF& m_bubbleManager;
    
    // Call id - call state map
    QMap<int, int> m_callStates;
    
    // Call id - bubble id map
    QMap<int, int> m_bubbles;
    
    // Call id - service id map
    QMap<int, int> m_services;
    
    // Conference call id list
    QList<int>     m_conferenceList;
};

#endif // PHONEBUBBLEWRAPPER_H

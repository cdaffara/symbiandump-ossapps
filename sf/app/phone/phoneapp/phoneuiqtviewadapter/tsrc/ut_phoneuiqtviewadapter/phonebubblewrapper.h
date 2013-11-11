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
* Description: 
*
*/
#ifndef PHONEBUBBLEWRAPPERMOCK_H
#define PHONEBUBBLEWRAPPERMOCK_H

#include <QObject>
#include <QMap>
#include <e32cmn.h>
#include "bubblemanagerif.h"

extern bool m_setDivertCalled;
extern bool m_bubblewrapperbool;
extern int m_bubblewrapperint;
extern bool m_setStateCalled;
extern bool m_setLabelCalled;
extern bool m_setCliCalled;
extern bool m_setSecondaryCliCalled;
extern bool m_setCallTypeCalled;
extern int m_createCallHeaderInt;
extern int m_setStateCallId;
extern int m_setStateBubbleId;
extern int m_setStateCallState;
extern const TDesC *m_setLabelText;
extern int m_setLabelBubbleId;
extern int m_setCliBubbleId;
extern const TDesC *m_setCliText;
extern int m_setSecondaryCliBubbleId;
extern const TDesC *m_setSecondaryCliText;
extern int m_setCallTypeBubbleId;
extern int m_setCallTypeType;
extern bool m_bubbleIdCalled;
extern int m_bubbleIdCallId;
extern bool m_activeCallCountCalled;
extern QMap<int, int> m_callStateMap;
extern QMap<int, int> m_bubblesMap;
extern QList<int> m_conferenceCallList;
extern bool m_creataConferenceCalled;
extern bool m_removeConferenceCalled;
extern bool m_removeCallFromConference;
extern int m_bubble_id;
extern QMap<int,int> m_services;
extern bool m_setCipheringCalled;


class PhoneBubbleWrapper : public QObject
{
    Q_OBJECT
    
public:
    PhoneBubbleWrapper (BubbleManagerIF& bubble, QObject *parent = 0);
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
    int activeCallCount ();
    
    const QMap<int, int> callStates() const;
    
    QMap<int, int> bubbles();
    
    int createConferenceBubble(int callId, int callState, 
            const TDesC &labelText, const TDesC &cliText);
    
    void setConferenceCallId(int callId);
    
    QList<int> conferenceCallList();
    
    void removeConferenceCallId(int callId);
    
    bool conferenceCallId(int callId);
    
    void removeCallFromConference(int callId);
    
    void removeConferenceBubble();
    
    int callIdByBubbleId(int bubbleId);
    
    void addToConferenceBubble();
    
    void setServiceId(int callId, int serviceId);
    
    int serviceIdByCallId(int callId) const;
    
    QMap<int,int> serviceIds() const;
    
private:
    BubbleManagerIF& m_bubble;
};

#endif // PHONEBUBBLEWRAPPERMOCK_H

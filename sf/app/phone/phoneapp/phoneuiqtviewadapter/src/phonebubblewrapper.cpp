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
* Description:  Bubble Wrapper.
*
*/

#include <pevirtualengine.h>

#include "bubblemanagerif.h"
#include "phonebubblewrapper.h"
#include "phoneconstants.h"
#include "qtphonelog.h"

PhoneBubbleWrapper::PhoneBubbleWrapper (BubbleManagerIF& bubble, QObject *parent) :
    QObject (parent), m_bubbleManager (bubble)
{
}

PhoneBubbleWrapper::~PhoneBubbleWrapper ()
{
    
}

void PhoneBubbleWrapper::updateCallState (int callId, int newState)
{
    // TODO: +1 callId because value 0 causes problems
    //       when using QMap. Bug in S60 Qt?
    m_callStates [callId] = newState;
}

int PhoneBubbleWrapper::callId (int state)
{
    // Returns call id or -1 if not found
    // TODO: callId -1 because we +1 callId when inserted
    //       This is because callId 0 causes problems in QMap.
    int callId = m_callStates.key (state, -1);
    return callId;
}

int PhoneBubbleWrapper::createCallHeader (int callId)
{
    PHONE_TRACE
    int bubble;

    if (!m_bubbles.contains (callId)) {
        bubble = m_bubbleManager.createCallHeader ();
        m_bubbles.insert (callId, bubble);
    }
    else {
        bubble = m_bubbles [callId];    
    }
    
    return bubble;
}

void PhoneBubbleWrapper::removeCallHeader (int callId)
{
    PHONE_TRACE
    if (m_bubbles.contains (callId)) {
        m_bubbleManager.removeCallHeader (m_bubbles [callId]);
        m_bubbles.remove (callId);       
    }
}

BubbleManagerIF& PhoneBubbleWrapper::bubbleManager ()
{
    return m_bubbleManager;
}

void PhoneBubbleWrapper::setState (int callId, int bubble, int callState)
{
    BubbleManagerIF::PhoneCallState state = BubbleManagerIF::None;
    
    switch (callState) {
        case EPEStateIdle:
            state = BubbleManagerIF::Disconnected;
            break;
        case EPEStateDialing:
            state = BubbleManagerIF::Outgoing;
            break;
        case EPEStateRinging:
            if ( m_bubbles.count() > 1 ) {
                state = BubbleManagerIF::Waiting;
            }
            else {
                state = BubbleManagerIF::Incoming;
            }
            break;
        case EPEStateConnecting:
            state = BubbleManagerIF::Alerting;
            break;
        case EPEStateConnected:
            state = BubbleManagerIF::Active;
            break;
        case EPEStateHeld:
            state = BubbleManagerIF::OnHold;
            break;
        case EPEStateDisconnecting:
            state = BubbleManagerIF::Disconnected;
            break;
        case EPEStateConferenceIdle:
            state = BubbleManagerIF::Disconnected;
            break;
        case EPEStateConnectedConference:
            state = BubbleManagerIF::Active;
            break;
        case EPEStateHeldConference:
            state = BubbleManagerIF::OnHold;
            break;
        case EPEStateUnknown:
            state = BubbleManagerIF::None;
            break;
        default:
            break;
    }
    
    updateCallState (callId, callState);
    m_bubbleManager.setState (bubble, state);

}

void PhoneBubbleWrapper::setLabel (int bubble, const TDesC &text)
{
    QString labelText = QString::fromUtf16 (text.Ptr (), text.Length ());
    PHONE_DEBUG2("PhoneBubbleWrapper::setLabel, label:", labelText); 
    m_bubbleManager.setLabel (bubble, labelText, Qt::ElideRight);
}

void PhoneBubbleWrapper::setCli (int bubble, const TDesC &cliText, 
                                 Qt::TextElideMode elide)
{
    QString text = QString::fromUtf16 (cliText.Ptr (), cliText.Length ());
    PHONE_DEBUG2("PhoneBubbleWrapper::setCli, cli:", text); 
    m_bubbleManager.setCli (bubble, text, elide);
}

void PhoneBubbleWrapper::setSecondaryCli (int bubble, const TDesC &cliText,
                                          Qt::TextElideMode elide)
{
    QString text = QString::fromUtf16 (cliText.Ptr (), cliText.Length ());
    PHONE_DEBUG2("PhoneBubbleWrapper::setSecondaryCli, SecondaryCli:", text); 
    m_bubbleManager.setSecondaryCli (bubble, text, elide);
}

void PhoneBubbleWrapper::setCallType (int bubble, int callType)
{
    BubbleManagerIF::PhoneCallFlags callflag = BubbleManagerIF::Normal;

    switch (callType) {
        case EPECallTypeCSVoice:
            callflag = BubbleManagerIF::Normal;
            break;
        case EPECallTypeVideo:
            callflag = BubbleManagerIF::Video;
            break;
        case EPECallTypeVoIP:
            callflag = BubbleManagerIF::VoIPCall;
            break;
        default:
            break;
    }

    m_bubbleManager.setCallFlag (bubble, callflag, true);
}

int PhoneBubbleWrapper::bubbleId (int callId)
{
    return m_bubbles.value (callId, -1);
}

void PhoneBubbleWrapper::setDivert (int bubble, bool enabled)
{
    if (enabled) {
        BubbleManagerIF::PhoneCallFlags divertedFlag = BubbleManagerIF::Diverted;
        m_bubbleManager.setCallFlag (bubble, divertedFlag, true);
    }
}

void PhoneBubbleWrapper::setCiphering(int bubble, bool indicatorAllowed, bool enabled)
{
    if (indicatorAllowed && !enabled) {
        m_bubbleManager.setCallFlag (bubble, BubbleManagerIF::NoCiphering, true);
    } else {
        m_bubbleManager.setCallFlag (bubble, BubbleManagerIF::NoCiphering, false);
    }
}

int PhoneBubbleWrapper::activeCallCount()
{
    PHONE_TRACE
    int count(0);
    QMap<int, int> callStateList = callStates();
    
    for (int i=0; i<callStateList.size(); ++i) {
        if ( callStateList.values().at(i) == EPEStateConnected ||
             callStateList.values().at(i) == EPEStateConnectedConference || 
             callStateList.values().at(i) == EPEStateDisconnecting ||
             callStateList.values().at(i) == EPEStateHeld ||
             callStateList.values().at(i) == EPEStateHeldConference ) {
             count++;
         }
    }
    
    return count;
}

QMap<int, int> PhoneBubbleWrapper::callStates() const 
{
    PHONE_TRACE
    QMap<int, int> ret;
    
    for (int i=0; i<m_callStates.size(); ++i) {
        if ( false == m_conferenceList.contains(m_callStates.keys().at(i)) 
             && m_bubbles.keys().contains(m_callStates.keys().at(i)) ) {
            ret.insert(m_callStates.keys().at(i), 
                       m_callStates.value(m_callStates.keys().at(i)));
        }
    }
    
    return ret;
}

QMap<int, int> PhoneBubbleWrapper::bubbles() const
{
    PHONE_TRACE
    QMap<int, int> ret;
    
    for (int i=0; i<m_bubbles.size(); ++i) {
        if ( false == m_conferenceList.contains(m_bubbles.keys().at(i)) ) {
            ret.insert(m_bubbles.keys().at(i), m_bubbles.value(m_bubbles.keys().at(i)));
        }
    }
    
    return ret;
}

int PhoneBubbleWrapper::createConferenceBubble(
        int callId,
        int callState,
        const TDesC &labelText, 
        const TDesC &cliText)
{
    PHONE_TRACE
    int callId1;
    int callId2;
    int bubble = -1;
    if (getCallIdsForConference(callId1, callId2)) {
        m_bubbleManager.startChanges();
        bubble = m_bubbleManager.createConference( 
                bubbleId(callId1), 
                bubbleId(callId2) );
        
        setConferenceCallId(callId1);
        setConferenceCallId(callId2);
        
        m_bubbles.insert(callId,bubble);
        setState(callId, bubble, callState );
        setLabel(bubble, labelText);
        setCli (bubble, cliText, Qt::ElideRight);
        
        //Set conference bubble expanded if no other
        //bubbles (like waiting bubble).
        m_bubbleManager.setExpandedConferenceCallHeader(
                (1==bubbles().size()));
        
        m_bubbleManager.endChanges();
    }
    
    return bubble;
}

void PhoneBubbleWrapper::setConferenceCallId(int callId) 
{
    if ( false == m_conferenceList.contains(callId) ) {
        m_conferenceList.append(callId);
    }
}

void PhoneBubbleWrapper::removeConferenceCallId(int callId) 
{
    if ( m_conferenceList.contains(callId) ) {
        m_conferenceList.removeOne(callId);
    }
}

bool PhoneBubbleWrapper::conferenceCallId(int callId) const
{
    return m_conferenceList.contains(callId);
}

void PhoneBubbleWrapper::removeCallFromConference(int callId)
{
    PHONE_TRACE
    if (m_conferenceList.contains(callId)) {
        m_bubbleManager.startChanges();
        if(1<m_conferenceList.size()) {
            m_bubbleManager.removeRowFromConference(m_bubbles.value(callId));
            m_conferenceList.removeOne(callId);
            }
        else {
            m_bubbleManager.removeConference();

            if (m_bubbles.contains(KConferenceCallId)) {
                m_bubbles.remove(KConferenceCallId);
            } 
            m_conferenceList.clear();
        }
        
        m_bubbleManager.endChanges();        
    }
}

QList<int> PhoneBubbleWrapper::conferenceCallList() const
    {
    return m_conferenceList;
    }

void PhoneBubbleWrapper::removeConferenceBubble()
{
    if (0 < m_conferenceList.size()) {
        m_bubbleManager.removeConference();
        m_bubbleManager.setExpandedConferenceCallHeader(false);
        
        if (m_bubbles.contains(KConferenceCallId)) {
            m_bubbles.remove(KConferenceCallId);
        }        
        m_conferenceList.clear();  
    }
}

int PhoneBubbleWrapper::callIdByBubbleId(int bubbleId)
{
    return m_bubbles.key(bubbleId, -1);
}

void PhoneBubbleWrapper::addToConferenceBubble()
{
    if (m_conferenceList.size()) {
        int callId = callIdForConference();
        if (-1 != callId && 
                false == m_conferenceList.contains(callId)) {
            m_bubbleManager.startChanges();
            setConferenceCallId(callId);
            m_bubbleManager.addRowToConference(bubbleId(callId));
            m_bubbleManager.endChanges();
        }        
    }
}
 
int PhoneBubbleWrapper::callIdForConference()
{
    int callId = -1;
    QMap<int, int> bubblelist = bubbles();
    for (int i=0; i<bubblelist.size() && callId == -1; ++i) {
        if (KConferenceCallId != bubblelist.keys().at(i)) {
            if ( bubblelist.size() > 2 ) {
                if (EPEStateRinging != 
                        m_callStates.value(bubblelist.keys().at(i))) {
                    callId = bubblelist.keys().at(i);
                }
            } else {
                callId = bubblelist.keys().at(i);
            }
        }
    }
    
    return callId;
}

bool PhoneBubbleWrapper::getCallIdsForConference(int &first, int &second)
{
    first = -1;
    second = -1;
    
    if (2==m_bubbles.values().size()) {
        first = m_bubbles.keys().first();
        second = m_bubbles.keys().at(1);
    } else if (2<=m_bubbles.values().size()) {
        for (int i=0;i<m_callStates.values().size();++i) {
            if ( m_callStates.values().at(i) != EPEStateRinging ) {
                if (first==-1) {
                    first = m_callStates.keys().at(i);
                } else {
                    second = m_callStates.keys().at(i);
                    break;
                }
            }
        } // For
    }
        
    return (first!=-1 && second!=-1);
}

void PhoneBubbleWrapper::setServiceId(int callId, int serviceId)
{
    m_services[callId] = serviceId;
}

int PhoneBubbleWrapper::serviceIdByCallId(int callId) const
{
    return m_services.value(callId, -1);
}

QMap<int, int> PhoneBubbleWrapper::serviceIds() const 
{    
    return m_services;
}


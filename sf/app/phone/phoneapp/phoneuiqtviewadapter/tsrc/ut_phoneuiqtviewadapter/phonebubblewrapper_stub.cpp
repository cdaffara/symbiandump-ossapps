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

#include "phonebubblewrapper.h"

bool m_setDivertCalled;
bool m_bubblewrapperbool;
int m_bubblewrapperint;
bool m_setStateCalled;
bool m_setLabelCalled;
bool m_setCliCalled;
bool m_setSecondaryCliCalled;
bool m_setCallTypeCalled;
int m_createCallHeaderInt;
int m_setStateCallId;
int m_setStateBubbleId;
int m_setStateCallState;
const TDesC *m_setLabelText;
int m_setLabelBubbleId;
int m_setCliBubbleId;
const TDesC *m_setCliText;
int m_setSecondaryCliBubbleId;
const TDesC *m_setSecondaryCliText;
int m_setCallTypeBubbleId;
int m_setCallTypeType;
bool m_bubbleIdCalled;
int m_bubbleIdCallId;
bool m_activeCallCountCalled;
QMap<int, int> m_callStateMap;
QMap<int, int> m_bubblesMap;
QList<int> m_conferenceCallList;
bool m_creataConferenceCalled;
bool m_removeConferenceCalled;
bool m_removeCallFromConference;
bool m_removeCallHeaderCalled;
int m_bubble_id;
bool m_addToConferenceBubbleCalled;
QMap<int,int> m_services;
bool m_setCipheringCalled;

PhoneBubbleWrapper::PhoneBubbleWrapper (BubbleManagerIF& bubble, QObject *parent) : m_bubble(bubble)
{
    m_setDivertCalled = false;
    m_bubblewrapperbool = false;
    m_bubblewrapperint = -1;
    m_setStateCalled = false;
    m_setLabelCalled = false;
    m_setCliCalled = false;
    m_setSecondaryCliCalled = false;
    m_setCallTypeCalled = false;
    m_createCallHeaderInt = false;
    m_setStateCallId = -1;
    m_setStateBubbleId = -1;
    m_setStateCallState = -1;
    m_setLabelBubbleId = -1;
    m_setCliBubbleId = -1;
    m_setSecondaryCliBubbleId = -1;
    m_setCallTypeBubbleId = -1;
    m_setCallTypeType = -1;
    m_bubbleIdCalled = false;
    m_bubbleIdCallId = -1;
    m_activeCallCountCalled = false;
    m_creataConferenceCalled = false;
    m_removeCallFromConference = false;
    m_removeCallHeaderCalled = false;
    m_bubble_id = -2;
    m_addToConferenceBubbleCalled = false;
    m_setCipheringCalled = false;
}

PhoneBubbleWrapper::~PhoneBubbleWrapper () 
{
}

void PhoneBubbleWrapper::updateCallState (int callId, int newState) 
{
}

int PhoneBubbleWrapper::callId (int state) 
{ 
    return m_callStateMap.key(state, 0);
}

int PhoneBubbleWrapper::createCallHeader (int callId) 
{
    m_createCallHeaderInt = callId;
    return 1; 
}

void PhoneBubbleWrapper::removeCallHeader (int callId) 
{
    m_removeCallHeaderCalled = true;
}

void PhoneBubbleWrapper::setState (int callId, int bubble, int callState) 
{
    m_setStateCalled = true;
    m_setStateCallId = callId;
    m_setStateBubbleId = bubble;
    m_setStateCallState = callState;
}

void PhoneBubbleWrapper::setLabel (int bubble, const TDesC &text) 
{
    m_setLabelCalled = true;
    m_setLabelBubbleId = bubble;
    m_setLabelText = &text;
}

void PhoneBubbleWrapper::setCli (int bubble, const TDesC &cliText, Qt::TextElideMode elide) 
{
    m_setCliCalled = true;
    m_setCliBubbleId = bubble;
    m_setCliText = &cliText;
}

void PhoneBubbleWrapper::setSecondaryCli (int bubble, const TDesC &cliText, Qt::TextElideMode elide) 
{
    m_setSecondaryCliCalled = true;
    m_setSecondaryCliBubbleId = bubble;
    m_setSecondaryCliText = &cliText;
}

void PhoneBubbleWrapper::setCallType (int bubble, int callType) 
{
    m_setCallTypeCalled = true;
    m_setCallTypeBubbleId = bubble;
    m_setCallTypeType = callType;
}

BubbleManagerIF& PhoneBubbleWrapper::bubbleManager () 
{ 
    return m_bubble;
}

int PhoneBubbleWrapper::bubbleId (int callId) 
{ 
    m_bubbleIdCalled = true;
    m_bubbleIdCallId = callId;

    if (m_bubblesMap.contains(callId)) {
        return m_bubblesMap.value(callId);
    }
    
    if (-2 != m_bubble_id)
        return m_bubble_id;
        
    return 2; 
}

void PhoneBubbleWrapper::setDivert (int bubble, bool enabled)
{
    m_setDivertCalled = true;	
    m_bubblewrapperint = bubble;
    m_bubblewrapperbool = enabled;
}

void PhoneBubbleWrapper::setCiphering(int bubble, bool indicatorAllowed, bool enabled)
{
    m_setCipheringCalled = true;
}

int PhoneBubbleWrapper::activeCallCount ()
{
    m_activeCallCountCalled = true;
    return 0;
}

const QMap<int, int> PhoneBubbleWrapper::callStates() const 
{
    return m_callStateMap;
}

QMap<int, int> PhoneBubbleWrapper::bubbles() 
{
    return m_bubblesMap;
}

int PhoneBubbleWrapper::createConferenceBubble(
        int callId, int callState, const TDesC &labelText, const TDesC &cliText)
{
    m_creataConferenceCalled = true;
    for (int i=0; i<m_bubblesMap.size(); ++i) {
        setConferenceCallId( m_bubblesMap.keys()[i] );
    }
}

void PhoneBubbleWrapper::setConferenceCallId(int callId) 
{
    m_conferenceCallList.append(callId);
}

void PhoneBubbleWrapper::removeConferenceCallId(int callId) 
{
    if (m_conferenceCallList.contains(callId))
        m_conferenceCallList.removeOne(callId);
}

bool PhoneBubbleWrapper::conferenceCallId(int callId)
{
    return m_conferenceCallList.contains(callId);
}

void PhoneBubbleWrapper::removeCallFromConference(int callId)
{
    m_removeCallFromConference = true;
    removeConferenceCallId(callId);
}

QList<int> PhoneBubbleWrapper::conferenceCallList()
    {
    return m_conferenceCallList;
    }

void PhoneBubbleWrapper::removeConferenceBubble()
{
    m_removeConferenceCalled = true;
    m_conferenceCallList.clear();
}

int PhoneBubbleWrapper::callIdByBubbleId(int bubbleId)
{
    return m_bubblesMap.key(bubbleId, -1);
}

void PhoneBubbleWrapper::addToConferenceBubble()
{
    m_addToConferenceBubbleCalled = true;
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


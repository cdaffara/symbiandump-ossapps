/** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include <QDebug>
#include <QMap>
#include <smcmockclassincludes.h>
#include "phonebubblewrapper.h"

typedef QMap <int , int > QMappi;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::PhoneBubbleWrapper
// -----------------------------------------------------------------------------
//
PhoneBubbleWrapper::PhoneBubbleWrapper( 
        BubbleManagerIF & bubble,
        QObject * parent )
    :
    QObject(parent),
    m_bubbleManager(bubble)
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::~PhoneBubbleWrapper
// -----------------------------------------------------------------------------
//
PhoneBubbleWrapper::~PhoneBubbleWrapper(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::updateCallState
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::updateCallState( 
        int callId,
        int newState )
    {
    SMC_MOCK_METHOD2( void, int, callId, 
        int, newState )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::callId
// -----------------------------------------------------------------------------
//
int PhoneBubbleWrapper::callId( 
        int state )
    {
    SMC_MOCK_METHOD1( int, int, state )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::createCallHeader
// -----------------------------------------------------------------------------
//
int PhoneBubbleWrapper::createCallHeader( 
        int callId )
    {
    SMC_MOCK_METHOD1( int, int, callId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::removeCallHeader
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::removeCallHeader( 
        int callId )
    {
    SMC_MOCK_METHOD1( void, int, callId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::setState
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::setState( 
        int callId,
        int bubble,
        int callState )
    {
    SMC_MOCK_METHOD3( void, int, callId, 
        int, bubble, 
        int, callState )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::setLabel
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::setLabel( 
        int bubble,
        const TDesC & text )
    {
    SMC_MOCK_METHOD2( void, int, bubble, 
        const TDesC &, text )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::setCli
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::setCli( 
        int bubble,
        const TDesC & cliText,
        Qt::TextElideMode elide )
    {
    SMC_MOCK_METHOD3( void, int, bubble, 
        const TDesC &, cliText, 
        Qt::TextElideMode, elide )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::setSecondaryCli
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::setSecondaryCli( 
        int bubble,
        const TDesC & cliText,
        Qt::TextElideMode elide )
    {
    SMC_MOCK_METHOD3( void, int, bubble, 
        const TDesC &, cliText, 
        Qt::TextElideMode, elide )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::setCallType
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::setCallType( 
        int bubble,
        int callType )
    {
    SMC_MOCK_METHOD2( void, int, bubble, 
        int, callType )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::bubbleManager
// -----------------------------------------------------------------------------
//
BubbleManagerIF & PhoneBubbleWrapper::bubbleManager(  )
    {
    return m_bubbleManager;
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::bubbleId
// -----------------------------------------------------------------------------
//
int PhoneBubbleWrapper::bubbleId( 
        int callId )
    {
    SMC_MOCK_METHOD1( int, int, callId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::setDivert
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::setDivert( 
        int bubble,
        bool enabled )
    {
    SMC_MOCK_METHOD2( void, int, bubble, 
        bool, enabled )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::setCiphering
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::setCiphering( 
        int bubble,
        bool indicatorAllowed,
        bool enabled )
    {
    SMC_MOCK_METHOD3( void, int, bubble, 
        bool, indicatorAllowed, 
        bool, enabled )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::activeCallCount
// -----------------------------------------------------------------------------
//
int PhoneBubbleWrapper::activeCallCount(  )
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::callStates
// -----------------------------------------------------------------------------
//
QMap <int , int > PhoneBubbleWrapper::callStates(  ) const
    {
    SMC_MOCK_METHOD0( QMappi )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::bubbles
// -----------------------------------------------------------------------------
//
QMap <int , int > PhoneBubbleWrapper::bubbles(  ) const
    {
    SMC_MOCK_METHOD0( QMappi )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::createConferenceBubble
// -----------------------------------------------------------------------------
//
int PhoneBubbleWrapper::createConferenceBubble( 
        int callId,
        int callState,
        const TDesC & labelText,
        const TDesC & cliText )
    {
    SMC_MOCK_METHOD4( int, int, callId, 
        int, callState, 
        const TDesC &, labelText, 
        const TDesC &, cliText )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::setConferenceCallId
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::setConferenceCallId( 
        int callId )
    {
    SMC_MOCK_METHOD1( void, int, callId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::conferenceCallList
// -----------------------------------------------------------------------------
//
QList <int > PhoneBubbleWrapper::conferenceCallList(  ) const
    {
    SMC_MOCK_METHOD0( QList <int > )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::removeConferenceCallId
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::removeConferenceCallId( 
        int callId )
    {
    SMC_MOCK_METHOD1( void, int, callId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::conferenceCallId
// -----------------------------------------------------------------------------
//
bool PhoneBubbleWrapper::conferenceCallId( 
        int callId ) const
    {
    SMC_MOCK_METHOD1( bool, int, callId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::removeCallFromConference
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::removeCallFromConference( 
        int callId )
    {
    SMC_MOCK_METHOD1( void, int, callId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::removeConferenceBubble
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::removeConferenceBubble(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::callIdByBubbleId
// -----------------------------------------------------------------------------
//
int PhoneBubbleWrapper::callIdByBubbleId( 
        int bubbleId )
    {
    SMC_MOCK_METHOD1( int, int, bubbleId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::addToConferenceBubble
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::addToConferenceBubble(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::setServiceId
// -----------------------------------------------------------------------------
//
void PhoneBubbleWrapper::setServiceId( 
        int callId,
        int serviceId )
    {
    SMC_MOCK_METHOD2( void, int, callId, 
        int, serviceId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::serviceIdByCallId
// -----------------------------------------------------------------------------
//
int PhoneBubbleWrapper::serviceIdByCallId( 
        int callId ) const
    {
    SMC_MOCK_METHOD1( int, int, callId )
    }


// -----------------------------------------------------------------------------
// PhoneBubbleWrapper::serviceIds
// -----------------------------------------------------------------------------
//
QMap <int , int > PhoneBubbleWrapper::serviceIds(  ) const
    {
    SMC_MOCK_METHOD0( QMappi )
    }



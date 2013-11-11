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
* Description:  Phone UI's symbian adapter for Qt view.
*
*/

#include "phonecallheadermanager.h"
#include "phonecallheaderutil.h"
#include "phoneuiqtviewif.h"
#include "phonebubblewrapper.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamaudioavailability.h"
#include "tphonecmdparamaudiooutput.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamemergencycallheaderdata.h"
#include "tphonecmdparamstring.h"
#include "phonerssbase.h"

bool m_createCallHeaderCalled;
bool m_createEmergencyCallHeaderCalled;
int m_createCallHeaderCallId;
int m_updateCallHeaderCallId;
int m_updateCallHeaderRemoteInfoCallId;
int m_updateCallHeaderRemoteInfoAndLabelCallId;
int m_handleCipheringInfoChangeCallId;
int m_createConferenceBubbleCallId;
bool m_removeAllCallHeadersCalled;
bool m_removeConferenceBubbleCalled;
int m_removeCallFromConferenceCallId;
int m_setPrivateFromConferenceCallId;
int m_removeCallHeaderCallId;
bool m_isVoiceCall;

#include <bubblemanagerif.h>
#include <pevirtualengine.h>
#include <mpeengineinfo.h>


//CONSTANTS


inline Qt::TextElideMode clipToElide(
    TPhoneCmdParamCallHeaderData::TPhoneTextClippingDirection clip)
    {
    return clip == TPhoneCmdParamCallHeaderData::ELeft ? 
                   Qt::ElideLeft : Qt::ElideRight;
    }

PhoneCallHeaderManager::PhoneCallHeaderManager(PhoneBubbleWrapper &bubbleWrapper, 
        PhoneUIQtViewIF &view, QObject *parent) :
    QObject(parent),
    m_bubbleWrapper(bubbleWrapper),
    m_view(view),
    m_callHeaderUtil(0),
    m_resourceAdapter(0)
{
    
}

PhoneCallHeaderManager::~PhoneCallHeaderManager ()
{
    //delete m_callHeaderUtil;
}



void PhoneCallHeaderManager::setEngineInfo(MPEEngineInfo* engineInfo)
{
    //m_engineInfo = engineInfo;
}

void PhoneCallHeaderManager::createCallHeader(
    int callId)
{
    m_createCallHeaderCalled = true;
    m_createCallHeaderCallId = callId;
}

void PhoneCallHeaderManager::createEmergencyCallHeader(int callId)
{
    m_createEmergencyCallHeaderCalled = true;
    m_createCallHeaderCallId = callId;
}

void PhoneCallHeaderManager::removeCallHeader(int callId)
{
    m_removeCallHeaderCallId = callId;
}

void PhoneCallHeaderManager::updateCallHeader(int callId)
{
    m_updateCallHeaderCallId = callId;
}

void PhoneCallHeaderManager::handleCipheringInfoChange(int callId)
{
    m_handleCipheringInfoChangeCallId = callId;
}


void PhoneCallHeaderManager::createConferenceBubble(int callId)
{
    m_createConferenceBubbleCallId = callId;;
}


void PhoneCallHeaderManager::removeConferenceBubble()
{
    m_removeConferenceBubbleCalled = true;
}

void PhoneCallHeaderManager::removeCallFromConference(int callId)
{
    m_removeCallFromConferenceCallId = callId;
}


void PhoneCallHeaderManager::setPrivateFromConference(int callId)
{
    m_setPrivateFromConferenceCallId = callId;
}


void PhoneCallHeaderManager::setExpandedConferenceCallHeader()
{

}


void PhoneCallHeaderManager::removeAllCallHeaders()
{
    m_removeAllCallHeadersCalled = true;
}

// ---------------------------------------------------------------------------
//  PhoneCallHeaderUtil::IsVoiceCall
// ---------------------------------------------------------------------------
//
bool PhoneCallHeaderManager::isVoiceCall(int callId)
    {
    return m_isVoiceCall;
    }


PhoneCallHeaderUtil *PhoneCallHeaderManager::callHeaderUtil()
{
    /*if (!m_callHeaderUtil) {
        m_callHeaderUtil = new PhoneCallHeaderUtil(*m_engineInfo);
    }*/

    return 0;
}



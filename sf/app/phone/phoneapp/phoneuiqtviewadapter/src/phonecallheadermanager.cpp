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
#include "cphonemediatorfactory.h"
#include "cphonemediatorsender.h"
#include "phoneviewcommanddefinitions.h"
#include "qtphonelog.h"

#include <bubblemanagerif.h>
#include <pevirtualengine.h>
#include <mpeengineinfo.h>


//CONSTANTS


inline Qt::TextElideMode clipToElide(
        PhoneCallHeaderUtil::ClippingDirection clip)
    {
    return clip == PhoneCallHeaderUtil::ELeft ? 
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
    delete m_callHeaderUtil;
}



void PhoneCallHeaderManager::setEngineInfo(MPEEngineInfo* engineInfo)
{
    m_engineInfo = engineInfo;
}

void PhoneCallHeaderManager::createCallHeader(
    int callId)
{
    PHONE_TRACE
    m_bubbleWrapper.bubbleManager().startChanges ();

    if (m_bubbleWrapper.bubbleManager().isConferenceExpanded())
        {
        m_bubbleWrapper.bubbleManager().setExpandedConferenceCallHeader(false);
        }

    int bubble = m_bubbleWrapper.createCallHeader(callId);
    
    setCommonCallHeaderData(callId, bubble);

    const TDesC& callerImage(callHeaderUtil()->CallerImage(callId));
    if (callerImage.Length()) {
        QString imagePath =
            QString::fromUtf16(callerImage.Ptr(), callerImage.Length());
        m_bubbleWrapper.bubbleManager().setCallObjectImage(bubble,imagePath);
    } else {
        m_bubbleWrapper.bubbleManager().setCallObjectFromTheme(bubble);
    }
    m_bubbleWrapper.bubbleManager().endChanges ();

    sendVideoCallData(callId);
}

void PhoneCallHeaderManager::createEmergencyCallHeader(int callId)
{
    m_bubbleWrapper.bubbleManager().startChanges ();
    int bubble = m_bubbleWrapper.createCallHeader (callId);
    m_bubbleWrapper.setLabel(bubble, callHeaderUtil()->AttemptingEmergencyText());
    m_bubbleWrapper.setCli (bubble, callHeaderUtil()->EmergencyHeaderText(), Qt::ElideRight);
    m_bubbleWrapper.setState(callId, bubble, EPEStateDialing);
    m_bubbleWrapper.setCallType(bubble, callHeaderUtil()->CallType(callId));
    m_bubbleWrapper.setCiphering(bubble, callHeaderUtil()->SecureSpecified(), callHeaderUtil()->Ciphering(callId));
    m_bubbleWrapper.bubbleManager().setCallFlag(bubble, BubbleManagerIF::EmergencyCall, true);
    m_bubbleWrapper.bubbleManager().setCallObjectFromTheme(bubble);
    m_bubbleWrapper.bubbleManager().endChanges ();

}

void PhoneCallHeaderManager::removeCallHeader(int callId)
{
    m_bubbleWrapper.bubbleManager().startChanges();
    int bubble = m_bubbleWrapper.bubbles().value(callId, -1);
    if ( bubble != -1 ) {
        m_view.clearBubbleCommands(bubble);
        m_view.removeExpandAction(bubble);
        m_bubbleWrapper.removeCallHeader(callId);
    }
    m_bubbleWrapper.bubbleManager().endChanges();
}

void PhoneCallHeaderManager::updateCallHeader(int callId)
{
    int bubble = m_bubbleWrapper.bubbleId(callId);
    if ( -1 != bubble ) {
        m_bubbleWrapper.bubbleManager ().startChanges ();
        setCommonCallHeaderData(callId, bubble);
        m_bubbleWrapper.bubbleManager().endChanges ();
    }
}

void PhoneCallHeaderManager::handleCipheringInfoChange(int callId)
{
    PHONE_TRACE    
    if (callId == KPEConferenceCallID) {
        bool secure(true);
        for (int i=0; i<m_bubbleWrapper.conferenceCallList().count(); i++) {
            if ( false == m_engineInfo->IsSecureCall(m_bubbleWrapper.conferenceCallList().at(i)) ) {
                secure = false;
                break;
            }
        }
        m_bubbleWrapper.bubbleManager().startChanges();
        m_bubbleWrapper.setCiphering(m_bubbleWrapper.bubbleId(callId),
                                     m_engineInfo->SecureSpecified(),
                                     secure);
        m_bubbleWrapper.bubbleManager().endChanges();
    } else {
        int bubble = m_bubbleWrapper.bubbleId(callId);        
        if ( -1 != bubble ) {        
            m_bubbleWrapper.bubbleManager().startChanges();
            m_bubbleWrapper.setCiphering(bubble,
                                         m_engineInfo->SecureSpecified(),
                                         m_engineInfo->IsSecureCall(callId));
            m_bubbleWrapper.bubbleManager().endChanges();
        }
    }
}




void PhoneCallHeaderManager::createConferenceBubble(int callId)
{
    m_bubbleWrapper.bubbleManager().startChanges();

    PhoneCallHeaderUtil::ClippingDirection clipping;
    TBuf<KCntMaxTextFieldLength> cliText; 
    callHeaderUtil()->GetCli(callId, cliText, clipping);
    int bubble = m_bubbleWrapper.createConferenceBubble(
            callId, 
            callHeaderUtil()->CallState(callId), 
            callHeaderUtil()->LabelText(callId), 
            cliText );
    
    m_bubbleWrapper.setServiceId(callId, m_engineInfo->ServiceId(callId));
    m_bubbleWrapper.setCiphering(bubble, m_engineInfo->SecureSpecified(), m_engineInfo->IsSecureCall(callId));
    m_bubbleWrapper.bubbleManager().endChanges();
}


void PhoneCallHeaderManager::removeConferenceBubble()
{
    m_bubbleWrapper.bubbleManager().startChanges();
    m_view.removeExpandAction(m_bubbleWrapper.bubbleId(KConferenceCallId));
    m_view.clearParticipantListActions();
    m_bubbleWrapper.removeConferenceBubble();
    m_bubbleWrapper.bubbleManager().endChanges();
}

void PhoneCallHeaderManager::removeCallFromConference(int callId)
{
    m_bubbleWrapper.bubbleManager().startChanges();
    m_bubbleWrapper.removeCallFromConference(callId);
    int bubbleId = m_bubbleWrapper.bubbles().value(callId);
    m_view.clearBubbleCommands(bubbleId);
    m_view.removeExpandAction(bubbleId);
    m_bubbleWrapper.removeCallHeader(callId);
    m_bubbleWrapper.bubbleManager().endChanges();
}


void PhoneCallHeaderManager::setPrivateFromConference(int callId)
{
    m_bubbleWrapper.bubbleManager().startChanges();
    m_bubbleWrapper.bubbleManager().setExpandedConferenceCallHeader(false);
    m_bubbleWrapper.removeCallFromConference(callId);
    m_view.clearBubbleCommands(m_bubbleWrapper.bubbles().value(callId));
    m_bubbleWrapper.bubbleManager().endChanges();
}


void PhoneCallHeaderManager::setExpandedConferenceCallHeader()
{
    int callId = -1;

    if ( 1==m_bubbleWrapper.bubbles().keys().size()
         || (1<m_bubbleWrapper.bubbles().keys().size()
         && false == m_bubbleWrapper.callStates().values().contains(EPEStateRinging)
         && false == m_bubbleWrapper.callStates().values().contains(EPEStateDialing)
         && false == m_bubbleWrapper.callStates().values().contains(EPEStateConnecting)
         && false == m_bubbleWrapper.callStates().values().contains(EPEStateHeldConference))) {

        int bubbleId = m_bubbleWrapper.bubbleManager().expandedBubble();
        callId = m_bubbleWrapper.callIdByBubbleId(bubbleId);
    }

    m_bubbleWrapper.bubbleManager().startChanges();
    m_bubbleWrapper.bubbleManager().setExpandedConferenceCallHeader(
            (KConferenceCallId == callId));
    m_bubbleWrapper.bubbleManager().endChanges();
}


void PhoneCallHeaderManager::removeAllCallHeaders()
{
    PHONE_TRACE
    if (m_bubbleWrapper.conferenceCallList().size()) {
        removeConferenceBubble();
    }

    QList<int> callIds = m_bubbleWrapper.bubbles().keys();

    for (int i=0; i<callIds.size(); ++i) {
        int callId = callIds.at(i);
        if (KEmergencyCallId != callId) {
            m_bubbleWrapper.bubbleManager().startChanges();
            int bubble = m_bubbleWrapper.bubbles().value(callId);
            m_view.clearBubbleCommands(bubble);
            m_view.removeExpandAction(bubble);
            m_bubbleWrapper.removeCallHeader (callId);
            m_bubbleWrapper.bubbleManager().endChanges();
        }
    }
}

// ---------------------------------------------------------------------------
//  PhoneCallHeaderUtil::IsVoiceCall
// ---------------------------------------------------------------------------
//
bool PhoneCallHeaderManager::isVoiceCall(int callId)
    {
    return callHeaderUtil()->IsVoiceCall(callId);
    }



void PhoneCallHeaderManager::setCommonCallHeaderData(int callId, int bubble)
    {
    m_bubbleWrapper.setState(callId, bubble, 
            callHeaderUtil()->CallState(callId));
    
    PhoneCallHeaderUtil::ClippingDirection cliClip;
    PhoneCallHeaderUtil::ClippingDirection secondaryCliClip;
    TBuf<KCntMaxTextFieldLength> cliText;
    TBuf<KCntMaxTextFieldLength> secondaryCliText;
    
    callHeaderUtil()->GetCliTexts(callId, cliText, 
            cliClip, secondaryCliText, secondaryCliClip );
    
    m_bubbleWrapper.setCli(bubble, cliText, clipToElide(cliClip));
    m_bubbleWrapper.setSecondaryCli(bubble, 
            secondaryCliText, clipToElide(secondaryCliClip));
    
    m_bubbleWrapper.setServiceId(callId, callHeaderUtil()->ServiceId(callId));
    m_bubbleWrapper.setLabel(bubble, callHeaderUtil()->LabelText(callId));
    m_bubbleWrapper.setCallType(bubble, callHeaderUtil()->CallType(callId));
    
    m_bubbleWrapper.bubbleManager().setCallFlag(
            bubble, BubbleManagerIF::EmergencyCall, callId == KEmergencyCallId);
    
    m_bubbleWrapper.setDivert(bubble, 
            callHeaderUtil()->IsCallForwarded(callId));
    m_bubbleWrapper.setCiphering(bubble, callHeaderUtil()->SecureSpecified(), 
            callHeaderUtil()->Ciphering(callId));
    }

void PhoneCallHeaderManager::sendVideoCallData(int callId)
    {
    TPhoneCmdParamCallHeaderData callHeaderParam;
    
    PhoneCallHeaderUtil::ClippingDirection clipping;
    TBuf<KCntMaxTextFieldLength> cliText; 
    callHeaderUtil()->GetCli(callId, cliText, clipping);
    TPhoneCmdParamCallHeaderData::TPhoneTextClippingDirection clip = 
            clipping == PhoneCallHeaderUtil::ERight ?
                TPhoneCmdParamCallHeaderData::ERight :
                TPhoneCmdParamCallHeaderData::ELeft;
    
    callHeaderParam.SetCLIText(cliText, clip);    
    callHeaderParam.SetCallState(callHeaderUtil()->CallState(callId));
    callHeaderParam.SetCallType(callHeaderUtil()->CallType(callId));
    callHeaderParam.SetRemotePhoneNumber(
            callHeaderUtil()->RemotePhoneNumber(callId));

    CPhoneMediatorFactory::Instance()->Sender()->
            SendEvent(EPhoneViewCreateCallHeader, callId, callHeaderParam);
    }

PhoneCallHeaderUtil *PhoneCallHeaderManager::callHeaderUtil()
{
    if (!m_callHeaderUtil) {
        m_callHeaderUtil = new PhoneCallHeaderUtil(*m_engineInfo);
    }

    return m_callHeaderUtil;
}



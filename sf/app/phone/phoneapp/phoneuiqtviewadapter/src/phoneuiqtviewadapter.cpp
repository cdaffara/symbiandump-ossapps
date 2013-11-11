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

#include "phoneuiqtviewadapter.h"
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
#include "tphonecmdparamkeycapture.h"
#include "cphonepubsubproxy.h"
#include "pevirtualengine.h"
#include "cphoneringingtonecontroller.h"
#include "phoneresourceadapter.h"
#include "phoneui.hrh"
#include "cphonemediatorfactory.h"
#include "phoneuiqtbuttonscontroller.h"
#include "phoneconstants.h"
#include "phonenotecontroller.h"
#include "telephonyservice.h"
#include "phoneuicommandcontroller.h"
#include "phonemessagecontroller.h"
#include "phoneindicatorcontroller.h"
#include "qtphonelog.h"
#include "phonevisibilityhandler.h"
#include "phoneapplauncher.h"
#include "cphonecenrepproxy.h"
#include "phonecallheadermanager.h"

#include <bubblemanagerif.h>
#include <hbaction.h>
#include <pevirtualengine.h>
#include <QtGlobal>
#include <xqserviceutil.h>
#include <hbinstance.h>
#include <QKeyEvent>
#include <hbextendedlocale.h>
#include <hbi18ndef.h>
#include <eikenv.h>
#include <w32std.h>
#include <hbstringutil.h>
#include <xqaiwdecl.h>
#include <mpeengineinfo.h>


//CONSTANTS
static const int PHONE_CALL_NOT_FOUND = -1;

const TUid KCRUidTelTouchButtons = {0x2001B2E6};
const TUint32 KTelIncallTouchButtons   = 0x00000001;


inline Qt::TextElideMode clipToElide(
    TPhoneCmdParamCallHeaderData::TPhoneTextClippingDirection clip)
    {
    return clip == TPhoneCmdParamCallHeaderData::ELeft ? 
                   Qt::ElideLeft : Qt::ElideRight;
    }

PhoneUIQtViewAdapter::PhoneUIQtViewAdapter (PhoneUIQtViewIF &view, QObject *parent) :
    QObject (parent),
    m_view (view),
    m_bubbleWrapper(0),
    m_ringingtonecontroller(0),
    m_resourceAdapter(0),
    m_noteController(0),
    m_telephonyService(0),
    m_uiCommandController(0),
    m_messageController(0),
    m_indicatorController(0),
    m_phoneCallHeaderManager(0),
    m_dialpadAboutToClose(false),
    m_homeScreenToForeground(false),
    m_visibilityHandler(0),
    m_appLauncher(0),
    m_clearDialpadOnClose(true),
    m_speakerAsDefaultButton(false),
    m_ringingTonePlaying(false)
{
    m_bubbleWrapper = new PhoneBubbleWrapper(m_view.bubbleManager(), this);
    m_noteController = new PhoneNoteController(this);
    m_uiCommandController = new PhoneUiCommandController(view, this);
    m_phoneCallHeaderManager = new PhoneCallHeaderManager(*m_bubbleWrapper, m_view, this);

    TRAPD( error, m_ringingtonecontroller = CPhoneRingingToneController::NewL () );
    qt_symbian_throwIfError(error);
    m_resourceAdapter = PhoneResourceAdapter::Instance(this);

    // display initial toolbar to make client area size correct
    TPhoneCmdParamInteger intParam;
    intParam.SetInteger(m_resourceAdapter->defaultToolbarResourceId());
    setToolbarButtons(&intParam);

    m_telephonyService = new TelephonyService (this, this);
    m_visibilityHandler = new PhoneVisibilityHandler(view, this);
    m_indicatorController = new PhoneIndicatorController(*m_visibilityHandler,this);
    m_appLauncher = new PhoneAppLauncher(this);
    m_messageController = new PhoneMessageController(*m_appLauncher, this);
    
    int touchButton(-1);
    if ( KErrNone == CPhoneCenRepProxy::Instance()->GetInt(
            KCRUidTelTouchButtons,
            KTelIncallTouchButtons,
            touchButton ) ) {
        if ( touchButton == 2 ) {
            m_speakerAsDefaultButton = true;
        }    
    }
    
}

PhoneUIQtViewAdapter::~PhoneUIQtViewAdapter ()
{
    delete m_ringingtonecontroller;
}

void PhoneUIQtViewAdapter::ExecuteCommandL (TPhoneViewCommandId aCmdId)
{
    switch (aCmdId) {
    case EPhoneAppShutDown:
        {
        m_view.shutdownPhoneApp();
        }
        break;

    case EPhoneViewBringPhoneAppToForeground:
        bringToForeground();
        break;
        
    case EPhoneViewMuteRingToneOnAnswer:
        m_ringingtonecontroller->MuteRingingToneOnAnswer();
        break;

    case EPhoneViewStopRingTone:
        m_ringingTonePlaying = false;
        m_ringingtonecontroller->StopPlaying();
        break;

    case EPhoneViewMuteRingTone:
        m_ringingTonePlaying = false;
        m_ringingtonecontroller->MuteRingingTone();
        break;

    case EPhoneViewHideNaviPaneAudioVolume:
        m_view.removeVolumeSlider ();
        break;

    case EPhoneViewOpenDialer:
        openDialpad();
        break;

    case EPhoneViewRemoveConferenceBubble:
        removeConferenceBubble();
        break;

    case EPhoneViewSendToBackground:
    case EPhoneViewBringIdleToForeground:
        m_visibilityHandler->sendToBackground(m_homeScreenToForeground);
        m_homeScreenToForeground = false;
        break;
    case EPhoneViewRemoveAllCallHeaders:
        removeAllCallHeaders();
        break;
    case EPhoneViewRemoveNumberEntry:
        removeDialpad();
        break;
    case EPhoneViewClearNumberEntryContent: 
        clearDialpad();
        break;
    case EPhoneViewAddToConference:
        addToConference();
        break;
    case EPhoneViewRemoveDtmfNote:
        m_noteController->removeDtmfNote();
        break;
    case EPhoneViewRemoveNote:
        m_noteController->removeNote();
        break;
    case EPhoneViewRemoveQuery:
        m_noteController->removeQuery();
        break;
    case EPhoneViewRemovePhoneDialogs:
        m_noteController->removeNote();
        m_noteController->removeQuery();
        break;
    case EPhoneViewRemoveGlobalNote:
    case EPhoneViewRemoveGlobalWaitNote:
        m_noteController->removeGlobalWaitNote();
        break;
    case EPhoneViewUpdateFSW:
        setHidden(true);
        break;
    default:
        break;
    }
 CPhoneMediatorFactory::Instance()->Sender()->SendEvent( aCmdId );
}

void PhoneUIQtViewAdapter::ExecuteCommandL (TPhoneViewCommandId aCmdId, TInt aCallId)
{
    switch (aCmdId) {
    case EPhoneViewRemoveCallHeader:
        {
	        m_phoneCallHeaderManager->removeCallHeader(aCallId);
        
	        if (!m_bubbleWrapper->bubbles().count()) {
	            m_indicatorController->clearActiveCallData();
	        }
        }
        break;
    case EPhoneViewRemoveFromConference:
        removeCallFromConference(aCallId);
        break;
    case EPhoneViewPrivateFromConference:
        setPrivateFromConference(aCallId);
        break;
    case EPhoneViewCreateCallHeader:
        createCallHeader(aCallId);
        break;
    case EPhoneViewUpdateBubble:
        updateCallHeader(aCallId);
        setExpandActions();
        break;
    case EPhoneViewCreateConference:
        createConferenceBubble(aCallId);
        break;
    case EPhoneViewCipheringInfoChange:
        handleCipheringInfoChange(aCallId);
        break;
    case EPhoneViewCreateEmergencyCallHeader:
        createEmergencyCallHeader(aCallId);
        break;
    default:
        break;
    }
   CPhoneMediatorFactory::Instance()->Sender()->SendEvent( aCmdId, aCallId );
}

void PhoneUIQtViewAdapter::ExecuteCommandL (TPhoneViewCommandId aCmdId, TPhoneCommandParam* aCommandParam)
{
    switch (aCmdId) {
    case EPhoneViewPlayRingTone:
        m_ringingTonePlaying = true;
        m_ringingtonecontroller->PlayRingToneL( aCommandParam );
        break;
    case EPhoneViewSetNaviPaneAudioVolume:
        setAudioVolumeSliderValue (aCommandParam);
        break;
    case EPhoneViewActivateMuteUIChanges:
        setMuteIndication(aCommandParam);
        break;
    case EPhoneViewGetCountOfActiveCalls:
        activeCallCount(aCommandParam);
        break;
    case EPhoneViewActivateAudioPathUIChanges:
        setAudioPath(aCommandParam);
        break;
    case EPhoneViewGetIsConference:
        isConference(aCommandParam);
        break;    
    case EPhoneViewShowGlobalNote:
        showGlobalNote(aCommandParam);
        break;
    case EPhoneViewSetNumberEntryVisible:
        setDialpadVisibility(aCommandParam);
        break;
    case EPhoneViewGetNumberEntryCount:
        getDialpadStringLength(aCommandParam);
        break;
    case EPhoneViewGetNumberFromEntry:
        getNumberFromDialpad(aCommandParam);
        break;
    case EPhoneViewShowNote:
        showNote(aCommandParam);
        break;
    case EPhoneViewShowQuery:
        m_noteController->showQuery(aCommandParam);
        break;
    case EPhoneViewOpenSoftRejectEditor:
        m_visibilityHandler->hideDeviceDialogs(false);
        m_messageController->openSoftRejectMessageEditor(aCommandParam);
        break;
    case EPhoneViewSetGlobalNotifiersDisabled:
        hideDeviceDialogs(aCommandParam);
        break;
    case EPhoneViewLaunchLogs: 
        openLogs(aCommandParam);
        break;
    default:
        break;
    }
    CPhoneMediatorFactory::Instance()->Sender()->SendEvent( aCmdId, *aCommandParam );
}

void PhoneUIQtViewAdapter::ExecuteCommandL (TPhoneViewCommandId aCmdId, TInt aCallId,
    TPhoneCommandParam *aCommandParam)
{
    switch (aCmdId) {
    case EPhoneViewGetCallExistsInConference:
        conferenceCallId(aCallId, aCommandParam);
        break;
    case EPhoneViewUpdateCallHeaderCallDuration:
    {
        TPhoneCmdParamInteger *time = static_cast<TPhoneCmdParamInteger *>(aCommandParam);
        QString duration = convertDuration(time->Integer());
        PHONE_DEBUG2("call duration:", duration);
        int bubbleId = m_bubbleWrapper->bubbleId(aCallId);
        
        if (-1 != bubbleId) {
            m_bubbleWrapper->bubbleManager().updateCallTime(bubbleId, duration);
        }
        break;
    }

    default:
        break;
    }
    CPhoneMediatorFactory::Instance()->Sender()->SendEvent( aCmdId, aCallId, *aCommandParam );
}

void PhoneUIQtViewAdapter::ExecuteCommandL (TPhoneViewCommandId aCmdId, TInt aCallId,
    TDesC &aMessage)
{
    switch (aCmdId) {

    default:
        break;
    }
    CPhoneMediatorFactory::Instance()->Sender()->SendEvent( aCmdId, aCallId, &aMessage );
}

TPhoneViewResponseId PhoneUIQtViewAdapter::HandleCommandL (TPhoneViewCommandId aCmdId)
{
    TPhoneViewResponseId response = EPhoneViewResponseSuccess;

    switch (aCmdId) {
    case EPhoneIsDTMFDialerVisible:
    case EPhoneIsCustomizedDialerVisible:
        // TODO: currently not supported
        response = EPhoneViewResponseFailed;
        break;
    case EPhoneViewGetNumberEntryIsVisibleStatus:
    case EPhoneViewGetNumberEntryIsUsedStatus: // Fall through
        {
        response = (m_view.isDialpadVisible() && !m_dialpadAboutToClose) ?
                        EPhoneViewResponseSuccess :
                        EPhoneViewResponseFailed;
        break;
        }
    default:
        break;
    }

    return response;
}

TPhoneViewResponseId PhoneUIQtViewAdapter::HandleCommandL (TPhoneViewCommandId aCmdId,
    TPhoneCommandParam *aCommandParam)
{
    TPhoneViewResponseId viewResponse = EPhoneViewResponseSuccess;

    switch (aCmdId) {
    case EPhoneViewGetCallIdByState:
        viewResponse = callIdByState (aCommandParam);
        break;
    case EPhoneViewSelectedConfMember:
        viewResponse = getSelectedConferenceMember (aCommandParam);
        break;
    default:
        break;
    }

    return viewResponse;
}

void PhoneUIQtViewAdapter::ExecuteCommand (TPhoneViewCommandId aCmdId)
{
    switch (aCmdId) {
    case EPhoneSetConferenceExpanded:
        m_bubbleWrapper->bubbleManager ().startChanges ();
        m_bubbleWrapper->bubbleManager().setExpandedConferenceCallHeader(
                m_bubbleWrapper->conferenceCallList().size());
        m_bubbleWrapper->bubbleManager ().endChanges ();
        break;
    case EPhoneViewBeginUpdate:
        m_bubbleWrapper->bubbleManager ().startChanges ();
        break;
    case EPhoneViewEndUpdate:
        m_bubbleWrapper->bubbleManager ().endChanges ();
        break;
    case EPhoneViewOpenContacts:
        openContacts();
        break;
    default:
        break;
    }
}

void PhoneUIQtViewAdapter::ExecuteCommand (TPhoneViewCommandId aCmdId, TPhoneCommandParam *aCommandParam)
{
    switch (aCmdId) {
    case EPhoneViewGetAudioVolumeLevel:
        writeAudioVolumeLevel (aCommandParam);
        break;
    case EPhoneViewGetNumberFromEntry:
        getNumberFromDialpad(aCommandParam);
        break;
    case EPhoneViewBackButtonActive: {
        TPhoneCmdParamBoolean *param = static_cast<TPhoneCmdParamBoolean *>(aCommandParam);
        m_view.setBackButtonVisible(param->Boolean());
		}
        break;
    case EPhoneViewHsToForegroundAfterCall: {
        TPhoneCmdParamBoolean *param = static_cast<TPhoneCmdParamBoolean *>(aCommandParam);
        m_homeScreenToForeground = param->Boolean();
        }
        break;
    case EPhoneViewSetIhfFlag: {
        TPhoneCmdParamBoolean *param = static_cast<TPhoneCmdParamBoolean *>(aCommandParam);

        PhoneResourceAdapter::Instance()->buttonsController()->
                setButtonFlags(PhoneUIQtButtonsController::Ihf, 
                               param->Boolean());
        }
        break;
    case EPhoneViewSetMuteFlag: {
        TPhoneCmdParamBoolean *param = static_cast<TPhoneCmdParamBoolean *>(aCommandParam);

        PhoneResourceAdapter::Instance()->buttonsController()->
                setButtonFlags(PhoneUIQtButtonsController::Mute, 
                               param->Boolean());
        }
        break;
    case EPhoneViewSetBlueToothFlag: {
        TPhoneCmdParamBoolean *param = static_cast<TPhoneCmdParamBoolean *>(aCommandParam);

        PhoneResourceAdapter::Instance()->buttonsController()->
                setButtonFlags(PhoneUIQtButtonsController::Btaa, 
                               param->Boolean());
        }
        break;
    case EPhoneViewSetBluetoothAvailableFlag: {
        TPhoneCmdParamBoolean *param = static_cast<TPhoneCmdParamBoolean *>(aCommandParam);
    
        PhoneResourceAdapter::Instance()->buttonsController()->
                setButtonFlags(PhoneUIQtButtonsController::BluetoothAvailable, 
                               param->Boolean());
        }
        break;
    case EPhoneViewStartCapturingKey: {
        TPhoneCmdParamKeyCapture *captureParam = 
                static_cast<TPhoneCmdParamKeyCapture *>(aCommandParam);        
        Qt::Key qtKey;
        
        if (convertKey(captureParam->KeyCode(), qtKey)) {
            m_view.captureKey(qtKey, true);
        }
    }
    break;
    case EPhoneViewSetSoftRejectDimmed: {
        TPhoneCmdParamBoolean *param = static_cast<TPhoneCmdParamBoolean *>(aCommandParam);

        PhoneResourceAdapter::Instance()->buttonsController()->
                setButtonFlags(PhoneUIQtButtonsController::DisableSoftReject, 
                               param->Boolean());
        }
        break;
    case EPhoneViewStopCapturingKey: {
        TPhoneCmdParamKeyCapture *captureParam = 
                static_cast<TPhoneCmdParamKeyCapture *>(aCommandParam);        
        Qt::Key qtKey;
        
        if (convertKey(captureParam->KeyCode(), qtKey)) {
            m_view.captureKey(qtKey, false);
        }
    }
    break;
    case EPhoneViewIsRingingTonePlaying: {
        TPhoneCmdParamBoolean *param = static_cast<TPhoneCmdParamBoolean *>(aCommandParam);
        param->SetBoolean(m_ringingTonePlaying);
        }
    break;
    case EPhoneViewUpdateCba:
        setToolbarButtons(aCommandParam);
        break;
    case EPhoneViewGetExpandedBubbleCallId:
        expandedBubbleCallId(aCommandParam);
        break;
    case EPhoneViewSetTouchPaneButtons:
        setTouchButtons (aCommandParam);
        setExpandActions();
        break;
    default:
        break;
    }
}

const TDesC& PhoneUIQtViewAdapter::FetchContent()
{
    return KNullDesC;
}

void PhoneUIQtViewAdapter::setEngineInfo(MPEEngineInfo* engineInfo)
{
    m_phoneCallHeaderManager->setEngineInfo(engineInfo);
    m_engineInfo = engineInfo;
}

void PhoneUIQtViewAdapter::dialpadClosed()
{
    if (m_clearDialpadOnClose) {
        m_view.clearDialpad();
    }
    m_dialpadAboutToClose = true;
    m_clearDialpadOnClose = true;
    setCallMenu();
}

PhoneNoteController* PhoneUIQtViewAdapter::noteController() const
{
    return m_noteController;
}

void PhoneUIQtViewAdapter::keyReleased(QKeyEvent */*event*/)
{
    if (m_view.isDialpadVisible() &&
        false == m_dialpadAboutToClose ) {
        setDialpadMenu();
    }
}

void PhoneUIQtViewAdapter::handleWindowActivated()
{
    m_view.captureKey(Qt::Key_Yes, true);
}

void PhoneUIQtViewAdapter::handleWindowDeactivated()
{
    m_view.captureKey(Qt::Key_Yes, false);
}

void PhoneUIQtViewAdapter::onFocusLost()
{
    m_indicatorController->enableActiveCallIndicator();
}

void PhoneUIQtViewAdapter::onFocusGained()
{
    m_indicatorController->disableActiveCallIndicator();
}

void PhoneUIQtViewAdapter::createCallHeader(
    int callId )
{
    m_phoneCallHeaderManager->createCallHeader(callId);
    
    if (1 == m_bubbleWrapper->bubbles().keys().count()) {
        setHidden(false);
    }

    if( m_phoneCallHeaderManager->isVoiceCall(callId) ){
        m_indicatorController->setActiveCallData();
    }

}

void PhoneUIQtViewAdapter::createEmergencyCallHeader(int callId)
{
    m_phoneCallHeaderManager->createEmergencyCallHeader(callId);    
    m_indicatorController->setActiveCallData();
}

void PhoneUIQtViewAdapter::updateCallHeader(int callId)
{
    PHONE_DEBUG("PhoneUIQtViewAdapter::updateCallHeaderState");

    m_phoneCallHeaderManager->updateCallHeader(callId);
    if ( m_phoneCallHeaderManager->isVoiceCall(callId) ) {
         m_indicatorController->setActiveCallData();
    }
}

void PhoneUIQtViewAdapter::handleCipheringInfoChange(int callId)
{
    m_phoneCallHeaderManager->handleCipheringInfoChange(callId);
}

TPhoneViewResponseId PhoneUIQtViewAdapter::callIdByState (TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdCallStateData);

    TPhoneCmdParamCallStateData &stateParam =
        static_cast<TPhoneCmdParamCallStateData &>(*commandParam);

    stateParam.SetCallId (m_bubbleWrapper->callId (stateParam.CallState ()));
    return EPhoneViewResponseSuccess;
}

void PhoneUIQtViewAdapter::setTouchButtons (TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdBoolean);

    TPhoneCmdParamBoolean &param =
        static_cast<TPhoneCmdParamBoolean &>(*commandParam);
    bool emergencyCall = param.Boolean();

    m_bubbleWrapper->bubbleManager ().startChanges ();

    QList<int> bubbles = m_bubbleWrapper->bubbles().keys();

    m_resourceAdapter->buttonsController()->setButtonFlags(
            PhoneUIQtButtonsController::DisableJoin,
            (5 <= m_bubbleWrapper->conferenceCallList().count()));
    
    setCommonButtonFlags();

    for (int j = 0; j < bubbles.size(); ++j){
        int callId = bubbles.at(j);
        // Clear bubble
        m_view.clearBubbleCommands(m_bubbleWrapper->bubbles().value(callId));

		// Get bubble actions by call type
        QMap<PhoneAction::ActionType, PhoneAction *> actions =
            m_uiCommandController->pushButtonActionsForCall(
                               m_bubbleWrapper->callStates().value(callId),
                               emergencyCall,
                               m_bubbleWrapper->callStates(),
                               m_bubbleWrapper->serviceIds(),
                               m_bubbleWrapper->serviceIdByCallId(callId),
                               callId);

        QList<PhoneAction *> values = actions.values();
        for (int i = 0; i < values.size (); ++i) {
            PhoneAction *action = values.at (i);
            m_view.addBubbleCommand(m_bubbleWrapper->bubbles().value(callId),
                                    *action);
            delete action;
        }
    }

    setExpandedConferenceCallHeader();
    setBubbleSelectionFlag();
    setMenu();

    m_bubbleWrapper->bubbleManager ().endChanges ();
}

void PhoneUIQtViewAdapter::setToolbarButtons (TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdInteger);

    TPhoneCmdParamInteger &intParam =
        static_cast<TPhoneCmdParamInteger &>(*commandParam);

    setCommonButtonFlags();
    
    int callId(-1);
    int serviceId(-1);
    if ( 0<m_bubbleWrapper->callStates().keys().size() ) {
        int bubbleId = m_bubbleWrapper->bubbleManager().expandedBubble();
        callId = m_bubbleWrapper->callIdByBubbleId(bubbleId);
        serviceId = m_bubbleWrapper->serviceIdByCallId(callId);
    }
    
    QList<PhoneAction*> actions = m_uiCommandController->toolBarActions(
                                        intParam.Integer(),
                                        m_bubbleWrapper->callStates(),
                                        m_bubbleWrapper->serviceIds(),
                                        serviceId,
                                        callId );
    
    if (actions.count()) {
        m_view.setToolbarActions(actions);
    }

    qDeleteAll(actions);
}

void PhoneUIQtViewAdapter::writeAudioVolumeLevel (TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdInteger);

    TPhoneCmdParamInteger &intParam =
        static_cast<TPhoneCmdParamInteger &>(*commandParam);
    intParam.SetInteger (m_view.volumeSliderValue ());
}

void PhoneUIQtViewAdapter::setAudioVolumeSliderValue (TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdInteger);

    TPhoneCmdParamInteger &intParam =
        static_cast<TPhoneCmdParamInteger &>(*commandParam);

    m_view.setVolumeSliderValue(intParam.Integer(),
                                EPhoneInCallCmdSetVolumeLevel,
                                KPhoneVolumeMaxValue,
                                KPhoneVolumeMinValue );
}

void PhoneUIQtViewAdapter::setMuteIndication (TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdBoolean);
    TPhoneCmdParamBoolean &boolParam =
            static_cast<TPhoneCmdParamBoolean &>(*commandParam);
    m_bubbleWrapper->bubbleManager().setPhoneMuted(boolParam.Boolean());
}

void PhoneUIQtViewAdapter::activeCallCount (TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdInteger);

    TPhoneCmdParamInteger &integerParam =
        static_cast<TPhoneCmdParamInteger&>(*commandParam);
    integerParam.SetInteger (m_bubbleWrapper->activeCallCount());
}

void PhoneUIQtViewAdapter::setAudioPath (TPhoneCommandParam *commandParam)
    {
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdAudioOutput);

    TPhoneCmdParamAudioOutput &outputParam =
        static_cast<TPhoneCmdParamAudioOutput &>(*commandParam);

    m_resourceAdapter->buttonsController()->setButtonFlags(PhoneUIQtButtonsController::Ihf,
                              outputParam.AudioOutput() == EPELoudspeaker);
    m_resourceAdapter->buttonsController()->setButtonFlags(PhoneUIQtButtonsController::Wired,
                              outputParam.AudioOutput() == EPEWiredAudioAccessory);
    m_resourceAdapter->buttonsController()->setButtonFlags(PhoneUIQtButtonsController::Btaa,
                              outputParam.AudioOutput() == EPEBTAudioAccessory);
    }

void PhoneUIQtViewAdapter::expandedBubbleCallId(TPhoneCommandParam *commandParam)
    {
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdInteger);

    TPhoneCmdParamInteger &outputParam =
            static_cast<TPhoneCmdParamInteger &>(*commandParam);
    int callId = PHONE_CALL_NOT_FOUND;

    if ( 0<m_bubbleWrapper->callStates().keys().size() ) {
        int bubbleId = m_bubbleWrapper->bubbleManager().expandedBubble();
        callId = m_bubbleWrapper->callIdByBubbleId(bubbleId);
    }

    outputParam.SetInteger( callId );
    }

void PhoneUIQtViewAdapter::openDialpad()
    {
    m_dialpadAboutToClose = false;
    m_view.showDialpad();
    setDialpadMenu();
    }

void PhoneUIQtViewAdapter::createConferenceBubble(int callId)
{
    m_bubbleWrapper->bubbleManager().startChanges();
    m_phoneCallHeaderManager->createConferenceBubble(callId);
    setParticipantListActions();
    m_bubbleWrapper->bubbleManager().endChanges();
}

void PhoneUIQtViewAdapter::conferenceCallId(int callId, TPhoneCommandParam *commandParam)
{
    PHONE_DEBUG("PhoneUIQtViewAdapter::conferenceCallId");
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdBoolean);

    TPhoneCmdParamBoolean*  conferenceDataValue =
        static_cast<TPhoneCmdParamBoolean*>(
                commandParam );

    conferenceDataValue->SetBoolean(m_bubbleWrapper->conferenceCallId(callId));
}

void PhoneUIQtViewAdapter::removeConferenceBubble()
{
    m_phoneCallHeaderManager->removeConferenceBubble();
    if (!m_bubbleWrapper->bubbles().count()) {
        m_indicatorController->clearActiveCallData();
    }

}

void PhoneUIQtViewAdapter::isConference(TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdBoolean);

    TPhoneCmdParamBoolean*  conferenceDataValue =
        static_cast<TPhoneCmdParamBoolean*>(
                commandParam );

    conferenceDataValue->SetBoolean(0<m_bubbleWrapper->conferenceCallList().size());
}

void PhoneUIQtViewAdapter::removeCallFromConference(int callId)
{
    m_phoneCallHeaderManager->removeCallFromConference(callId);
}

TPhoneViewResponseId PhoneUIQtViewAdapter::getSelectedConferenceMember(
        TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdInteger);
    TPhoneViewResponseId ret = EPhoneViewResponseSuccess;
    TPhoneCmdParamInteger*  integerValue =
        static_cast<TPhoneCmdParamInteger*>(
                commandParam );

    int bubbleId = m_bubbleWrapper->bubbleManager().selectionIdInConference();
    int callId = m_bubbleWrapper->callIdByBubbleId(bubbleId);

    if (callId == -1) {
        ret = EPhoneViewResponseFailed;
    } else {
        integerValue->SetInteger(callId);
    }

    return ret;
}

void PhoneUIQtViewAdapter::setPrivateFromConference(int callId)
{
    m_phoneCallHeaderManager->setPrivateFromConference(callId);
}

void PhoneUIQtViewAdapter::setExpandActions()
{
    /* If there is 2 or more calls ongoing then we should
     * enable expancion swap*/
    int heldBubble = -1;

    if ( 2 <= m_bubbleWrapper->callStates().values().size() ) {
        if (callsExists(EPEStateConnected,EPEStateHeld)) {
            heldBubble =
                m_bubbleWrapper->bubbleId(m_bubbleWrapper->callId(EPEStateHeld));
            m_view.setExpandAction(heldBubble, EPhoneInCallCmdSwap);
        } else if (callsExists(EPEStateConnected,EPEStateHeldConference)) {
            heldBubble =
                m_bubbleWrapper->bubbleId(m_bubbleWrapper->callId(EPEStateHeldConference));
            m_view.setExpandAction(heldBubble, EPhoneInCallCmdSwap);
        } else if (callsExists(EPEStateRinging,EPEStateHeld)) {
            heldBubble =
                m_bubbleWrapper->bubbleId(m_bubbleWrapper->callId(EPEStateHeld));
            m_view.setExpandAction(heldBubble, EPhoneInCallCmdUnhold);
        }

        QList<int> calls = m_bubbleWrapper->bubbles().keys();
        foreach (int call, calls) {
            int bubbleId = m_bubbleWrapper->bubbleId(call);
            if ( bubbleId != heldBubble ) {
                 m_view.setExpandAction(bubbleId, EPhoneCmdUpdateUiControls);
            }
        }
    }
}

bool PhoneUIQtViewAdapter::callsExists(int call1State, int call2State)
{
    bool callsExist(false);
    if ( m_bubbleWrapper->callStates().values().contains(call1State) &&
         m_bubbleWrapper->callStates().values().contains(call2State) ) {
        callsExist = true;
    }
    return callsExist;
}

void PhoneUIQtViewAdapter::setParticipantListActions()
{
    if ( 0<m_bubbleWrapper->conferenceCallList().size() ) {
        m_view.addParticipantListAction(
                EPhoneInCallCmdPrivate,
                hbTrId("txt_phone_button_private"),
                HbIcon("qtg_mono_private_call"));
        m_view.addParticipantListAction(
                EPhoneInCallCmdDrop,
                hbTrId("txt_phone_button_drop"),
                HbIcon("qtg_mono_drop_call"));
    }
}

void PhoneUIQtViewAdapter::setExpandedConferenceCallHeader()
{
    m_phoneCallHeaderManager->setExpandedConferenceCallHeader();
}

void PhoneUIQtViewAdapter::bringToForeground()
{
    m_visibilityHandler->bringToForeground();
}

void PhoneUIQtViewAdapter::hideDeviceDialogs(TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdBoolean);
    TPhoneCmdParamBoolean* booleanParam =
        static_cast<TPhoneCmdParamBoolean*>(commandParam);
    m_visibilityHandler->hideDeviceDialogs(booleanParam->Boolean());
}

void PhoneUIQtViewAdapter::showGlobalNote(
        TPhoneCommandParam *commandParam)
{
    m_noteController->showGlobalNote(commandParam);
}

void PhoneUIQtViewAdapter::setDialpadVisibility(
        TPhoneCommandParam *commandParam)
{
    Q_ASSERT (commandParam->ParamId () == TPhoneCommandParam::EPhoneParamIdBoolean);
    TPhoneCmdParamBoolean* booleanParam =
        static_cast<TPhoneCmdParamBoolean*>(commandParam);

    if (booleanParam->Boolean()) {
        m_dialpadAboutToClose = false;
        m_view.showDialpad();
    } else {
        m_clearDialpadOnClose = false;
        m_view.hideDialpad();
    }
}

void PhoneUIQtViewAdapter::removeAllCallHeaders()
{
    m_phoneCallHeaderManager->removeAllCallHeaders();
    if (!m_bubbleWrapper->bubbles().count()) {
        m_indicatorController->clearActiveCallData();
    }

}

void PhoneUIQtViewAdapter::getNumberFromDialpad(
        TPhoneCommandParam *commandParam)
{

    TPhoneCmdParamString* entryContent =
        static_cast<TPhoneCmdParamString*>(commandParam);
    
    QString westernNumber = 
        HbStringUtil::convertDigitsTo(m_view.dialpadText(), WesternDigit);
    
    (*entryContent->String()).Copy(westernNumber.utf16());         
}

void PhoneUIQtViewAdapter::getDialpadStringLength(
        TPhoneCommandParam *commandParam)
{

    TPhoneCmdParamInteger* integerParam =
        static_cast<TPhoneCmdParamInteger*>(commandParam);

    integerParam->SetInteger(m_view.dialpadText().length());
}

void PhoneUIQtViewAdapter::removeDialpad()
{
    m_view.clearAndHideDialpad();
}

void PhoneUIQtViewAdapter::clearDialpad()
{
    // After the sending of SS strings the view stays in Dialer.
    if (!m_visibilityHandler->phoneVisible()){ 
        m_appLauncher->launchLogs( XQService::LogsViewAll, true,QString(""));
    }else{
        m_view.clearDialpad();
    }
}

void PhoneUIQtViewAdapter::addToConference()
{
    m_bubbleWrapper->addToConferenceBubble();
}

void PhoneUIQtViewAdapter::setMenu()
{
    if (m_view.isDialpadVisible() && false == m_dialpadAboutToClose) {
        setDialpadMenu();
    } else {
        setCallMenu();
    }
}

void PhoneUIQtViewAdapter::setCallMenu()
{
    if ( 0<m_bubbleWrapper->callStates().keys().size() ) {
        int bubbleId = m_bubbleWrapper->bubbleManager().expandedBubble();
        int callId = m_bubbleWrapper->callIdByBubbleId(bubbleId);
        m_uiCommandController->setCallMenuActions(
                m_bubbleWrapper->callStates(),
                m_bubbleWrapper->serviceIds(),
                m_bubbleWrapper->serviceIdByCallId(callId),
                callId );
    }

}

void PhoneUIQtViewAdapter::setDialpadMenu()
{
    m_uiCommandController->setDialpadMenuActions();
}

void PhoneUIQtViewAdapter::showNote(TPhoneCommandParam *commandParam)
{
    m_noteController->showNote(commandParam);
}

QString PhoneUIQtViewAdapter::convertDuration(TInt secs)
    {
    QTime init(0,0,0);
    QTime duration = init.addSecs(secs);
    HbExtendedLocale locale = HbExtendedLocale::system();
    QString durationString = locale.format(duration, QString(r_qtn_time_durat_long));
    return HbStringUtil::convertDigits(durationString); 
} 

void PhoneUIQtViewAdapter::setBubbleSelectionFlag()
{
    bool selectionFlag = ((m_bubbleWrapper->callStates().
                              values().contains(EPEStateRinging) &&
                           m_bubbleWrapper->conferenceCallList().count())||
                          m_bubbleWrapper->callStates().
                              values().contains(EPEStateDialing) ||
                          m_bubbleWrapper->callStates().
                              values().contains(EPEStateConnecting));

    m_bubbleWrapper->bubbleManager().setBubbleSelectionDisabled(selectionFlag);
}

void PhoneUIQtViewAdapter::setHidden(bool hidden)
{
    TRAP_IGNORE(SetHiddenL(hidden));
}

void PhoneUIQtViewAdapter::SetHiddenL(bool hidden)
{
    CEikonEnv* env = CEikonEnv::Static();
    
    if (env) {
        CApaWindowGroupName* windowGroupName =
            CApaWindowGroupName::NewLC(
                    env->WsSession(),
                    env->RootWin().Identifier() );
        
        windowGroupName->SetHidden( hidden );
        User::LeaveIfError(
                windowGroupName->SetWindowGroupName(
                        env->RootWin() ) );
        
        CleanupStack::PopAndDestroy( windowGroupName );
        
        // Avkon removal
    }
}
void PhoneUIQtViewAdapter::openContacts()
{
    m_appLauncher->launchContacts();
}

void PhoneUIQtViewAdapter::openLogs(TPhoneCommandParam *commandParam)
{
    TPhoneCmdParamString* entryContent =
            static_cast<TPhoneCmdParamString*>(commandParam);
    
    bool phoneVisible = m_visibilityHandler->phoneVisible();
    
    m_homeScreenToForeground = false;
    m_visibilityHandler->sendToBackground(m_homeScreenToForeground);
    
    if (phoneVisible) { 
        // Activate logs dialer only if telephone is on the top.
        m_appLauncher->launchLogs(
                XQService::LogsViewAll,
                true,
                QString::fromUtf16(
                    entryContent->String()->Ptr(),
                    entryContent->String()->Length()));
    } 
}

void PhoneUIQtViewAdapter::setCommonButtonFlags()
{
    m_resourceAdapter->buttonsController()->setButtonFlags(
            PhoneUIQtButtonsController::FullConference,
            (5 <= m_bubbleWrapper->conferenceCallList().count()));

    m_resourceAdapter->buttonsController()->setButtonFlags(
            PhoneUIQtButtonsController::IhfAsPushButton,
            m_speakerAsDefaultButton);
}

bool PhoneUIQtViewAdapter::convertKey(
        TKeyCode symbianKey, Qt::Key &qtKey)
{
    bool ret = false;
    
    switch(symbianKey) {
    case EKeyNo: {
        qtKey = Qt::Key_No;
        ret = true; 
    }
    break;
    default:
    break;
    }
    
    return ret;
}



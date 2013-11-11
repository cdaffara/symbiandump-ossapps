/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Acessory and Bluetooth handler.
*
*/


// INCLUDE FILES
#include    "phonenoteutil.h"
#include    "cphonecenrepproxy.h"
#include    "cphonemainresourceresolver.h"
#include    "mphoneneclearedhandler.h"
#include    "mphonestatemachine.h"
#include    "mphoneviewcommandhandle.h"
#include    "cphonestatehandle.h"
#include    "tphonecmdparamboolean.h"
#include    "tphonecmdparamnote.h"
#include    "tphonecmdparamquery.h"
#include    "cphoneclearblacklist.h"
#include    "cphonemainerrormessageshandler.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// PhoneNoteUtil::SendGlobalInfoNoteL
// ---------------------------------------------------------
//
void PhoneNoteUtil::SendGlobalInfoNoteL( TInt aResourceId, 
    TBool aNotificationDialog )
    {    
    MPhoneViewCommandHandle* viewCommandHandle = 
            ViewCommandHandle();

    SetGlobalNotifierDisabledL( viewCommandHandle );

    TPhoneCmdParamGlobalNote globalNoteParam;
    TPhoneNotificationType type = aNotificationDialog ? 
        EPhoneNotificationDialog : EPhoneMessageBoxInformation;

    SetCommonGlobalNoteParamsL(
            globalNoteParam, aResourceId,
            aNotificationDialog, type,
            EPhoneInformationTone );
    
    viewCommandHandle->ExecuteCommandL(
        EPhoneViewShowGlobalNote, &globalNoteParam );
    }

// ---------------------------------------------------------
//  PhoneNoteUtil::SendGlobalWarningNoteL
// ---------------------------------------------------------
//
void PhoneNoteUtil::SendGlobalWarningNoteL( 
        TInt aResourceId, TBool aNotificationDialog )
    {   
    MPhoneViewCommandHandle* viewCommandHandle = 
            ViewCommandHandle();
    
    SetGlobalNotifierDisabledL( viewCommandHandle );

    TPhoneCmdParamGlobalNote globalNoteParam;
    TPhoneNotificationType type = aNotificationDialog ? 
                EPhoneNotificationDialog : EPhoneMessageBoxWarning;

    SetCommonGlobalNoteParamsL(
            globalNoteParam, aResourceId,
            aNotificationDialog, type,
            EPhoneWarningTone );

    viewCommandHandle->ExecuteCommandL(
        EPhoneViewShowGlobalNote, &globalNoteParam );
    }

// ---------------------------------------------------------
//  PhoneNoteUtil::SendGlobalErrorNoteL
// ---------------------------------------------------------
//
void PhoneNoteUtil::SendGlobalErrorNoteL( 
        TInt aResourceId, TBool aNotificationDialog )
    {   
    MPhoneViewCommandHandle* viewCommandHandle = 
            ViewCommandHandle();
    
    SetGlobalNotifierDisabledL( viewCommandHandle );

    TPhoneCmdParamGlobalNote globalNoteParam;
    TPhoneNotificationType type = aNotificationDialog ? 
                EPhoneNotificationDialog : EPhoneMessageBoxInformation;
    
    SetCommonGlobalNoteParamsL(
            globalNoteParam, aResourceId,
            aNotificationDialog, type,
            EPhoneErrorTone );

    viewCommandHandle->ExecuteCommandL(
        EPhoneViewShowGlobalNote, &globalNoteParam );

    }

// ---------------------------------------------------------
//  PhoneNoteUtil::SendGlobalNoteL
// ---------------------------------------------------------
//
void PhoneNoteUtil::SendGlobalNoteL( 
        TInt aResourceId, TBool aNotificationDialog )
    {   
    MPhoneViewCommandHandle* viewCommandHandle = 
            ViewCommandHandle();
    
    SetGlobalNotifierDisabledL( viewCommandHandle );

    TPhoneCmdParamGlobalNote globalNoteParam;
    TPhoneNotificationType type = aNotificationDialog ? 
                EPhoneNotificationDialog : EPhoneMessageBoxInformation;
    
    SetCommonGlobalNoteParamsL(
            globalNoteParam, aResourceId,
            aNotificationDialog, 
            type );

    viewCommandHandle->ExecuteCommandL(
        EPhoneViewShowGlobalNote, &globalNoteParam );

    }

// ---------------------------------------------------------
//  PhoneNoteUtil::SendGlobalNoteL
// ---------------------------------------------------------
//
void PhoneNoteUtil::SendGlobalNoteL( 
        TPhoneNotificationType aType, const TDesC& aText )
    {
    TPhoneCmdParamGlobalNote globalNoteParam;
    globalNoteParam.SetType( aType );
    globalNoteParam.SetText( aText );

    ViewCommandHandle()->ExecuteCommandL(
        EPhoneViewShowGlobalNote, &globalNoteParam );
    }

// -----------------------------------------------------------
// PhoneNoteUtil::ShowNoteL
// -----------------------------------------------------------
//
void PhoneNoteUtil::ShowNoteL( TInt aResourceId )
    {   
    TPhoneCmdParamNote noteParam;
    noteParam.SetType( EPhoneNotePermanent );
    noteParam.SetResourceId( CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( aResourceId ) );
    ViewCommandHandle()->ExecuteCommandL( EPhoneViewShowNote, &noteParam );
    }

// -----------------------------------------------------------
// PhoneNoteUtil::ShowQueryL
// -----------------------------------------------------------
//
void PhoneNoteUtil::ShowQueryL( TInt aResourceId )
    {  
    TPhoneCmdParamQuery queryParam;
    queryParam.SetQueryType( EPhoneQueryDialog );
    queryParam.SetQueryResourceId( CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( aResourceId ) );
    ViewCommandHandle()->ExecuteCommandL( EPhoneViewShowQuery, &queryParam );
    }

// -----------------------------------------------------------
// PhoneNoteUtil::ShowTextQueryL
// -----------------------------------------------------------
//
void PhoneNoteUtil::ShowTextQueryL(
    TInt aDialogResourceId,
    TInt aDefaultCbaResourceId,
    TInt aContentCbaResourceId,
    TDes* aDataText,
    TBool aSendKeyEnabled )
    { 
    TPhoneCmdParamQuery queryDialogParam;
    queryDialogParam.SetQueryType( EPhoneTextQueryDialog );
    queryDialogParam.SetQueryResourceId( aDialogResourceId );
    queryDialogParam.SetDefaultCba( aDefaultCbaResourceId );
    queryDialogParam.SetContentCba( aContentCbaResourceId );
    queryDialogParam.SetDataText( aDataText );
    queryDialogParam.SetSendKeyEnabled( aSendKeyEnabled );
    ViewCommandHandle()->ExecuteCommandL( EPhoneViewShowQuery,
        &queryDialogParam );
    }

// -----------------------------------------------------------
// PhoneNoteUtil::HandleErrorL
// Default handling for HandleError message
// (other items were commented in a header).
// -----------------------------------------------------------
//
void PhoneNoteUtil::HandleErrorL( const TPEErrorInfo& aErrorInfo )
    {
    // Do the common error handling (display proper notes etc)
    CPhoneMainErrorMessagesHandler::Instance()->ShowErrorSpecificNoteL( aErrorInfo );
    switch( aErrorInfo.iErrorCode )
        {
        case ECCPErrorNoService:
            // No network -> hide volume popup
            ViewCommandHandle()->
                    ExecuteCommandL( EPhoneViewHideNaviPaneAudioVolume );
            break;

        case ECCPErrorSatControl:
            ViewCommandHandle()->
                ExecuteCommandL( EPhoneViewClearNumberEntryContent );
            break;

        default:
            break;
        }
    // clear call blacklist if call failure occurs
    CPhoneClearBlacklist::Instance()->ClearBlackListOnNextKey();
    }

// -----------------------------------------------------------------------------
// PhoneNoteUtil::HandleRemoteBusyNoteL
// -----------------------------------------------------------------------------
//
void PhoneNoteUtil::HandleRemoteBusyNoteL( TInt aCallId )
    {
    TPEErrorInfo info;
    info.iCallId = aCallId;
    info.iErrorCode = ECCPErrorBusy;
    info.iErrorType = EPECcp;
    CPhoneMainErrorMessagesHandler::Instance()->
            ShowErrorSpecificNoteL( info );
    }

// ---------------------------------------------------------
//  PhoneNoteUtil::SetGlobalNotifierDisabledL
// ---------------------------------------------------------
//
void PhoneNoteUtil::SetGlobalNotifierDisabledL( 
        MPhoneViewCommandHandle* aViewCommandHandle )
    {
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    aViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
    }

// ---------------------------------------------------------
//  PhoneNoteUtil::SetCommonGlobalNoteParamsL
// ---------------------------------------------------------
//
void PhoneNoteUtil::SetCommonGlobalNoteParamsL( 
        TPhoneCmdParamGlobalNote& aGlobalNoteParam,
        TInt aResourceId, 
        TBool aNotificationDialog,
        TPhoneNotificationType aNoteType,
        TPhoneNotificationToneType aTone )
    {
    aGlobalNoteParam.SetType( aNoteType );
    aGlobalNoteParam.SetToneType( aTone );

    aGlobalNoteParam.SetTextResourceId(
        CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( aResourceId ) );
    
    aGlobalNoteParam.SetNotificationDialog( aNotificationDialog );
    }

// ---------------------------------------------------------
//  PhoneNoteUtil::ViewCommandHandle
// ---------------------------------------------------------
//
MPhoneViewCommandHandle* PhoneNoteUtil::ViewCommandHandle()
    {   
    return CPhoneStateHandle::Instance()->ViewCommandHandle();
    }

//  End of File  

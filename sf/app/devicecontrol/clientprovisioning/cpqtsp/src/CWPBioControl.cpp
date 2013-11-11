/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Bio control for Provisioning documents.
*
*/

// INCLUDE FILES
#include "CWPBioControl.h"
#include <msvapi.h>
#include <bldvariant.hrh>
#include <msvids.h>
#include <featmgr.h>
#include <sysutil.h>
#include <CWPEngine.h>
#include <CWPAdapter.h>
#include <MWPPhone.h>
#include <WPPhoneFactory.h>
#include <CWPPushMessage.h>
#include "ProvisioningUIDs.h"
#include <MWPContextManager.h>
#include "ProvisioningDebug.h"
#include "CWPNameValue.h"
#include <aknmessagequerydialog.h> 
#include <ProvisioningInternalCRKeys.h>
#include <HbMessageBox>
#include <HbAction>
#include <QVariant>
#include <hbdevicedialog.h>
#include <QObject>
#include <devicedialogconsts.h>
#include "PnpUtilLogger.h"

// CONSTANTS
_LIT(KNone,"");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
//
CWPBioControl::CWPBioControl(CMsvSession* /*aSession*/,TMsvId /*aId*/): CActive(EPriorityNormal)
                          
    {
		CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CWPBioControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPBioControl::ConstructL(CMsvSession* aSession,TMsvId aId,CpQtSp* sp)
    {
    FLOG( _L( "[ProvisioningBC] CWPBioControl::ConstructL:" ) );  
    FeatureManager::InitializeLibL();
    iSession = aSession;
    iId = aId;
    iSp = sp;
    iEngine = CWPEngine::NewL();    
    iDevDialog = NULL;
    ibootstrap = NULL;
    iMsg = new(ELeave)CpMessage(this);
    iWait = new( ELeave ) CActiveSchedulerWait;
    FLOG( _L( "[ProvisioningBC] CWPBioControl::ConstructL: done" ) );
    }

// -----------------------------------------------------------------------------
// CWPBioControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPBioControl* CWPBioControl::NewL(CMsvSession* aSession,TMsvId aId,CpQtSp* sp)
     {
    CWPBioControl* self = new( ELeave ) CWPBioControl(aSession,aId);                                                       
    CleanupStack::PushL( self );
    self->ConstructL(aSession,aId,sp);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPBioControl::~CWPBioControl()
    {
    Cancel();
    delete iEngine;
    delete iMessage;
    FeatureManager::UnInitializeLib();
    if (iDevDialog)
        {
        delete iDevDialog;
        }
    delete ibootstrap;
    delete iWait;
    }
// ----------------------------------------------------------------------------
// CWPBioControl ::RestoreMsgL
// ----------------------------------------------------------------------------
//
void CWPBioControl::RestoreMsgL()
    {                    
    CMsvEntry* entry = iSession->GetEntryL(iId);   
    CleanupStack::PushL( entry );
    iEntry = entry->Entry();
    iMessage = CWPPushMessage::NewL();
    CMsvStore* readStore = entry->ReadStoreL();
    CleanupStack::PushL( readStore );
    iMessage->RestoreL( *readStore );
    CleanupStack::PopAndDestroy(); // readStore
 
    TRAPD( result, iEngine->ImportDocumentL( iMessage->Body() ) );
    if( result == KErrCorrupt )
        {
        //result = KErrMsgBioMessageNotValid;
        }
    FTRACE(RDebug::Print(_L("[ProvisioningBC] CWPBioControl::RestoreMsgL result (%d)"), result));
    User::LeaveIfError( result );    
	AuthenticateL( *iMessage );
	CleanupStack::PopAndDestroy();
	if(iAuth == KAUTENTICATIONCANCEL || iAuth == KAUTENTICATIONSUCCESS)
	    {
        iSp->authenticationStatus();
	    }	
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::postAuthetication
// ----------------------------------------------------------------------------
//

void CWPBioControl::postAuthetication()
    {    
    iMsg->senderUnknown();
    }      

// ----------------------------------------------------------------------------
// CWPBioControl ::senderUnknownContinue
// ----------------------------------------------------------------------------
//

void CWPBioControl::senderUnknownContinue()
    {
    iAuth = KAUTENTICATIONSUCCESS;
    iSp->authenticationStatus();
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::senderUnknownCancel
// ----------------------------------------------------------------------------
//

void CWPBioControl::senderUnknownCancel()
    {
    iAuth = KAUTENTICATIONCANCEL;
    iSp->authenticationStatus();
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::updateAuthetication
// ----------------------------------------------------------------------------
//

void CWPBioControl::updateAuthetication()
{
    if (iMessage->Authenticated() )
           {
           //update Cenrep key
           TBuf8<100> orig;
           orig.Copy(iMessage->Originator());
           TRAPD(err, SetCenrepKeyL(orig));
           User::LeaveIfError(err);
           }

    TBool preAuthenticated( iMessage->Authenticated() );
    if( !iEntry.ReadOnly() && preAuthenticated != iMessage->Authenticated() )
        {
        // If the entry can be written to, get its edit store and save
        // authentication flag. Failing is not dangerous, as the only
        // effect is that the user has to re-enter the PIN.
        // FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 4 " ) );
        TRAPD( ignoreError, SaveMessageL() );
        if ( ignoreError ) ignoreError = 0; // prevent compiler warning 
        }
    
    iEngine->PopulateL();
    if( iEngine->ItemCount() == 0 )
        {
        FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 7 " ) );
        }

    TPtrC8 orig8( iMessage->Originator() );
    HBufC* orig16 = HBufC::NewLC( orig8.Length() );
    orig16->Des().Copy( orig8 );
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 8 " ) );
    if( iMessage->Authenticated() 
        && iEngine->ContextExistsL( *orig16 ) )
        {
        TUint32 context( iEngine->ContextL( *orig16 ) );
        iEngine->SetCurrentContextL( context );
        }
    else
        {
        iEngine->SetCurrentContextL( KWPMgrUidNoContext );
        }
        
    
    CleanupStack::PopAndDestroy();
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL done" ) );  
}

// ----------------------------------------------------------------------------
// CWPBioControl ::getAuth
// ----------------------------------------------------------------------------
//

TInt CWPBioControl::getAuth()
    {
    return iAuth;
    }
// ----------------------------------------------------------------------------
// CWPBioControl ::RestoreMsgL
// ----------------------------------------------------------------------------
//	
TBool CWPBioControl::AuthenticateL( CWPPushMessage& aMessage )
    {
     if (!aMessage.Authenticated())
        {
        MWPPhone* phone = NULL;
        TBuf<KMaxPinSize> imsi;
        TRAPD(err, phone = WPPhoneFactory::CreateL());
        if (err != KErrNone)
            {
            imsi = KNone;
            }
        else
            {
            imsi = phone->SubscriberId();
            }
        ibootstrap = CWPBootstrap::NewL(imsi);

        if (phone)
            delete phone;

        CWPBootstrap::TBootstrapResult result(ibootstrap->BootstrapL(aMessage,*iEngine, KNullDesC));
        if (result == CWPBootstrap::EPinRequired)
            {
            CMsvEntry* entry = iSession->GetEntryL(iId);
            TInt count = (entry->Entry()).MtmData1();
            // Create a new device dialog if it is not there
            if( !iDevDialog )
              {                 
                iDevDialog = CHbDeviceDialogSymbian::NewL();
                LOGSTRING("CWPBioControl::AuthenticateL() iDevDialog is created");
              }

            CHbSymbianVariantMap* parameters = CHbSymbianVariantMap::NewL();
            CleanupStack::PushL(parameters);
            for (TInt i(0); i < KPinRetries && !aMessage.Authenticated(); i++)
                {
                // Add dialog key and dialog type to the parameters
                const TPtrC dialog (static_cast<const TUint16*>(keydialog.utf16()), keydialog.length());
                CHbSymbianVariant* dialogvariant = NULL;
                const TInt dialogID = EOmaCPPinDialog;
                dialogvariant = CHbSymbianVariant::NewL( &dialogID, CHbSymbianVariant::EInt );
                parameters->Add(dialog, dialogvariant );
                
                // Add retries value to the parameters                
                itries = i;
                const TPtrC ptrkeyparam1 (static_cast<const TUint16*>(keyparam1.utf16()), keyparam1.length());
                //Retries left after this attempt
                TInt retries = KPinRetries - itries;
                CHbSymbianVariant* retriesvariant = NULL;
                retriesvariant = CHbSymbianVariant::NewL( &retries, CHbSymbianVariant::EInt );
                parameters->Add(ptrkeyparam1, retriesvariant );
                
                if (iDevDialog && iIsDisplayingDialog)
                    {
                    LOGSTRING("[CWPBioControl] CWPBioControl::AuthenticateL updating the dialog");
                    iDevDialog->Update(*parameters);
                    }
                else if (iDevDialog && !iIsDisplayingDialog)
                    {
                    // Show the device dialog
                    iDevDialog->Show(KDeviceDialogType,*parameters, this);
                    iIsDisplayingDialog = ETrue;
                    // Wait for user response
                    TInt error = WaitUntilDeviceDialogClosed();
                    LOGSTRING2("WaitUntilDeviceDialogClosed err is %d", error);
                    User::LeaveIfError(error);                    
                        if(ireturnkey == EHbRSK)
                        {
                        iAuth = KAUTENTICATIONCANCEL;
                        CleanupStack::PopAndDestroy(parameters);
                        return false;
                        }
                    }
                
     
                }
            
            CleanupStack::PopAndDestroy(parameters);
            
            if (iDevDialog)
                {
                iDevDialog->Cancel();
                delete iDevDialog;
                iDevDialog = NULL;
                }
                   
            if( !aMessage.Authenticated() && (itries == KPinRetries - 1)  )
                {
                 iMsg->autheticationFailure();
                 return false;
                }
            }
        }                
    if(!aMessage.Authenticated())
        {
        postAuthetication();        
        }
    else if(aMessage.Authenticated())
        {
        iAuth = KAUTENTICATIONSUCCESS;   
        }
    FLOG( _L( "[ProvisioningBC] CWPBioControl::AuthenticateL done" ) );   
    return true;    
  }

// ----------------------------------------------------------------------------
// CWPBioControl ::authenticationFailed
// ----------------------------------------------------------------------------
//

void CWPBioControl::authenticationFailed()
    {
    iAuth = KAUTENTICATIONFAILURE;    
    iSp->authenticationStatus();
    }
// ----------------------------------------------------------------------------
// CWPBioControl ::SaveMessageL
// ----------------------------------------------------------------------------
//
void CWPBioControl::SaveMessageL()
    {
    CMsvEntry* entry = iSession->GetEntryL(iId);
    CleanupStack::PushL( entry );

    CMsvStore* editStore = entry->EditStoreL();
    CleanupStack::PushL( editStore );
    iMessage->StoreL( *editStore );
    editStore->CommitL();
    CleanupStack::PopAndDestroy(2); // editStore, entry
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::CollectItemsL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CWPNameValue>* CWPBioControl::CollectItemsLC() const
    {
    RPointerArray<CWPNameValue>* array = new(ELeave) RPointerArray<CWPNameValue>;
    CleanupStack::PushL( TCleanupItem( Cleanup, array ) );

    TInt count( iEngine->ItemCount() );
    FLOG( _L( "[ProvisioningBC] CWPBioControl::RestoreMsgL 83 " ) );
    for( TInt index = 0; index < count; index++ )
        {
        const TDesC& text = iEngine->SummaryText( index );
        const TDesC& title = iEngine->SummaryTitle( index );

        CWPNameValue* pair = CWPNameValue::NewLC( title, text );
        User::LeaveIfError( array->Append( pair ) );
        CleanupStack::Pop( pair );
        }
    array->Sort( TLinearOrder<CWPNameValue>( CWPBioControl::Compare ) );
    return array;
    }

// -----------------------------------------------------------------------------
// CWPBioControl::Cleanup
// -----------------------------------------------------------------------------
//
void CWPBioControl::Cleanup( TAny* aAny )
    {
    RPointerArray<CWPNameValue>* array = reinterpret_cast<RPointerArray<CWPNameValue>*>( aAny );
    array->ResetAndDestroy();
    array->Close();
    delete array;
    }

// -----------------------------------------------------------------------------
// CWPBioControl::Compare
// -----------------------------------------------------------------------------
//
TInt CWPBioControl::Compare( const CWPNameValue& aItem1,
                             const CWPNameValue& aItem2 )
    {
    return aItem1.Name().Compare( aItem2.Name() );
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::SaveSettingsL
// ----------------------------------------------------------------------------
//
void CWPBioControl::SaveSettingsL()
    {
    if( !iMessage->Saved())
    {
        DoSaveL();
        SaveSettingStatus();
    }
    else
    {    
        iMsg->saveSettingMessage();
    }
   }

// ----------------------------------------------------------------------------
// CWPBioControl ::SaveSettingStatus
// ----------------------------------------------------------------------------
//

void CWPBioControl::SaveSettingStatus()
    {
    iMessage->SetSaved( ETrue );
    SaveMessageL();
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::DoSaveL
// ----------------------------------------------------------------------------
//
void CWPBioControl::DoSaveL()
    {
    TInt numSaved( 0 );
    CWPSaver* saver = new(ELeave) CWPSaver( *iEngine, EFalse );
    TBufC<KLENGTH> Value;
    TInt err( saver->ExecuteLD( numSaved, Value ) );
    if( err >= 0 )
        {
        // Check if any setting can be made default
        TBool setDefault( EFalse );
        TInt itemCount( iEngine->ItemCount() );
        for( TInt i( 0 ); i < itemCount && !setDefault; i++ )
            {
            if( iEngine->CanSetAsDefault( i ) )
                {
                setDefault = ETrue;
                }
            }
        
        // Ask the user if the saved settings should be 
        // made default.                     
        if( setDefault )
            {
            iMsg->setAsDefault();
            }
        }
    else
        {
         iMsg->settingNotSaved();
        }
    
        
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::saveDefaultSetting
// ----------------------------------------------------------------------------
//

void CWPBioControl::saveDefaultSetting()
    {
     CWPSaver* setter = new(ELeave) CWPSaver( *iEngine, ETrue );
     TInt numSet( 0 );
     TBufC<KLENGTH> Value;
     TInt result = KErrNone;
     result = setter->ExecuteLD( numSet,Value );                            
     if (result)
     {
        iMsg->settingNotSaved();
     }
    
        
    }
// ----------------------------------------------------------------------------
// CWPBioControl ::SetCenrepKey
// ----------------------------------------------------------------------------
//
void CWPBioControl::SetCenrepKeyL(const TDesC8& aValue)
    {

    FLOG( _L( "[ProvisioningBC] CWPBioControl::SetCenrepKeyL" ) );

    CRepository* rep= NULL;
    TInt errorStatus = KErrNone;

    TRAPD( errVal, rep = CRepository::NewL( KCRUidOMAProvisioningLV ))
    ;

    if (errVal == KErrNone)
        {
        errorStatus = rep->Set(KOMAProvOriginatorContent, aValue);
        }
    else
        {
        errorStatus = errVal;
        }

    if (rep)
        {
        delete rep;
        }

    if (errorStatus != KErrNone)
        {
        User::Leave(errorStatus);
        }

    FLOG( _L( "[ProvisioningBC] CWPBioControl::SetCenrepKeyL done" ) );

    }

// ----------------------------------------------------------------------------
// CWPBioControl ::DataReceived
// Slot to handle the data received from the plugin
// ----------------------------------------------------------------------------
//
void CWPBioControl::DataReceived(CHbSymbianVariantMap& aData)
    {
    FLOG(_L("CWPBioControl::DataReceived() start"));

    // Get the return key
    const TPtrC ptrreturnkey(static_cast<const TUint16*> (returnkey.utf16()),
            returnkey.length());
    const CHbSymbianVariant* returnkeyvariant = aData.Get(ptrreturnkey);

    if (returnkeyvariant)
        {
        TInt* returnkey = returnkeyvariant->Value<TInt> ();
        if (returnkey)
            {
            ireturnkey = *returnkey;
            FLOG(_L("CWPBioControl::DataReceived() ireturnkey"));
            LOGSTRING2("ireturnkey is %d", ireturnkey);
            if (ireturnkey == EHbLSK)
                {
                // Ok key pressed 
                FLOG(_L("Ok key pressed"));
                LOGSTRING("CWPBioControl::DataReceived() Ok key pressed");
                // Get the PIN code
                const TPtrC ptrpinquery(
                        static_cast<const TUint16*> (pinquery.utf16()),
                        pinquery.length());
                const CHbSymbianVariant* pinvariant = aData.Get(ptrpinquery);
                if (pinvariant)
                    {
                    TPtrC pin = *pinvariant->Value<TDesC> ();
                    // Call BootstrapL to find out whether the PIN is correct or not
                    CWPBootstrap::TBootstrapResult result =
                            CWPBootstrap::EPinRequired;
                    TInt err = KErrNone;
                    TRAP( err, QT_TRYCATCH_LEAVING(result = ibootstrap->BootstrapL(*iMessage, *iEngine, pin) ));
                    if (err == KErrNone && result
                            == CWPBootstrap::EAuthenticationFailed && itries
                            < KPinRetries - 1)
                        {
                        FLOG(_L("CWPBioControl:deviceDialog:DataReceived() Authentication Failed"));
                        LOGSTRING("CWPBioControl::DataReceived() Authentication Failed");
                        }

                    }

                }
            }

        }

    FLOG(_L("CWPBioControl:deviceDialog:DataReceived() end"));
    }

// ----------------------------------------------------------------------------
// CWPBioControl ::DeviceDialogClosed
// Slot to handle the deviceDialogClosed signal from the plugin
// ----------------------------------------------------------------------------
//
void CWPBioControl::DeviceDialogClosed(TInt aCompletionCode)
    {
    FLOG(_L("CWPBioControl::DeviceDialogClosed() start"));
    LOGSTRING("CWPBioControl::DeviceDialogClosed() start");
    iCompletionCode = aCompletionCode;
    LOGSTRING2("CWPBioControl::DeviceDialogClosed() iCompletionCode is %d", iCompletionCode);
    
    iIsDisplayingDialog = EFalse;

    TRequestStatus* status(&iStatus);
    User::RequestComplete(status, KErrNone);
    LOGSTRING("CWPBioControl::DeviceDialogClosed() end");
    FLOG(_L("CWPBioControl::DeviceDialogClosed() end"));
    }

// ---------------------------------------------------------------------------
// CWPBioControl::DoCancel()
// ---------------------------------------------------------------------------
//
void CWPBioControl::DoCancel()
    {
    FLOG(_L("CWPBioControl::DoCancel() start"));
    LOGSTRING("CWPBioControl::DoCancel() start");
    if (iWait && iWait->IsStarted() && iWait->CanStopNow())
        {
        LOGSTRING("CWPBioControl::DoCancel() In the iWait block");
        iCompletionCode = KErrCancel;
        iWait->AsyncStop();
        }
    LOGSTRING("CWPBioControl::DoCancel() end");
    FLOG(_L("CWPBioControl::DoCancel() end"));
    }

// ---------------------------------------------------------------------------
// CWPBioControl::RunL()
// ---------------------------------------------------------------------------
//
void CWPBioControl::RunL()
    {
    FLOG(_L("CWPBioControl::RunL() start"));
    LOGSTRING("CWPBioControl::RunL() start");
    if (iWait)
        {
        LOGSTRING("CWPBioControl::RunL() iWait block ");
        iWait->AsyncStop();
        }
    LOGSTRING("CWPBioControl::RunL() end");
    FLOG(_L("CWPBioControl::RunL() end"));
    }

// ---------------------------------------------------------------------------
// CWPBioControl::WaitUntilDeviceDialogClosed()
// ---------------------------------------------------------------------------
//
TInt CWPBioControl::WaitUntilDeviceDialogClosed()
    {
    FLOG(_L("CWPBioControl::WaitUntilDeviceDialogClosed() start"));
    iCompletionCode = KErrInUse;
    ireturnkey = KErrUnknown;
    if (!IsActive() && iWait && !iWait->IsStarted())
        {
        FLOG(_L("CWPBioControl::WaitUntilDeviceDialogClosed() In WaitAsynch start block"));
        LOGSTRING("CWPBioControl::WaitUntilDeviceDialogClosed() In WaitAsynch start block");
        iStatus = KRequestPending;
        SetActive();
        LOGSTRING2("CWPBioControl::WaitUntilDeviceDialogClosed() iCompletionCode is %d", iCompletionCode);
        iWait->Start();
        LOGSTRING2("CWPBioControl::WaitUntilDeviceDialogClosed() iCompletionCode is %d", iCompletionCode);
        }
    FLOG(_L("CWPBioControl::WaitUntilDeviceDialogClosed() end"));
    return iCompletionCode;
    }





//  End of File

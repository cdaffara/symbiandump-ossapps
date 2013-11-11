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
 * Description:  This provides the messaging mw interface for MMS
 *
 */

#include <commdb.h>
#include <commdbconnpref.h>
#include <msvapi.h>
#include <msvstd.h>
#include <msvuids.h>
#include <mmsaccount.h>
#include <mmssettings.h>
#include <mtclreg.h> //For client reg
#include <mmsclient.h> //For MMS client
#include <mmsconst.h>

#include "debugtraces.h"


#include "mmssettingprivate.h"

/**
 * Message Server session event handler 
 */
class CEventHandler : public CBase, public MMsvSessionObserver
{
public:
    void HandleSessionEvent(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                            TAny* aArg3);
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);
};
void CEventHandler::HandleSessionEvent(TMsvSessionEvent /*aEvent*/,
                                       TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}
void CEventHandler::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                        TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}

//two phase constructor
MmsSettingsPrivate* MmsSettingsPrivate::NewL()
{
    MmsSettingsPrivate* self = new (ELeave) MmsSettingsPrivate();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

//---------------------------------------------------------------
// MmsSettingsPrivate::MmsSettingsPrivate
// @see header
//---------------------------------------------------------------
MmsSettingsPrivate::MmsSettingsPrivate()
{
    //do nothing
}

//2nd phase constructor
void MmsSettingsPrivate::ConstructL()
{
    //for debug only
    //createRepositoryL();    
}

//---------------------------------------------------------------
// MmsSettingsPrivate::~MmsSettingsPrivate
// @see header
//---------------------------------------------------------------
MmsSettingsPrivate::~MmsSettingsPrivate()
{
    // do nothing
}

//---------------------------------------------------------------
// MmsSettingsPrivate::createRepository
// @see header
//---------------------------------------------------------------
void MmsSettingsPrivate::createRepositoryL()
{
    CEventHandler* pObserver = new (ELeave) CEventHandler();
    CleanupStack::PushL(pObserver);

    CMsvSession* msvSession = CMsvSession::OpenSyncL(*pObserver);
    CleanupStack::PushL( msvSession);

    CMmsSettings* mmsSetting = CMmsSettings::NewL();
    CleanupStack::PushL(mmsSetting);

    mmsSetting->LoadSettingsL();

    TMsvId entryToBeKilled = KMsvNullIndexEntryId;
    // Get access to root index
    CMsvEntry* cEntry = msvSession->GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(cEntry);

    entryToBeKilled = mmsSetting->Service();
    cEntry->SetSortTypeL(TMsvSelectionOrdering(KMsvNoGrouping,
                                               EMsvSortByNone,
                                               ETrue));
    if (entryToBeKilled == KMsvNullIndexEntryId)
    {
        mmsSetting->CreateNewServiceL(*msvSession);
    }

    CleanupStack::PopAndDestroy(); // cEntry          

    mmsSetting->SetReceivingModeHome(EMmsReceivingAutomatic);
    mmsSetting->SetReceivingModeForeign(EMmsReceivingAutomatic);

    mmsSetting->SaveSettingsL();

    CleanupStack::PopAndDestroy(3); // settings    
}

void MmsSettingsPrivate::setMMSRetrievalL(MsgSettingEngine::MmsRetrieval aRetrieval)
{
    CEventHandler* pObserver = new (ELeave) CEventHandler();
    CleanupStack::PushL(pObserver);

    CMsvSession* mMsvSession = CMsvSession::OpenSyncL(*pObserver);
    CleanupStack::PushL(mMsvSession);

    CClientMtmRegistry* mMtmReg = CClientMtmRegistry::NewL(*mMsvSession);
    CleanupStack::PushL(mMtmReg);
    
    CMmsClientMtm* mMmsClient = static_cast<CMmsClientMtm*> 
                                      (mMtmReg->NewMtmL(KUidMsgTypeMultimedia));
    CleanupStack::PushL(mMmsClient);
    
    CMmsSettings* mMmsSetting = CMmsSettings::NewL();
    CleanupStack::PushL(mMmsSetting);

    //Load settings
    mMmsClient->RestoreSettingsL();
    mMmsSetting->CopyL( mMmsClient->MmsSettings() );

    switch ( aRetrieval )
                {
                case MsgSettingEngine::AutomInHomeNetwork:
                    {
                    mMmsSetting->SetReceivingModeHome( EMmsReceivingAutomatic );
                    mMmsSetting->SetReceivingModeForeign( EMmsReceivingManual );
                    break;
                    }
                case MsgSettingEngine::AlwaysAutomatic:
                    { 
                    mMmsSetting->SetReceivingModeHome( EMmsReceivingAutomatic );
                    mMmsSetting->SetReceivingModeForeign( EMmsReceivingAutomatic );
                    break;
                    }
                case  MsgSettingEngine::Manual:
                    {
                    mMmsSetting->SetReceivingModeHome( EMmsReceivingManual );
                    mMmsSetting->SetReceivingModeForeign( EMmsReceivingManual );
                    break;
                    }
                case MsgSettingEngine::Off:
                    {   
                    mMmsSetting->SetReceivingModeHome( EMmsReceivingPostpone );
                    mMmsSetting->SetReceivingModeForeign( EMmsReceivingPostpone );
                    break;
                    }
                case MsgSettingEngine::No:
                   {   
                   mMmsSetting->SetReceivingModeHome( EMmsReceivingReject );
                   mMmsSetting->SetReceivingModeForeign( EMmsReceivingReject );
                   break;
                   }
                default:
                    break;
                }
    
    
    //save the settings
    mMmsClient->SetSettingsL( *mMmsSetting );
    //TODO:User Input should be blocked when save is being done
    //This should be called as this takes care of automatic 
    //downloading of messages upon change of retrieval mode
    mMmsClient->StoreSettingsL();

    CleanupStack::PopAndDestroy(5);
}

void MmsSettingsPrivate::setAnonymousMessagesL(TBool aAnonymous)
{
    CMmsAccount* mMmsAccount = CMmsAccount::NewL();
    CleanupStack::PushL(mMmsAccount);

    CMmsSettings* mMmsSetting = CMmsSettings::NewL();
    CleanupStack::PushL(mMmsSetting);

    mMmsAccount->LoadSettingsL(*mMmsSetting);

    //do your operation
    mMmsSetting->SetAcceptAnonymousMessages(aAnonymous);

    //save the settings
    mMmsAccount->SaveSettingsL(*mMmsSetting);
    CleanupStack::PopAndDestroy(2);
}

void MmsSettingsPrivate::setReceiveMMSAdvertsL(TBool aReceiveAdvert)
{
    CMmsAccount* mMmsAccount = CMmsAccount::NewL();
    CleanupStack::PushL(mMmsAccount);

    CMmsSettings* mMmsSetting = CMmsSettings::NewL();
    CleanupStack::PushL(mMmsSetting);

    mMmsAccount->LoadSettingsL(*mMmsSetting);

    //do your operation
    mMmsSetting->SetAcceptAdvertisementMessages(aReceiveAdvert);

    //save the settings
    mMmsAccount->SaveSettingsL(*mMmsSetting);
    CleanupStack::PopAndDestroy(2);
}

void MmsSettingsPrivate::advanceMmsSettingsL(
                                            MsgSettingEngine::MmsRetrieval& aRetrieval,
                                            TBool& aAnonymousStatus,
                                            TBool& aMmsAdvertsStatus)
{
    CMmsAccount* mMmsAccount = CMmsAccount::NewL();
    CleanupStack::PushL(mMmsAccount);

    CMmsSettings* mMmsSetting = CMmsSettings::NewL();
    CleanupStack::PushL(mMmsSetting);

    mMmsAccount->LoadSettingsL(*mMmsSetting);

    TMmsReceivingMode receveModeHome = mMmsSetting->ReceivingModeHome();
    TMmsReceivingMode receveModeForeign = mMmsSetting->ReceivingModeForeign();
   
    
    // if default value is not acceptable, choose next by keeping costs low
       if ( receveModeHome == EMmsReceivingAutomatic &&
           receveModeForeign == EMmsReceivingAutomatic )
       {
       aRetrieval = MsgSettingEngine::AlwaysAutomatic;
       }
       else if ( receveModeHome == EMmsReceivingManual &&
           receveModeForeign == EMmsReceivingManual )
       {
       aRetrieval = MsgSettingEngine::Manual;   
       }
       else if ( receveModeHome == EMmsReceivingReject &&
           receveModeForeign == EMmsReceivingReject )
       {
       aRetrieval = MsgSettingEngine::No; 
       }
       else if ( receveModeHome == EMmsReceivingPostpone &&
           receveModeForeign == EMmsReceivingPostpone )
       {
       aRetrieval = MsgSettingEngine::Off;   
       }
       // Should always be automatic@home & manaul@roam
       else
       {
       aRetrieval = MsgSettingEngine::AutomInHomeNetwork;   
       }
    aAnonymousStatus = mMmsSetting->AcceptAnonymousMessages();

    aMmsAdvertsStatus = mMmsSetting->AcceptAdvertisementMessages();

    CleanupStack::PopAndDestroy(2);
}

//---------------------------------------------------------------
// SmsSettings::iAPSelector
// @see header
//---------------------------------------------------------------
void MmsSettingsPrivate::getAllAccessPointsL(
                                            RPointerArray<HBufC>& aAccessPoints,
                                            TInt& aDefaultIndex)
{
#ifdef _DEBUG_TRACES_
		qDebug() << "Enter MmsSettingsPrivate::getAllAccessPoints: Index= " << aDefaultIndex;
#endif

    CCommsDatabase *db;
    CCommsDbTableView *view;
    TInt result, err;

    //select the access point and store it in cenrep
    CMmsAccount* mmsAccount = CMmsAccount::NewL();
    CleanupStack::PushL(mmsAccount);

    CMmsSettings* mMmsSetting = CMmsSettings::NewL();
    CleanupStack::PushL(mMmsSetting);

    mmsAccount->LoadSettingsL(*mMmsSetting);

    //select the default access point
    TInt32 defaultId = mMmsSetting->AccessPoint(0);

    TBuf<KCommsDbSvrMaxColumnNameLength> iapName;
    TBuf<KCommsDbSvrMaxFieldLength> serviceType;
    TUint32 iapId = 0;
    db = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(db);
    view = db->OpenTableLC(TPtrC(IAP));

    // Walk through records
    result = view->GotoFirstRecord();
    TInt recordIndex = 0;
    while (result == KErrNone)
    {
        TRAP(err, view->ReadTextL(TPtrC(COMMDB_NAME), iapName));
        TRAP(err, view->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType));
#ifdef __WINSCW__
        if (serviceType.CompareF(TPtrC(LAN_SERVICE)) == 0)
        {
            HBufC* accessPointName = iapName.Alloc();
            aAccessPoints.AppendL(accessPointName);
            TRAP(err, view->ReadUintL(TPtrC(COMMDB_ID), iapId));
            
            // check to see if this is default set for MMsAccesspoint
            if (defaultId == iapId)
            {
               aDefaultIndex = recordIndex;
            }
            recordIndex++;
        }
#else
        if(serviceType.CompareF(TPtrC(OUTGOING_GPRS)) == 0)
        {
            HBufC* accessPointName = iapName.Alloc();
            aAccessPoints.AppendL(accessPointName);
            TRAP(err, view->ReadUintL(TPtrC(COMMDB_ID), iapId));
            
            // check to see if this is default set for MMsAccesspoint
            if (defaultId == iapId)
            {
                aDefaultIndex = recordIndex;
            }
        recordIndex++;
        }
#endif           
        TRAP(err, result = view->GotoNextRecord());
    }
    CleanupStack::PopAndDestroy(4); // nMmsSettings, db,view
#ifdef _DEBUG_TRACES_
		qDebug() << "Exit MmsSettingsPrivate::getAllAccessPoints: Index=" 
	          << aDefaultIndex
	          << "\n";
#endif

}

//---------------------------------------------------------------
// MmsSettingsPrivate::setMMSAccesspoint
// @see header
//---------------------------------------------------------------
void MmsSettingsPrivate::setMMSAccesspointL(TInt& aDefaultIndex)
{
#ifdef _DEBUG_TRACES_
		qDebug() << "Enter MmsSettingsPrivate::setMMSAccesspoint: Index=" 
	          << aDefaultIndex
	          << "\n";
#endif

	          
    //select the access point and store it in cenrep
    CMmsAccount* mmsAccount = CMmsAccount::NewL();
    CleanupStack::PushL(mmsAccount);

    CMmsSettings* mMmsSetting = CMmsSettings::NewL();
    CleanupStack::PushL(mMmsSetting);

    mmsAccount->LoadSettingsL(*mMmsSetting);

    CCommsDatabase *db;
    CCommsDbTableView *view;
    TInt result, err;

    TCommDbConnPref prefs;

    TBuf<KCommsDbSvrMaxColumnNameLength> iapName;
    TBuf<KCommsDbSvrMaxFieldLength> serviceType;
    TUint32 iapId = 0;
    db = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(db);
    view = db->OpenTableLC(TPtrC(IAP));

    // Walk through records
    result = view->GotoFirstRecord();
    TInt serviceCounter = 0;
    while (result == KErrNone)
    {
        TRAP(err, view->ReadTextL(TPtrC(COMMDB_NAME), iapName));
        TRAP(err, view->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType));

#ifdef __WINSCW__
        if (serviceType.CompareF(TPtrC(LAN_SERVICE)) == 0 && serviceCounter
                == aDefaultIndex)
        {
            TRAP(err, view->ReadUintL(TPtrC(COMMDB_ID), iapId));
            prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
            prefs.SetDirection(ECommDbConnectionDirectionOutgoing);
            prefs.SetIapId(iapId);

            while (mMmsSetting->AccessPointCount())
            {
                mMmsSetting->DeleteAccessPointL(0);
            }
            if (iapId != 0)
            {
#ifdef _DEBUG_TRACES_
		        qDebug() << "Set MMs Accesspoint IAP ID:"
            	         << iapId;
#endif


                mMmsSetting->AddAccessPointL(iapId, 0);
            }
            mmsAccount->SaveSettingsL(*mMmsSetting);
            serviceCounter++;
        }
        else if (serviceType.CompareF(TPtrC(LAN_SERVICE)) == 0)
        	{
        		serviceCounter++;
        	}
#else
        if(serviceType.CompareF(TPtrC(OUTGOING_GPRS)) == 0 &&
                serviceCounter == aDefaultIndex)
        {
            TRAP(err, view->ReadUintL(TPtrC(COMMDB_ID), iapId));
            prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
            prefs.SetDirection(ECommDbConnectionDirectionOutgoing);
            prefs.SetIapId(iapId);

            while ( mMmsSetting->AccessPointCount() )
            {
#ifdef _DEBUG_TRACES_
                qDebug() << "Set MMs Accesspoint IAP ID:"
            	         << iapId;
#endif

                mMmsSetting->DeleteAccessPointL( 0 );
            }
            if ( iapId != 0 )
            {
                mMmsSetting->AddAccessPointL(iapId, 0);
            }
            mmsAccount->SaveSettingsL(*mMmsSetting);
            serviceCounter++;
        }
        else if(serviceType.CompareF(TPtrC(OUTGOING_GPRS)) == 0)
        {
        		serviceCounter++;
        }        	
#endif          
        TRAP(err, result = view->GotoNextRecord());
    }

    CleanupStack::PopAndDestroy(4); // mMmsSetting, db,view
#ifdef _DEBUG_TRACES_
		qDebug() << "Exit MmsSettingsPrivate::setMMSAccesspoint: Index=" 
	          << aDefaultIndex
	          << "\n";
#endif

}

//eof


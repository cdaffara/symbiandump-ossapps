/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     A class which takes care of reading the possible new
 *     service centres from SIM and adds them to pda-side Sms Settings.
 *
 */

// INCLUDE FILES
#include <mtuireg.h>
#include <mmlist.h>
#include <smutset.h>
#include <csmsaccount.h>
#include <centralrepository.h>
#include <MessagingVariant.hrh>
#include <MessagingInternalCRKeys.h>    
#include <startupdomainpskeys.h>
#include <rcustomerserviceprofilecache.h>

#include "coutboxobserver.h"
#include "simscnumberdetector.h"
#include "startupmonitor.h"

#include "debugtraces.h"



// ================= MEMBER FUNCTIONS =======================

EXPORT_C CMsgSimOperation* CMsgSimOperation::NewL( 
        MSimOperationObserver& aObserver )
    {
    QDEBUG_WRITE("CMsgSimOperation::NewL enter")

    CMsgSimOperation* self = new (ELeave) CMsgSimOperation(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    QDEBUG_WRITE("CMsgSimOperation::NewL End")
    return self;


    }


CMsgSimOperation::CMsgSimOperation(MSimOperationObserver& aObserver) :
iRetryCount(0), iObserver(aObserver)
    {
    }

void CMsgSimOperation::ConstructL()
    {   
    QDEBUG_WRITE("CMsgSimOperation::ConstructL enter")

    iCenRepSession = CRepository::NewL(KCRUidSmum);

    // initialise
    iMsvSession = CMsvSession::OpenSyncL(*this);
   
    // Observes the OUTBOX for any offline messages...
    iOutBoxObserver = COutboxObserver::NewL();

    // Create the SMS Service	
    TMsvId serviceId = CreateSmsServiceL();

    // Update the SMS cenrep with the default settings.
    CreateDefaultSettingsL(serviceId);

    iClientRegistry = CClientMtmRegistry::NewL(*iMsvSession); 
    iSmsClientMtm = 
    STATIC_CAST( CSmsClientMtm*, iClientRegistry->NewMtmL( KUidMsgTypeSMS )); 

    // Start the System state monitor
    iStartupMonitor = CStartUpMonitor::NewL(this);
    
    // Start the Auto-send AO, to handle offline SMS messages
    iOutBoxObserver->HandleMsvSessionReadyL(*iMsvSession);

    QDEBUG_WRITE("CMsgSimOperation::ConstructL exit")
    }

TMsvId CMsgSimOperation::CreateSmsServiceL()
    {
    TMsvId serviceEntryId = KMsvNullIndexEntryId;
    TInt err = KErrNone;
    TRAP( err, serviceEntryId = ServiceIdL());

    // If no service, create one
    if (err == KErrNotFound)
        {
        TMsvEntry entry;
        entry.iMtm = KUidMsgTypeSMS;
        entry.iType = KUidMsvServiceEntry;
        entry.SetReadOnly(EFalse);
        entry.SetVisible(EFalse);
        entry.iDate.HomeTime();
        entry.iDetails.Set(KSmsService);
        CMsvEntry* root = iMsvSession->GetEntryL(KMsvRootIndexEntryId);
        CleanupStack::PushL(root);

        // In case no root store, create one...
        if (!root->HasStoreL())
            {
            // --- The entry does not have a store. EditStoreL() will create one ---
            CMsvStore* store = root->EditStoreL();
            CleanupStack::PushL(store);
            store->CommitL();
            CleanupStack::PopAndDestroy(); // store
            store = NULL; // destroyed
            }
        root->CreateL(entry);
        CleanupStack::PopAndDestroy(); // root
        serviceEntryId = entry.Id();

        }
    return serviceEntryId;
    }


TMsvId CMsgSimOperation::ServiceIdL()
    {
    TMsvId id = KMsvNullIndexEntryId;
    CMsvEntry* root = iMsvSession->GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(root);
    TSmsUtilities::ServiceIdL(*root, id);
    CleanupStack::PopAndDestroy(root);
    return id;
    }


void CMsgSimOperation::CreateDefaultSettingsL(TMsvId aServiceId)
    {
    QDEBUG_WRITE("CMsgSimOperation::CreateDefaultSettingsL enter")

    CSmsSettings* serviceSettings = CSmsSettings::NewL();
    CleanupStack::PushL(serviceSettings);
    CSmsAccount* smsAccount = CSmsAccount::NewLC();

    // Read the RFS related settings from shared data.
    TInt originalCount = 0;
    smsAccount->LoadSettingsL(*serviceSettings);
    originalCount = serviceSettings->ServiceCenterCount();

    if (!originalCount)
        {
        QDEBUG_WRITE("Original count = 0")

        ReadDefaultSettingsFromSharedDataL(serviceSettings);

        // Rest of the sms settings, which are fixed.
        serviceSettings->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFInteger); //relative
        serviceSettings->SetDelivery(ESmsDeliveryImmediately);
        serviceSettings->SetCanConcatenate(ETrue);
        serviceSettings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxInvisible);
        serviceSettings->SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);
        serviceSettings->SetRejectDuplicate(ETrue);
        TInt descriptionLength = KSmsDescriptionLength;

        // Read the value for description length 
        CRepository* repository = CRepository::NewLC(KCRUidMuiuSettings);
        if (KErrNone == repository->Get(KMuiuDescriptionLength,
                descriptionLength))
            {
            //Make sure value is not zero
            descriptionLength = Max(descriptionLength, KSmsDescriptionLength);
            }
        CleanupStack::PopAndDestroy(); // repository
        serviceSettings->SetDescriptionLength(descriptionLength);

        // Set saving to commsdb
        serviceSettings->SetCommDbAction(CSmsSettings::EStoreToCommDb);
        serviceSettings->SetSmsBearerAction(CSmsSettings::EStoreToCommDb);

        }

    // Save settings
    CMsvEntry* service = iMsvSession->GetEntryL(aServiceId);
    CleanupStack::PushL(service);
    CMsvStore* msvstore = service->EditStoreL();
    CleanupStack::PushL(msvstore);

    TInt maxTries(5);
    TBool done(EFalse);
    while (maxTries && !done)
        {
        TRAPD( err, smsAccount->SaveSettingsL( *serviceSettings ) );
        if (err == KErrNone)
            {
            QDEBUG_WRITE("CMsgSimOperation::CreateDefaultSettingsL settings saved")

            done = ETrue;
            }
        else if (err == KErrLocked)
            {
            QDEBUG_WRITE("CMsgSimOperation::CreateDefaultSettingsL KErrLocked")

            // Wait a while and retry.
            User::After(100000); // 0.1 seconds
            maxTries--;
            }
        else
            {
            User::Leave(err);
            }
        }

    msvstore->CommitL();
    CleanupStack::PopAndDestroy(2); // msvstore, service
    CleanupStack::PopAndDestroy(2); // serviceSettings, smsAccount

    QDEBUG_WRITE("CMsgSimOperation::CreateDefaultSettingsL Exit") 
    }


void CMsgSimOperation::
ReadDefaultSettingsFromSharedDataL(CSmsSettings* aServiceSettings)
    {
    QDEBUG_WRITE("CMsgSimOperation::ReadDefaultSettingsFromSharedDataL Enter") 

    if ( iCenRepSession )
        {
        TInt readedSetting;

        // Delivery report 
        if (iCenRepSession->Get(KSmumDeliveryReport, readedSetting) != KErrNone)
            {
            readedSetting = KDefDeliveryReport;
            }
        aServiceSettings->SetDeliveryReport(readedSetting);

        // Validity period
        if (iCenRepSession->Get(KSmumValidityPeriod, readedSetting) != KErrNone)
            {
            readedSetting = KDefValidityPeriod;
            }
        aServiceSettings->SetValidityPeriod(readedSetting);

        // Message conversion
        if (iCenRepSession->Get(KSmumMessageConversion, readedSetting)
                != KErrNone)
            {
            readedSetting = KDefMessageConversion;
            }
        aServiceSettings->SetMessageConversion((TSmsPIDConversion) readedSetting);

        // Preferred connection
        if (iCenRepSession->Get(KSmumPreferredConnection, readedSetting)
                != KErrNone)
            {
            readedSetting = KDefPreferredConnection;
            }
        aServiceSettings->SetSmsBearer((CSmsSettings::TMobileSmsBearer) readedSetting);

        // Check if we need to store SMSC time stamp OR device Time stamp
        if (iCenRepSession->Get(KSmumShowSMSCTimeStamp, readedSetting) != KErrNone) 
					 {
            readedSetting = 0;
       		 }
        aServiceSettings->SetUseServiceCenterTimeStampForDate(readedSetting);

        // Reply via same centre 
        if (iCenRepSession->Get(KSmumRemoveReplyViaSameCentre, readedSetting)
                != KErrNone)
            {
            if (iCenRepSession->Get(KSmumReplyViaSameCentre, readedSetting)
                    != KErrNone)
                {
                readedSetting = KDefReplyViaSameCentre;
                }
            }
        else
            {
            if (!readedSetting)
                {
                if (iCenRepSession->Get(KSmumReplyViaSameCentre, readedSetting)
                        != KErrNone)
                    {
                    readedSetting = KDefReplyViaSameCentre;
                    }
                }
            }
        aServiceSettings->SetReplyPath(readedSetting);
        }

    QDEBUG_WRITE("CMsgSimOperation::ReadDefaultSettingsFromSharedDataL Exit") 

    }

CMsgSimOperation::~CMsgSimOperation()
    {
    QDEBUG_WRITE("CMsgSimOperation::~CMsgSimOperation Enter") 
    delete iSimOperation;
    delete iSmsClientMtm;
    delete iClientRegistry;
    
    delete iOutBoxObserver;
    iOutBoxObserver = NULL;
    
    delete iMsvSession;
    delete iStartupMonitor;
    iStartupMonitor = NULL;
    QDEBUG_WRITE("CMsgSimOperation::~CMsgSimOperation Exit") 
    }

void CMsgSimOperation::StartL()
    {

    QDEBUG_WRITE("CMsgSimOperation::StartL Enter") 

    // Retry is used to define the times ReadSimParamsL() is called
    iRetryCount++;

    if ( IsSIMPresent() )
        {
        if ( HasSIMChanged() || HasNoSmscSettings() )
            {
            QDEBUG_WRITE("CMsgSimOperation::StartL Reading sim settings start") 

            CMsvOperationActiveSchedulerWait* wait = CMsvOperationActiveSchedulerWait::NewLC();
            iSimOperation = iSmsClientMtm->ReadSimParamsL(wait->iStatus);       
            wait->Start();
            TInt err = wait->iStatus.Int();
            StartRunL(err);
            CleanupStack::PopAndDestroy();
						QDEBUG_WRITE("CMsgSimOperation::StartL Reading sim settings end") 
            }
        }
    CompleteClientRequest(0);

    QDEBUG_WRITE("CMsgSimOperation::StartL Exit")   
    }

void CMsgSimOperation::CompleteClientRequest(TInt /*aValue*/)
    {
    iObserver.CompleteOperation();
    }

void CMsgSimOperation::Panic(TSimOperationPanic aPanic)
    {
    _LIT(KSimOpPanicCategory, "SIMOP");
    User::Panic(KSimOpPanicCategory, aPanic);
    }

void CMsgSimOperation::StartRunL(TInt aErr)
    {
    QDEBUG_WRITE("CMsgSimOperation::StartRunL Enter") 

    TInt error = aErr;

    if (error == KErrNone)
        { 
        TRAP( error, DoStartRunL());
        }

    // if problems with above; retry 
    TInt maxRetryCount = KSmumRetryCount;
    if (error == KErrTimedOut)
        {
        QDEBUG_WRITE("CMsgSimOperation::StartRunL ErrorTimed Out")     
        // no use to retry many times if timed out already
        maxRetryCount = KSmumRetryCount / 10; 
        }
    if (error != KErrNone && iRetryCount <= maxRetryCount)
        {
        // first cancel the current simOp if still ongoing
        if (iSimOperation)
            {         
            iSimOperation->Cancel();
            delete iSimOperation;
            iSimOperation = NULL;
            }

        // wait a bit and actual retry
        User::After(KSmumRetryDelay);
        StartL();
        return;
        }

    QDEBUG_WRITE("CMsgSimOperation::StartRunL Exit")     
    }

// ----------------------------------------------------
// CCMsgSimOperation::DoRunL
//
// ----------------------------------------------------
void CMsgSimOperation::DoStartRunL()
    {
    QDEBUG_WRITE("CMsgSimOperation::DoStartRunL Enter") 

    TIntBuf progressBuf;
    progressBuf.Copy(iSimOperation->ProgressL());
    TInt error = progressBuf();

    if (error != KErrNone)
        {
        QDEBUG_WRITE("CMsgSimOperation::DoStartRunL "
                "iSimOperation->ProgressL() error ") 
                return;
        }


    // Load current settings
    CSmsSettings* smsSettings = CSmsSettings::NewLC();
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    smsAccount->LoadSettingsL(*smsSettings);

    // Remove all old SMSC's configured
    TInt numSCAddresses = smsSettings->ServiceCenterCount();

    QDEBUG_WRITE_FORMAT("CMsgSimOperation::DoStartRunL numSCAddresses =",numSCAddresses)

    for (TInt j = numSCAddresses; j > 0; j--)
        {
        smsSettings->RemoveServiceCenter(j - 1);
        }

    // Add all SMSC's from SIM
    CMobilePhoneSmspList* centersList = iSimOperation->ServiceCentersLC();
    TInt count = centersList->Enumerate(); 

    QDEBUG_WRITE_FORMAT("CMsgSimOperation::DoStartRunL count from sim operation =",count)

    for ( TInt i = 0; i < count; i++ ) 
        {
        QDEBUG_WRITE("CMsgSimOperation::DoStartRunL inside for loop") 
        
        RMobileSmsMessaging::TMobileSmspEntryV1 entry;
        entry = centersList->GetEntryL(i);

        QDEBUG_WRITE("CMsgSimOperation::DoStartRunL Mobile sms entry read")
        
        // If empty tel number field, don't add
        if (entry.iServiceCentre.iTelNumber == KNullDesC)
            {
            continue;
            }

        QDEBUG_WRITE("CMsgSimOperation::DoStartRunL create name")
		
		TBool duplicateFound(EFalse);

		numSCAddresses = smsSettings->ServiceCenterCount();
        for ( TInt j = 0; j < numSCAddresses; j++ )
			{
			if ( entry.iServiceCentre.iTelNumber == smsSettings->GetServiceCenter( j ).Address() )
				{
                QDEBUG_WRITE_FORMAT("DoStartRunL - Duplicate. SMSC ", i)
                QDEBUG_WRITE_FORMAT("DoStartRunL -    of Sms Settings SMSC ", j)
                duplicateFound = ETrue;
                break;
				}
             }
        if(!duplicateFound)
            {            
			TBuf<KMaxNameLength> name;
            name = entry.iText;        
            if ( name == KNullDesC )
                {        
                name.Append(KSmscSimDefaultName); 
                name.AppendNum(i);
                }
    
            QDEBUG_WRITE("CMsgSimOperation::DoStartRunL name created")        
            smsSettings->AddServiceCenterL(name, entry.iServiceCentre.iTelNumber);
            QDEBUG_WRITE("CMsgSimOperation::DoStartRunL AddServiceCenterL completed")
            }
        
      	if (i==0)
            {
			smsSettings->SetDefaultServiceCenter(i);
			QDEBUG_WRITE("CMsgSimOperation::DoStartRunL SetDefaultServiceCenter completed")
			}      
        }

    // save settings
    smsAccount->SaveSettingsL(*smsSettings);    
    
    QDEBUG_WRITE("CMsgSimOperation::DoStartRunL SaveSettingsL completed")
    
    CleanupStack::PopAndDestroy(3, smsSettings); // centersList, smsAccount, smsSettings

    QDEBUG_WRITE("CMsgSimOperation::DoStartRunL Exit") 

    }

void CMsgSimOperation::HandleSessionEventL(TMsvSessionEvent aEvent,
        TAny* /*aArg1*/, TAny* /*aArg2*/,
        TAny* /*aArg3*/)
    {
    // problem case handling
    if (aEvent == EMsvServerFailedToStart)
        {
        // Nothing to do here 
        CompleteClientRequest(0);
        }

    else if ( (aEvent == EMsvServerTerminated) || (aEvent == EMsvCloseSession))
        {      
        delete iSimOperation; // These objects must be deleted first
        iSimOperation = NULL; // as they can't exist without a MsvSession

        delete iSmsClientMtm;
        iSmsClientMtm = NULL;

        delete iClientRegistry;
        iClientRegistry = NULL;
        
        iOutBoxObserver->HandleMsvSessionClosedL();
        delete iOutBoxObserver;
        iOutBoxObserver = NULL;
        
        delete iMsvSession;
        iMsvSession = NULL;

        CompleteClientRequest(0);
        }
    }

void CMsgSimOperation::HandleStartupReadyL()
    {
    QDEBUG_WRITE("CMsgSimOperation::HandleStartupReadyL Enter") 
    // Boot ready, start the real SimOperation
    StartL();

    QDEBUG_WRITE("CMsgSimOperation::HandleStartupReadyL Exit") 
    }

TBool CMsgSimOperation::IsSIMPresent()
    {
    QDEBUG_WRITE("CMsgSimOperation::IsSIMPresent Enter") 

    TInt status = KErrNone;
    TInt value = 0;
		status = RProperty::Get(KPSUidStartup, KPSSimStatus, value);

    if (status == KErrNone && value != ESimNotPresent)
        {
        QDEBUG_WRITE("CMsgSimOperation::IsSIMPresent returned True")

        return ETrue;      
        }
    
		QDEBUG_WRITE("CMsgSimOperation::IsSIMPresent returned False")
		return EFalse;
    }

TBool CMsgSimOperation::HasSIMChanged()
    {
    QDEBUG_WRITE("CMsgSimOperation::HasSIMChanged Enter") 

    TInt simValue = 0;
    TInt status = RProperty::Get(KPSUidStartup, KPSSimChanged, simValue);
    if (status == KErrNone && simValue == ESimChanged)
        {
        QDEBUG_WRITE("CMsgSimOperation::HasSIMChanged  returned True")
        return ETrue;
        }

    QDEBUG_WRITE("CMsgSimOperation::HasSIMChanged  returned False")
    return EFalse;
    }


TBool CMsgSimOperation::HasNoSmscSettings()
    {
    QDEBUG_WRITE("CMsgSimOperation::HasNoSmscSettings Enter") 

    CSmsSettings &settings = iSmsClientMtm->ServiceSettings();
    if (settings.ServiceCenterCount() > 0)
        {
        QDEBUG_WRITE("CMsgSimOperation::HasNoSmscSettings returned False")

        return EFalse;
        }

    QDEBUG_WRITE("CMsgSimOperation::HasNoSmscSettings returned True")
    return ETrue;
    }

//  End of File  

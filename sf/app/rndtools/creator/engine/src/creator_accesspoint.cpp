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
*
*/




//#include "creator_accesspoint.h" 
#include "creator_traces.h"
#include <ApListItemList.h>
#include <ApListItem.h> 
#include "enginewrapper.h"

static const TInt KAccessPointsFieldLength = 128;

_LIT(KCreatorAccessPointsPrefixName, "CR_");
_LIT(KZeroIP, "0.0.0.0");

//----------------------------------------------------------------------------

CAccessPointsParameters::CAccessPointsParameters()
    {
    LOGSTRING("Creator: CAccessPointsParameters::CAccessPointsParameters");

    iConnectionName = HBufC::New(KAccessPointsFieldLength);
    iWapStartPage = HBufC::New(KAccessPointsFieldLength);
    iGprsAcessPointName = HBufC::New(KAccessPointsFieldLength);
    iIspLoginName = HBufC::New(KAccessPointsFieldLength);
    iIspLoginPass = HBufC::New(KAccessPointsFieldLength);
    iWapGatewayAddress = HBufC::New(KAccessPointsFieldLength);
    iIspIPAddr = HBufC::New(KAccessPointsFieldLength);
    iIspIPNameServer1 = HBufC::New(KAccessPointsFieldLength);
    iIspIPNameServer2 = HBufC::New(KAccessPointsFieldLength);
    iIspDefaultTelNumber = HBufC::New(KAccessPointsFieldLength);
    iProxyServerAddress = HBufC::New(KAccessPointsFieldLength);
    }

CAccessPointsParameters::~CAccessPointsParameters()
    {
    LOGSTRING("Creator: CAccessPointsParameters::~CAccessPointsParameters");

    delete iProxyServerAddress;
    delete iIspDefaultTelNumber;
    delete iIspIPNameServer2;
    delete iIspIPNameServer1;
    delete iIspIPAddr;
    delete iWapGatewayAddress;
    delete iIspLoginPass;
    delete iIspLoginName;
    delete iGprsAcessPointName;
    delete iWapStartPage;
    delete iConnectionName;
    }

void CAccessPointsParameters::SetRandomCMNameL(CCreatorEngine& aEngine)
	{
	TInt num = aEngine.RandomNumber(1, 3);
	TBuf<10> apType;

	if (num==1)
		apType = _L("mms");
	else if (num==2)
		apType = _L("wap");
	else
		apType = _L("internet");

	TBuf<160> company = aEngine.RandomString(CCreatorEngine::ECompany);

	iConnectionName->Des() = KCreatorAccessPointsPrefixName;
	iConnectionName->Des().Append( company );
	iConnectionName->Des().Append(_L(" "));
	iConnectionName->Des().Append(apType);
	}

void CAccessPointsParameters::SetRandomLoginNameL(CCreatorEngine& aEngine)
	{
	iIspLoginName->Des() = aEngine.RandomString(CCreatorEngine::EFirstName);
	iIspLoginName->Des().LowerCase();
	}

void CAccessPointsParameters::SetRandomLoginPassL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomSecureAuthenticationL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomPromptPasswordL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomProxyAddressL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomProxyPortL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomStartPageL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomIPAddressL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomIP4NameServer1L(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomIP4NameServer2L(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomTelephoneNumberL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomBearerTypeIsdnL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomBearerSpeedL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomWapWspOptionL(CCreatorEngine& /*aEngine*/)
	{}

void CAccessPointsParameters::SetRandomGatewayAddressL(CCreatorEngine& /*aEngine*/)
	{}

//----------------------------------------------------------------------------

CCreatorAccessPoints* CCreatorAccessPoints::NewL(CCreatorEngine* aEngine)
    {
    CCreatorAccessPoints* self = CCreatorAccessPoints::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorAccessPoints* CCreatorAccessPoints::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorAccessPoints* self = new (ELeave) CCreatorAccessPoints;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCreatorAccessPoints::CCreatorAccessPoints()
    {
    }

void CCreatorAccessPoints::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorAccessPoints::ConstructL");

    iEngine = aEngine;

    iCommsDb = CCommsDatabase::NewL();;
    iApDataHandler = CApDataHandler::NewLC(*iCommsDb);
    CleanupStack::Pop();
    iApUtils = CApUtils::NewLC(*iCommsDb);
    CleanupStack::Pop();
    }

CCreatorAccessPoints::~CCreatorAccessPoints()
    {
    LOGSTRING("Creator: CCreatorAccessPoints::~CCreatorAccessPoints");

    if ( iEngine && iEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iEntryIds, KUidDictionaryUidIAP ) );
        }
    iEntryIds.Reset();
    iEntryIds.Close();
    
    delete iApUtils;
    delete iApDataHandler;
    delete iCommsDb;

    if (iParameters)
        delete iParameters;
    }

//----------------------------------------------------------------------------

TBool CCreatorAccessPoints::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorAccessPoints::AskDataFromUserL");
    
    CCreatorModuleBase::AskDataFromUserL( aCommand );//ignore retval
    
    if( aCommand == ECmdDeleteIAPs )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all Access Points?"), this, ECreatorModuleDelete );
        }
    else if( aCommand == ECmdDeleteCreatorIAPs )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all Access Points created with Creator?"), this, ECreatorModuleDelete );
        }

    return iEngine->GetEngineWrapper()->EntriesQueryDialog(&iEntriesToBeCreated, _L("How many entries to create?"), EFalse, this, ECreatorModuleStart );
    }


//----------------------------------------------------------------------------

TInt CCreatorAccessPoints::CreateConnectionSettingsEntryL(CCreatorModuleBaseParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorAccessPoints::CreateAccessPointEntryL");

    CAccessPointsParameters* parameters = (CAccessPointsParameters*) aParameters;
    
    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    
    
            
    // random data needed if no predefined data available
    if (!parameters)
        {
        TBuf<160> company = iEngine->RandomString(CCreatorEngine::ECompany);
        TInt num = iEngine->RandomNumber(1, 3);
        TBuf<10> apType;
        
        if (num==1)
            apType = _L("mms");
        else if (num==2)
            apType = _L("wap");
        else
            apType = _L("internet");
        
        iParameters = new(ELeave) CAccessPointsParameters;
        parameters = iParameters;

        parameters->SetRandomCMNameL(*iEngine);

        // in random mode just create GPRS access points
        parameters->iBearerType = EApBearerTypeGPRS;

        parameters->SetRandomLoginNameL(*iEngine);

        iParameters->iIspLoginPass->Des() = iEngine->RandomString(CCreatorEngine::ESurname);
        iParameters->iIspLoginPass->Des().LowerCase();

        iParameters->iSecureAuthentication = EFalse;
        iParameters->iPromptPassword = EFalse;

        iParameters->iProxyServerAddress->Des() = _L("127.0.0.1");
        iParameters->iProxyPortNumber = 80;

        iParameters->iWapStartPage->Des() = _L("http://");
        iParameters->iWapStartPage->Des().Append(apType);
        iParameters->iWapStartPage->Des().Append(_L("."));
        iParameters->iWapStartPage->Des().Append(company);
        iParameters->iWapStartPage->Des().Append(_L(".com"));
        iParameters->iWapStartPage->Des().LowerCase();

        iParameters->iIspIPAddr->Des() = KZeroIP;
        iParameters->iIspIPNameServer1->Des() = KZeroIP;
        iParameters->iIspIPNameServer1->Des() = KZeroIP;
        iParameters->iGprsAcessPointName->Des() = apType;
        iParameters->iIspDefaultTelNumber->Des() = iEngine->RandomString(CCreatorEngine::EPhoneNumber);
        iParameters->iIspBearerCallTypeIsdn = ECallTypeISDNv110;
        iParameters->iIspBearerSpeed = KSpeed14400;

        iParameters->iWapWspOption = EWapWspOptionConnectionOriented;
        iParameters->iWapGatewayAddress->Des() = KZeroIP;
        }
    else
        {
        // otherwise replace the parameters object
        iParameters = parameters;
        }

    TInt err = KErrNone;

    // access point item
    CApAccessPointItem* apItem = CApAccessPointItem::NewLC();

    // set the bearer type, if it fails try to set a default type
    TRAPD(bearerErr, apItem->SetBearerTypeL(iParameters->iBearerType));
    if (bearerErr != KErrNone)
        {
        iParameters->iBearerType = EApBearerTypeGPRS;
        TRAPD(bearerErr2, apItem->SetBearerTypeL(iParameters->iBearerType));
        bearerErr2=bearerErr2;
        }

    if (iParameters->iBearerType == EApBearerTypeGPRS) // GPRS
        {
        err = apItem->WriteLongTextL(EApGprsAccessPointName, iParameters->iGprsAcessPointName->Des());
        }
    else  // CSD or HCSD
        {
        err = apItem->WriteTextL(EApIspDefaultTelNumber, iParameters->iIspDefaultTelNumber->Des());
        err = apItem->WriteUint(EApIspBearerCallTypeIsdn, iParameters->iIspBearerCallTypeIsdn);
        err = apItem->WriteUint(EApIspBearerSpeed, iParameters->iIspBearerSpeed);
        }

    err = apItem->WriteTextL(EApIspLoginName, iParameters->iIspLoginName->Des());
    err = apItem->WriteTextL(EApIspIfAuthName, iParameters->iIspLoginName->Des());
    err = apItem->WriteTextL(EApIspLoginPass, iParameters->iIspLoginPass->Des());
    err = apItem->WriteTextL(EApIspIfAuthPass, iParameters->iIspLoginPass->Des());
    err = apItem->WriteTextL(EApGprsIfAuthPassword, iParameters->iIspLoginPass->Des());
    err = apItem->WriteBool(EApIspDisablePlainTextAuth, iParameters->iSecureAuthentication);
    err = apItem->WriteBool(EApIspPromptForLogin, iParameters->iPromptPassword);
    err = apItem->WriteTextL(EApWapGatewayAddress, iParameters->iWapGatewayAddress->Des());
    err = apItem->WriteLongTextL(EApProxyServerAddress, iParameters->iProxyServerAddress->Des());
    err = apItem->WriteUint(EApProxyPortNumber, iParameters->iProxyPortNumber);
    err = apItem->WriteLongTextL(EApWapStartPage, iParameters->iWapStartPage->Des());
    err = apItem->WriteUint(EApWapWspOption, iParameters->iWapWspOption);
    err = apItem->WriteTextL(EApGprsIPNameServer1, iParameters->iIspIPNameServer1->Des());
    err = apItem->WriteTextL(EApGprsIPNameServer2, iParameters->iIspIPNameServer2->Des());
    err = apItem->WriteTextL(EApIspIPAddr, iParameters->iIspIPAddr->Des());


    TUint32 uid = iApDataHandler->CreateFromDataL(*apItem);  // save the access point, returns the uid of the AP

    // id has been generated, store it for being able to delete
    // entries created only with Creator
    iEntryIds.AppendL( uid );
        
    TBuf<256> name = iParameters->iConnectionName->Des();
    iApUtils->SetNameL(name, uid); // sets the AP name


    // if no defaults set, set the created AP as a default

    // MMS access point
    if (iParameters->iWapStartPage->Des().FindF(_L("mms")) >=0  ||  iParameters->iConnectionName->Des().FindF(_L("mms")) >=0 )
        {
        //User::Panic(_L("Not done yet..."), 123);
        }


    // WAP access point
    else if (iParameters->iConnectionName->Des().FindF(_L("wap")) >=0  ||  iParameters->iGprsAcessPointName->Des().FindF(_L("wap")) >= 0)
        {
        TUint32 defID = 0;
        TRAP( err, defID = iApDataHandler->DefaultL(ETrue) );  // ETrue==WAP

        if (defID <= 1)
            TRAP( err, iApDataHandler->SetAsDefaultL(uid, EIspTypeWAPOnly) );

        }

    // Internet access point
    else if (iParameters->iConnectionName->Des().FindF(_L("internet")) >=0  ||  iParameters->iGprsAcessPointName->Des().FindF(_L("internet")) >= 0)
        {
        TUint32 defID = 0;
        TRAP( err, defID = iApDataHandler->DefaultL(EFalse) );  // EFalse==IAP

        if (defID <= 1)
            TRAP( err, iApDataHandler->SetAsDefaultL(uid, EIspTypeInternetOnly) );  //  <--  POSSIBLY NOT CORRECT, NEEDS MORE TESTING
        }



    CleanupStack::PopAndDestroy(); //apItem

    return err;
    }

//----------------------------------------------------------------------------

TUint32 CCreatorAccessPoints::AccessPointNameToIdL(const TDesC& aAPName, TBool /*aAnyIfNotFound*/ )
    {
    LOGSTRING("Creator: CCreatorEngine::AccessPointNameToIdL");

    TBuf<256> apName = aAPName.Left(256);
    apName.Trim();

    TUint32 APId = iEngine->GetDefaultIAPL();

    // return default IAP if requested
    if (apName.CompareF(_L("DEFAULT-IAP")) == 0 || apName.CompareF(_L("DEFAULT")) == 0)
        {
        return APId;
        }

    CCommsDatabase* db = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(db);
    CApUtils* apUtils = CApUtils::NewLC(*db);
    CApSelect* apSelect = CApSelect::NewLC(*db, KEApIspTypeAll, EApBearerTypeAll, KEApSortNameAscending);


    // loop through all access points and try to find a matching name
    if (apSelect->MoveToFirst())
        {
        do
            {
            TBuf<256> apName2;
            apUtils->NameL(apSelect->Uid(), apName2);
            apName2.Trim();

            if (apName2.CompareF( apName ) == 0)
                {
                // match found, get uid and break the loop
                TRAPD( err, APId = apUtils->IapIdFromWapIdL(apSelect->Uid()) );
                err=err;
                break;
                }
            }
        while (apSelect->MoveNext());
        }

    CleanupStack::PopAndDestroy(3);  //apSelect, apUtils, db

    return APId;
    }

//----------------------------------------------------------------------------

void CCreatorAccessPoints::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) // from MMsvSessionObserver
    {
    }

//----------------------------------------------------------------------------
void CCreatorAccessPoints::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorEngine::DeleteAllL");
    
    CApSelect* selector = CApSelect::NewLC( *iCommsDb, 
                                            KEApIspTypeAll,
                                            EApBearerTypeAllBearers,
                                            KEApSortUidAscending );
    
    CApListItemList* list = new (ELeave) CApListItemList();
    CleanupStack::PushL( list );
    TInt count = selector->AllListItemDataL( *list );
    for ( TInt i = 0; i < count; i++ )
        {
        TRAP_IGNORE( iApDataHandler->RemoveAPL( (*list)[i]->Uid() ) );
        }

    CleanupStack::PopAndDestroy( list );
    CleanupStack::PopAndDestroy( selector );

    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the Logs related registry
    iEngine->RemoveStoreL( KUidDictionaryUidIAP );
    }

//----------------------------------------------------------------------------

void CCreatorAccessPoints::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorEngine::DeleteAllCreatedByCreatorL");
    iEntryIds.Reset();
    
    // fetch ids of entries created by Creator
    iEngine->ReadEntryIdsFromStoreL( iEntryIds, KUidDictionaryUidIAP );
    
    // delete entries
    for ( TInt i = 0; i < iEntryIds.Count(); i++ )
        {
        TRAP_IGNORE( iApDataHandler->RemoveAPL( iEntryIds[i] ) );
        }
    
    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the Logs related registry
    iEngine->RemoveStoreL( KUidDictionaryUidIAP );
    }

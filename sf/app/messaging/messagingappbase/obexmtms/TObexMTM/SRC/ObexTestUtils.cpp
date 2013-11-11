// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include "ObexTestUtils.h"
// System includes
#include <msvids.h>
#include <msvuids.h>
#include <mtclbase.h>     // CBaseMtm
#include <mtsr.h>         // CBaseServerMtm
#include <btmsgtypeuid.h> // KUidMsgTypeBt
#include <irmsgtypeuid.h> // KUidMsgTypeIr
// User includes

_LIT(KObexTestPanicLiteral, "Obex Test");               // literal for panic


CObexTestUtils* CObexTestUtils::NewL(RTest& aTest, TUint aCreationFlags)
    {
    CObexTestUtils* self = NewLC(aTest, aCreationFlags);
    CleanupStack::Pop();
    return self;
    }

CObexTestUtils* CObexTestUtils::NewLC(RTest& aTest, TUint aCreationFlags)
    {
    CObexTestUtils* self = new (ELeave) CObexTestUtils(aTest);
    CleanupStack::PushL(self);
    self->ConstructL(aCreationFlags);
    return self;
    }

CObexTestUtils::CObexTestUtils(RTest& aTest) :
 CMsvTestUtils(aTest),
 iBTServiceId(KMsvUnknownServiceIndexEntryId),
 iIRServiceId(KMsvUnknownServiceIndexEntryId),
 iCurrentMtm(KUidMsvNullEntry)
    {
//	StartC32();     //TODO:- required?
    }

void CObexTestUtils::ConstructL(TUint aCreationFlags)
    {
    CMsvTestUtils::ConstructL(aCreationFlags);
    }

CObexTestUtils::~CObexTestUtils()
    {
    delete iBTClientMtm;
    delete iBTServerMtm;
    delete iIRClientMtm;
    delete iIRServerMtm;
    }

const CBaseMtm& CObexTestUtils::GetCurrentObexClientMtm() const
    {
    __ASSERT_ALWAYS( iCurrentMtm != KUidMsvNullEntry, User::Panic(KObexTestPanicLiteral, KErrNotReady) );
    return (iCurrentMtm == KUidMsgTypeBt) ? *iBTClientMtm : *iIRClientMtm;
    }

CBaseMtm& CObexTestUtils::GetCurrentObexClientMtm()
    {
    __ASSERT_ALWAYS( iCurrentMtm != KUidMsvNullEntry, User::Panic(KObexTestPanicLiteral, KErrNotReady) );
    return (iCurrentMtm == KUidMsgTypeBt) ? *iBTClientMtm : *iIRClientMtm;
    }

const CBaseServerMtm& CObexTestUtils::GetCurrentObexServerMtm() const
    {
    __ASSERT_ALWAYS( iCurrentMtm != KUidMsvNullEntry, User::Panic(KObexTestPanicLiteral, KErrNotReady) );
    return (iCurrentMtm == KUidMsgTypeBt) ? *iBTServerMtm : *iIRServerMtm;
    }

CBaseServerMtm& CObexTestUtils::GetCurrentObexServerMtm()
    {
    __ASSERT_ALWAYS( iCurrentMtm != KUidMsvNullEntry, User::Panic(KObexTestPanicLiteral, KErrNotReady) );
    return (iCurrentMtm == KUidMsgTypeBt) ? *iBTServerMtm : *iIRServerMtm;
    }

TMsvId CObexTestUtils::GetCurrentServiceId() const
    {
    __ASSERT_ALWAYS( iCurrentMtm != KUidMsvNullEntry, User::Panic(KObexTestPanicLiteral, KErrNotReady) );
    return (iCurrentMtm == KUidMsgTypeBt) ? iBTServiceId : iIRServiceId;
    }

void CObexTestUtils::InstantiateClientMtmsL()
    {
    InstantiateBTClientMtmL();
    InstantiateIRClientMtmL();
    }

void CObexTestUtils::InstantiateServerMtmsL()
    {
	InstantiateBTServerMtmL();
	InstantiateIRServerMtmL();
    }

void CObexTestUtils::InstantiateBTClientMtmL()
    {
    delete iBTClientMtm;
    iBTClientMtm = NULL;
    iBTClientMtm = InstantiateClientMtmL(KUidMsgTypeBt, iBTServiceId);
    }

void CObexTestUtils::InstantiateBTServerMtmL()
    {
    delete iBTServerMtm;
    iBTServerMtm = NULL;
    iBTServerMtm = InstantiateServerMtmL(KUidMsgTypeBt, iBTServiceId);
    }

void CObexTestUtils::InstantiateIRClientMtmL()
    {
    delete iIRClientMtm;
    iIRClientMtm = NULL;
    iIRClientMtm = InstantiateClientMtmL(KUidMsgTypeIrUID, iIRServiceId);
    }

void CObexTestUtils::InstantiateIRServerMtmL()
    {
    delete iIRServerMtm;
    iIRServerMtm = NULL;
    iIRServerMtm = InstantiateServerMtmL(KUidMsgTypeIrUID, iIRServiceId);
    }

void CObexTestUtils::DeleteServicesL()
    {
    DeleteServiceL(KUidMsgTypeBt);
    DeleteServiceL(KUidMsgTypeIrUID);
    }

void CObexTestUtils::CreateServicesL()
    {
    iBTServiceId = CreateObexServiceL(KUidMsgTypeBt);
    iIRServiceId = CreateObexServiceL(KUidMsgTypeIrUID);
    }

void CObexTestUtils::FindExistingServicesL()
    {
	TInt err = KErrNone;

    TRAP(err, ServiceIdL(KUidMsgTypeBt, iBTServiceId));

	if (err != KErrNone)
		{
		_LIT(KBTEntryNotFound, "Bluetooth Service Entry Not Found\n");
		Printf(KBTEntryNotFound);
		}

	TRAP(err, ServiceIdL(KUidMsgTypeIrUID, iIRServiceId));

	if (err != KErrNone)
		{
		_LIT(KIREntryNotFound, "Ir Service Entry Not Found\n");
		Printf(KIREntryNotFound);
		}
    }

void CObexTestUtils::CreateServerMtmRegsL()
    {
    User::Leave(KErrNotSupported);
    }

void CObexTestUtils::InstallMtmGroupsL()
    {
	if(iMsvSession)
		{
		User::LeaveIfError(iMsvSession->InstallMtmGroup(_L("z:\\system\\libs\\BTMTM.RSC")));
		User::LeaveIfError(iMsvSession->InstallMtmGroup(_L("z:\\system\\libs\\IRMTM.RSC")));
		}
	else
		WriteComment(_L("can't install mtms as there's no session\n"));
    }

TMsvId CObexTestUtils::CreateObexServiceL(TUid aMsgType)
    {
    __ASSERT_ALWAYS( aMsgType == KUidMsgTypeBt || aMsgType == KUidMsgTypeIrUID, Panic(KErrArgument) );
    
   	TInt err = KErrNone;
	TMsvId serviceId = 0;
	
    //Get the first service entry for the message type
    TRAP(err, ServiceIdL(aMsgType, serviceId));

	//Create a new service entry if one doesn't exist.  Otherwise, use the existing one
    if (err != KErrNone)
		{
		_LIT(KCreatingServiceEntry, "Creating New Service Entry.\n");
		Printf(KCreatingServiceEntry);
        serviceId = CreateServiceL(aMsgType);
		}
	else
		{
		_LIT(KUsingExistingServiceEntry, "Using Existing Service Entry.\n");
		Printf(KUsingExistingServiceEntry);
		}

    //set the service Id attribute for the particular message type
    TMsvId& id = (aMsgType == KUidMsgTypeBt ? iBTServiceId : iIRServiceId);
    id = serviceId;
    
    return serviceId;
    }


void CObexTestUtils::Reset()
    {
    delete iBTClientMtm;
    iBTClientMtm = NULL;
    delete iBTServerMtm;
    iBTServerMtm = NULL;
    delete iIRClientMtm;
    iIRClientMtm = NULL;
    delete iIRServerMtm;
    iIRServerMtm = NULL;

    CMsvTestUtils::Reset();
    }

void CObexTestUtils::Panic(TInt aPanic)
    {
    User::Panic(KObexTestPanicLiteral, aPanic);
    }


TUid CObexTestUtils::CurrentObexMtm() const
    {
    return iCurrentMtm;
    }

TBool CObexTestUtils::SelectObexMtmL(TUid aMsgType)
    {
    __ASSERT_ALWAYS( aMsgType == KUidMsgTypeBt || aMsgType == KUidMsgTypeIrUID, Panic(KErrArgument) );

    if (!iClientMtmRegistry)
		{
		iClientMtmRegistry = CClientMtmRegistry::NewL(*iMsvSession);
		}

    TBool present = iClientMtmRegistry->IsPresent(aMsgType);

    if(present)
        iCurrentMtm = aMsgType;

    return present;
    }


void CObexTestUtils::RegisterIRMtmL()
	{
	if(iMsvSession)
		{
		User::LeaveIfError(iMsvSession->InstallMtmGroup(_L("z:\\resource\\messaging\\mtm\\IRMTM.RSC")));
		}
	else
		WriteComment(_L("can't install IR Mtm as there's no session\n"));
	}

void CObexTestUtils::RegisterBTMtmL()
	{
	if(iMsvSession)
		{
		User::LeaveIfError(iMsvSession->InstallMtmGroup(_L("z:\\resource\\messaging\\mtm\\BTMTM.RSC")));
		}
	else
		WriteComment(_L("can't install BT Mtm as there's no session\n"));
	}

void CObexTestUtils::UnRegisterIRMtmL()
	{
	if(iMsvSession)
		{
		User::LeaveIfError(iMsvSession->DeInstallMtmGroup(_L("z:\\system\\libs\\IRMTM.RSC")));
		}
	else
		WriteComment(_L("can't deinstall IR Mtm as there's no session\n"));
	}

void CObexTestUtils::UnRegisterBTMtmL()
	{
	if(iMsvSession)
		{
		User::LeaveIfError(iMsvSession->DeInstallMtmGroup(_L("z:\\system\\libs\\BTMTM.RSC")));
		}
	else
		WriteComment(_L("can't deinstall BT Mtm as there's no session\n"));
	}

/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cserverdiscoveryguard.h"
#include "cimageprintbody.h"
#include "clog.h"
#include "tmessagewrp2.h"

CServerDiscoveryGuard* CServerDiscoveryGuard::NewL( CImagePrintBody& aEngine )
    {
	CServerDiscoveryGuard* self = new ( ELeave ) CServerDiscoveryGuard( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop();    // self
    return self;
    }

CServerDiscoveryGuard::CServerDiscoveryGuard( CImagePrintBody& aEngine ) : iEngine( aEngine ),
													iRequestActive( EFalse )
    {
    }

CServerDiscoveryGuard::~CServerDiscoveryGuard()
    {
    LOG("CServerDiscoveryGuard::~CServerDiscoveryGuard begin");
    iBuffer.Close();
    LOG("CServerDiscoveryGuard::~CServerDiscoveryGuard end");
    }

void CServerDiscoveryGuard::ConstructL()
    {
    }

TInt CServerDiscoveryGuard::StartL( TMessageWrp2& aMessage )
	{
	LOG("CServerDiscoveryGuard::StartL begin");
	iRequestActive = EFalse;
	iBuffer.Reset();
	TInt protocols = aMessage.Int0();
	TInt err = iEngine.StartDiscovery( *this, protocols );
	LOG1("CServerDiscoveryGuard::StartL err: %d", err);
	return err;
	}

void CServerDiscoveryGuard::ContinueL( TMessageWrp2& aMessage )
	{
	LOG("CServerDiscoveryGuard::ContinueL begin");
	if( iMessage ) iMessage->SetDisposable( ETrue );
	iMessage = &aMessage;
	iRequestActive = ETrue;
	if( iBuffer.Count() )
		{
		LOG("CServerDiscoveryGuard::ContinueL before ProcessL");
		ProcessL();
		LOG("CServerDiscoveryGuard::ContinueL after ProcessL");
		}
	LOG("CServerDiscoveryGuard::ContinueL end");
	}

void CServerDiscoveryGuard::Stop()
	{
	LOG("CServerDiscoveryGuard::Stop begin");
	iBuffer.Reset();
	if( iRequestActive )
		{
		LOG("CServerDiscoveryGuard::Stop cancelling...");
		iMessage->Complete( KErrCancel );
		iRequestActive = EFalse;
		}
	LOG("CServerDiscoveryGuard::Stop end");
	}

void CServerDiscoveryGuard::FoundPrinterL(const TPrinter& aPrinterInfo)
	{
	LOG("CServerDiscoveryGuard::FoundPrinterL begin");
	LOG1("CServerDiscoveryGuard::FoundPrinterL aPrinterInfo.iProtocol: %d", aPrinterInfo.iProtocol);
	LOG1("CServerDiscoveryGuard::FoundPrinterL aPrinterInfo.iProperties: %d", aPrinterInfo.iProperties);
	LOG1("CServerDiscoveryGuard::FoundPrinterL aPrinterInfo.iPrinterID: %d", aPrinterInfo.iPrinterID);
	LOG1("CServerDiscoveryGuard::FoundPrinterL aPrinterInfo.iDisplayName: %S", &(aPrinterInfo.iDisplayName));
	LOG1("CServerDiscoveryGuard::FoundPrinterL aPrinterInfo.iVendor: %d", aPrinterInfo.iVendor);
	TDiscoveryGuardData data;
	data.iCb = TDiscoveryGuardData::EFoundCb;
	data.iPrinterInfo = aPrinterInfo;
	User::LeaveIfError( iBuffer.Append( data ) );
	ProcessL();
	LOG("CServerDiscoveryGuard::FoundPrinterL end");
	}

void CServerDiscoveryGuard::DiscoveryStatusL(TInt aStatus, TInt aErrorCode, TInt aErrorStringCode)
	{
	LOG("CServerDiscoveryGuard::DiscoveryStatusL begin");
	LOG1("CServerDiscoveryGuard::DiscoveryStatusL aStatus: %d", aStatus);
	LOG1("CServerDiscoveryGuard::DiscoveryStatusL aErrorCode: %d", aErrorCode);
	LOG1("CServerDiscoveryGuard::DiscoveryStatusL aErrorStringCode: %d", aErrorStringCode);
	TDiscoveryGuardData data;
	data.iCb = TDiscoveryGuardData::EStatusCb;
	data.iStatus = aStatus;
	data.iErrorCode = aErrorCode;
	data.iErrorStringCode = aErrorStringCode;
	User::LeaveIfError( iBuffer.Append( data ) );
	ProcessL();
	LOG("CServerDiscoveryGuard::DiscoveryStatusL end");
	}

void CServerDiscoveryGuard::RemovePrinterL(const TPrinter& aPrinterInfo)
	{
	LOG("CServerDiscoveryGuard::RemovePrinterL begin");
	LOG1("CServerDiscoveryGuard::RemovePrinterL aPrinterInfo.iProtocol: %d", aPrinterInfo.iProtocol);
	LOG1("CServerDiscoveryGuard::RemovePrinterL aPrinterInfo.iProperties: %d", aPrinterInfo.iProperties);
	LOG1("CServerDiscoveryGuard::RemovePrinterL aPrinterInfo.iPrinterID: %d", aPrinterInfo.iPrinterID);
	LOG1("CServerDiscoveryGuard::RemovePrinterL aPrinterInfo.iDisplayName: %S", &(aPrinterInfo.iDisplayName));
	TDiscoveryGuardData data;
	data.iCb = TDiscoveryGuardData::ERemoveCb;
	data.iPrinterInfo = aPrinterInfo;
	User::LeaveIfError( iBuffer.Append( data ) );
	ProcessL();
	LOG("CServerDiscoveryGuard::RemovePrinterL end");
	}

void CServerDiscoveryGuard::ProcessL()
	{
	LOG("CServerDiscoveryGuard::ProcessL begin");
	if( iBuffer.Count() && iRequestActive )
		{
		LOG("CServerDiscoveryGuard::ProcessL step 1");
		TPtr8 ptr(reinterpret_cast<TUint8*>(&(iBuffer[0])), sizeof(iBuffer[0]), sizeof(iBuffer[0]));
		LOG("CServerDiscoveryGuard::ProcessL step 2");
		iMessage->WriteL( 0, ptr );
		LOG("CServerDiscoveryGuard::ProcessL step 3");
		iMessage->Complete( KErrNone );
		LOG("CServerDiscoveryGuard::ProcessL step 4");
		iRequestActive = EFalse;
		LOG("CServerDiscoveryGuard::ProcessL step 5");
		iBuffer.Remove( 0 );
		LOG("CServerDiscoveryGuard::ProcessL step 6");
		}
	LOG("CServerDiscoveryGuard::ProcessL end");
	}

//  End of File

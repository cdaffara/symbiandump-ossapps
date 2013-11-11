/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/



#include "directprintserverjobguard.h"
#include "directprintbody.h"
#include "clog.h"
#include "directprintmessage.h"

CDirectPrintServerJobGuard* CDirectPrintServerJobGuard::NewL( CDirectPrintBody& aEngine )
	{
	CDirectPrintServerJobGuard* self = new ( ELeave ) CDirectPrintServerJobGuard( aEngine );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();    // self
	return self;
	}

CDirectPrintServerJobGuard::CDirectPrintServerJobGuard( CDirectPrintBody& aEngine )
	: iEngine( aEngine )
	, iRequestActive( EFalse )
	{
	}

CDirectPrintServerJobGuard::~CDirectPrintServerJobGuard()
	{
	LOG("CDirectPrintServerJobGuard::~CDirectPrintServerJobGuard begin");
	iImages.ResetAndDestroy();
	iImages.Close();
	iBuffer.Close();
	LOG("CDirectPrintServerJobGuard::~CDirectPrintServerJobGuard end");
	}

void CDirectPrintServerJobGuard::ConstructL()
	{
	}

void CDirectPrintServerJobGuard::PrepareL( TDpMessage& aMessage )
	{
	LOG("CDirectPrintServerJobGuard::PrepareL begin");
	TInt len = aMessage.GetDesLength( 0 );
	HBufC* buf = HBufC::NewLC( len );
	TPtr ptr( buf->Des() );
	aMessage.ReadL( 0, ptr );
	iImages.AppendL( buf );
	CleanupStack::Pop(); // buf
	LOG("CDirectPrintServerJobGuard::PrepareL end");
	}

void CDirectPrintServerJobGuard::Stop()	
	{
	LOG("CDirectPrintServerJobGuard::Stop begin");
	iImages.ResetAndDestroy();
	if( iRequestActive )
		{
		LOG("CDirectPrintServerJobGuard::Stop cancelling...");
		iMessage->Complete( KErrCancel );
		iRequestActive = EFalse;
		}
	LOG("CDirectPrintServerJobGuard::Stop end");
	}

TInt CDirectPrintServerJobGuard::CreateL( TDpMessage& aMessage )
	{
	LOG("CDirectPrintServerJobGuard::CreateL begin");
	iRequestActive = EFalse;
	iBuffer.Reset();
	TInt printer = aMessage.Int0();
	TInt err = iEngine.CreatePrintJobL( printer, iImages, *this );
	LOG1("CDirectPrintServerJobGuard::CreateL err: %d", err);
	if( err )
		{
		iImages.ResetAndDestroy();
		}
	LOG1("CDirectPrintServerJobGuard::CreateL return: %d", err);
	return err;
	}

void CDirectPrintServerJobGuard::ContinueCreateL( TDpMessage& aMessage )
	{
	LOG("CDirectPrintServerJobGuard::ContinueCreateL begin");
	if( iMessage ) iMessage->SetDisposable( ETrue );
	iMessage = &aMessage;
	iRequestActive = ETrue;
	if( iBuffer.Count() )
		{
		LOG("CDirectPrintServerJobGuard::ContinueCreateL before ProcessL");
		Process();
		LOG("CDirectPrintServerJobGuard::ContinueCreateL after ProcessL");
		}
	LOG("CDirectPrintServerJobGuard::ContinueCreateL end");
	}

void CDirectPrintServerJobGuard::PrintJobProgressEvent(TInt aStatus, TInt aPercentCompletion, TInt aJobStateCode)
	{
	LOG("CDirectPrintServerJobGuard::PrintJobProgressEvent begin");
	LOG1("CDirectPrintServerJobGuard::PrintJobProgressEvent aStatus: %d", aStatus);
	LOG1("CDirectPrintServerJobGuard::PrintJobProgressEvent aPercentCompletion: %d", aPercentCompletion);
	LOG1("CDirectPrintServerJobGuard::PrintJobProgressEvent aJobStateCode: %d", aJobStateCode);
	TDirectPrintJobGuardData data;
	data.iCb = TDirectPrintJobGuardData::EProgressEventCb;
	data.iStatus = aStatus;
	data.iPercentCompletion = aPercentCompletion;
	data.iJobStateCode = aJobStateCode;
	TRAPD( err, DoPreProcessL( data ) );
	LOG1("CDirectPrintServerJobGuard::PrintJobProgressEvent DoPreProcessL's TRAP err: %d", err);
	Process( err );
	LOG("CDirectPrintServerJobGuard::PrintJobProgressEvent end");
	}

void CDirectPrintServerJobGuard::PrintJobErrorEvent(TInt aError, TInt aErrorStringCode)
	{
	LOG("CDirectPrintServerJobGuard::PrintJobErrorEvent begin");
	LOG1("CDirectPrintServerJobGuard::PrintJobErrorEvent aError: %d", aError);
	LOG1("CDirectPrintServerJobGuard::PrintJobErrorEvent aErrorStringCode: %d", aErrorStringCode);
	TDirectPrintJobGuardData data;
	data.iCb = TDirectPrintJobGuardData::EErrorEventCb;
	data.iError = aError;
	data.iErrorStringCode = aErrorStringCode;
	TRAPD( err, DoPreProcessL( data ) );
	LOG1("CDirectPrintServerJobGuard::PrintJobErrorEvent DoPreProcessL's TRAP err: %d", err);
	Process( err );
	LOG("CDirectPrintServerJobGuard::PrintJobErrorEvent end");
	}

void CDirectPrintServerJobGuard::PrinterStatusEvent(TInt aError, TInt aErrorStringCode)
	{
	LOG("CDirectPrintServerJobGuard::PrinterStatusEvent begin");
	LOG1("CDirectPrintServerJobGuard::PrinterStatusEvent aError: %d", aError);
	LOG1("CDirectPrintServerJobGuard::PrinterStatusEvent aErrorStringCode: %d", aErrorStringCode);
	TDirectPrintJobGuardData data;
	data.iCb = TDirectPrintJobGuardData::EStatusEventCb;
	data.iError = aError;
	data.iErrorStringCode = aErrorStringCode;
	TRAPD( err, DoPreProcessL( data ) );
	LOG1("CDirectPrintServerJobGuard::PrinterStatusEvent DoPreProcessL's TRAP err: %d", err);
	Process( err );
	LOG("CDirectPrintServerJobGuard::PrinterStatusEvent end");
	}

void CDirectPrintServerJobGuard::PreviewImageEvent(TInt aFsBitmapHandle)
	{
	LOG("CDirectPrintServerJobGuard::PreviewImageEvent begin");
	LOG1("CDirectPrintServerJobGuard::PreviewImageEvent aFsBitmapHandle: %d", aFsBitmapHandle);
	TDirectPrintJobGuardData data;
	data.iCb = TDirectPrintJobGuardData::EImageEventCb;
	data.iFsBitmapHandle = aFsBitmapHandle;
	TRAPD( err, DoPreProcessL( data ) );
	LOG1("CDirectPrintServerJobGuard::PreviewImageEvent DoPreProcessL's TRAP err: %d", err);
	Process( err );
	LOG("CDirectPrintServerJobGuard::PreviewImageEvent end");
	}

void CDirectPrintServerJobGuard::DoPreProcessL( const TDirectPrintJobGuardData& aData )
	{
	LOG("CDirectPrintServerJobGuard::DoPreProcessL begin");
	User::LeaveIfError( iBuffer.Append( aData ) );
	LOG("CDirectPrintServerJobGuard::DoPreProcessL end");
	}

void CDirectPrintServerJobGuard::DoProcessL()
	{
	LOG("CDirectPrintServerJobGuard::ProcessL begin");
	if( iBuffer.Count() && iRequestActive )
		{
		LOG("CDirectPrintServerJobGuard::DoProcessL step 1");
		TPtr8 ptr(reinterpret_cast<TUint8*>(&(iBuffer[0])), sizeof(iBuffer[0]), sizeof(iBuffer[0]));
		LOG("CDirectPrintServerJobGuard::DoProcessL step 2");
		iMessage->WriteL( 0, ptr );
		LOG("CDirectPrintServerJobGuard::DoProcessL step 3");
		iMessage->Complete( KErrNone );
		LOG("CDirectPrintServerJobGuard::DoProcessL step 4");
		iRequestActive = EFalse;
		LOG("CDirectPrintServerJobGuard::DoProcessL step 5");
		iBuffer.Remove( 0 );
		LOG("CDirectPrintServerJobGuard::DoProcessL step 6");
		}	
	LOG("CDirectPrintServerJobGuard::ProcessL end");
	}

void CDirectPrintServerJobGuard::Process( TInt aErr )
	{
	LOG("CDirectPrintServerJobGuard::Process begin");
	if( aErr )
		{
		LOG1("CDirectPrintServerJobGuard::Process aErr: %d", aErr);
		iMessage->Complete( aErr );
		iRequestActive = EFalse;
		}
	else
		{
		LOG("CDirectPrintServerJobGuard::Process calling DoProcessL");
		TRAPD( err, DoProcessL() );
		LOG1("CDirectPrintServerJobGuard::Process DoProcessL's TRAP err: %d", err);
		if( err )
			{
			iMessage->Complete( err );
			iRequestActive = EFalse;
			}
		}
	LOG("CDirectPrintServerJobGuard::Process end");	
	}



// DEPRECATED

void CDirectPrintServerJobGuard::ShowMessageL( TInt /*aMsgLine1Code*/, TInt /*aMsgLine2Code*/ )
	{
	}

TBool CDirectPrintServerJobGuard::AskYesNoQuestionL( TInt /*aMsgLine1Code*/, TInt /*aMsgLine2Code*/ )
	{
	return EFalse;
	}

const TDesC& CDirectPrintServerJobGuard::AskForInputL( TInt /*aMsgLine1Code*/, TInt /*aMsgLine2Code*/ )
	{
	return KNullDesC;
	}

//  End of File

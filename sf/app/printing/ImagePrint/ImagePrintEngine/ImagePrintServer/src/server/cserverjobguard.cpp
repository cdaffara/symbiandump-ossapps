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



#include "cserverjobguard.h"
#include "cimageprintbody.h"
#include "clog.h"
#include "tmessagewrp2.h"

CServerJobGuard* CServerJobGuard::NewL( CImagePrintBody& aEngine )
    {	
	CServerJobGuard* self = new ( ELeave ) CServerJobGuard( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop();    // self
    return self;
    }

CServerJobGuard::CServerJobGuard( CImagePrintBody& aEngine ) : iEngine( aEngine ),
													iRequestActive( EFalse )
    {
    }

CServerJobGuard::~CServerJobGuard()
    { 
    LOG("CServerJobGuard::~CServerJobGuard begin");
    iImages.ResetAndDestroy();
    iImages.Close();
    iBuffer.Close();
    LOG("CServerJobGuard::~CServerJobGuard end");
    }

void CServerJobGuard::ConstructL()
    {
    }
    
void CServerJobGuard::PrepareL( TMessageWrp2& aMessage )
	{
	LOG("CServerJobGuard::PrepareL begin");	
	TInt len = aMessage.GetDesLength( 0 );
	HBufC* buf = HBufC::NewLC( len );
	TPtr ptr( buf->Des() );
	aMessage.ReadL( 0, ptr );
	iImages.AppendL( buf );
	CleanupStack::Pop(); // buf
	LOG("CServerJobGuard::PrepareL end");
	} 
	
void CServerJobGuard::Stop()	
	{
	LOG("CServerJobGuard::Stop begin");
	iImages.ResetAndDestroy();
	if( iRequestActive )
		{
		LOG("CServerJobGuard::Stop cancelling...");
		iMessage->Complete( KErrCancel );
		iRequestActive = EFalse;	
		}
	LOG("CServerJobGuard::Stop end");	
	}
	
TInt CServerJobGuard::CreateL( TMessageWrp2& aMessage )
	{
	LOG("CServerJobGuard::CreateL begin");
	iRequestActive = EFalse;
	iBuffer.Reset();
	TInt printer = aMessage.Int0();
	TInt err = iEngine.CreatePrintJobL( printer, iImages, *this );
	LOG1("CServerJobGuard::CreateL err: %d", err);
	if( err )
		{
		iImages.ResetAndDestroy();
		}
	LOG1("CServerJobGuard::CreateL return: %d", err);
	return err;
	}
	
void CServerJobGuard::ContinueCreateL( TMessageWrp2& aMessage )
	{
	LOG("CServerJobGuard::ContinueCreateL begin");
	if( iMessage ) iMessage->SetDisposable( ETrue );
	iMessage = &aMessage;
	iRequestActive = ETrue;
	if( iBuffer.Count() )
		{
		LOG("CServerJobGuard::ContinueCreateL before ProcessL");
		Process();
		LOG("CServerJobGuard::ContinueCreateL after ProcessL");
		}
	LOG("CServerJobGuard::ContinueCreateL end");
	}
    
void CServerJobGuard::PrintJobProgressEvent(TInt aStatus, TInt aPercentCompletion, TInt aJobStateCode)
	{
	LOG("CServerJobGuard::PrintJobProgressEvent begin");	
	LOG1("CServerJobGuard::PrintJobProgressEvent aStatus: %d", aStatus);
	LOG1("CServerJobGuard::PrintJobProgressEvent aPercentCompletion: %d", aPercentCompletion);
	LOG1("CServerJobGuard::PrintJobProgressEvent aJobStateCode: %d", aJobStateCode);		
	TJobGuardData data;
	data.iCb = TJobGuardData::EProgressEventCb;
	data.iStatus = aStatus;
	data.iPercentCompletion = aPercentCompletion;
	data.iJobStateCode = aJobStateCode;	
	TRAPD( err, DoPreProcessL( data ) );
	LOG1("CServerJobGuard::PrintJobProgressEvent DoPreProcessL's TRAP err: %d", err);	
	Process( err );	
	LOG("CServerJobGuard::PrintJobProgressEvent end");
	}
	
void CServerJobGuard::PrintJobErrorEvent(TInt aError, TInt aErrorStringCode)
	{
	LOG("CServerJobGuard::PrintJobErrorEvent begin");		
	LOG1("CServerJobGuard::PrintJobErrorEvent aError: %d", aError);
	LOG1("CServerJobGuard::PrintJobErrorEvent aErrorStringCode: %d", aErrorStringCode);	
	TJobGuardData data;
	data.iCb = TJobGuardData::EErrorEventCb;
	data.iError = aError;
	data.iErrorStringCode = aErrorStringCode;	
	TRAPD( err, DoPreProcessL( data ) );
	LOG1("CServerJobGuard::PrintJobErrorEvent DoPreProcessL's TRAP err: %d", err);	
	Process( err );	
	LOG("CServerJobGuard::PrintJobErrorEvent end");
	}

void CServerJobGuard::PrinterStatusEvent(TInt aError, TInt aErrorStringCode)
	{
	LOG("CServerJobGuard::PrinterStatusEvent begin");
	LOG1("CServerJobGuard::PrinterStatusEvent aError: %d", aError);
	LOG1("CServerJobGuard::PrinterStatusEvent aErrorStringCode: %d", aErrorStringCode);
	TJobGuardData data;
	data.iCb = TJobGuardData::EStatusEventCb;
	data.iError = aError;
	data.iErrorStringCode = aErrorStringCode;		
	TRAPD( err, DoPreProcessL( data ) );
	LOG1("CServerJobGuard::PrinterStatusEvent DoPreProcessL's TRAP err: %d", err);	
	Process( err );
	LOG("CServerJobGuard::PrinterStatusEvent end");	
	}

void CServerJobGuard::PreviewImageEvent(TInt aFsBitmapHandle)
	{
	LOG("CServerJobGuard::PreviewImageEvent begin");
	LOG1("CServerJobGuard::PreviewImageEvent aFsBitmapHandle: %d", aFsBitmapHandle);
	TJobGuardData data;
	data.iCb = TJobGuardData::EImageEventCb;
	data.iFsBitmapHandle = aFsBitmapHandle;		
	TRAPD( err, DoPreProcessL( data ) );
	LOG1("CServerJobGuard::PreviewImageEvent DoPreProcessL's TRAP err: %d", err);	
	Process( err );
	LOG("CServerJobGuard::PreviewImageEvent end");	
	}

void CServerJobGuard::DoPreProcessL( const TJobGuardData& aData )
	{
	LOG("CServerJobGuard::DoPreProcessL begin");
	User::LeaveIfError( iBuffer.Append( aData ) );
	LOG("CServerJobGuard::DoPreProcessL end");	
	}

void CServerJobGuard::DoProcessL()
	{
	LOG("CServerJobGuard::ProcessL begin");
	if( iBuffer.Count() && iRequestActive )
		{
		LOG("CServerJobGuard::DoProcessL step 1");																
		TPtr8 ptr(reinterpret_cast<TUint8*>(&(iBuffer[0])), sizeof(iBuffer[0]), sizeof(iBuffer[0]));
		LOG("CServerJobGuard::DoProcessL step 2");
		iMessage->WriteL( 0, ptr );
		LOG("CServerJobGuard::DoProcessL step 3");				
		iMessage->Complete( KErrNone );
		LOG("CServerJobGuard::DoProcessL step 4");
		iRequestActive = EFalse;
		LOG("CServerJobGuard::DoProcessL step 5");
		iBuffer.Remove( 0 );
		LOG("CServerJobGuard::DoProcessL step 6");
		}	
	LOG("CServerJobGuard::ProcessL end");
	}

void CServerJobGuard::Process( TInt aErr )
	{
	LOG("CServerJobGuard::Process begin");
	if( aErr )
		{
		LOG1("CServerJobGuard::Process aErr: %d", aErr);
		iMessage->Complete( aErr );
		iRequestActive = EFalse;
		}
	else
		{
		LOG("CServerJobGuard::Process calling DoProcessL");
		TRAPD( err, DoProcessL() );
		LOG1("CServerJobGuard::Process DoProcessL's TRAP err: %d", err);
		if( err )
			{
			iMessage->Complete( err );
			iRequestActive = EFalse;
			}		
		}			
	LOG("CServerJobGuard::Process end");	
	}	
	


// DEPRECATED

void CServerJobGuard::ShowMessageL( TInt /*aMsgLine1Code*/, TInt /*aMsgLine2Code*/ )
	{		
	}

TBool CServerJobGuard::AskYesNoQuestionL( TInt /*aMsgLine1Code*/, TInt /*aMsgLine2Code*/ )
	{
	return EFalse;	
	}

const TDesC& CServerJobGuard::AskForInputL( TInt /*aMsgLine1Code*/, TInt /*aMsgLine2Code*/ )
	{
	return KNullDesC;	
	}

//  End of File

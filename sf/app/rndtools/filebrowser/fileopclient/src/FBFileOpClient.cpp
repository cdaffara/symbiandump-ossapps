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
* Description:  
*
*/


// needed for client interface
#include "FBFileOpClient.h"

const TUint KDefaultMessageSlots=4;

// --------------------------------------------------------------------------------------------

static TInt StartServer()
//
// Start the server process. Simultaneous launching
// of two such processes should be detected when the second one attempts to
// create the server object, failing with KErrAlreadyExists.
//
	{
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);
	RProcess server;
	TInt r=server.Create(KMyServerImg,KNullDesC,serverUid);
	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		server.Kill(0);		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

// --------------------------------------------------------------------------------------------

TVersion RFBFileOpServerSession::Version(void) const
	{
	return(TVersion(KCopyServMajorVersionNumber,KCopyServMinorVersionNumber,KCopyServBuildVersionNumber));
	}

// --------------------------------------------------------------------------------------------

TInt RFBFileOpServerSession::Connect()
//
// Connect to the server, attempting to start it if necessary
//
	{
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KMyServerName,Version(),KDefaultMessageSlots);
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}

// --------------------------------------------------------------------------------------------

void RFBFileOpServerSession::Copy(const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch, TRequestStatus& aStatus) 
    {
    TFileOpArgs argsStruct = TFileOpArgs();
    argsStruct.iBuf1.Copy(aSourceFullName);
    argsStruct.iBuf2.Copy(aTargetFullName);
    argsStruct.iUint1 = aSwitch;
    
    iPckgBuf = argsStruct;
    TIpcArgs args(&iPckgBuf);
    SendReceive(EFileOpCopy, args, aStatus);
    }

// --------------------------------------------------------------------------------------------

TInt RFBFileOpServerSession::Rename(const TDesC& anOld, const TDesC& aNew, TUint aSwitch) 
    {
    TFileOpArgs argsStruct = TFileOpArgs();
    argsStruct.iBuf1.Copy(anOld);
    argsStruct.iBuf2.Copy(aNew);
    argsStruct.iUint1 = aSwitch;
    
    TPckgBuf<TFileOpArgs> pckgBuf;
    pckgBuf = argsStruct;
	TIpcArgs args(&pckgBuf);
	return SendReceive(EFileOpRename, args);
    }

// --------------------------------------------------------------------------------------------

TInt RFBFileOpServerSession::Attribs(const TDesC& aName, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch) 
    {
    TFileOpArgs argsStruct = TFileOpArgs();
    argsStruct.iBuf1.Copy(aName);
    argsStruct.iUint1 = aSetMask;
    argsStruct.iUint2 = aClearMask;
    argsStruct.iTime1 = aTime;
    argsStruct.iUint3 = aSwitch;
    
    TPckgBuf<TFileOpArgs> pckgBuf;
    pckgBuf = argsStruct;
	TIpcArgs args(&pckgBuf);
	return SendReceive(EFileOpAttribs, args);
    }

// --------------------------------------------------------------------------------------------

void RFBFileOpServerSession::RmDir( const TDesC& aDirName,
                                    TUint aSwitch,
                                    TRequestStatus& aStatus ) 
    {
    TFileOpArgs argsStruct = TFileOpArgs();
    argsStruct.iBuf1.Copy(aDirName);
    argsStruct.iUint1 = aSwitch;

    iPckgBuf = argsStruct;
    TIpcArgs args(&iPckgBuf);
	SendReceive(EFileOpRmDir, args, aStatus);
    }

// --------------------------------------------------------------------------------------------

void RFBFileOpServerSession::Delete(const TDesC& aName, TUint aSwitch, TRequestStatus& aStatus) 
    {
    TFileOpArgs argsStruct = TFileOpArgs();
    argsStruct.iBuf1.Copy(aName);
    argsStruct.iUint1 = aSwitch;

    iPckgBuf = argsStruct;
    TIpcArgs args(&iPckgBuf);
	SendReceive(EFileOpDelete, args, aStatus);
    }

// --------------------------------------------------------------------------------------------

TInt RFBFileOpServerSession::MkDirAll(const TDesC& aPath) 
    {
    TFileOpArgs argsStruct = TFileOpArgs();
    argsStruct.iBuf1.Copy(aPath);
    
    TPckgBuf<TFileOpArgs> pckgBuf;
    pckgBuf = argsStruct;
	TIpcArgs args(&pckgBuf);
	return SendReceive(EFileOpMkDirAll, args);
    }

// --------------------------------------------------------------------------------------------

TInt RFBFileOpServerSession::CreateEmptyFile(const TDesC& aName) 
    {
    TFileOpArgs argsStruct = TFileOpArgs();
    argsStruct.iBuf1.Copy(aName);
    
    TPckgBuf<TFileOpArgs> pckgBuf;
    pckgBuf = argsStruct;
	TIpcArgs args(&pckgBuf);
	return SendReceive(EFileOpCreateEmptyFile, args);
    }
// --------------------------------------------------------------------------------------------

TInt RFBFileOpServerSession::EraseMBR(TUint aDriveNumber) 
    {
    TFileOpArgs argsStruct = TFileOpArgs();
    argsStruct.iUint1 = aDriveNumber;
        
    TPckgBuf<TFileOpArgs> pckgBuf;
    pckgBuf = argsStruct;
	TIpcArgs args(&pckgBuf);
	return SendReceive(EFileOpEraseMBR, args);
    }
    
// --------------------------------------------------------------------------------------------

TInt RFBFileOpServerSession::PartitionDrive(TUint aDriveNumber, TUint aNumberOfPartitions) 
    {
    TFileOpArgs argsStruct = TFileOpArgs();
    argsStruct.iUint1 = aDriveNumber;
    argsStruct.iUint2 = aNumberOfPartitions;
        
    TPckgBuf<TFileOpArgs> pckgBuf;
    pckgBuf = argsStruct;
	TIpcArgs args(&pckgBuf);
	return SendReceive(EFileOpPartitionDrive, args);
    }

// --------------------------------------------------------------------------------------------

TInt RFBFileOpServerSession::CancelOp() 
    {
    TIpcArgs args( NULL );
    return SendReceive( EFileOpCancel, args );
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

EXPORT_C CFBFileOpClient* CFBFileOpClient::NewL()
    {
    CFBFileOpClient* self = new(ELeave) CFBFileOpClient;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C CFBFileOpClient::~CFBFileOpClient()
    {
    iFBFileOpServerSession.Close();
    }

// --------------------------------------------------------------------------------------------

CFBFileOpClient::CFBFileOpClient()
    {
    }

// --------------------------------------------------------------------------------------------

void CFBFileOpClient::ConstructL()
    {	
    User::LeaveIfError(iFBFileOpServerSession.Connect());
    }

// --------------------------------------------------------------------------------------------

EXPORT_C TInt CFBFileOpClient::Copy( const TDesC& aSourceFullName,
                                     const TDesC& aTargetFullName,
                                     TUint aSwitch ) 
    {
    CAsyncWaiterClient* waiter = CAsyncWaiterClient::NewLC();
    iFBFileOpServerSession.Copy( aSourceFullName, aTargetFullName, aSwitch, waiter->iStatus );
    waiter->StartAndWait();
    TInt err = waiter->Result();
    
    if ( err == KErrServerTerminated )
        {
        User::LeaveIfError( iFBFileOpServerSession.Connect() );
        iFBFileOpServerSession.Copy( aSourceFullName, aTargetFullName, aSwitch, waiter->iStatus );
        waiter->StartAndWait();
        err = waiter->Result();
        }
    CleanupStack::PopAndDestroy( waiter );
    return err;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C TInt CFBFileOpClient::Rename(const TDesC& anOld, const TDesC& aNew, TUint aSwitch) 
    {
    TInt err = iFBFileOpServerSession.Rename(anOld, aNew, aSwitch);
        
    if (err == KErrServerTerminated)
        {
        User::LeaveIfError(iFBFileOpServerSession.Connect());
        err = iFBFileOpServerSession.Rename(anOld, aNew, aSwitch);
        }
    
    return err;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C TInt CFBFileOpClient::Attribs(const TDesC& aName, TUint aSetMask, TUint aClearMask, const TTime& aTime, TUint aSwitch) 
    {
    TInt err = iFBFileOpServerSession.Attribs(aName, aSetMask, aClearMask, aTime, aSwitch);
    
    if (err == KErrServerTerminated)
        {
        User::LeaveIfError(iFBFileOpServerSession.Connect());
        err = iFBFileOpServerSession.Attribs(aName, aSetMask, aClearMask, aTime, aSwitch);
        }
    
    return err;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C TInt CFBFileOpClient::RmDir(const TDesC& aDirName, TUint aSwitch)
    {
    CAsyncWaiterClient* waiter = CAsyncWaiterClient::NewLC();
    iFBFileOpServerSession.RmDir( aDirName, aSwitch, waiter->iStatus );
    waiter->StartAndWait();
    TInt err = waiter->Result();
    
    if ( err == KErrServerTerminated )
        {
        User::LeaveIfError( iFBFileOpServerSession.Connect() );
        iFBFileOpServerSession.RmDir( aDirName, aSwitch, waiter->iStatus );
        waiter->StartAndWait();
        err = waiter->Result();
        }
    CleanupStack::PopAndDestroy( waiter );
    return err;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C TInt CFBFileOpClient::Delete(const TDesC& aName, TUint aSwitch)
    {
    CAsyncWaiterClient* waiter = CAsyncWaiterClient::NewLC();
    iFBFileOpServerSession.Delete(aName, aSwitch, waiter->iStatus );
    waiter->StartAndWait();
    TInt err = waiter->Result();
    
    if ( err == KErrServerTerminated )
        {
        User::LeaveIfError( iFBFileOpServerSession.Connect() );
        iFBFileOpServerSession.Delete(aName, aSwitch, waiter->iStatus );
        waiter->StartAndWait();
        err = waiter->Result();
        }
    CleanupStack::PopAndDestroy( waiter );
    return err;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C TInt CFBFileOpClient::MkDirAll(const TDesC& aPath) 
    {
    TInt err = iFBFileOpServerSession.MkDirAll(aPath);
    
    if (err == KErrServerTerminated)
        {
        User::LeaveIfError(iFBFileOpServerSession.Connect());
        err = iFBFileOpServerSession.MkDirAll(aPath);
        }
            
    return err;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C TInt CFBFileOpClient::CreateEmptyFile(const TDesC& aName) 
    {
    TInt err = iFBFileOpServerSession.CreateEmptyFile(aName);
    
    if (err == KErrServerTerminated)
        {
        User::LeaveIfError(iFBFileOpServerSession.Connect());
        err = iFBFileOpServerSession.CreateEmptyFile(aName);
        }

    return err;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C TInt CFBFileOpClient::EraseMBR(TUint aDriveNumber) 
    {
    TInt err = iFBFileOpServerSession.EraseMBR(aDriveNumber);
    
    if (err == KErrServerTerminated)
        {
        User::LeaveIfError(iFBFileOpServerSession.Connect());
        err = iFBFileOpServerSession.EraseMBR(aDriveNumber);
        }

    return err;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C TInt CFBFileOpClient::PartitionDrive(TUint aDriveNumber, TUint aNumberOfPartitions) 
    {
    TInt err = iFBFileOpServerSession.PartitionDrive(aDriveNumber, aNumberOfPartitions);
    
    if (err == KErrServerTerminated)
        {
        User::LeaveIfError(iFBFileOpServerSession.Connect());
        err = iFBFileOpServerSession.PartitionDrive(aDriveNumber, aNumberOfPartitions);
        }

    return err;
    }

// --------------------------------------------------------------------------------------------

EXPORT_C void CFBFileOpClient::CancelOp() 
    {
    TInt err = iFBFileOpServerSession.CancelOp();
    
    if ( err == KErrServerTerminated )
        {
        User::LeaveIfError( iFBFileOpServerSession.Connect() );
        err = iFBFileOpServerSession.CancelOp(); // Ignore return value this time
        }
    }

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

CAsyncWaiterClient* CAsyncWaiterClient::NewL( TInt aPriority )
    {
    CAsyncWaiterClient* self = new(ELeave) CAsyncWaiterClient( aPriority );
    return self;
    }

// --------------------------------------------------------------------------------------------

CAsyncWaiterClient* CAsyncWaiterClient::NewLC( TInt aPriority )
    {
    CAsyncWaiterClient* self = new (ELeave) CAsyncWaiterClient( aPriority );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------------------------
    
CAsyncWaiterClient::CAsyncWaiterClient( TInt aPriority ) : CActive( aPriority )
    {
    CActiveScheduler::Add( this );
    }   

// --------------------------------------------------------------------------------------------

CAsyncWaiterClient::~CAsyncWaiterClient()
    {
    Cancel();
    }

// --------------------------------------------------------------------------------------------
    
void CAsyncWaiterClient::StartAndWait()
    {
    SetActive();
    iWait.Start();
    }

// --------------------------------------------------------------------------------------------
    
TInt CAsyncWaiterClient::Result() const
    {
    return iError;
    }

// --------------------------------------------------------------------------------------------
    
void CAsyncWaiterClient::RunL()
    {
    iError = iStatus.Int();
    iWait.AsyncStop();
    }

// --------------------------------------------------------------------------------------------
    
void CAsyncWaiterClient::DoCancel()
    {
    iError = KErrCancel;
    if( iStatus == KRequestPending )
        {
        TRequestStatus* s = &iStatus;
        User::RequestComplete( s, KErrCancel );
        }
    iWait.AsyncStop();
    }

// --------------------------------------------------------------------------------------------


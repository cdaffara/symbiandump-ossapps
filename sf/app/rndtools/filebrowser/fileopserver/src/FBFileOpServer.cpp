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


#include "FBFileOpServer.h"
#include "FBDrivePartitioner.h"

#include <e32svr.h>
#include <bautils.h>

// --------------------------------------------------------------------------------------------

//***********************************
//CFBFileOpServer - implementations
//***********************************

CServer2* CFBFileOpServer::NewLC()
	{
	CFBFileOpServer* self = new(ELeave) CFBFileOpServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// --------------------------------------------------------------------------------------------

//
// 2nd phase construction - ensure the timer and server objects are running
//
void CFBFileOpServer::ConstructL()
	{
	StartL(KMyServerName);
	iShutdown.ConstructL();
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown.Start();
	}

// --------------------------------------------------------------------------------------------

//
// Cretae a new client session. This should really check the version number.
//
CSession2* CFBFileOpServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	return new(ELeave) CFBFileOpServerSession();
	}

// --------------------------------------------------------------------------------------------

//
// A new session is being created
// Cancel the shutdown timer if it was running
//
void CFBFileOpServer::AddSession()
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

// --------------------------------------------------------------------------------------------

//
// A session is being destroyed
// Start the shutdown timer if it is the last session.
//
void CFBFileOpServer::DropSession()
	{
	if (--iSessionCount==0)
		iShutdown.Start();
	}

// --------------------------------------------------------------------------------------------
    
CRequestObserver::CRequestObserver( TInt aPriority ) : CActive( aPriority )
    {
    CActiveScheduler::Add( this );
    }   

CRequestObserver::~CRequestObserver()
    {
    Cancel();
    }
    
void CRequestObserver::StartWaiting( const RMessage2& aMsg )
    {
    iMsg = aMsg;
    SetActive();
    }
    
void CRequestObserver::RunL()
    {
    iMsg.Complete( iStatus.Int() );
    }
    
void CRequestObserver::DoCancel()
    {
    iMsg.Complete( KErrCancel );
    }

// --------------------------------------------------------------------------------------------

//***********************************
//CFBFileOpServerSession - implementations
//***********************************

//
// 2nd phase construct for sessions - called by the CServer framework
//
void CFBFileOpServerSession::CreateL()
	{
	User::LeaveIfError(iFs.Connect());
	iFileMan = CFileMan::NewL( iFs, this );
	iReqObserver = new (ELeave) CRequestObserver( CActive::EPriorityStandard );
	Server().AddSession();
	}

// --------------------------------------------------------------------------------------------

CFBFileOpServerSession::~CFBFileOpServerSession()
	{
	if (iFileMan)
		{
		delete iFileMan;
		iFileMan = NULL;
		}
	iFs.Close();
	delete iReqObserver;
	Server().DropSession();
	}

// --------------------------------------------------------------------------------------------
	
/**
Services a client request.
*/
void CFBFileOpServerSession::ServiceL(const RMessage2& aMessage)
	{
	TBool completeImmediately( ETrue );
	TRAPD( err,DispatchMessageL( aMessage, completeImmediately ) );
	if ( completeImmediately ) aMessage.Complete( err );
	}

// --------------------------------------------------------------------------------------------

/**
Called by ServiceL()

It tests the function code and then delegates to
the appropriate function.
*/
void CFBFileOpServerSession::DispatchMessageL(const RMessage2& aMessage, TBool& aComplete)
	{
	switch (aMessage.Function())
        {
        case EFileOpCopy:
            CopyL(aMessage);
            aComplete = EFalse;
            return;

        case EFileOpRename:
            RenameL(aMessage);
            return;            

        case EFileOpAttribs:
            AttribsL(aMessage);
            return; 

        case EFileOpRmDir:
            RmDirL(aMessage, aComplete);
            return; 

        case EFileOpDelete:
            DeleteL(aMessage);
            aComplete = EFalse;
            return; 

        case EFileOpMkDirAll:
            MkDirAllL(aMessage);
            return;             

        case EFileOpCreateEmptyFile:
            CreateEmptyFileL(aMessage);
            return;            

        case EFileOpEraseMBR:
            EraseMBRL(aMessage);
            return;
            
        case EFileOpPartitionDrive:
            PartitionDriveL(aMessage);
            return;
            
        case EFileOpCancel: 
            CancelOp();
            return; 
            
        default:
            PanicClient(aMessage, EPanicIllegalFunction);
            return;
        }
	}

// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::CopyL( const RMessage2& aMessage )
    {
    __ASSERT_ALWAYS( iReqObserver && !iReqObserver->IsActive(), User::Leave( KErrServerBusy ) );
    TPckgBuf<TFileOpArgs> pckgBuf;
    aMessage.ReadL( 0, pckgBuf, 0 );
    TFileOpArgs argsStruct = pckgBuf();

    TInt err = iFileMan->Copy( argsStruct.iBuf1, argsStruct.iBuf2, argsStruct.iUint1, iReqObserver->iStatus );
    User::LeaveIfError( err );
    iFileManObserverResult = MFileManObserver::EContinue;
    iReqObserver->StartWaiting( aMessage ); // start asynchronous waiting
    }

// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::RenameL(const RMessage2& aMessage)
    {
    TPckgBuf<TFileOpArgs> pckgBuf;
    aMessage.ReadL(0, pckgBuf, 0);
    TFileOpArgs argsStruct = pckgBuf();
    
    iFileManObserverResult = MFileManObserver::EContinue;
    User::LeaveIfError(iFileMan->Rename(argsStruct.iBuf1, argsStruct.iBuf2, argsStruct.iUint1));
    }

// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::AttribsL(const RMessage2& aMessage)
    {
    TPckgBuf<TFileOpArgs> pckgBuf;
    aMessage.ReadL(0, pckgBuf, 0);
    TFileOpArgs argsStruct = pckgBuf();
    
    iFileManObserverResult = MFileManObserver::EContinue;
    User::LeaveIfError(iFileMan->Attribs(argsStruct.iBuf1, argsStruct.iUint1, argsStruct.iUint2, argsStruct.iTime1, argsStruct.iUint3));
    }

// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::RmDirL( const RMessage2& aMessage, TBool& aComplete )
    {
    TPckgBuf<TFileOpArgs> pckgBuf;
    aMessage.ReadL( 0, pckgBuf, 0 );
    TFileOpArgs argsStruct = pckgBuf();
    
    if ( argsStruct.iUint1 & CFileMan::ERecurse )
        {
        __ASSERT_ALWAYS( iReqObserver && !iReqObserver->IsActive(), User::Leave( KErrServerBusy ) );
        User::LeaveIfError( iFileMan->RmDir( argsStruct.iBuf1, iReqObserver->iStatus ) );
        iFileManObserverResult = MFileManObserver::EContinue;
        aComplete = EFalse;
        iReqObserver->StartWaiting( aMessage ); // start asynchronous waiting
        }
    else
        {
        iFileManObserverResult = MFileManObserver::EContinue;
        User::LeaveIfError( iFs.RmDir( argsStruct.iBuf1 ) );
        }
    }

// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::DeleteL( const RMessage2& aMessage )
    {
    __ASSERT_ALWAYS( iReqObserver && !iReqObserver->IsActive(), User::Leave( KErrServerBusy ) );
    TPckgBuf<TFileOpArgs> pckgBuf;
    aMessage.ReadL( 0, pckgBuf, 0 );
    TFileOpArgs argsStruct = pckgBuf();
    
    User::LeaveIfError( iFileMan->Delete(argsStruct.iBuf1, argsStruct.iUint1, iReqObserver->iStatus ) ); 
    iFileManObserverResult = MFileManObserver::EContinue;
    iReqObserver->StartWaiting( aMessage ); // start asynchronous waiting
    }

// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::MkDirAllL(const RMessage2& aMessage)
    {
    TPckgBuf<TFileOpArgs> pckgBuf;
    aMessage.ReadL(0, pckgBuf, 0);
    TFileOpArgs argsStruct = pckgBuf();
    
    User::LeaveIfError(iFs.MkDirAll(argsStruct.iBuf1));
    }

// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::CreateEmptyFileL(const RMessage2& aMessage)
    {
    TPckgBuf<TFileOpArgs> pckgBuf;
    aMessage.ReadL(0, pckgBuf, 0);
    TFileOpArgs argsStruct = pckgBuf();
    
    TInt err(KErrNone);
    RFile newFile;
    err = newFile.Create(iFs, argsStruct.iBuf1, EFileShareExclusive);
    if (err == KErrNone)
        err = newFile.Flush(); 
    newFile.Close(); 
    
    User::LeaveIfError(err);
    }

// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::EraseMBRL(const RMessage2& aMessage)
    {
    TPckgBuf<TFileOpArgs> pckgBuf;
    aMessage.ReadL(0, pckgBuf, 0);
    TFileOpArgs argsStruct = pckgBuf();

	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

    User::LeaveIfError(FBDrivePartioner::EraseMBR(fs, argsStruct.iUint1));

    CleanupStack::PopAndDestroy(); //fs
    }
    
// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::PartitionDriveL(const RMessage2& aMessage)
    {
    TPckgBuf<TFileOpArgs> pckgBuf;
    aMessage.ReadL(0, pckgBuf, 0);
    TFileOpArgs argsStruct = pckgBuf();

	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

    FBDrivePartioner::CreatePartitions(fs, argsStruct.iUint1, argsStruct.iUint2);
    User::LeaveIfError(FBDrivePartioner::FormatPartitions(fs, argsStruct.iUint1, argsStruct.iUint2));

    CleanupStack::PopAndDestroy(); //fs
    }
               
// --------------------------------------------------------------------------------------------

//
// Panics the client
//
void CFBFileOpServerSession::PanicClient(const RMessage2& aMessage, TInt aPanic) const
	{
	_LIT(KTxtServer,"FBFileOpServer");
	aMessage.Panic(KTxtServer, aPanic);
	}

// --------------------------------------------------------------------------------------------

//
// Handle an error from CFBFileOpServerSession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise use the default handling (report the error to the client)
//
void CFBFileOpServerSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError==KErrBadDescriptor)
		PanicClient(aMessage,EPanicBadDescriptor);
	CSession2::ServiceError(aMessage,aError);
	}

// --------------------------------------------------------------------------------------------

MFileManObserver::TControl CFBFileOpServerSession::NotifyFileManStarted()
    {
    return iFileManObserverResult;
    }

// --------------------------------------------------------------------------------------------

MFileManObserver::TControl CFBFileOpServerSession::NotifyFileManOperation()
    {
    return iFileManObserverResult;
    }
// --------------------------------------------------------------------------------------------

MFileManObserver::TControl CFBFileOpServerSession::NotifyFileManEnded()
    {
    return iFileManObserverResult;
    }

// --------------------------------------------------------------------------------------------

void CFBFileOpServerSession::CancelOp()
    {
    iFileManObserverResult = MFileManObserver::ECancel;
    iReqObserver->Cancel();
    }

// --------------------------------------------------------------------------------------------

//**********************************
//Global functions
//**********************************

//
// Initiate server exit when the timer expires
//
void CShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}

// --------------------------------------------------------------------------------------------

//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(RThread::RenameMe(KMyServerName));
	//
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CFBFileOpServer::NewLC();
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

// --------------------------------------------------------------------------------------------

//
// Server process entry-point
//
TInt E32Main()
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}

// --------------------------------------------------------------------------------------------

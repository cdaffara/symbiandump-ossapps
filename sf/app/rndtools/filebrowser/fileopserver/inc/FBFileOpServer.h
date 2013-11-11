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


#ifndef __FBFILEOPSERVER_H__
#define __FBFILEOPSERVER_H__

#include <e32base.h>
#include <f32file.h>
#include "FBFileOpClientServer.h"

// needed for creating server thread.
//const TUint KDefaultHeapSize=0x10000;

// reasons for server panic
enum TMyPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction
	};

class CShutdown : public CTimer
	{
	enum {KMyShutdownDelay=0x200000};	// approx 2s

public :
	inline CShutdown();
	inline void ConstructL();
	inline void Start();

private :
	void RunL();
	};

//**********************************
//CFBFileOpServer
//**********************************
/**
Our server class - an active object - and therefore derived ultimately from CActive.
It accepts requests from client threads and forwards
them to the client session to be dealt with. It also handles the creation
of the server-side client session.
*/
class CFBFileOpServer : public CServer2
	{

public :
	static CServer2* NewLC();
	void AddSession();
	void DropSession();

protected :

private :
	CFBFileOpServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

public :

private :
	TInt iSessionCount;
	CShutdown iShutdown;
	
	};

//**********************************
//CRequestObserver
//**********************************
/**
This class is for monitoring asynchronous request
and for completing related RMessage2 object.
*/
class CRequestObserver : public CActive
    {
public:
    CRequestObserver( TInt aPriority );
    virtual ~CRequestObserver();
    
    void StartWaiting( const RMessage2& aMsg );
    
private:
    // from CActive
    void RunL();
    void DoCancel();
    
private:
    RMessage2 iMsg;
    };


//**********************************
//CFBFileOpServerSession
//**********************************
/**
This class represents a session with the  server.
Functions are provided to respond appropriately to client messages.
*/
class CFBFileOpServerSession : public CSession2,
                               public MFileManObserver
	{

public :
	CFBFileOpServerSession();

private: // From MFileManObserver
    
    TControl NotifyFileManStarted();
    TControl NotifyFileManOperation();
    TControl NotifyFileManEnded();
    
protected :
	// panic the client
	void PanicClient(const RMessage2& aMessage,TInt aPanic) const;

private :
	~CFBFileOpServerSession();
	inline CFBFileOpServer& Server();
	void CreateL();
	
	//service requests
	void ServiceL(const RMessage2& aMessage);
	void DispatchMessageL(const RMessage2& aMessage, TBool& aComplete);
	
	void CopyL(const RMessage2& aMessage);
	void RenameL(const RMessage2& aMessage);
	void AttribsL(const RMessage2& aMessage);
	void RmDirL(const RMessage2& aMessage, TBool& aComplete);
	void DeleteL(const RMessage2& aMessage);
	void MkDirAllL(const RMessage2& aMessage);
	void CreateEmptyFileL(const RMessage2& aMessage);
	void EraseMBRL(const RMessage2& aMessage);
	void PartitionDriveL(const RMessage2& aMessage);

	void ServiceError(const RMessage2& aMessage, TInt aError);
	
	void CancelOp();
					
private :
	RFs iFs;
	RFile iFile;
	CFileMan* iFileMan;
	MFileManObserver::TControl iFileManObserverResult;
	CRequestObserver* iReqObserver;
	};


//**********************************
//Inlines
//**********************************

inline CShutdown::CShutdown()
	:CTimer(-1)
	{CActiveScheduler::Add(this);}

inline void CShutdown::ConstructL()
	{CTimer::ConstructL();}

inline void CShutdown::Start()
	{After(KMyShutdownDelay);}

inline CFBFileOpServer::CFBFileOpServer()
	:CServer2(0,ESharableSessions)
	{}

inline CFBFileOpServerSession::CFBFileOpServerSession()
	{}

inline CFBFileOpServer& CFBFileOpServerSession::Server()
	{return *static_cast<CFBFileOpServer*>(const_cast<CServer2*>(CSession2::Server()));}

#endif

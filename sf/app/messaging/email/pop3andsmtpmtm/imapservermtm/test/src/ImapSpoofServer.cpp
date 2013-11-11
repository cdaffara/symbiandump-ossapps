// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// imapserver.cpp
// 
//
 
#include "imapspoofserver.h"

//
// CImapSpoofServer
//

const TInt KImapPortNumber = 143;

CImapSpoofServer::CImapSpoofServer() : CActive(EPriorityStandard)
	{
	iServerStatus=EIdle;
	}

EXPORT_C CImapSpoofServer* CImapSpoofServer::NewL(MImapTestEventHandler* aOwner,const TDesC& aScriptFile)
	{
	CImapSpoofServer* self = NewLC(aOwner,aScriptFile);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CImapSpoofServer* CImapSpoofServer::NewLC(MImapTestEventHandler* aOwner,const TDesC& aScriptFile)
	{
	CImapSpoofServer* self = new(ELeave) CImapSpoofServer;
	CleanupStack::PushL(self);
	self->ConstructL(aOwner,aScriptFile);
	return self;
	}
	
CImapSpoofServer::~CImapSpoofServer()
	{
	Cancel();	
	iListeningSocket.Close();
	iServiceSocket.Close();
	iSocketServer.Close();
	delete iScriptFileProcessor;
	}

EXPORT_C TInt CImapSpoofServer::RunError(TInt aError)
	{
	iOwner->HandleEvent(aError);
	return KErrNone;
	}

void CImapSpoofServer::ConstructL(MImapTestEventHandler* aOwner,const TDesC& aScriptFile)
	{
	CActiveScheduler::Add(this); 
	iOwner=aOwner;
	//connect to the socket server
	User::LeaveIfError(iSocketServer.Connect());
	//create the script file processor
	iScriptFileProcessor = CScriptFileProcessor::NewL(aScriptFile);	
	}

void CImapSpoofServer::DoCancel()
	{
	iListeningSocket.CancelAccept();
	}
	
void CImapSpoofServer::StartL()
	{
	iServerStatus = EWriting;
	//set up the listening socket
	User::LeaveIfError(iListeningSocket.Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp));
	//bind the listener to any address from port 143
	TSockAddr address;
	address.SetPort(KImapPortNumber);
	iListeningSocket.Bind(address);
	//listen for incomming connections
	iListeningSocket.Listen(KListeningSocketQueueSize);
	//accept incomming connections and use iServiceSocket to communicate with it.
	User::LeaveIfError(iServiceSocket.Open(iSocketServer));
	iListeningSocket.Accept(iServiceSocket,iStatus);
	SetActive();	
	}

	
void CImapSpoofServer::IssueWrite()
	{			
    iBuffer.Zero();
  	iBuffer.Append(iNextLine);
	iBuffer.Append(_L("\r\n"));
	iServiceSocket.Write(iBuffer, iStatus);
	SetActive();
	}

void CImapSpoofServer::IssueRead()
	{
    iBuffer.Zero();
  	iServiceSocket.RecvOneOrMore(iBuffer, 0, iStatus,iReadLength);
  	SetActive();
	}
 
void CImapSpoofServer::RunL()
	{	
	
	CScriptFileProcessor::TDataDirection direction;

	if (iStatus == KErrNone)
		{		
		iBuffer.TrimAll();
		if(iServerStatus==EReading)
			{
			if(iNextLine!=iBuffer)
				{
				iOwner->HandleEvent(KErrCorrupt);		
				}		
			}
		//read next line of script file
		//if weve reached the end of the file then inform the owning object
		TInt err = iScriptFileProcessor->ReadLine(iNextLine);
		
		if(err==KErrEof)
			{
			iOwner->HandleEvent(KErrEof);	
			}
		else if(err==KErrNone)
			{
			direction=iScriptFileProcessor->DataDirection();
		
    		//if we are sending then write the line, else read 
    		if(direction==CScriptFileProcessor::EFromServer)
    			{
    			iServerStatus=EWriting;	
    			IssueWrite();
    			}
    		else if(direction==CScriptFileProcessor::EFromClient)
    			{
    			iServerStatus=EReading;
    			IssueRead();
    			}    	
			}
		else
			{
			iOwner->HandleEvent(err);	
			}
		}	
	else
		{
		iOwner->HandleEvent(iStatus.Int());		
		}
		
	}


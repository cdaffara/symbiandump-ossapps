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
//

#include "scriptfileprocessor.h"


const TChar KDelimiter=0x0A;
//
//CScriptFileProcessor
//

CScriptFileProcessor* CScriptFileProcessor::NewLC(const TDesC& aScriptFile)
	{
	CScriptFileProcessor* self=new (ELeave)CScriptFileProcessor;
	CleanupStack::PushL(self);
	self->ConstructL(aScriptFile);
	return self;
	}


CScriptFileProcessor* CScriptFileProcessor::NewL(const TDesC& aScriptFile)
	{
	CScriptFileProcessor* self=CScriptFileProcessor::NewLC(aScriptFile);
	CleanupStack::Pop(self);
	return self;
	}
	
void CScriptFileProcessor::ConstructL(const TDesC& aScriptFile)
	{
	User::LeaveIfError(iFSession.Connect());
    User::LeaveIfError(iFile.Open(iFSession, aScriptFile, EFileRead));
    iFileReadStream.Attach(iFile);   
	}

CScriptFileProcessor::CScriptFileProcessor()
	{
	}	
CScriptFileProcessor::~CScriptFileProcessor()
	{
	iFile.Close();
	iFileReadStream.Close();
	iFSession.Close();
	}
	
//
//DataDirection
//
 
CScriptFileProcessor::TDataDirection CScriptFileProcessor::DataDirection() const
	{
	return iDataDirection; 	
	}

//
//ProcessLine
//

 TInt CScriptFileProcessor::ProcessLine(TDes8& aLine)
	{
		
	//get the direction of the data
	const TInt KSpecifierLength=3;	
	_LIT8(KFromServer,"<< ");
	_LIT8(KFromClient,">> ");
	
	//if the data is from the server then the specifier should be at the begining of the line
	TInt pos;
	pos=aLine.Find(KFromServer);
	if(pos==0)
		{
		iDataDirection=EFromServer;	
		}
	else 
		{
		//if the data is from the client then the specifier should be at the begining of the line
		pos=aLine.Find(KFromClient);
		if(pos==0)
			{
			iDataDirection=EFromClient;	
			}	
		else
			{
			//there has been an error, the spefifier was not found
			return KErrNotFound;
			}
		} 
		
	//trim the specifier from the start of the line	
	aLine=aLine.Right(aLine.Length()-KSpecifierLength);
	//trim off trailing \r\n
	aLine.TrimRight();
	
	return KErrNone;
	}
	
//
//ReadLine
//
	
TInt CScriptFileProcessor::ReadLine(TDes8& aLine)
 {
 
	aLine.Zero();
	//read the line
 	TRAPD(err, iFileReadStream.ReadL(aLine, KDelimiter));
 	if (err != KErrNone)
    	{
     	return err; 
     	} 
    //process the line
    err=ProcessLine(aLine);
 
    // Haven't reached end of the file yet
 	return err; 
 }


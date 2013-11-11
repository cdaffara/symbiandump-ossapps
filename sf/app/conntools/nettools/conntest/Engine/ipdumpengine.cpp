/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CIpdumpEngine is used to control the ip logging protocol
*
*/

#include <f32file.h>
#include <nifman.h>
#include "ipdumpengine.h"


//
// CIpdumpEngine.
//

CIpdumpEngine* CIpdumpEngine::NewL()
{
    CIpdumpEngine* self = new (ELeave) CIpdumpEngine();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CIpdumpEngine::CIpdumpEngine()
: iLoggingEnabled(EFalse)
{
    // If changed change the filename also
    // in the ip logging protocol to match
    _LIT(KLogFilePath, "c:\\logs\\tcpdump\\probe.cap"); 
    iLogFileName.Append(KLogFilePath);
}

// This is to get around of the restrictions of the secure file system,
// when the conntest and the probe are installed from a sis
// You cannot install the esk to the right place directly, so copy it here
// Requires all files capability
_LIT(KIpDumpEskImport, "\\private\\101f7989\\import\\esock\\ip.probe.esk");
_LIT(KIpDumpEskEsock, "\\private\\101f7989\\esock\\ip.probe.esk");

void AttemptToCopyEskFile()
	{
	RFs fs;
	TInt err = fs.Connect();

	if (err)
		{
		return;
		}

	CFileMan* fileman = NULL;
	TRAP(err, fileman = CFileMan::NewL(fs));

	if (err)
		{
		fs.Close();
		return;
		}

	(void)fileman->Copy(KIpDumpEskImport, KIpDumpEskEsock, CFileMan::ERecurse);

	delete fileman;
	fs.Close();
	}

void CIpdumpEngine::ConstructL()
{
	AttemptToCopyEskFile();
}


CIpdumpEngine::~CIpdumpEngine()
{
    DisableLogging();
}



TInt CIpdumpEngine::CreateLogDirectory()
{

    RFs fs;
    TInt r;
    
    r = fs.Connect();
    
    if(r==KErrNone)
    {
        
        // Delete the old log file. After disabling the logging the file
        // is in use for short period of time, therefore doing the deletion
        // in loop to avoid error message in UI.
        while((r = fs.Delete(iLogFileName)) == KErrInUse)
        	User::After(1);
        
        if(r == KErrNotFound || r == KErrPathNotFound)
        {
            r = fs.MkDirAll(iLogFileName);
            r = (r==KErrAlreadyExists) ? KErrNone : r;
        }
    }
    
    fs.Close();
    return r;
}




void CIpdumpEngine::EnableLoggingL()
{
    
    if(iLoggingEnabled)
    {
        return;
    }
    
    User::LeaveIfError(CreateLogDirectory());
    
    User::LeaveIfError(iSocketServ.Connect());    
    
    TProtocolDesc protocol;
    TProtocolName name(_S("probe"));
        
    User::LeaveIfError(iSocketServ.FindProtocol(name, protocol));
    
    User::LeaveIfError(iSocket.Open(iSocketServ, name));
        
    iLoggingEnabled = ETrue;
    
    return;
}

void CIpdumpEngine::DisableLogging()
{
    if(iLoggingEnabled)
    {
        iSocket.Close();
        iSocketServ.Close();
        
        iLoggingEnabled = EFalse;
    }
}

TBool CIpdumpEngine::LoggingEnabled() const
{
    
    return iLoggingEnabled;
}

const TDesC& CIpdumpEngine::LogFileName() const
{
       return iLogFileName;
}



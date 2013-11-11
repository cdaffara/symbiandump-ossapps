// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "IMSKSCR.H"
#include "IMSK.H"
#include "IMCVTEXT.H"


_LIT8(K_IM_SCRIPT_WAIT_STRING, "** Read cancelled");

CImTextServerScript::CImTextServerScript() : CActive(EPriorityStandard)
	{
	}

CImTextServerScript *CImTextServerScript::NewL(TInt aPortNum)
	{
	CImTextServerScript* self =new(ELeave)CImTextServerScript();
	CleanupStack::PushL(self);
	self->ConstructL(aPortNum);
	CleanupStack::Pop();
	return self;
	}
//
// 2nd phase of construction
//
void CImTextServerScript::ConstructL(TInt aPortNum)
	{
	CActiveScheduler::Add(this);
	User::LeaveIfError(iFs.Connect());

	TBuf<35> scriptFile; 
	scriptFile.Format(TRefByValue<const TDesC>_L("c:\\Logs\\email\\imsk%d.scr"),aPortNum);
	User::LeaveIfError(iFile.Open(iFs,scriptFile,EFileShareAny));

#ifndef _UNICODE
	iScriptFileText.Set(iFile);
#endif

	}

CImTextServerScript::~CImTextServerScript()
	{
	iFile.Close();
	iFs.Close();
	}

void CImTextServerScript::RetrieveResponse(TDes8& theBuffer, TRequestStatus& aStatus)
	{
	iReportStatus = &aStatus;
    TInt fileReadErr = KErrNone;

#ifndef _UNICODE
	fileReadErr = iScriptFileText.Read(iData);
#else
    //  _UNICODE read one line ASCII data
    fileReadErr = ReadLine();
#endif

	iNextFileSeg.Copy(iData);

	if( fileReadErr == KErrNone)
		{
		theBuffer.Append(iNextFileSeg);
		}
	else if(fileReadErr == KErrTooBig)
		{
		for(;;)
			{
			// read response from file
#ifndef _UNICODE
			fileReadErr = iScriptFileText.Read(iData);
#else
            //  _UNICODE read one line ASCII data
            fileReadErr = ReadLine();
#endif
			iNextFileSeg.Copy(iData);
			TInt crLfPos = iNextFileSeg.Find(KImcvCRLF);
			if(crLfPos == KErrNotFound)
				{
				theBuffer.Append(iNextFileSeg);
				break;
				}
			else
				{
				theBuffer.Append(iNextFileSeg.Left(crLfPos));
				}
			}
		}

	iStatus = KRequestPending;
	aStatus = KRequestPending;
	SetActive();

	if ((theBuffer.CompareF(K_IM_SCRIPT_WAIT_STRING) != 0)
		&& (fileReadErr != KErrEof))
		{
		theBuffer.Append(KImcvCRLF);
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

//
//  ReadLine() read ASCII into TBuf8 thingy if unicode build.
//
#ifdef _UNICODE
TInt CImTextServerScript::ReadLine()
    {
    TInt err = KErrNone;

    //  Read into the buffer
    TBuf8<256> buffer;              // Max read of the Read() function..    
    iData.Zero();

    //  Get the current file position
    TInt filePos = 0;
    iFile.Seek(ESeekCurrent, filePos);

    //  Read the buffer
    err = iFile.Read(buffer);
    
    //end of file?
    TInt s = buffer.Length();
    if( s == 0)
        {
        err = KErrEof;
        }

    if(err == KErrNone)
        {
        //  Copy to the lfcr and then set the file pointer
        //  to the point after that...
        TInt pos = buffer.Find(KImcvCRLF);
        if( pos != -1)
            {
            iData.Justify(buffer, pos, ELeft, ' ');
            filePos += (pos+2);

            //  Set the file pointer back to after the lfcr..
            iFile.Seek(ESeekStart, filePos);                
            }
        //  Else fill the whole buffer 256 chars..
        else
            {
            iData.Copy(buffer);
            }
        }
    return err;
    }
#endif

void CImTextServerScript::RunL()
	{
	User::RequestComplete(iReportStatus, KErrNone);
	}

void CImTextServerScript::DoCancel()
	{
	if (IsActive())
		{
		if (iStatus == KRequestPending)
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		User::RequestComplete(iReportStatus, KErrNone);
		}
	}


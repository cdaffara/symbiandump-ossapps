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
// IMAP4 utils
// 
//

#if !defined(__IMPSUTIL_H__)
#define __IMPSUTIL_H__

class CImap4Utils : public CBase
/**
@internalComponent
@released
*/
	{
public:
	static CImap4Utils* NewL(CMsvServerEntry* aEntry);
	~CImap4Utils();
	
	// prepare the iLogMessage block with details of the next message
	void SetUpLogMessageL(TMsvId aId);

	// clear details recorded so that SendLog won't do anything
	void ClearLogMessage();
	
	// log the event previously set up
	TBool SendLogMessageL(TInt aError, TRequestStatus &aStatus);

private:
	void ConstructL(CMsvServerEntry* aEntry);
	
private:
	// logging object (maybe NULL)
	CImLogMessage*			iLogMessage;

	// Access to message database
	CMsvServerEntry*		iEntry;
	};

#endif

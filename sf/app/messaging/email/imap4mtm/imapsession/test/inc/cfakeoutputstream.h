// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CFAKEOUTPUTSTREAM_H
#define CFAKEOUTPUTSTREAM_H

#include <e32base.h>
#include <test/tefunit.h>
#include "moutputstream.h"

class MOutputStreamObserver;
class MOutputStreamSecureObserver;

/**
A dummy output stream object.
This object records data that is "sent" via SendDataReq()
The owner of this object can access this data via PtrOutput() in order
to test that expected data has been sent.
@internalTechnology
@prototype
*/
class CFakeOutputStream : public CBase
						, public MOutputStream
	{
public:
	static CFakeOutputStream* NewL(CTestExecuteLogger& aLogger);	
	~CFakeOutputStream();	

	// Implement MOutputStream
	void Bind(MOutputStreamObserver& aObserver, TInt aLogId);
	void BindSecure(MOutputStreamSecureObserver& aObserver);
	void SendDataReq(const TDesC8& aBuffer, TInt aIdleTime = 0);
	void SecureClientReq(const TDesC8& aHostName);
	void Close();
	
	// Access to recorded output data
	TPtrC8 PtrOutput();
	void ResetOutput();
	
	// Logging
	inline CTestExecuteLogger& Logger() { return iLogger; }
	
private:
	CFakeOutputStream(CTestExecuteLogger& aLogger);
	void ConstructL();
	
	/**
	Records data that has been sent via SendDataReq()
	*/
	CBufFlat* iOutputBuffer;

	CTestExecuteLogger&	iLogger;
	
	MOutputStreamObserver* iOutputStreamObserver;
	};

#endif // CFAKEOUTPUTSTREAM_H

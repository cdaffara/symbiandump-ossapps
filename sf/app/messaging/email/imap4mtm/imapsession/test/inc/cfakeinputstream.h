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

#ifndef CFAKEINPUTSTREAM_H
#define CFAKEINPUTSTREAM_H

#include <e32base.h>
#include <test/tefunit.h>
#include "minputstream.h"
#include "minputstreamobserver.h"

typedef RArray<TPtrC8> RArrayPtrC8;

/**
Observer interface that notifies interested unit tests when CFakeInputStream
has finished delivering the data it has been given to deliver.
This gives unit test the opportunity to perform tasks (such as Canceling a request)
before all the data has been delivered.
A unit test can initially give CFakeInputStream a partial set of data
(e.g. not containing the tagged response)
and then give it the remainder of the data (e.g. containing the tagged response) 
upon notification of this observer interface.
@internalTechnology
@prototype
*/
class MFakeInputStreamIsEmptyObserver
	{
	public:
		/**
		This method is called by CFakeInputStream when it has delivered all the data in iInputStrings.
		@param aInputStrings update this array with more data to deliver.
		*/
		virtual void OnInputStreamIsEmptyL() =0;
	};

/**
A dummy input stream object.
The owner of this object can set up a sequence of data to send to the observer via SetInputStrings.
Each item in the iInputStrings array will be sent to the observer in a single call to its
ReceivedDataIndL() method.  The next string in the array will not be sent until ReadReq() has 
been called by the observer to indicate that the previous string has been consumed.
@internalTechnology
@prototype
*/
class CFakeInputStream : public CActive
					   , public MInputStream
	{
public:
	static CFakeInputStream* NewL(CTestExecuteLogger& aLogger);
	~CFakeInputStream();

	void NotifyWhenStreamIsEmpty(MFakeInputStreamIsEmptyObserver& aStreamIsEmptyObserver);

	void AppendInputStringL(const TDesC8& aString);
	void ResetInputStrings();

protected:
	// Implement MInputStream
	void Bind(MInputStreamObserver& aInputStreamObserver, TInt aLogId);
	void ReadReq(TInt aIdleTime = 0);
	void CancelReadReq();
	TBool IsReading();
	
	void CompleteSelf(TInt aError);
				
	// Implements CActive
	void RunL();
	void DoCancel();
		
	inline CTestExecuteLogger& Logger() { return iLogger; }
	
private:
	CFakeInputStream(CTestExecuteLogger& aLogger);
		
private:
	CTestExecuteLogger&	iLogger;
	MInputStreamObserver* iInputStreamObserver;
	
	MFakeInputStreamIsEmptyObserver* iStreamIsEmptyObserver;
	
	RArrayPtrC8 iInputStrings;
	TInt iNextInputString;
	
	TBool iIsReading;
	};

#endif // CFAKEINPUTSTREAM_H

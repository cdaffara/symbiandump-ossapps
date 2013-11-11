// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGSSIGNAL_H__
#define __AGSSIGNAL_H__


// class TAgnSignal

class TAgnSignal
	{
	public:
inline TAgnSignal() {}
	inline TInt Set(const TDesC& aCommand);
	//
	inline TAgnSignal(TRequestStatus& aStatus);
	inline TPtrC Command() const;
public:
	TRequestStatus* iStatus;
	TThreadId iId;
	};

inline TAgnSignal::TAgnSignal(TRequestStatus& aStatus)
	:iStatus(&aStatus),iId(RThread().Id())
	{aStatus=KRequestPending;}
inline TPtrC TAgnSignal::Command() const
	{return TPtrC((const TText*)this,sizeof(*this)/sizeof(TText));}
inline TInt TAgnSignal::Set(const TDesC& aCommand)
	{return aCommand.Size()!=sizeof(*this) ? KErrGeneral : (Mem::Copy(this,aCommand.Ptr(),sizeof(*this)),KErrNone);}


TInt ThreadFunction(TAgnSignal& aPtr);

#endif

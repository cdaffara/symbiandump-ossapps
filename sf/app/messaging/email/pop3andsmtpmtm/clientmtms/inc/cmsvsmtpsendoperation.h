// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CMSVSMTPSENDOPERATION_H__
#define __CMSVSMTPSENDOPERATION_H__


#include <msvapi.h>


class CMsvSmtpProgressOperation : public CMsvSendOperation
	{
public:
	static CMsvSmtpProgressOperation* NewL(CMsvSession& aMsvSession, 
								const CMsvEntrySelection& aSelection, TInt aFunctionId, 
								TDes8& aParameter, TRequestStatus& aObserverRequestStatus);
								
	~CMsvSmtpProgressOperation();
private:							
	void ConstructL(const CMsvEntrySelection& aSelection, TInt aFunctionId, TDes8& aParameter);
	CMsvSmtpProgressOperation(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus);
	// from CMsvSendOperation		
	virtual const TDesC8& TranslateProgress(const TDesC8& aProgress);
private:
	CMsvEntrySelection*	iSelection;
	};


#endif // __CMSVSMTPSENDOPERATION_H__


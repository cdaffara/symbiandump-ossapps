/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#ifndef TJOBGUARDDATA_H
#define TJOBGUARDDATA_H

#include <e32def.h>

class TJobGuardData
	{
	public:
	
	enum TCallbackNum
			{
			EProgressEventCb = 0,
			EErrorEventCb,
			EStatusEventCb,
			EImageEventCb			
			};
	
	public:
	
		TInt iStatus;
		TInt iPercentCompletion;
		TInt iJobStateCode;	
		TInt iError;
		TInt iErrorStringCode;
		TInt iFsBitmapHandle;
		TCallbackNum iCb;
	};

#endif // TJOBGUARDDATA_H

//  End of File

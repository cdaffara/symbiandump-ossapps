/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/



#ifndef TDISCOVERYGUARDDATA_H
#define TDISCOVERYGUARDDATA_H

#include <e32def.h>

#include "tprinter.h"

class TDiscoveryGuardData
	{
	public:
	
		enum TCallbackNum
			{
			EFoundCb = 0,
			EStatusCb,
			ERemoveCb
			};
	
	public:
	
		TPrinter iPrinterInfo;
		TInt iStatus;		
		TInt iErrorCode;
		TInt iErrorStringCode;
		TCallbackNum iCb;
	};

#endif // TDISCOVERYGUARDDATA_H

//  End of File

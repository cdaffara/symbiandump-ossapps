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
// Bluetooth Phone Book Access Profile (PBAP) client side API header file.
// Describes the PBAP API for use by clients of PBAP server.
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef PBAPCLI_H
#define PBAPCLI_H

#include <e32base.h>

// Macros used for Memory checking functionality
#ifdef _DEBUG
#define DEBUG_SEND_RECIEVE(opcode, args) SendReceive(opcode, args);
#else
#define DEBUG_SEND_RECIEVE(opcode, args) KErrNotSupported;
#endif

/**
 Provides client side access to the PBAP server.
 
 @publishedPartner
 @released
 */
class RPbapSession : public RSessionBase
	{
public:
	IMPORT_C RPbapSession();
	IMPORT_C TInt Connect();
	IMPORT_C TInt Start();
	IMPORT_C void Stop();
	IMPORT_C TInt SetPassword(const TDesC& aPassword);
	
	//
	// Memory checking functionality for Debug builds only.
	//
	IMPORT_C TInt __DbgMarkHeap();
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);
	IMPORT_C TInt __DbgFailNext(TInt aCount);	

private:
	TVersion Version() const;
	
private:
	// This data padding has been added to help prevent future binary compatibility breaks
	// iPadding hasn't been zero'd because it is currently not used
	TUint32 iPadding;	
	};

#endif //PBAPCLI_H

// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ObexUtils
// Obex Utility Class
// 
//

#ifndef __OBEXUTILS_H__
#define __OBEXUTILS_H__

#include <e32base.h>
#include <obex.h>



// Messaging Suite
#include <msvapi.h>

#include <listenercontainer.h>

class CObexUtils : public CBase
/**
Class CObexUtils
The purpose of this class is multifold:

Handles storage / deletion of the received object in the messaging inbox.

@internalTechnology
@removed
*/
	{
public:
	IMPORT_C static void DummySaveObjToInboxL(CMsvSession& , CBaseContainer& , TUid , TMsvId& , TDesC&);

	IMPORT_C static void DummyRemoveFromInboxL(CMsvSession& , TMsvId);

	IMPORT_C static void DummyFinaliseObexMessageL(CMsvSession& , TMsvId);



	

	};


#endif // __OBEXUTILS__

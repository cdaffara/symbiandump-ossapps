/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Utils implements general tools for bandwidth and 
* packet loss calculation and IAP selection etc
*
*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <e32std.h>

#include "uinotify.h"

class Utils  
{
public:
	Utils();
	virtual ~Utils();

    /**
    * Adjust given Nifman's idle timeout for every bearer.
    */
    static void SetTimeoutsL(const TInt aTimeout);

    /**
    * Set Nifman's idle times for each record in given table.
    */
    static void SetTableTimeoutL(const TInt aTimeout,const TDesC& aTableName);

    /**
    * Calculate throughput
    */
	static void CalculateThroughput(TDes8& aBuffer, TTime aStartTime, TInt aSize);

    /**
    * Set IapId value into connection preferences table.
    */
    static void SetConnectionPrefsIapL();

    /**
    * Create data chunk for POST performance tests.
    */
    static void CreateDataChunk(TDes8& aBuffer, TInt aLength);

    /**
    * Ask IAP id to be used from the user
    * @param aId Selected IAP id
    * @return Status of the selection (selection made or cancelled)
    */
    static TInt AskIapIdL(TUint32& aId);    

    /**
    * Ask Network id from the user.
    */
    static TInt AskNetworkIdL(TUint32& aId);    

    /**
    * Does the real work for selecting IAP or Network ide.
    */
    static TInt SelectIdFromTableL(TUint32& aId, const TDesC& aTableName);

};

#endif // __UTILS_H__

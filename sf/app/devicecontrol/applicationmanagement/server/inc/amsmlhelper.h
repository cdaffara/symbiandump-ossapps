/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */

#ifndef __AMSMLHELPER_H__
#define __AMSMLHELPER_H__

#include <SyncMLDef.h>
#include <SyncMLTransportProperties.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>

namespace NApplicationManagement
    {

    /**
     * Utility class that wraps access to DM client functionality
     */
    class SmlHelper
        {
public:

        // NOTE: this enum must match with property array described in 
        // SyncML_Sync_Agent_Client_API_Specification.doc.
        enum TNSmlSyncTransportProperties
            {
            EPropertyIntenetAccessPoint = 0,
            EPropertyHttpUsed = 5,
            EPropertyHttpUserName = 6,
            EPropertyHttpPassword = 7
            };

        /**
         * GetCurrentProfileIdL Finds out the current syncml DM session profile id.
         * @param aProfile Will contain the current profile id if call is successfull
         * @return Success status (KErrNone if successful)
         */
        static TInt GetCurrentProfileIdL(TSmlProfileId &aProfile);
        /**
         * LaunchSessionL Launches SyncML DM session using given profile id.
         * @param aProfile The id of profile which is to be used to start DM session
         * @return Success status (KErrNone if successful)
         */
        static TInt LaunchSessionL(TSmlProfileId &aProfile);

        /**
         * Fetch the default IAP used by the currently active DM profile
         * @param aDefaultIAP on successfull return contains the default IAP number
         */

        static void GetDefaultIAPFromDMProfileL(TInt& aDefaultIAP);

        static void GetConnectionPropertyNameL(RSyncMLSession& aSyncSession,
                TDes8& aText, TInt aPropertyPos);

        static TInt StrToInt(const TDesC& aText, TInt& aNum);

        static HBufC8* GetCurrentServerIDL();

        };

    }
	
	
#endif

//  End of File

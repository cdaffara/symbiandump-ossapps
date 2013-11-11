/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Telephony events and commands related information
*
*/

#ifndef CPEENGINEINFO_H
#define CPEENGINEINFO_H

#include <e32base.h>
#include "mpedatastore.h"

/**
 * Holds telephony events and commands related information
 * + Call information is valid for a lifetime of a call
 * + Command information is considered valid at all times and it is 
 * the clients responsibility to set it correctly
 * + Device information related to telephony is valid after startup
 * is completed and is updated according to a set of monitored indications
 *
 */
NONSHARABLE_CLASS( CPEEngineInfo ) 
    : public CBase, public MPEDataStore
    {
    
public:
        
        /**
         * Two-phased constructor.
         * 
         * @return  The newly created object. Ownership is transferred to the caller.
         */
        IMPORT_C static CPEEngineInfo* NewL();
        
        /**
         * Destructor.
         */
        virtual ~CPEEngineInfo();
        
protected:
        
        /**
         * Constructor.
         */
        CPEEngineInfo();
    };

#endif /*CPEENGINEINFO_H_*/

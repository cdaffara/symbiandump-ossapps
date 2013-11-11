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


#ifndef CBASECAPABILITY_H
#define CBASECAPABILITY_H

#include <e32std.h>
#include <e32base.h>

//  TYPE DEFINITIONS
typedef TBuf<64> TCapabilityTitle;

/**
 *  
 *  Capability base class, pure virtual, needs to be derived
 *
 */
class CBaseCapability
    : public CBase
    {
    public:     // New methods

        virtual ~CBaseCapability() {};

        /**
         *  Creates text for the setting item list box
         *  
         *  @return Correctly formatted list box text
         */
        virtual HBufC* ListBoxTextL() = 0;

        /**
         *  Launches pop-up window to modify the values
         *
         *  @return Did user change the value or not
         */
        virtual TBool LaunchPopupListL() = 0;

        /**
         *  Clones and returns copy of itself
         *
         *  @return Cloned copy of the instance itself
         */
        virtual CBaseCapability* CloneL() = 0;

    public:     // Data

        // The index on the list from the top (0 = topmost)
        TBool iIndexOnList;        

        // Title of this capability
        TCapabilityTitle iTitle;

        // UID of the capability
        TInt iUid;
        
        // Value
        TInt iValue;
    };

#endif  // CBASECAPABILITY_H

//  End of File

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


#ifndef CTINTCAPABILITY_H
#define CTINTCAPABILITY_H

#include "cbasecapability.h"

/**
 *
 *  Capability type for TInt type of settings
 *
 */
class CTIntCapability :
    public CBaseCapability
    {
    public:     //  Constructors and destructors
        
        /**
         *  Destructor
         */
        virtual ~CTIntCapability();

    public:     //  Methods derived from CBaseCapability

        virtual HBufC* ListBoxTextL();
        virtual TBool LaunchPopupListL();
        virtual CBaseCapability* CloneL();

    public:     // Data

        // Minimum and maximum value
        TInt iMin;
        TInt iMax;
    };

#endif  //  CTINTCAPABILITY_H

//  End of File

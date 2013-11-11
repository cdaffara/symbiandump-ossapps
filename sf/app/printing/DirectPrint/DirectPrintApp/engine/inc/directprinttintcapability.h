/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef DIRECTPRINTTINTCAPABILITY_H
#define DIRECTPRINTTINTCAPABILITY_H

// User includes
#include "directprintbasecapability.h"

/**
 *
 *  Capability type for TInt type of settings
 *
 */
class CDirectPrintTIntCapability : public CDirectPrintBaseCapability
    {
    public:     //  Constructors and destructors

        /**
         *  Destructor
         */
        virtual ~CDirectPrintTIntCapability();

    public:     //  Methods derived from CBaseCapability

        virtual HBufC* ListBoxTextL();
        virtual CDirectPrintBaseCapability* CloneL();
        virtual CAknSettingItem* SettingItemLC(TInt aIndex);

    public:     // Data

        // Minimum and maximum value
        TInt iMin;
        TInt iMax;
    };

#endif  //  DIRECTPRINTTINTCAPABILITY_H

//  End of File

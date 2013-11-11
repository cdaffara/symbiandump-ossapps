/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Session type base class.
*
*/



#ifndef MVTCTLSESSIONINFO_H
#define MVTCTLSESSIONINFO_H

//  INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
*  Base class data types.
*
*  @lib CSCallCtrl
*  @since 2.6
*/
class TVtCtlType
    {
    public:
        
        /**
        * Returns extension id
        * @return extension id
        */
        inline TInt Extension() const;

    protected:
        // Extension id to be used in defining actual data type.
        TInt iExtension;
    };

#include    "TVtCtlType.inl"

#endif      // MVTCTLSESSIONINFO_H
            
// End of File

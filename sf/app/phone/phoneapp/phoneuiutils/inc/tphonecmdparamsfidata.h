/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*   See class description.
*
*/


#ifndef __TPHONECMDPARAMSFIDATA_H
#define __TPHONECMDPARAMSFIDATA_H

//  INCLUDES

#include <w32std.h>
#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A parameter class for call header information.
*/
class TPhoneCmdParamSfiData : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamSfiData();

    public: 
        
        /**
        * Sets the call header label text.
        * @param aLabel is the label
        */
        IMPORT_C void SetNumber( const TDesC& aLabelText );

        /**
        * Sets the call header short label text
        * @param aShortLabel is the short label
        */
        IMPORT_C void SetName( const TDesC& aShortLabelText );

        /**
        * Returns the call header label text
        * @return Returns the label
        */
        IMPORT_C const TDesC& Number() const;

        /**
        * Returns the call header short label text.
        * @return Returns the short label
        */
        IMPORT_C const TDesC& Name() const;

    private:    
        
        /**
        * Call header label text
        */
        TPtrC iNumber;

        /**
        * Call header short label text
        */
        TPtrC iName;

    };

#endif // __TPHONECMDPARAMSFIDATA_H   
            
// End of File

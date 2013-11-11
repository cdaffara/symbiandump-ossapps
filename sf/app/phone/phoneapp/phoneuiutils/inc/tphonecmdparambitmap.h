/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Command parameter class for Bitmap objects
*     
*
*/


#ifndef TPHONECMDPARAMBITMAP_H
#define TPHONECMDPARAMBITMAP_H

//  INCLUDES
#include <fbs.h>
#include "tphonecommandparam.h"

// CLASS DECLARATION

/**
*  A concrete parameter class for passing a CFbsBitmap around. This
*  is used for various purposes where CFbsBitmap is useful. 
*/
class TPhoneCmdParamBitmap : public TPhoneUICommandParam
    {
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamBitmap();

        /**
        * Sets the Bitmap value.
        * @param aBitmap is the bitmap.
        */
        IMPORT_C void SetBitmap( CFbsBitmap* aBitmap );

        /**
        * Returns the Bitmap.
        * @return Returns the Bitmap
        */
        IMPORT_C CFbsBitmap* Bitmap() const;

        /**
        * Sets the mask bitmap value.
        * @param aMaskBitmap is the masking bitmap.
        */
        IMPORT_C void SetMaskBitmap( CFbsBitmap* aMaskBitmap );

        /**
        * Returns the mask bitmap.
        * @return Returns the mask bitmap
        */
        IMPORT_C CFbsBitmap* MaskBitmap() const;


    private:    
        
        /**
        * Pointer to bitmap
        */
        CFbsBitmap* iBitmap;

        /**
        * Pointer to masking bitmap
        */
        CFbsBitmap* iMaskBitmap;

    };

#endif      // TPHONECMDPARAMBITMAP_H 
            
// End of File

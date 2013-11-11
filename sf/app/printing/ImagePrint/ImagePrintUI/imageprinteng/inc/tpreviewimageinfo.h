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


#ifndef TPREVIEWIMAGEINFO_H
#define TPREVIEWIMAGEINFO_H

#include <e32def.h>
#include <e32std.h>
#include <bitmaptransforms.h>

/**
 *
 *  Data structure that presents a single image on the preview bitmap
 *
 */
class TPreviewImageInfo
    {
    public:     // Constructors and destructors  
    
	    /**
		 * Constructor
		 */		
		TPreviewImageInfo();
           
    public: // data
		
        // Position of the image on preview bitmap
		TPoint iPos;                       

        // Size of the image on previwe bitmap
        TSize iSize;

        // Rotation status & orientation of the image
        TBool iRotated;                

        // Orientation of the image
        CBitmapRotator::TRotationAngle iOrientation;        
    };

#endif //  TPREVIEWIMAGEINFO_H

//  End of File

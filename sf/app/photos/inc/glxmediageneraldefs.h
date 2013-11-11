/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definitions for media attributes
*
*/



#ifndef __T_GLXMEDIAGENERALDEFS_H__
#define __T_GLXMEDIAGENERALDEFS_H__

#include <e32base.h>
#include <mpxattribute.h>

/**
* Content ID identifying progress messages
*/
const TInt KGlxMediaIdGeneral   = 0x200071A8;

/**
 * Boolean value of whether item is a system item. 
 * System albums true (Favourites, Camera album), otherwise false 
 * (For collection plugins true, but will probably not be asked)
 * TBool 
 */
const TMPXAttributeData KGlxMediaGeneralSystemItem ={KGlxMediaIdGeneral, 0x01};

/**
 * Original dimensions of image or video clip 
 * TSize
 */
const TMPXAttributeData KGlxMediaGeneralDimensions ={KGlxMediaIdGeneral, 0x02};

/**
 * Frame count (animated GIF)
 * TInt
 */
const TMPXAttributeData KGlxMediaGeneralFramecount ={KGlxMediaIdGeneral, 0x04};

/**
 * Location
 * TCoordinate (defined in lbsposition.h)
 */
const TMPXAttributeData KGlxMediaGeneralLocation ={KGlxMediaIdGeneral, 0x08};

/**
 * Slideshowable Contents (count or -1 for non-containers)
 * TInt
 */
const TMPXAttributeData KGlxMediaGeneralSlideshowableContent ={KGlxMediaIdGeneral, 0x20};

/**
 * Slideshowable Contents (count or -1 for non-containers)
 * TInt
 */
const TMPXAttributeData KGlxMediaGeneralLastModifiedDate ={KGlxMediaIdGeneral, 0x40};

//MDS 2.5 Migration. Remove the Comments
/**
 * Slideshowable Contents (count or -1 for non-containers)
 * TInt
 */
const TMPXAttributeData KGlxMediaGeneralDRMRightsValid ={KGlxMediaIdGeneral, 0x80};

/**
*  DEPRECATED, please use consts above
*  Attributes supported for KGlxMediaIdGeneral
*/
enum
    {
    /**
      * Boolean value of whether item is a system item. 
      * System albums true (Favourites, Camera album), otherwise false 
      * (For collection plugins true, but will probably not be asked)
      * TBool 
      */
    EGlxMediaGeneralSystemItem = 0x01,
    
    /**
      * Original dimensions of image or video clip 
      * TSize
      */
    EGlxMediaGeneralDimensions = 0x02,

    /**
     * Frame count (animated GIF)
     * TInt
     */
    EGlxMediaGeneralFramecount = 0x04,
    
    /**
     * Location
     * TCoordinate (defined in lbsposition.h)
     */
    EGlxMediaGeneralLocation = 0x08,
    };
    
//MDS 2.5 Migration. Remove the Comments    
enum TGlxMediaGeneralRightsValidity
    {
    EGlxDrmRightsValidityUnknown,
    EGlxDrmRightsValid,
    EGlxDrmRightsInvalid
    }; 
#endif // __T_GLXMEDIAGENERALDEFS_H__

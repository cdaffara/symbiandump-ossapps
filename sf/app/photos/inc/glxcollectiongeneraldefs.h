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



#ifndef __T_GLXCOLLECTIONGENERALDEFS_H__
#define __T_GLXCOLLECTIONGENERALDEFS_H__

#include <e32base.h>
#include <mpxattribute.h>

/**
* Content ID identifying progress messages
*/
const TInt KGlxMediaIdCollectionPluginSpecific = 0x2000A771;

/** 
 * Plugin Subtitle
 */
const TMPXAttributeData KGlxMediaCollectionPluginSpecificSubTitle ={KGlxMediaIdCollectionPluginSpecific, 0x01};

/** 
 * Plugin  Popup title
 */
const TMPXAttributeData KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle ={KGlxMediaIdCollectionPluginSpecific, 0x02};

/** 
 * Plugin item title
 */
const TMPXAttributeData KGlxMediaCollectionPluginSpecificNewMediaItemTitle ={KGlxMediaIdCollectionPluginSpecific, 0x04};

/** 
 * Plugin default media title
 */
const TMPXAttributeData KGlxMediaCollectionPluginSpecificDefaultMediaTitle ={KGlxMediaIdCollectionPluginSpecific, 0x08};


/**
 *  DEPRECATED, please use consts above
 *  Attributes supported for KGlxMediaIdCollectionPluginSpecific
 */
enum
    {
    /** 
    * Plugin Subtitle
    */
    EGlxMediaCollectionPluginSpecificSubTitle = 0x01,
    
    /** 
    * Plugin  Popup title
    */
    EGlxMediaCollectionPluginSpecificSelectMediaPopupTitle = 0x02,
    
    /** 
    * Plugin item title
    */
    EGlxMediaCollectionPluginSpecificNewMediaItemTitle = 0x04,
    
    /** 
    * Plugin default media title
    */
    EGlxMediaCollectionPluginSpecificDefaultMediaTitle = 0x08
    };

#endif // __T_GLXCOLLECTIONGENERALDEFS_H__

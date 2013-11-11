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



#ifndef __T_GLXCOLLECTIONMESSAGEDEFS_H__
#define __T_GLXCOLLECTIONMESSAGEDEFS_H__

#include <e32base.h>

/**
* Content ID identifying progress messages
*/
const TInt KGlxCollectionMessageSpecific = 0x2000A763;

/**
*  Attributes supported for KGlxMediaIdCollectionPluginSpecific
*/
enum
    {
    /** 
    * TMPXItemId
    */
    EGlxCollectionMessageContainerId = 0x01,
    
    /** 
    * TMPXGeneralCategory
    */
    EGlxCollectionMessageContainerCategory = 0x02,
    
    /** 
    * TBool
    */
    EGlxCollectionMessageContainerCameraAlbum = 0x04
    
    };
// TMPXItemId
const TMPXAttributeData KGlxCollectionMessageContainerId = {KGlxCollectionMessageSpecific, EGlxCollectionMessageContainerId};
// TMPXGeneralCategory
const TMPXAttributeData KGlxCollectionMessageContainerCategory = {KGlxCollectionMessageSpecific, EGlxCollectionMessageContainerCategory};
// TBool
const TMPXAttributeData KGlxCollectionMessageContainerCameraAlbum = {KGlxCollectionMessageSpecific, EGlxCollectionMessageContainerCameraAlbum};

#endif // __T_GLXCOLLECTIONMESSAGEDEFS_H__

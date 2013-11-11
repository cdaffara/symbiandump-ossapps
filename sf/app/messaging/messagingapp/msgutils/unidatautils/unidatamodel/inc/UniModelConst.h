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
*       UniEditor constant definitions
*
*
*/


#ifndef __UNIMODELCONST_H
#define __UNIMODELCONST_H

// INCLUDES
#include <msvids.h>                     // TMsvId
/*rushi
#include <unieditorfeatures.h>
*/

// CONSTANTS
const TInt KDefResFileSignature = 4;  // Dummy resource file signature
_LIT( KUniDataModelResourceFilename, "z:UniDataModel.rsc" );

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// Value returned by SMIL model's ParseL method.
enum TUniParseResult
    {
    EParsedFromList            = 0x0001, // (1) <-+ These are exclusive for each other
    EParsedFromSmil            = 0x0002, // (1) <-/
    EUnsupportedObjects        = 0x0100,
    EMissingObjects            = 0x0200,
    EBadlyReferencedObjects    = 0x0400,
    ERegionMediaTypeMismatch   = 0x0800,
    EOneNonConformantObject    = 0x1000,
    EMultiNonConformantObject  = 0x2000,
    ETextAndImageRegionsSame   = 0x4000,
    EObjectRemoved             = 0x8000
    };

enum TUniSmilType
    {
    ENoSmil,
    EMultipleSmil,
    EMmsSmil,
    ETemplateSmil,
    E3GPPSmil
    };

//  TUniLayout
//  Defines order of objects in display.
enum TUniLayout
    {
    EUniUndefinedLayout = 0,
    EUniImageFirst,
    EUniTextFirst
    };

enum TUniRegion
    {
    EUniRegionUnresolved,
    EUniRegionImage,
    EUniRegionText,
    EUniRegionAudio
    };

// FUNCTION PROTOTYPES

// CLASS DECLARATION

#endif // __UNIMODELCONST_H

// End of file

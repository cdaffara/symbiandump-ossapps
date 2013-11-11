/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MYLOCATIONSDEFINES_H__
#define __MYLOCATIONSDEFINES_H__

#include <QString>

/** Defines map tile fetching states.
 */

enum TMapTileFetchingState
    {
    /** Map tile fetching completed */
    EMapTileFectchingCompleted=0,
    /** Map tile fetching in progress */
    EMapTileFetchingInProgress,
    /** Map  tile fetching n/w error */
    EMapTileFetchingNetworkError,
    /** Map tile fetching invalid address */
    EMapTileFetchingInvalidAddress,
    /** Map tile fetching unknown erro */
    EMapTileFetchingUnknownError
    };
// contacts category in landmarks db
_LIT( KContactsCategory, "Contacts" );

// calendar category in landmarks db
_LIT( KCalendarCategory, "Calendar" );

// lookup database folder path
_LIT( KLookupDbPath, "c:\\mylocations\\" );

#ifdef LOCPICKER_UNIT_TEST
const QString KLocationDataLookupDbName = "c:\\locationdatalookuptestdb.db"; 
#else    
const QString KLocationDataLookupDbName = "c:\\locationdatalookupdb.db"; 
#endif

const TInt KBufSize=256;

//Different Maptile sizes for different applications
const int MapTileWidth = 614;
const int MapTileHeight = 250;
const int MaptilePortraitWidth = 334;
const int MaptilePortraitHeight = 250;
const int MaptileContactLandscapeWidth =  382;
const int MaptileContactLandscapeHeight = 168;
const int MaptileCalendarLandscapeWidth = 614;
const int MaptileCalendarLandscapeHeight = 202;    
const int MaptileHurriganesWidth = 228;    
const int MaptileHurriganesHeight = 170;

static const char*  MAPTILE_IMAGE_PORTRAIT = "_Vertical";
static const char*  MAPTILE_IMAGE_LANDSCAPE = "_Horizontal";
static const char*  MAPTILE_IMAGE_CALENDAR = "_Calendar";
static const char*  MAPTILE_IMAGE_CONTACT  = "_Contact"; 
static const char*  MAPTILE_IMAGE_HURRIGANES = "_Hurriganes";
static const char*  MAPTILE_IMAGE_TYPE = "PNG";
       
#endif  // __MYLOCATIONSDEFINES_H__
// End of file


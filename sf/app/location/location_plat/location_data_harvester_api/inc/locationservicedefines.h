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
* Description: defines constants and types.
*
*/


#ifndef LOCATIONSERVICEDEFINES_H
#define LOCATIONSERVICEDEFINES_H
#include <QString>

/**
 * MaptileLookupItem struct.
 * This structure defines a maptile lookup item.
 */

class MaptileLookupItem
{
public:
    // Uid of the source entry
    quint32 iUid;

    // Source type
    quint32 iSource;
   
    // File Path
    QString iFilePath;
    
    // map tile fetching status
    quint32 iFetchingStatus;
    
    // user setting status
    bool iUserSetting;
};

/** Defines uid source type
 */
enum TUidSourceType
{
    /** Uid Source type calendar */
    ESourceCalendar,
    /** Uid Source type landmarks */
    ESourceLandmarks,
    /** Uid Source type maps history */
    ESourceMapsHistory,
    /** Uid Source type contacts default/prefered address */
    ESourceContactsPref,
    /** Uid Source type contacts home address */
    ESourceContactsHome,
    /** Uid Source type contacts work address */
    ESourceContactsWork,
    /** Uid Source type landmarks category */
    ESourceLandmarksCategory,
    /** Uid Source type landmarks user created category */
    ESourceLandmarksUserCat,
    /** Uid Source type landmarks 'contacts' category */
    ESourceLandmarksContactsCat,
    /** Uid Source type landmarks 'calendar' category */
    ESourceLandmarksCalendarCat,
    /** Uid Source type maps history' category */
    ESourceLandmarksHistoryCat,
    /** Source type invalid */
    ESourceInvalid
};

/** Defines entry change type
*/
enum TEntryChangeType
    {
    /** Entry added */
    EEntryAdded,
    /** Entry modified */
    EEntryModified,
    /** Entry deleted */
    EEntryDeleted,
    /** Entry change unknown */
    EEntryUnknown
    };


#endif // QLOCATIONPICKERITEM_H

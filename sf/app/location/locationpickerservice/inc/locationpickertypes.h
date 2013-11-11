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
* Description: defines types and constants for location picker
*
*/

#ifndef LOCATIONPICKERTYPES_H
#define LOCATIONPICKERTYPES_H

#include <hbglobal.h>
#include <locationservicedefines.h>

// separator
const QString KSeparator(",");

// space
const QString KSpace(" ");

//colon
const QString KTitleSeparator(":");

//Icon Paths

// dummy image for a LandMarks
const QString KDummyImage("qtg_large_maps");
//Image for calendar entries
const QString KCalendarImage("qtg_large_calendar");
//Image for contacts entries
const QString KContactsImage("qtg_large_phonebook");

//Contacts collection icon
const QString KCollectionsContacts("qtg_small_phonebook");

//Calendar collection icon
const QString KCollectionsCalendar("qtg_small_calendar");

//Places collection icon
const QString KCollectionsPlaces("qtg_small_favorite");

//Contacts type home icon
const QString KContactHomeIcon("qtg_mono_home");
//Contacts type work icon
const QString KContactWorkIcon("qtg_mono_work");

/** 
 * Defines view type in location picker
*/
enum TViewType
{
    /** LocationPicker Content */
    ELocationPickerContent,
    /** Collection list Content*/
    ELocationPickerCollectionListContent,
    /** Search View */
    ELocationPickerSearchView,
    /** Collection content */
    ELocationPickerCollectionContent
};


#endif // LOCATIONPICKERTYPES_H


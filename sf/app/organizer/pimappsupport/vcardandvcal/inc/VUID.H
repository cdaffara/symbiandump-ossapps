// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @publishedAll
 @released 
*/

#ifndef __VUID_H__
#define __VUID_H__
#include <e32cmn.h>
/** The default size for 8-bit buffers used in Versit. */
#define KVersitDefaultBufferSize 32
/** The maximum length for a Versit date/time buffer. */
#define KVersitMaxDateTimeLength 16
/** Not used. */
#define KVersitMaxVersionLength 16
_LIT8(KVersitQuestionMark, "?");

//
// Uid's
// 268440459-268440508 have been allocated to Versit initially
//

/** The UID for a KVersitTokenBEGIN property.

Used in vCards and vCalendars. */
#define KVersitTokenBeginUid							268440459
/** The UID for a KVersitTokenEND property.

Used in vCards and vCalendars. */
#define KVersitTokenEndUid								268440460
/** The UID for a property with an unrecognised name. */
#define KVersitTokenUnknownUid							268440461
/** The UID for a version property.

Used in vCards and vCalendars. */
#define KVersitTokenVersionUid							270490587
/** The UID for an agent property.

Used in vCards. */
#define KVCardPropertyAgentUid							268440471
/** The UID for a SOUND property.

Used in vCards. */
#define KVCardPropertySoundUid							270537213
/** The UID for an alarm property.

Used in vCalendars and associated with a vEvent or vToDo. */
#define KVCalPropertyAlarmUid							268440472
/** The UID for a vEvent entity. */
#define KVCalEntityUidVEvent							268440473
/** The UID for a vTodo entity. */
#define KVCalEntityUidVTodo								268440474
/** A grouped property UID. This is used to determine whether a property 
is grouped. */
#define KVersitGroupedPropertyUid						268440475
/** The UID for a time zone property.

Used in vCards and vCalendars. */
#define KVersitPropertyTimeZoneUid						268440476
/** The UID for a date property. */
#define KVersitPropertyDateUid							268440477
/** A time property UID. */
#define KVersitTimePropertyUid							268440478

/** The UID for an rich (extended) alarm property.

Used in vCalendars and associated with a vEvent or vToDo. */
#define KVCalPropertyExtendedAlarmUid					268440479



/** Used by the contacts database to identify a vCard. */
#define KVersitEntityUidVCard							268440483		//Used by cntmodel and not by versit
/** The UID for a recurrence property. */
#define KVCalPropertyRecurrenceUid						268440484
/** The UID for a daylight savings property. */
#define KVersitPropertyDaylightUid						268440485
/** The UID for a character set property parameter. */
#define KVersitParamCharsetUid							268440488

/** The UID for a quoted printable to text converter. */
#define KQuotedPrintableToTextConverter					268441637
/** The UID for a text to quoted printable converter. */
#define KTextToQuotedPrintableConverter					268441638
/** The UID for a base 64 to text converter. */
#define KBase64ToTextConverter							268440005
/** The UID for a text to base 64 converter. */
#define KTextToBase64Converter							268437281 

/** The UID for a character encoding property parameter. */
#define KVersitParamEncodingUid							268440463
/** The UID for a QUOTED-PRINTABLE property parameter. */
#define KVersitParamEncodingQuotedPrintableUid			268440464
/** The UID for a BASE64 property parameter. */
#define KVersitParamEncodingBase64Uid					268440465


/*
 * These are versit data-type uids for property values. They
 * are versit-internal.
 */

/** The UID for a heap descriptor property. */
#define KVersitPropertyHBufCUid							268440466
/** The UID for a binary property.

Used in vCards for logos and photos. */
#define KVersitPropertyBinaryUid						268470833
/** The UID for a descriptor array property.

Used in vCards and vCalendars. */
#define KVersitPropertyCDesCArrayUid					268440467
/** The UID for a multiple date/time property.

Used in vCalendars. */
#define KVersitPropertyMultiDateTimeUid					268440468
/** The UID for a date/time property.

Used in vCards and vCalendars. */
#define KVersitPropertyDateTimeUid						268440469
/** The UID for an integer property.

Used in vCalendars, e.g. for priorities and sequences. */
#define KVersitPropertyIntUid							268440470


/*
 * The following is a list of UIDs that were used by versit but are not now
 */
//UID's for tokens
//268440462
//UID's used for character sets
//268440489, 268440499-506, 268457255-6, 268457654-8, 268478097
//UID's used for Quoted Printable with a character set
//268440486, 268440490-8, 268457253-4, 268457801-5
//UID's used for Base64 with a character set
//268440480-82, 268440487, 268440508, 268456960, 268457257-62, 268470904-6, 268479493

#endif

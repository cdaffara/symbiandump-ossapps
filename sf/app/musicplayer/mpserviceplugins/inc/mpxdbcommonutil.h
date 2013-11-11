/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class MPXDbCommonUtil which contains utilities functions
*
*/


#ifndef MPXDBCOMMONUTIL_H
#define MPXDBCOMMONUTIL_H

// INCLUDES
#include <apmstd.h>
#include <f32file.h>
#include <badesca.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxmessage2.h>
#include <mpxcollectionpath.h>

#include "mpxdbcommonstd.h"


// FORWARD DECLARATIONS
class CMPXRowSet;
class CMPXDatabase;
class CMPXMedia;
class CMPXMediaArray;
class RSqlStatement;

// CLASS DECLARATION

/**
* This static class contains utilities functions used by MPX Collection DB Plugins
*/
class MPXDbCommonUtil
    {

    public:

        /**
        * Set HBufC data member.
        * Free the memory and set the aBuf to the new location.
        * @param aBuf, the buf to be set
        * @param aSource, source descriptor
        * @param aMaxLen, max length allowed in aBuf, -1 is unlimited.
        * @return KErrOverflow if the length of source is greater than MaxLen
        */
        IMPORT_C static TInt SetHBuf(HBufC*& aBuf,
                            const TDesC* aSource,
                            TInt aMaxLen = -1);

        /**
        * Replace the single quote in the SQL string
        * @param aSrc source descriptor
        * @param aTrg target descriptor
        */
        IMPORT_C static void FindAndReplaceSingleQuote(const TDesC& aSrc, TDes& aTrg);

        /**
        * Delete from file system (includes read-only files)
        * @param aFs File server session handle
        * @param aFile File path to be deleted
        * @return KErrNone if file deleted successfully
        *         System-wide error code otherwise
        */
        IMPORT_C static TInt DeleteFile(RFs& aFs, const TDesC& aFile);

        /**
        * Get the drive Id with a given volume unique Id
        * @param aFs File server session handle
        * @param aVolumeId volume ID to match
        * @return drive Id. If volume Id not found, return KErrNotFound
        */
        IMPORT_C static TInt GetDriveIdMatchVolIdL(RFs& aFs, TUint aVolumeId);

        /**
        * Get the volume Id with a given drive Id
        * @param aFs File server session handle
        * @param aDriveId: a drive ID to match
        * @return volume Id. If no correspending media is found for given
        *         aDriveId, 0 will return
        */
        IMPORT_C static TUint GetVolIdMatchDriveIdL(RFs& aFs, TInt aDriveId);

        /**
        * Generate a 32bits Unique Id with MD5
        * If the input is a filename, only the path part will be hashed
        * The drive part of the path will be replaced by the corresponding
        * volume Id in order to maintain uniquenss as one media may mount as
        * different drive letter in different devices
        * @param aFs File server session handle
        * @param aTableId Use category as table Id
        * @param aName Category name or file name as input to encoder
        * @param aCaseSensitve ETrue if case sensitivity should be taken
        *                      into account when generating unique ID
        * @return unique Id
        * NOTE If aName given is a path and the drive/media doesn't
        * exist (e.g. MMC not inserted), full path will be hashed
        */
        IMPORT_C static TUint32 GenerateUniqueIdL(RFs& aFs, TMPXGeneralCategory aTableId,
            const TDesC& aName, TBool aCaseSensitive);

        /**
        * Create a media object with title, type, category and ID and append it to the media array
        * @param aArray media array
        * @param aTitle title
        * @param aType attribute type
        * @param aCat attribute category
        * @param aId unique ID
        * @param aNonPermissibleActions bitmask specifying the non-permissible actions for the entry
        * @param aDbflag Dbflag value for the new media entry
        */
        IMPORT_C static void AppendMediaL(CMPXMediaArray& aArray, const TDesC& aTitle, TMPXGeneralType aType,
            TMPXGeneralCategory aCat, TMPXItemId aId = 0, TInt aNonPermissibleActions = 0,
            TUint aDbflag = 0);

        /**
        * Create a media object with title, type, category and ID and prepend it to the media array
        * @param aArray media array
        * @param aTitle title
        * @param aType attribute type
        * @param aCat attribute category
        * @param aId unique ID
        * @param aNonPermissibleActions bitmask specifying the non-permissible actions for the entry
        * @param aDbflag Dbflag value for the new media entry
        * @param aPos postion of the new item, 0 is the first item
        */
        IMPORT_C static void PrependMediaL(CMPXMediaArray& aArray, const TDesC& aTitle, TMPXGeneralType aType,
            TMPXGeneralCategory aCat, TMPXItemId aId = 0, TInt aNonPermissibleActions = 0,
            TUint aDbflag = 0,
            TInt aPos = 0);

        /**
        * Fill in the supported UID array based on what is requested in the attribute array
        * @param aAttrs requested attributes
        * @param aSupportedIds supported UID array
        */
        IMPORT_C static void FillInSupportedUIDsL(const TArray<TMPXAttribute>& aAttrs, RArray<TInt>& aSupportedIds);

        /**
        * Fill in the message with item changed information
        * @param aMessage a message to be filled with the supplied information
        * @param aId item id
        * @param aChangeType change type
        * @param aCategory category of the item changed
        * @param aUid plugin UID
        * @param aDeprecatedId deprecated id of the item after it's been moved/renamed
        */
        IMPORT_C static void FillItemChangedMessageL(CMPXMessage& aMessage, TMPXItemId aId,
            TMPXChangeEventType aChangeType, TMPXGeneralCategory aCategory, TUint aUid,
            TMPXItemId aDeprecatedId = 0);

        /**
        * Constructs an item changed message and adds it to the array
        * @param aMessageArray array to add the message to
        * @param aId item ID
        * @param aChangeType change type
        * @param aCategory category of the item changed
        * @param aUid plugin UID
        * @param aDeprecatedId deprecated id of the item after it's been moved/renamed
        */
        IMPORT_C static void AddItemChangedMessageL(CMPXMessageArray& aMessageArray, TMPXItemId aId,
            TMPXChangeEventType aChangeType, TMPXGeneralCategory aCategory, TUint aUid,
            TMPXItemId aDeprecatedId = 0);

        /**
        * Find the matching item changed message from the array
        * @param aMessageArray array containing item changed messages
        * @param aMessage the message to search for in the array.
        * @return KErrNotFound if the message cannot be found in the array; otherwise,
        *         the index of the 1st matching message in the array.
        */
        IMPORT_C static TInt FindItemChangedMessageL(const CMPXMessageArray& aMessageArray,
            const CMPXMessage& aMessage);

        /**
        * Get the DRM protection type of the file
        * @param aFile the file to retrieve the DRM type
        * @return DRM protection type of the file
        */
        IMPORT_C static TMCDrmType GetDRMTypeL(const TDesC& aFile);

        /**
        * Processes the string for single quotes
        * @param aString source string
        * @return processed string
        */
        IMPORT_C static HBufC* ProcessSingleQuotesLC(const TDesC& aString);

        /**
        * Processes the string for pattern characters so that they will be treated
        * as normal characters.
        * @param aString source string
        * @return processed string where those pattern characters will no longer be
        *         treated as special characters, they will be treated as regular
        *         characters.
        */
        IMPORT_C static HBufC* ProcessPatternCharsLC(const TDesC& aString);

        /**
        * Constructs an SQL criterion (name = value)
        * @param aCriterion name string
        * @param aValue value
        * @return criterion string on the CS
        */
        IMPORT_C static HBufC* SqlCriterionLC(
            const TDesC& aCriterion, TInt aValue);

        /**
        * Constructs an SQL criterion (name = value)
        * @param aCriterion name string
        * @param aValue value
        * @return criterion string on the CS
        */
        IMPORT_C static HBufC* SqlCriterion64LC(
            const TDesC& aCriterion, TInt64 aValue);

        /**
        * Constructs an SQL criterion (name = value)
        * @param aCriterion name string
        * @param aValue1 value1
        * @param aValue2 value2
        * @return criterion string on the CS
        */
        IMPORT_C static HBufC* SqlCriterionLC(
            const TDesC& aCriterion, TInt aValue1, TInt aValue2);

        /**
        * Constructs an SQL criterion (name = value)
        * @param aCriterion name string
        * @param aValue1 TInt64 value1
        * @param aValue2 TInt64 value2
        * @return criterion string on the CS
        */
        IMPORT_C static HBufC* SqlCriterion64LC(
            const TDesC& aCriterion, TInt64 aValue1, TInt64 aValue2);

        /**
        * Constructs an SQL criterion (name = value)
        * @param aCriterion name string
        * @param aValue value string
        * @return criterion string on the CS
        */
        IMPORT_C static HBufC* SqlCriterionLC(
            const TDesC& aCriterion, const TDesC& aValue);

        /**
        * Constructs an SQL criterion (name = value)
        * @param aCriterion name string
        * @param aValue1 value1
        * @param aValue2 value2
        * @return criterion string on the CS
        */
        IMPORT_C static HBufC* SqlCriterionLC(
            const TDesC& aCriterion, const TDesC& aValue1, const TDesC& aValue2);

        /**
        * Constructs an SQL criterion (name = value)
        * @param aCriterion name string
        * @param aValue1 value1
        * @param aValue2 value2
        * @param aValue3 value3
        * @param aValue4 value4
        * @return criterion string on the CS
        */
        IMPORT_C static HBufC* SqlCriterionLC(
            const TDesC& aCriterion, const TDesC& aValue1, TInt aValue2,
            const TDesC& aValue3, TInt aValue4);

        /**
        * Constructs an SQL criterion (name = value) and adds it to the array
        * @param aCriteriaArray array to add the criterion to
        * @param aCriterion name
        * @param aValue value
        * @return number of characters in the criterion string
        */
        IMPORT_C static TInt AddSqlCriterionL(CDesCArray& aCriteriaArray, const TDesC& aCriterion,
            TInt aValue);

        /**
        * Constructs an SQL criterion (name = value) and adds it to the array
        * @param aCriteriaArray array to add the criterion to
        * @param aCriterion name
        * @param aValue value
        * @return number of characters in the criterion string
        */
        IMPORT_C static TInt AddSqlCriterionL(CDesCArray& aCriteriaArray, const TDesC& aCriterion,
            const TDesC& aValue);

        /**
        * Create a full path with input drive Id and path
        * @param aDriveId a drive Id to added in front of path
        * @param aPath path contains path and filename
        * @return created full path, ownership transferred to client
        */
        IMPORT_C static HBufC* CreateFullPathL(TInt aDriveId, const TDesC& aPath);

        /**
        * Appends a name and a string value to the parameter arrays.
        * @param aFields field names array to append to
        * @param aValues field values array to append to
        * @param aField field name
        * @param aValue field value
        */
        IMPORT_C static void AppendValueL(CDesCArray& aFields, CDesCArray& aValues,
            const TDesC& aField, const TDesC& aValue);

        /**
        * Appends a name and an int value to the parameter arrays.
        * @param aFields field names array to append to
        * @param aValues field values array to append to
        * @param aField field name
        * @param aValue field value
        */
        IMPORT_C static void AppendValueL(CDesCArray& aFields, CDesCArray& aValues,
            const TDesC& aField, TUint32 aValue);

        /**
        * Returns the MIME type for a given URI
        * @param aUri URI
        * @return the MIME type
        */
        IMPORT_C static TDataType GetMimeTypeForUriL(const TDesC& aUri);

        /**
        * Constructs a string from the specified array like:
        *   <item1>aSeparator<item2>aSeparator...<itemN>
        * @param aArray source array
        * @param aSeparator separator string
        * @return constructed string on the CS
        */
        IMPORT_C static HBufC* StringFromArrayLC(const CDesCArray& aArray, const TDesC& aSeparator);

        /**
        * Constructs a string from two arrays like:
        *   <aNameArray.item1>aValueSeparator<aValueArray.item1>aEntitySeparator...
        * @param aNameArray array containing the names
        * @param aValueArray array containing the values
        * @param aValueSeparator value separator
        * @param aEntitySeparator entity separator
        * @return constructed string on the CS
        */
        IMPORT_C static HBufC* StringFromArraysLC(const CDesCArray& aNameArray,
            const CDesCArray& aValueArray, const TDesC& aValueSeparator,
            const TDesC& aEntitySeparator);

        /**
        * Converts TTime to a datetime string representation.
        * @param aTime TTime object
        * @return string containing date and time on the CS
        */
        IMPORT_C static HBufC* TTimeToDesLC(const TTime& aTime);

        /**
        * Converts string representation of datetime into TTime.
        * @param aDateTime string, containing date and time
        * @return aTime TTime object
        */
        IMPORT_C static TTime DesToTTimeL(const TDesC& aDateTime);

        /**
        * Returns the current time in the string format used by the database.
        * @return string containing current date and time on the CS
        */
        IMPORT_C static HBufC* CurrentTimeDesLC();

        /**
        * Returns the current date in the string format used by the database.
        * @return string containing current date on the CS
        */
        IMPORT_C static HBufC* CurrentDateDesLC();

        /**
        * Fills the drive from the URI. if the URI does not include a drive letter
        * the method does not modify aDrive and returns KErrNotFound
        * @param aUri URI to be analysed
        * @param aDrive returns the drive
        * @return system error code
        */
        IMPORT_C static TInt GetDriveL(const TDesC& aUri, TDriveUnit& aDrive);

        /**
        * Checks if a specified attribute exists in the attribute array
        * @param aAttrs attribute array
        * @param aAttribute attribute to be checked
        * @return ETrue if the attribute exists, Efalse otherwise
        */
        IMPORT_C static TBool AttributeExists(const TArray<TMPXAttribute>& aAttrs,
            const TMPXAttribute& aAttribute);

        /**
        * Reads the value of a text field. This method is required because on ARMv5
        * the RSqlStatement::ColumnTextL method does not return KNullDesC for empty
        * text fields.
        * @param aStatement recordset to read the field from
        * @param aField field index
        * @return Text field value or KNullDesC if empty
        */
        IMPORT_C static TPtrC GetColumnTextL(RSqlStatement& aStatement, TInt aField);

        /**
        * Constructs an item changed message and adds it to the array
        * @param aMessageArray array to add the message to
        * @param aId item ID
        * @param aChangeType change type
        * @param aCategory category of the item changed
        * @param aUid plugin UID
        * @param aAlbumArt is the flag for album art changed
        * @param aDeprecatedId deprecated id of the item after it's been moved/renamed
        */
        IMPORT_C static void AddItemAlbumChangedMessageL(CMPXMessageArray& aMessageArray, TMPXItemId aId,
            TMPXChangeEventType aChangeType, TMPXGeneralCategory aCategory, TUint aUid,
            TBool aAlbumArt, TMPXItemId aDeprecatedId = 0 );
    private:
        /**
        * Create a media object with title, type, category and ID and push onto the CS.
        * @param aTitle title
        * @param aType attribute type
        * @param aCat attribute category
        * @param aId unique ID
        * @param aNonPermissibleActions bitmask specifying the non-permissible actions for the entry
        * @param aDbflag Dbflag value for the new media entry
        */
        static CMPXMedia* ConstructMediaLC(const TDesC& aTitle, TMPXGeneralType aType,
            TMPXGeneralCategory aCat, TMPXItemId aId, TInt aNonPermissibleActions,
            TUint aDbflag);
    };

#endif // MPXDBCOMMONUTIL_H

// End of File


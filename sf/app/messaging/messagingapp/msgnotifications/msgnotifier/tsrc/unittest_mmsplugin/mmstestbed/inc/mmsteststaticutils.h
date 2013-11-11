/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 */

#ifndef MMSTESTSTATICUTILS_H
#define MMSTESTSTATICUTILS_H

#include <e32std.h>
#include <msvstd.h>
#include <badesca.h>

#include "mmsconst.h"

class RFs;
class CMsvSession;
class CMmsEncode;
class CMmsHeaders;

#ifdef __WINS__
_LIT( KRootPath, "c:\\" );
#else
// This is removable disk.
// drive letter should not be hard coded, must be investigated further
// We use c: for a while for the tests (to be cahnged later)
_LIT( KRootPath, "c:\\" );
//_LIT( KRootPath, "e:\\" );
#endif

// directory where mmbox descriptions are created
_LIT( KMmsMMBoxDescriptionDirectory, "C:\\mmsmmboxdescriptions\\");
// directory where created headers are dumped
_LIT( KMmsDumpDirectory, "C:\\mmsdump\\");
// content type of MMS PDUs
_LIT8( KMmsMimeType, "application/vnd.wap.mms-message" );

_LIT( KWild, "*" );


/**
 *  joined array structure
 *
 */
NONSHARABLE_CLASS( TMmsJoinedArrays ): public MDesCArray
    {
public:
    TMmsJoinedArrays(CDesCArray* const & aArray1, CDesCArray* const & aArray2);
    TInt MdcaCount() const;
    TPtrC16 MdcaPoint(TInt aIndex) const;
private:
    CDesCArray* const & iArray1;
    CDesCArray* const & iArray2;
    };

/**
 *  static utility functions
 *
 */
class TMmsTestUtils
    {

public:

    /**
     *
     */
    TMmsTestUtils();

    /**
     * read phone number for sending messages
     *
     * @param aFileName full path for filename
     * @param aFs file system handle
     * @return allocated pointer containing the name
     *     The caller must delete the pointer when no longer needed
     */
    static HBufC* ReadContactFromFileL( TDesC& aFileName, RFs& aFs );

    static void DecodeLoggingOnL();
    static void DecodeLoggingOffL();
    static void BinaryDumpOnL();
    static void BinaryDumpOffL();

    /**
     * Clean all temporary directories used in local mode.
     * Does not delete the directories, only the contents
     *
     * @param aFs file system handle
     */
    static void CleanDirectoryL( RFs& aFs );

    /**
     * Dump a binary message
     *
     * @param aBuffer buffer containing the binary message
     * @param aFilename full path of the directory whenre the dump goes
     * @param aParse reference to parser (to save stack space)
     * @param aFs file system handle
     */
    static void Dump(
        CBufFlat& aBuffer, TFileName& aFilename, TParse& aParse, RFs& aFs );

    /**
     * Delete all MMS messages and notifications from a given folder
     *
     * @param aBoxId folder id
     * @param aSession Messaging server session
     */
    static void CleanBoxL(TMsvId aBoxId, CMsvSession& aSession);

    /**
     * turn on logging messages sent to email addresses
     */
    static void EmailLoggingOnL();

    /**
     * turn off logging messages sent to email addresses
     */
    static void EmailLoggingOffL();

    /**
     * load list of children into selection and return count
     *
     * @param aBoxId folder id
     * @param aMsvEntrySelection reference to a pointer that will contain the list of child ids
     *     caller is responsible of deleting the pointer afterwards.
     *     Pointer may be NULL at entry, and a new pointer will be allocated
     * @param aSession messge server session
     * @param aMessageType child MTM type, default is multimedia message
     * @return number of entries in selection
     */
    static TInt CountChildrenL(
        TMsvId aBoxId,
        CMsvEntrySelection*& aMsvEntrySelection,
        CMsvSession& aSession,
        TUid aMessageType = KUidMsgTypeMultimedia);

    /**
     * Create a folder entry
     *
     * @param aSession messge server session
     * @param aParentFolder the parent folder
     * @param aFolderName name of the folder
     * @param aFolderId will contain the if of the new folder if creation was successful
     */
    static void CreateFolderEntryL(
        CMsvSession& aSession,
        TMsvId aParentFolder,
        const TDesC& aFolderName,
        TMsvId& aFolderId );

    /**
    * Create an MMS notification from data in buffer
    * @param aNotificationFolder mms notification folder (target folder)
    * @param aServiceId id of MMS service
    * @param aEncodeBuffer buffer that contains the data
    * @param aSession message server session
    * @return id of the created notification
    */
    static TMsvId CreateNotificationEntryL(
        TMsvId aNotificationFolder,
        TMsvId aServiceId,
        CBufFlat* aEncodeBuffer,
        CMsvSession& aSession );

    /**
    * Encode an MMS notification into buffer
    * @param aUrl url of the notification for local messages a filepath
    * @param aSize size of the message
    * @param aMmsHeaders MMS headers structure for creating the notification
    * @param aMmsEncoder reference to CMmsEncode class
    * @param aEncodeBuffer flat buffer that will contain the encoded notification
    */
    static void FormNotification(
        TDesC8& aUrl,
        TInt aSize,
        CMmsHeaders& aMmsHeaders,
        CMmsEncode& aMmsEncoder,
        CBufFlat* aEncodeBuffer );


    static TBool IsDrive(const TDesC& aFileName);
    static TBool IsDir(const TDesC& aFileName, RFs& aFs);
    static TBool IsFile(const TDesC& aFileName, RFs& aFs);


private:

    };


#endif // MMSTESTSTATICUTILS_H

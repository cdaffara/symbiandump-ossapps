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
* Description:  CSconSyncRelationship header
*
*/


#ifndef SCONSYNCRELATIONSHIP_H_
#define SCONSYNCRELATIONSHIP_H_

#include <e32base.h>
#include <f32file.h> 
#include <s32file.h>
#include <smldatasyncdefs.h>
#include <syncmldef.h>

class CDictionaryFileStore;

NONSHARABLE_CLASS ( CSconSyncRelationship ) : public CBase, public MSmlSyncRelationship
    {
public:
    /**
     * Creates CSconSyncRelationship object.
     * @param   aFs             reference to open RFs connection
     * @param   aRelationUid    uniqueID for each server-client pair
     * @return new CSconSyncRelationship object
     */
    static CSconSyncRelationship* NewL( RFs& aFs, TUid aRelationUid, TSmlDataProviderId aProviderId );
	~CSconSyncRelationship();
	
	static void SetTimeStampL( RFs& aFs, TUid aRelationUid, TSmlDataProviderId aId );
	static void GetTimeStampL( RFs& aFs, TUid aRelationUid, TSmlDataProviderId aId, TDateTime& aTimeStamp );
	
public: // From MSmlSyncRelationship
    
	/**
     * Returns the unique identifier for the synchronisation relationship.
     *
     * @return             A unique identifier for the sync relationship.
     */
    TSmlSyncTaskKey SyncTaskKey() const;
    
    /**
     * Opens a specified stream for reading, and places an item on the cleanup stack to close it.
     *
     * @param               aReadStream    On return, an open read stream
     * @param               aStreamUid     The UID of the stream to open for reading.
     */
    void OpenReadStreamLC(RReadStream& aReadStream, TUid aStreamUid);
    
    /**
     * Opens the specified stream, or creates a new one if it does not exist, and places an item on the cleanup stack to close it.
     *
     * @param              aWriteStream    On return, an open write stream
     * @param              aStreamUid      The UID of the stream to open or create
     */
    void OpenWriteStreamLC(RWriteStream& aWriteStream, TUid aStreamUid);
    
    /**
     * Tests if the specified stream identified exists in the store.
     *
     * @param              aStreamUid The stream UID
     * @return             Non-zero if the stream exists, otherwise EFalse.
     */
    TBool IsStreamPresentL(TUid aStreamUid) const;
    
private:
	CSconSyncRelationship( RFs& aFs, TUid aRelationUid, TSmlDataProviderId aProviderId );
	void ConstructL();
	
private:
    
	CDictionaryFileStore* iDictionaryStore;
	RFs&   iFs;
	TUid   iID;
	TSmlDataProviderId iProviderId;
    };

#endif /*SCONSYNCRELATIONSHIP_H_*/

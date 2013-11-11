/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file defines class CIpsPlgMruList.
*
*/


#ifndef IPSPLGMRULIST_H
#define IPSPLGMRULIST_H


const TUid KCRUidIpsSosMru = { 0x20010128 };

/**
 *  Most recent used name and address store
 *
 *  Reads and writes most recent used email addresses to central
 *  repository. Central repository stores key - value -pairs. In this
 *  case, first 16 bits of a key is used to identify mailbox and last 16
 *  bits makes key unique for name and address information. Example:
 *
 *  Key         Value
 *  0000 0000   <mailboxId1>                     : integer
 *  0000 0001   <1. name of mailboxid1>          : descriptor
 *  0000 0002   <1. email addr of mailboxid1>    : descriptor
 *  0000 0003   <2. name of mailboxid1>          : descriptor
 *  0000 0004   <2. email addr of mailbodid1>    : descriptor
 *  .... ....
 *  .... ....
 *  0001 0000   <mailboxId2>                     : integer
 *  0001 0001   <1. email addr of mailboxid2>    : descriptor
 *  0001 0002   <1. email addr of mailboxid2>    : descriptor
 *  .... ....
 *  .... ....
 *  FFFF 0000   <mailboxIdFFFF+1>                : integer
 *  FFFF 0001   <1. email addr of mailboxIdNNNN> : descriptor
 *  FFFF 0002   <1. email addr of mailboxIdNNNN> : descriptor
 *
 *  @lib ipssosplugin.lib
 *  @since
 */
NONSHARABLE_CLASS( CIpsPlgMruList )
    : public CBase
    {
public:
    /**
     * Two-phased constructor.
     */
     static CIpsPlgMruList* NewL( );

    /**
     * Destructor.
     */
    virtual ~CIpsPlgMruList();

    /**
     * Reads last used contacts from centrep and return
     * array of descriptors that contains data. Data is read
     * from cache if not changed in centrep. On error situation
     * return empty descriptor array.
     *
     * @param aMailbox unique mailbox id
     * @return  descriptor array
     * @since FS 1.0
     */
    CDesCArrayFlat* GetMruListL( const TFSMailMsgId& aMailbox );

    /**
     * Writes last used contacts to centrep.
     * On error all data consern mailbox is cleared from centrep
     *
     * @param aMailbox unique mailbox id
     * @param aNewMruList contact descriptor array
     * @since FS 1.0
     */
    void SetMruListL( const TFSMailMsgId& aMailBoxId,
        MDesCArray* aNewMruList );

    /**
     * Cleares all data stored under mailbox id
     *
     * @param aMailbox unique mailbox id
     * @since FS 1.0
     */
    void ClearDataL( const TFSMailMsgId& aMailBoxId );

private:

    /**
     * Constructor
     */
    CIpsPlgMruList( );

    /**
     * Constructor for leaving methods
     */
    void ConstructL( );

    /**
     * Reads descriptor identified by aKey from centrep
     *
     * @param aKey centrep key for descriptor
     * @return KErrCorrupt or error from centrep transaction
     */
    TInt StoreToArrayL(TUint32 aKey);

    /**
     * Read all mailbox keys from centrep to array
     * finds key that contains mailboxid
     *
     * @param aMboxId mailbox id key to be finded
     * @param aMboxKey correct key or invalid key if mailbox id not
     *                 founded from centrep
     * @param aArray array where all mailbox keys will be stored
     * @return KErrNone or KErrNotFound
     */
    TInt GetMailboxKeyFromRepositoryL(
        TInt aMboxId,
        TUint32& aMboxKey,
        RArray<TUint32>& aArray );

    /**
     * inline for creating iRepository, for performance reasons
     * not created in constructl
     */
    inline static void CreateRepositoryL( CRepository*& aRep );

    /**
     * Find smallest key that not founded in array
     *
     * @param aKeys set of keys which is sorted in fuction
     * @return unique key
     */
    TUint32 GetUnusedMboxKey( RArray<TUint32>& aKeys );

    /**
     * Find smallest key that not founded in array
     *
     * @param aMboxKey mailbox key which data error conserns
     * @param aError system wide error code
     */
    void HandleRepositoryError( TUint32 aMboxKey, TInt aError );

private: // data

    // Mru list repository instance
    CRepository*            iRepository;

    // array for repository data
    RPointerArray<HBufC>    iHbufArray;

    // array for repository keys
    RArray<TUint32>         iKeys;

    };



// inline for centrep object creation
inline void CIpsPlgMruList::CreateRepositoryL( CRepository*& aRep )
    {
    if ( !aRep )
        {
        // real uid needed
        aRep = CRepository::NewL( KCRUidIpsSosMru );
        }
    }

#endif // IPSPLGMRULIST_H

// End of file

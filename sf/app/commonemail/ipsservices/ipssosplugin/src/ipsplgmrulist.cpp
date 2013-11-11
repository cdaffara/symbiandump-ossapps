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
* Description: This file implements class CIpsPlgMruList.
*
*/



#include "emailtrace.h"
#include "ipsplgheaders.h"

const TUint32 KMruListMailboxMask = 0x0000FFFF;
const TUint32 KMruListMailboxPartKey = 0x0;
const TUint32 KMruListValueMask = 0xFFFF0000;
const TUint32 KMruListIncorrectMBoxKey = 0xFFFFFFFF;
const TUint32 KMruListKeyBaseValue = 0x00010000;
// what are actual size in Freestyle framework?
const TInt KMruListPrefDesSize = 40;
const TInt KMruListMaxDesSize = 200;
const TInt KMruListArrayInitSize = 8;

// <cmail>
const TInt KIpsSmtpOperationCharLessThan = '<';
const TInt KIpsSmtpOperationCharMoreThan = '>';
// </cmail>

// ---------------------------------------------------------------------------
// CIpsPlgMruList::NewL( )
// ---------------------------------------------------------------------------
//
CIpsPlgMruList* CIpsPlgMruList::NewL( )
    {
    FUNC_LOG;
    CIpsPlgMruList* self = new( ELeave ) CIpsPlgMruList( );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMruList::~CIpsPlgMruList()
// ---------------------------------------------------------------------------
//
CIpsPlgMruList::~CIpsPlgMruList()
    {
    FUNC_LOG;
    delete iRepository;
    iHbufArray.ResetAndDestroy();
    iHbufArray.Close();
    iKeys.Reset();
    iKeys.Close();
    }

// ---------------------------------------------------------------------------
// CIpsPlgMruList::CIpsPlgMruList()
// ---------------------------------------------------------------------------
//
CIpsPlgMruList::CIpsPlgMruList( )
    : iRepository( NULL )
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
// CIpsPlgMruList::ConstructL( )
// ---------------------------------------------------------------------------
//
void CIpsPlgMruList::ConstructL( )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMruList::GetMruListL
// ---------------------------------------------------------------------------
//
CDesCArrayFlat* CIpsPlgMruList::GetMruListL( const TFSMailMsgId& aMailbox )
    {
    FUNC_LOG;
    // create iRepository if not yet created
    CreateRepositoryL( iRepository );

    TBool returnEmpty( EFalse );

    // Do not cache anything, there are multiple plugin instances
    iKeys.Reset();
    TUint32 mboxKey;
    if ( KErrNone != GetMailboxKeyFromRepositoryL( aMailbox.Id(), mboxKey, iKeys ) )
        {
        returnEmpty = ETrue;
        }
    iKeys.Reset();

    // get correct mailbox key and fill corresponding name and address keys
    // in keys array
    if ( !returnEmpty )
        {
        if ( KErrNone != iRepository->FindL( mboxKey, KMruListValueMask, iKeys ) )
            {
            returnEmpty = ETrue;
            }
        }

    // if we have found correct key find all keys that
    // maches pattern 0xMKEY**** key array should contain
    // at least 3 items, and size shoud be odd
    // or array is empty or corrupted
    TInt keyCount( iKeys.Count() );
    if ( !returnEmpty && keyCount >= 3 && ( keyCount%2 ) == 1 )
        {
        iHbufArray.ResetAndDestroy();
        iKeys.SortUnsigned();
        TInt i(0);

        // first item in array should be mbox key
        if ( iKeys[0] == mboxKey )
            {
            // index to first name key
            i = 1;
            // store name and address in one loop cycle
            while ( !returnEmpty && i <= ( keyCount-2 ) )
                {
                TUint key = iKeys[i];
                // name is always in odd and address in even key
                if ( (key%2==1) && (iKeys[i+1]==key+1) )
                    {
                    // adding name and email to hbuf array
                    if ( StoreToArrayL(iKeys[i]) != KErrNone )
                        {
                        // something went wrong in name store
                        // just break the loop and return empty
                        HandleRepositoryError( mboxKey, KErrCorrupt );
                        returnEmpty = ETrue;
                        }
                    if ( StoreToArrayL(iKeys[i+1]) != KErrNone )
                        {
                        // store is corrupted
                        HandleRepositoryError( mboxKey, KErrCorrupt );
                        returnEmpty = ETrue;
                        }
                    }
                i = i+2;
                }
            }
        else
            {
            // first item was not mailbox key
            // centrep data is corrupted
            HandleRepositoryError( mboxKey, KErrCorrupt );
            returnEmpty = ETrue;
            }
        }
    else if ( !returnEmpty )
        {
        // mailbox found but there is no data or centrep is corrupted,
        // return empty
        HandleRepositoryError( mboxKey, KErrCorrupt );
        returnEmpty = ETrue;
        }

    // create and return des array
    CDesCArrayFlat* retArr = NULL;

    if ( !returnEmpty )
        {
        retArr = new ( ELeave ) CDesCArrayFlat( KMruListArrayInitSize );
        CleanupStack::PushL( retArr );

        TInt count( iHbufArray.Count() );
        // fill descriptor array
        for ( TInt i(0); i < count; i++ )
            {
            HBufC* app = iHbufArray[i];
            retArr->AppendL(*app);
            }
        CleanupStack::Pop( retArr );
        }
    else
        {
        iHbufArray.ResetAndDestroy();
        retArr = new ( ELeave ) CDesCArrayFlat( KMruListArrayInitSize );
        }

    return retArr;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMruList::SetMruListL
// ---------------------------------------------------------------------------
//
void CIpsPlgMruList::SetMruListL( const TFSMailMsgId& aMailBoxId,
    MDesCArray*  aNewMruList )
    {
    FUNC_LOG;
    // create iRepository if not yet created
    CreateRepositoryL( iRepository );

    TUint32 mboxKey = KMruListIncorrectMBoxKey;
    iKeys.Reset();
    // get mailbox keys to keys array and
    // check if mailboxid is already stored in some key
    TInt error = GetMailboxKeyFromRepositoryL(
        aMailBoxId.Id(), mboxKey, iKeys );

    if ( KErrNone == error )
        {
        // mailbox found
        TUint32 errorKey;
        error = iRepository->Delete( mboxKey, KMruListValueMask, errorKey );
        HandleRepositoryError( mboxKey, error );
        }
    else
        {
        // mailbox not found, create new key
        mboxKey = GetUnusedMboxKey( iKeys );
        }

    // set new values to central repository
    // create mailboxid key
    error = iRepository->Create( mboxKey, TInt( aMailBoxId.Id() ) );
    // handle error
    TInt count( aNewMruList->MdcaCount() );
    // <cmail>
    for ( TInt i(0); error == KErrNone && i < count; i += 2 )
        {
        if( i + 1 < count )
            {
            TPtrC displayName  = aNewMruList->MdcaPoint(i);
            TPtrC emailAddress = aNewMruList->MdcaPoint(i + 1);

            // Need to locate angle brackets from end of the data, if there
            // is display name with special characters
            TInt start( emailAddress.LocateReverse( KIpsSmtpOperationCharLessThan ) ); 
            TInt end( emailAddress.LocateReverse( KIpsSmtpOperationCharMoreThan ) );
            
            if ( KErrNotFound != start && KErrNotFound != end )
                {
                start += 1;
                emailAddress.Set( aNewMruList->MdcaPoint(i + 1).Mid( start, ( end - start ) ) );
                }
// <qmail> someone disabled in Qmail?
            // check validity of email before saving diplay name and address
            /*if( IpsSetUtils::IsValidEmailAddressL(emailAddress) )
                {
                // set values to ascending keys
                mboxKey++;

                if ( aNewMruList->MdcaPoint(i).Length() <= KMruListMaxDesSize )
                    {
                    error = iRepository->Create( mboxKey, aNewMruList->MdcaPoint(i) );
                    // handle errors
                    }
                else
                    {
                    TDesC des = aNewMruList->MdcaPoint(i);
                    error = iRepository->Create(
                        mboxKey, des.Mid(0,KMruListMaxDesSize) );
                    }
                
                mboxKey++;

                if( error == KErrNone )
                    {
                    if ( aNewMruList->MdcaPoint(i + 1).Length() <= KMruListMaxDesSize )
                        {
                        error = iRepository->Create( mboxKey, aNewMruList->MdcaPoint(i + 1) );
                        }
                    else
                        {
                        TDesC des = aNewMruList->MdcaPoint(i + 1);
                        error = iRepository->Create(
                            mboxKey, des.Mid(0,KMruListMaxDesSize) );
                        }
                    }
                }*/
            }
// </qmail>
        }
    // </cmail>

    if ( KErrNone != error )
        {
        HandleRepositoryError( mboxKey, error );
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgMruList::ClearDataL
// ---------------------------------------------------------------------------
//
void CIpsPlgMruList::ClearDataL( const TFSMailMsgId& aMailBoxId )
	{
    FUNC_LOG;
	CreateRepositoryL( iRepository );

    TUint32 mboxKey = KMruListIncorrectMBoxKey;
    iKeys.Reset();
    // get mailbox keys to keys array and
    // check if mailboxid is already sotred in some key
    TInt error = GetMailboxKeyFromRepositoryL(
        aMailBoxId.Id(), mboxKey, iKeys );

    // we have found correct mboxKey
    if ( KErrNone == error )
        {
        TUint32 errorKey;
        error = iRepository->Delete( mboxKey, KMruListValueMask, errorKey );
        HandleRepositoryError( mboxKey, error );
        }
	}

// ---------------------------------------------------------------------------
// CIpsPlgMruList::StoreToArrayL
// ---------------------------------------------------------------------------
//
TInt CIpsPlgMruList::StoreToArrayL( TUint32 aKey )
    {
    FUNC_LOG;
    HBufC* hbuf = HBufC::NewLC( KMruListPrefDesSize );

    TPtr pdes = hbuf->Des();
    TInt actualSize(0);

    TInt error = iRepository->Get( aKey, pdes, actualSize );

    // the case descriptor is larger than pref size
    if ( error == KErrOverflow )
        {
        if ( actualSize > KMruListMaxDesSize )
            {
            // centrep contains too large descriptors
            // impossible if centrep is edited only by this code,
            // set corrupted
            error = KErrCorrupt;
            }
        else
            {
            CleanupStack::PopAndDestroy( hbuf );
            hbuf = HBufC::NewLC( actualSize );
            TPtr pdes2 = hbuf->Des();
            error = iRepository->Get( aKey, pdes2 );
            }
        }

    // add or delete
    if ( KErrNone == error )
        {
        iHbufArray.AppendL( hbuf );
        CleanupStack::Pop( hbuf );
        }
    else
        {
        CleanupStack::PopAndDestroy( hbuf );
        }
    return error;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMruList::GetMailboxKeyFromRepositoryL
// ---------------------------------------------------------------------------
//
TInt CIpsPlgMruList::GetMailboxKeyFromRepositoryL(
    TInt aMboxId,
    TUint32& aMboxKey,
    RArray<TUint32>& aArray )
    {
    FUNC_LOG;
    // keys is found from centrep with partkey 0x0 and mask 0x0000FFFF
    // result is array of keys that maches to pattern 0x****0000
    TInt error = iRepository->FindL(
        KMruListMailboxPartKey, KMruListMailboxMask, aArray );

    // No need to handle other errors than not found,
    // this function should only return KErrNone or KErrNotFound
    TInt tempId;
    error = KErrNotFound;

    //find correct mailbox key from array:
    for ( TInt i = 0; error == KErrNotFound && i < aArray.Count(); i++ )
        {
        if ( iRepository->Get( aArray[i], tempId ) == KErrNone
            && tempId == aMboxId )
            {
            aMboxKey = aArray[i];
            error = KErrNone;
            }
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMruList::GetUnusedMboxKey
// ---------------------------------------------------------------------------
//
TUint32 CIpsPlgMruList::GetUnusedMboxKey( RArray<TUint32>& aKeys )
    {
    FUNC_LOG;
    // Warning: this function is complex and may contain bugs!!!

    TUint32 newKey = aKeys.Count()*KMruListKeyBaseValue;

    if ( aKeys.Count() > 1 )
        {
        // this loop finds smallest unused key from sorted key array,
        // if array[i] == i*basekey at every i when i == 0...count-1
        // then newKey is not changed
        aKeys.SortUnsigned();
        TUint32 tempKey = newKey;
        for ( TInt i = 0; tempKey == newKey && i < ( aKeys.Count() - 1); i++ )
            {
            if ( aKeys[i] + KMruListKeyBaseValue != aKeys[i+1] )
                {
                tempKey = aKeys[i] + KMruListKeyBaseValue;
                }
            }
        newKey = tempKey;
        }
    else if ( aKeys.Count() == 1 && aKeys[0] == KMruListKeyBaseValue )
        {
        newKey = 0;
        }


    return newKey;
    }

// ---------------------------------------------------------------------------
// CIpsPlgMruList::HandleRepositoryError
// ---------------------------------------------------------------------------
//
void CIpsPlgMruList::HandleRepositoryError( TUint32 aMboxKey, TInt aError )
    {
    FUNC_LOG;
    TBool doClear = EFalse;
    switch ( aError )
        {
            // log these error cases
        case KErrNone:
            // no error
            break;
        case KErrNotFound:
            // no serious error
            break;
        case KErrPermissionDenied:
            // this should be panic because application has no
            // privileges for mru repository
            break;
        case KErrAbort:
            // transaction has previously failed
            // clear mailbox data if it helps...
            doClear = ETrue;
            break;
        case KErrArgument:
            // error in argument, for ex. trying to put integer to descriptor
            // key, solution clear all keys matching aMailbox
            doClear = ETrue;
            break;
        case KErrCorrupt:
            // just clear all keys matching aMailbox
            doClear = ETrue;
            break;
        case KErrAlreadyExists:
            // should handle elsewhere, if ends here clear data
            doClear = ETrue;
            break;
        default:
            // something wrong , clear data
            doClear = ETrue;
            break;
        }

    if ( doClear )
        {
        // don't care about errors...
        TUint32 errorKey;
        TInt error = iRepository->Delete( aMboxKey, KMruListValueMask, errorKey );
        }
    }


// End of file



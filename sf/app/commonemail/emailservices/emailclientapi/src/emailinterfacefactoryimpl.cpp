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
* Description: This file implements class CEmailInterfaceFactoryImpl.
*
*/

#include <e32property.h>                // RProperty
#include <s32mem.h>                     // RDesRead/WriteStream

#include "emailinterfacefactoryimpl.h"
#include "emailcontent.h"
#include "CFSMailClient.h"
#include "emailclientapiimpldefs.h"
#include "emailclientapiimpl.h"
#include "emailaddress.h"
#include "emailmessagesearch.h"
#include "emailshutdownconst.h"

_LIT( KEmailImplPanic, "Email client API" );
const TInt KEmailUidExtraBuffer = 2 * KEmailPlatformApiUidItemSize;

// ---------------------------------------------------------------------------
// Email client API panic wrapper
// ---------------------------------------------------------------------------
void Panic( TEmailImplPanic aPanic )
    {
    User::Panic( KEmailImplPanic(), aPanic );
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::NewL
// ---------------------------------------------------------------------------
CEmailInterfaceFactoryImpl* CEmailInterfaceFactoryImpl::NewL()
    {
    CEmailInterfaceFactoryImpl* self = new (ELeave) CEmailInterfaceFactoryImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::~CEmailInterfaceFactoryImpl
// ---------------------------------------------------------------------------
CEmailInterfaceFactoryImpl::~CEmailInterfaceFactoryImpl()
    {
    TRAP_IGNORE( AppendOrRemoveUidL( EEmailUidModeRemove ) );
    }

// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::CEmailInterfaceFactoryImpl
// ---------------------------------------------------------------------------
CEmailInterfaceFactoryImpl::CEmailInterfaceFactoryImpl() : 
    CEmailInterfaceFactory()
    {
    }

// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::ConstructL
// ---------------------------------------------------------------------------
void CEmailInterfaceFactoryImpl::ConstructL()
    {
	// This leaves if related P&S keys are not defined by EmailServerMonitor,
	// so EmailServerMonitor need to be started before using client API.
	// TRAP_IGNORE should be removed after EmailServerMonitor is added to
	// starter list.
    TRAP_IGNORE( AppendOrRemoveUidL( EEmailUidModeAppend ) );
    }

// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::InterfaceL
// ---------------------------------------------------------------------------
MEmailInterface* CEmailInterfaceFactoryImpl::InterfaceL( const TInt aInterfaceId )
    {
    MEmailInterface* interface = NULL;
    switch ( aInterfaceId )
        {
        case KEmailClientApiInterface:
            interface = CEmailClientApi::NewL();
            break;
        case KEmailIFUidAddress:
            interface = CEmailAddress::NewL( MEmailAddress::EUndefined, EClientOwns );
            break;
        case KEmailIFUidTextContent:
        default:
            break;
        }
    if ( !interface )
        {
        User::Leave( KErrNotSupported );
        }
    return interface;
    }

// ---------------------------------------------------------------------------
// CEmailInterfaceFactoryImpl::AppendOrRemoveUidL
// ---------------------------------------------------------------------------
void CEmailInterfaceFactoryImpl::AppendOrRemoveUidL(
        const TEmailUidAppendRemoveMode aMode )
    {
    // Read buffer length
    TInt bufLength( 0 );
    User::LeaveIfError( RProperty::Get( KEmailShutdownPsCategory,
                                        EEmailPsKeyPlatformApiAppsToCloseLength,
                                        bufLength ) );

    // Allocate buffer for reading and then read the list of UIDs from P&S.
    // Adding some extra buffer just in case the size key and actual list
    // are out of sync. This shouldn't happen, but you never know.
    HBufC8* readBuf = HBufC8::NewLC( bufLength + KEmailUidExtraBuffer );
    TPtr8 readPtr = readBuf->Des();
    
    User::LeaveIfError( RProperty::Get( KEmailShutdownPsCategory,
                                        EEmailPsKeyPlatformApiAppsToClose,
                                        readPtr ) );
    
    // For writing get the size of the original buffer + room for our own UID
    // if needed
    TInt writeBufSize = readPtr.Length();
    if( aMode == EEmailUidModeAppend )
        {
        writeBufSize += KEmailPlatformApiUidItemSize;
        }
    
    HBufC8* writeBuf = HBufC8::NewLC( writeBufSize );
    TPtr8 writePtr = writeBuf->Des();

    // Read and write streams used to read/write the UIDs from/to descriptors
    RDesReadStream readStream( readPtr );
    CleanupClosePushL( readStream );

    RDesWriteStream writeStream( writePtr );
    CleanupClosePushL( writeStream );

    // Get our own process UID
    RProcess ownProcess;
    TUid ownUid = ownProcess.SecureId();
    ownProcess.Close();

    TInt itemsCount = readPtr.Length() / KEmailPlatformApiUidItemSize;

    TBool ownUidFound = EFalse;
    TInt writeLength = 0;
    for ( TInt ii = 0;ii < itemsCount; ++ii )
        {
        // Read next UID from the stream
        TUid item = TUid::Uid( readStream.ReadInt32L() );
        
        // We can skip our own UID. If we are removing, then we don't want
        // our UID to be written. If we are adding, we don't need to set
        // the new values as our UID already exists in the list.
        if( item == ownUid )
            {
            ownUidFound = ETrue;
            if( aMode == EEmailUidModeAppend )
                {
                // Our own UID is already in the list, so no need to update
                // the list. Hence we can quit here.
                break;
                }
            }
        else
            {
            writeStream.WriteInt32L( item.iUid );
            writeLength += KEmailPlatformApiUidItemSize;
            }
        }

    // If we are appending our UID and it wasn't found from the list,
    // write it to the stream
    if( aMode == EEmailUidModeAppend && !ownUidFound )
        {
        writeStream.WriteInt32L( ownUid.iUid );
        writeLength += KEmailPlatformApiUidItemSize;
        }

    // Set correct length for the write ptr buffer as it might not be
    // updated correctly by the write stream
    writePtr.SetLength( writeLength );

    // Set new values to P&S only if something has changed, so either:
    // 1) We are appending our UID and it didn't exist before
    // 2) We are removing our UID and it did exist before
    if( ( aMode == EEmailUidModeAppend && !ownUidFound ) ||
        ( aMode == EEmailUidModeRemove && ownUidFound ) )
        {
        // Write first the UID list as it is more probable to fail, writing
        // plain integer value shouldn't fail in any case. This way these
        // values stay in sync also in case of error, as the list length
        // gets updated only if the list itself is updated succesfully.
        User::LeaveIfError( RProperty::Set( KEmailShutdownPsCategory,
                        EEmailPsKeyPlatformApiAppsToClose,
                        writePtr ) );

        User::LeaveIfError( RProperty::Set( KEmailShutdownPsCategory,
                        EEmailPsKeyPlatformApiAppsToCloseLength,
                        writeLength ) );
        }

    CleanupStack::PopAndDestroy( 4, readBuf );
    }

// End of file

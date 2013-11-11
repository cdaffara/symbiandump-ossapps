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
* Description: Message store utilities implementation
*
*/

#include "messagestoreutils.h"
#include "ContainerStoreUtils.h"


//internal file copying buffer; the mass drives perform optimally with sizes of
//128KBs and 256KBs.
const TInt KInternalCopyBufferSize = 128*1024;


/**
 * 
 */
/*public static*/
void MessageStoreUtils::PrependBufferAndCopyFileL(
    RFs& aRfs,
    CContainerStoreUtils& aUtils,
    const TDesC& aSource,
    const TDesC& aTarget,
    const TDesC8& aPrepend )
    {
    RFile source;
    User::LeaveIfError( source.Open(
        aRfs, aSource, EFileRead | EFileShareAny ) );
    CleanupClosePushL( source );

    PrependBufferAndCopyFileInternalL(
        aRfs, aUtils, source, aTarget, aPrepend );
    
    CleanupStack::PopAndDestroy( &source );
    }

/**
 *
 */
/*public static*/
void MessageStoreUtils::PrependBufferAndMoveFileL(
    RFs& aRfs,
    CContainerStoreUtils& aUtils,
    const TDesC& aSource,
    const TDesC& aTarget,
    const TDesC8& aPrepend )
    {
    //open the source file.
    RFile source;
    User::LeaveIfError( source.Open(
        aRfs, aSource, EFileRead | EFileShareAny ) );
    CleanupClosePushL( source );

    PrependBufferAndCopyFileInternalL(
        aRfs, aUtils, source, aTarget, aPrepend );
    
    CleanupStack::PopAndDestroy( &source );
    
    User::LeaveIfError( aRfs.Delete( aSource ) );
    User::LeaveIfError( aRfs.Rename( aTarget, aSource ) );
    }

/**
 *
 */
/*public static*/
void MessageStoreUtils::ReplaceFileWithFileL(
    RFs& aRfs,
    CContainerStoreUtils& aUtils,
    RFile& aSource,
    const TDesC& aTarget )
    {
    PrependBufferAndCopyFileInternalL(
        aRfs, aUtils, aSource, aTarget, KNullDesC8 );
    }

/**
 *
 */
/*private static*/
void MessageStoreUtils::PrependBufferAndCopyFileInternalL(
    RFs& aRfs,
    CContainerStoreUtils& aUtils,
    RFile& aSource,
    const TDesC& aTarget,
    const TDesC8& aPrepend )
    {
    //disk space check.
        {
        TInt fileSize;
        User::LeaveIfError( aSource.Size( fileSize ) );
        aUtils.LeaveIfLowDiskSpaceL( fileSize + aPrepend.Length() );
        }

    //prepare the target file. 
    RFile target;
    User::LeaveIfError( target.Replace(
        aRfs, aTarget, EFileRead | EFileWrite ) );
    CleanupClosePushL( target );

    if ( aPrepend.Size() )
        {
        User::LeaveIfError( target.Write( aPrepend ) );
        }
    
    //source-to-target file copying loop.
        {
        HBufC8* buf = HBufC8::NewLC( KInternalCopyBufferSize );
        TPtr8 des = buf->Des();
    
        TInt pos = 0;
        aSource.Seek( ESeekStart, pos );
        
        User::LeaveIfError( aSource.Read( des ) );
        while ( des.Size() )
            {
            User::LeaveIfError( target.Write( des ) );
            User::LeaveIfError( aSource.Read( des ) );
            }

        CleanupStack::PopAndDestroy( buf );
        }
    
    CleanupStack::PopAndDestroy( &target );
    }

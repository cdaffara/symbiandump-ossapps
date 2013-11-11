/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    DM DevDetail Adapter Source Code, WINSCW implementation
*
*/




// INCLUDES
#include "nsmldmdevdetailadapter.h"
#include "nsmldebug.h"

// CONSTANTS
_LIT( KNSmlDevDetailSwVFile,    "z:\\Resource\\DevMan\\DevDetail_SwV.txt" );
_LIT( KNSmlDevDetailHwVFile,    "z:\\Resource\\DevMan\\DevDetail_HwV.txt" );

const TInt KNSmlDevDetailMaxDataSize = 2048;

// ---------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::GetDevDetailDataL( )
// ---------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::GetDevDetailDataL( 
                                CBufBase& aObject, 
                                TNSmlDevDetailData aElement ) const
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::GetDevDetailDataL(): begin (WINSCW)");
    
    // Select file according to aElement
    HBufC* name = NULL;
    
    switch( aElement )
        {
        case ESwVersion:
            name = KNSmlDevDetailSwVFile().AllocLC();
            break;
        case EHwVersion:
            name = KNSmlDevDetailHwVFile().AllocLC();
            break;
        default:
            User::Panic( KNSmlDevDetailPanic, KErrArgument );
        }
    
    // open file server session
    RFs fileSession;
    User::LeaveIfError( fileSession.Connect() );
    CleanupClosePushL( fileSession );
    
    // open file
    RFile file;
    TInt err = file.Open( fileSession, *name, EFileRead|EFileShareReadersOnly );
    
    // if opening file succeeded, read the contents of the file
    if ( err == KErrNone )
        {
        CleanupClosePushL( file );
        
        // read only if data size is not too large
        TInt fileSize = 0;
        User::LeaveIfError( file.Size( fileSize ) );
        if ( fileSize <= KNSmlDevDetailMaxDataSize )
            {
            // read data
            HBufC8* data = HBufC8::NewLC( fileSize );
            TPtr8 dataPtr = data->Des();
            file.Read( dataPtr, fileSize );
            
            // insert data to result buffer
            aObject.Reset();
            aObject.InsertL( 0, *data );
            
            CleanupStack::PopAndDestroy( data );
            }
        
        CleanupStack::PopAndDestroy( &file );
        }
    
    CleanupStack::PopAndDestroy( &fileSession );
    CleanupStack::PopAndDestroy( name );
    
    _DBG_FILE("CNSmlDmDevDetailAdapter::GetDevDetailDataL(): end (WINSCW)");
    }


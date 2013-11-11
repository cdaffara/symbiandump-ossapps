/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common utility methods.
*
*/


_LIT( KPSDatabaseFileName, "pspresets.db" );            // File name of the preset database.

// ---------------------------------------------------------------------------
// Gets the full file name and path to the preset database file.
// ---------------------------------------------------------------------------
//
inline void PSUtils::GetDatabaseFullNameL( TFileName& aFullName )
    {
    PSDEBUG( "PSUtils::GetDatabaseFullNameL( TFileName& ) - Enter" );

    TParsePtrC phoneMemoryRootPath( PathInfo::PhoneMemoryRootPath() );
    TPtrC drive( phoneMemoryRootPath.Drive() );

    TParse parse;

    // The database file resides in the server's private directory from 3.2 onwards.
    TFileName privatePath;
    
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );
    
    User::LeaveIfError( fsSession.PrivatePath( privatePath ) );
    parse.Set( privatePath, &KPSDatabaseFileName, &drive );

    CleanupStack::PopAndDestroy( &fsSession );

    aFullName = parse.FullName();

    PSDEBUG2( "PSUtils::GetDatabaseFullNameL( aFullName = %S ) - Exit", &aFullName );
    }

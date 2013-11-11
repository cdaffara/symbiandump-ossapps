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
* Description:  HtiFileHlp implementation. This exe is used for file operations
*                to TCB folders (requiring capability ALL).
*
*/


// INCLUDE FILES
#include "../../symbian_version.hrh"
#include <e32std.h>
#include <swi/sisregistrysession.h>

// CONSTANTS
_LIT( KTcbHlpName, "HtiTcbHlp" );

_LIT( KCmdAppRegInfo,           "AppRegInfo");
_LIT( KCmdAppRegInfo_Add,       "add" );
_LIT( KCmdAppRegInfo_Remove,    "remove" );

_LIT( KDelimiter, "|" );
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ LOCAL FUNCTIONS ===============================

#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_4 )
LOCAL_C void HandleAppRegFileInfoL(const TDesC& aAppRegFile, TBool aAdded)
    {
    Swi::RSisRegistrySession regSession;
    User::LeaveIfError( regSession.Connect() );
    CleanupClosePushL( regSession );
    if(aAdded)
        {
        regSession.AddAppRegInfoL(aAppRegFile);
        }
    else
        {
        regSession.RemoveAppRegInfoL(aAppRegFile);
        }
    CleanupStack::PopAndDestroy(1);
    }
#endif

LOCAL_C TInt StartL()
    {
    TInt cmdLen = User::CommandLineLength();

    HBufC* cmdLine = HBufC::NewLC( cmdLen );
    TPtr ptCmdLine = cmdLine->Des();
    User::CommandLine( ptCmdLine );

    TInt paramStart = 0;
    TInt paramEnd = 0;

    // Take first parameter (the command)
    paramEnd = cmdLine->Find( KDelimiter );
    if ( paramEnd <= paramStart )
        {
        User::Leave( KErrArgument );
        }
    TPtrC cmd = cmdLine->Mid( paramStart, paramEnd - paramStart );
    
    if(cmd == KCmdAppRegInfo)
        {
#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_4 )
        // Take the next parameter either until next delimiter or
        // the rest of the command line.
        paramStart = paramEnd + 1;
        paramEnd = cmdLine->Mid( paramStart ).Find( KDelimiter ) + paramStart;
        if ( paramEnd < paramStart )
            {
            // No delimiter found - this is the last parameter
            paramEnd = cmdLen;
            }
    
        TPtrC param1 = cmdLine->Mid( paramStart, paramEnd - paramStart );
    
        paramStart = paramEnd + 1;
        if ( paramStart >= cmdLen )
            {
            User::Leave( KErrArgument );
            }
    
        if ( param1 == KCmdAppRegInfo_Add )
            {
            TPtrC param2 = cmdLine->Mid( paramStart );
            HandleAppRegFileInfoL( param2, ETrue );
            }
        else if ( param1 == KCmdAppRegInfo_Remove )
            {
            TPtrC param2 = cmdLine->Mid( paramStart );
            HandleAppRegFileInfoL( param2, EFalse );
            }
        else
            {
            User::Leave( KErrArgument );
            }
#endif
        }
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::PopAndDestroy(); // cmdLine

    return KErrNone;
    }

GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
    CActiveScheduler::Install( scheduler );

    User::RenameThread( KTcbHlpName );

    TRAPD( error, StartL() );

    delete scheduler;
    delete cleanup;
    __UHEAP_MARKEND;

    //__ASSERT_ALWAYS( !error, User::Panic( KTcbHlpName, error ) );
    return error;
    }


// End of File

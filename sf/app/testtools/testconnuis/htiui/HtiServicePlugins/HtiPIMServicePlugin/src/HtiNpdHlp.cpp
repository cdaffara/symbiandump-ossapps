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
* Description:  HtiNpdHlp implementation.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <NpdApi.h>
#include <eikenv.h>


#ifdef __ENABLE_LOGGING__

#include <flogger.h>
_LIT( KLogFolder, "hti" );
_LIT( KLogFile,   "HtiNpdHlp.txt" );

#define HTI_LOG_TEXT(a1) {_LIT(temp, a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#define HTI_LOG_DES(a1) {RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, a1);}
#define HTI_LOG_FORMAT(a1,a2) {_LIT(temp, a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, temp, (a2));}

_LIT8(KFuncIn, "-=> %S");
_LIT8(KFuncOut, "<=- %S");

#define HTI_LOG_FUNC_IN(a1) {_LIT8(temp, a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, KFuncIn, &temp);}
#define HTI_LOG_FUNC_OUT(a1) {_LIT8(temp, a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, KFuncOut, &temp);}

#else   // !__ENABLE_LOGGING__

#define HTI_LOG_TEXT(a1)
#define HTI_LOG_DES(a1)
#define HTI_LOG_FORMAT(a1,a2)
#define HTI_LOG_FUNC_IN(a1)
#define HTI_LOG_FUNC_OUT(a1)

#endif // __ENABLE_LOGGING__

// CONSTANTS
_LIT( KHtiNpdHlp, "HtiNpdHlp" );

_LIT( KCmdAddMemo,         "AddMemo" );
_LIT( KCmdAddMemoFromFile, "AddMemoFromFile" );
_LIT( KCmdDeleteAll,       "DeleteAll" );

_LIT( KNotepadDatabaseFile, "c:\\Private\\100012a5\\DBS_101F8878_Notepad.dat" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ LOCAL FUNCTIONS ===============================
void HandleAddMemoL( TLex& aParser )
    {
    HTI_LOG_FUNC_IN( "HandleAddMemoL" );

    aParser.SkipSpace();
    TPtrC text = aParser.Remainder();

    if ( text.Length() == 0 )
        {
        HTI_LOG_TEXT( "missing text arg" );
        User::Leave(KErrArgument);
        }

    // NOTE: We need to create CCoeEnv to make CNotepadApi work.
    // This also creates a new cleaupstack so we need to destory
    // CCoeEnv's cleanupstack before leaving to make our cleanupstack
    // the topmost stack

    CCoeEnv* env = new CCoeEnv;
    TRAPD( err, env->ConstructL() );
    if ( err )
        {
        HTI_LOG_FORMAT( "CEikonEnv::ConstructL err %d", err );
        User::Leave( err );
        }

    TRAP( err, CNotepadApi::AddContentL( text ) );
    if ( err )
        {
        HTI_LOG_FORMAT( "CNotepadApi::AddContentL err %d", err );
        env->DestroyEnvironment();
        User::Leave( err );
        }

    env->DestroyEnvironment();

    HTI_LOG_FUNC_OUT( "HandleAddMemoL" );
    }


void HandleAddMemoFromFileL( TLex& aParser )
    {
    HTI_LOG_FUNC_IN( "HandleAddMemoFromFileL" );

    aParser.SkipSpaceAndMark();
    aParser.SkipCharacters();
    if ( aParser.TokenLength() <= 0 )
        {
        User::Leave(KErrArgument);
        }

    TPtrC filename = aParser.MarkedToken();

    /*
    NOTE: Maybe will add this later

    aParser.SkipSpaceAndMark();
    aParser.SkipCharacters();

    TPtrC encoding = KNullDesC;
    if ( aParser.TokenLength() > 0 )
        {
        encoding.Set( aParser.MarkedToken() );
        }
    */

    CCoeEnv* env = new CCoeEnv;
    TRAPD( err, env->ConstructL() );
    if ( err )
        {
        HTI_LOG_FORMAT( "CEikonEnv::ConstructL err %d", err );
        User::Leave( err );
        }

    TRAP( err, CNotepadApi::SaveFileAsMemoL( filename ) );
    if ( err )
        {
        HTI_LOG_FORMAT( "CNotepadApi::SaveFileAsMemoL err %d", err );
        env->DestroyEnvironment();
        User::Leave( err );
        }

    env->DestroyEnvironment();

    HTI_LOG_FUNC_OUT( "HandleAddMemoFromFileL" );
    }

void HandleDeleteAllL()
    {
    HTI_LOG_FUNC_IN( "HandleDeleteAllL" );
    TInt err = KErrNone;
    RFs fs;
    err = fs.Connect();
    if ( err == KErrNone )
        {
        err = fs.Delete( KNotepadDatabaseFile );
        fs.Close();
        }
    if ( err != KErrNone && err != KErrNotFound )
        {
        HTI_LOG_FORMAT( "Notepad database delete failed %d", err );
        User::Leave( err );
        }
    HTI_LOG_FUNC_OUT( "HandleDeleteAllL" );
    }


LOCAL_C TInt StartL()
    {
    HTI_LOG_FUNC_IN( "StartL" );
    TInt cmdLen = User::CommandLineLength();
    HBufC* cmdLine = HBufC::NewLC( cmdLen );
    TPtr ptCmdLine = cmdLine->Des();
    User::CommandLine( ptCmdLine );

    HTI_LOG_DES( *cmdLine );

    //parse command line
    TLex parser( *cmdLine );
    parser.SkipCharacters();

    if ( parser.TokenLength() <= 0 )
        {
        HTI_LOG_TEXT( "missing arguments" );
        User::Leave( KErrArgument );
        }


    TPtrC cmd = parser.MarkedToken();

    if ( cmd == KCmdAddMemo )
        {
        HandleAddMemoL( parser );
        }
    else if ( cmd == KCmdAddMemoFromFile )
        {
        HandleAddMemoFromFileL( parser );
        }
    else if ( cmd == KCmdDeleteAll )
        {
        HandleDeleteAllL();
        }
    else
        {
        HTI_LOG_TEXT( "Unknown argument" );
        User::Leave( KErrArgument );
        }

    CleanupStack::PopAndDestroy(); // cmdLine
    HTI_LOG_FUNC_OUT( "StartL" );
    return KErrNone;
    }


GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    User::RenameThread( KHtiNpdHlp );
    TRAPD( err, StartL() );
    delete cleanup;
   __UHEAP_MARKEND;
    return err;
    }


// End of File

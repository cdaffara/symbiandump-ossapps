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
* Description:  HtiDeviceReboot implementation.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <hal.h>
#include <starterclient.h>
#include <syslangutil.h>

// CONSTANTS
_LIT( KHtiDeviceRebootName, "HtiDeviceReboot" );
_LIT( KParamNormalRfs, "rfsnormal" );
_LIT( KParamDeepRfs,   "rfsdeep" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
LOCAL_C TInt StartL();
LOCAL_C TInt Reboot(RStarterSession::TResetReason aReason);
LOCAL_C TInt RestoreFactorySettings( TBool aIsDeepRfs );


// FORWARD DECLARATIONS

// ============================ LOCAL FUNCTIONS ===============================

LOCAL_C TInt StartL()
    {
    TInt error = KErrNone;

    TInt cmdLen = User::CommandLineLength();
    HBufC* cmdLine = HBufC::NewLC( cmdLen );
    TPtr ptCmdLine = cmdLine->Des();
    User::CommandLine( ptCmdLine );

    if ( cmdLen == 0 )
        {
        error = Reboot(RStarterSession::EUnknownReset);
        }

    else
        {
        TLex parser( *cmdLine );
        TInt input;
        TInt ret = parser.Val(input);
        
        parser.SkipCharacters();
        if ( parser.MarkedToken().CompareF( KParamNormalRfs ) == 0 )
            {
            error = RestoreFactorySettings( EFalse );
            }
        else if ( parser.MarkedToken().CompareF( KParamDeepRfs ) == 0 )
            {
            error = RestoreFactorySettings( ETrue );
            }
        else
            {
            if(ret != KErrNone )
                {
                error = KErrArgument;  // Unknown argument
                }
            else
                {
                error = Reboot(static_cast<RStarterSession::TResetReason>(input));
                }
            }
        }

    CleanupStack::PopAndDestroy(); // cmdLine
    return error;
    }

LOCAL_C TInt Reboot(RStarterSession::TResetReason aReason)
    {
    RStarterSession session;
    TInt error = session.Connect();
    if ( error == KErrNone )
        {
        session.Reset( aReason );
        session.Close();
        }
    return error;
    }

LOCAL_C TInt RestoreFactorySettings( TBool aIsDeepRfs )
    {
    // In case of deep rfs, set language to default
    if ( aIsDeepRfs )
        {
        TInt lang = 0;
        if ( SysLangUtil::GetDefaultLanguage( lang ) == KErrNone )
            {
            HAL::Set( HALData::ELanguageIndex, lang );
            }
        }

    // Do reboot with appropriate rfs reason code
    RStarterSession session;
    TInt error = session.Connect();
    if ( error == KErrNone )
        {
        session.Reset( aIsDeepRfs ? RStarterSession::EDeepRFSReset :
            RStarterSession::ENormalRFSReset );
        session.Close();
        }
    return error;
    }

GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler *scheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install( scheduler );

    User::RenameThread( KHtiDeviceRebootName );

    TRAPD( err, StartL() );

    delete scheduler;
    delete cleanup;

   __UHEAP_MARKEND;

    return err;
    }

// End of File

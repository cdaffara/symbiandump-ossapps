/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MMSConformanceAPITestBlocks implementation*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MMSConformanceAPITest.h"
#include <MMSConformance.h>
#include <MsgMediaResolver.h>
#include <MsgMimeTypes.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS
#define NO_VERSION 0
// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMMSConformanceAPITest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMMSConformanceAPITest::Delete() 
    {
    if( iMmsConformance )
        {
        delete iMmsConformance;
        iMmsConformance = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMMSConformanceAPITest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMMSConformanceAPITest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 

        //ADD NEW ENTRY HERE
        ENTRY( "NewL", CMMSConformanceAPITest::TestMmsConfNewL ),
        ENTRY( "NewLC", CMMSConformanceAPITest::TestMmsConfNewLC ),
        ENTRY( "IsConformantMime", CMMSConformanceAPITest::TestIsConformantMimeL ),
        ENTRY( "MediaConformance", CMMSConformanceAPITest::TestMediaConformanceL ),
        ENTRY( "ConformanceVersionL", CMMSConformanceAPITest::TestConformanceVersion ),                
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CMMSConformanceAPITest::TestMmsConfNewL
// Tests NewL of CMMSConformance
// -----------------------------------------------------------------------------
//
TInt CMMSConformanceAPITest::TestMmsConfNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT(KTestFunction,"MmsConformanceNewL");
    iLog->Log( KTestFunction );    
    if( iMmsConformance )
        {
        return KErrNone;
        }
    else
        {
        return KErrCancel;
        }
    }

// -----------------------------------------------------------------------------
// CMMSConformanceAPITest::TestMmsConfNewLC
// Tests NewLC of CMMSConformance
// -----------------------------------------------------------------------------
//
TInt CMMSConformanceAPITest::TestMmsConfNewLC( CStifItemParser& /*aItem*/ )
    {
    _LIT(KTestFunction,"MmsConformanceNewLC");
    iLog->Log( KTestFunction );    
    CMmsConformance* mmsConfornace = CMmsConformance::NewLC();
    CleanupStack::PopAndDestroy(mmsConfornace);    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMMSConformanceAPITest::TestIsConformantMimeL
// Tests IsConformantMimeL of CMMSConformance
// -----------------------------------------------------------------------------
//
TInt CMMSConformanceAPITest::TestIsConformantMimeL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"IsConformantMime");
    iLog->Log( KTestFunction ); 
    
    if( InitDataL( aItem ) == KErrNone )
        {
        if ( iMmsConformance->IsConformantMime( iMediaInfo->MimeType()) )
            {
            iLog->Log( _L("Given file is MMSConformant") );
            }
        else
            {
            iLog->Log( _L("Given file is non MMSConformant") );
            }
        CleanUp();
        return KErrNone;
        }
    else
        {
        return KErrCancel;
        }
    }

// -----------------------------------------------------------------------------
// CMMSConformanceAPITest::TestMediaConformanceL
// Tests MediaConformance of CMMSConformance.
// -----------------------------------------------------------------------------
//
TInt CMMSConformanceAPITest::TestMediaConformanceL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"MediaConformance");
    iLog->Log( KTestFunction );
    if ( InitDataL( aItem ) == KErrNone )
        {
        TMmsConformance conformance = iMmsConformance->MediaConformance(*iMediaInfo);
        CleanUp();
        return KErrNone;
        }
    else
        {
        return KErrCancel;
        }
    }

// -----------------------------------------------------------------------------
// CMMSConformanceAPITest::TestConformanceVersion
// Tests ConformanceVersionL of CMmsConformance 
// -----------------------------------------------------------------------------
//
TInt CMMSConformanceAPITest::TestConformanceVersion( CStifItemParser& /*aItem*/ )
    {
    _LIT(KTestFunction,"ConformanceVersionL");
    iLog->Log( KTestFunction );     
    TUint8 version = NO_VERSION;
    TRAPD(err, version = iMmsConformance->ConformanceVersionL());
    if( err != KErrNone )
        {
        _LIT( KError, "ConformanceVersionL Leave error: %d");
        iLog->Log(KError, err); 
        return err;
        }
    _LIT(KVersion,"ConformanceVersion: %d");
    iLog->Log(KVersion, version);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMMSConformanceAPITest::InitDataL
// Does Required initializations like creating file handle and mediainfo object
// -----------------------------------------------------------------------------
//
TInt CMMSConformanceAPITest::InitDataL( CStifItemParser& aItem )
    {
    TPtrC string;
    CMsgMediaResolver* mediaResolver = NULL;
    RFile fileHandle;
    // file present in c: or e: \Testdata folder
    
    if ( aItem.GetNextString ( string ) != KErrNone )
        {
        iLog->Log( _L("Inside helper function - Parameter Not Provided"));
        return KErrCancel;
        }
    else
        {
        TBuf16<256> fileName;
        /*Start:-Added For Hard Code Path Elimanation*/
        #ifdef __WINSCW__
        fileName.Format( KTestFilePath, &string );
        #else
        TPtrC attachfilepath;
        aItem.GetNextString(attachfilepath);
        TDesC attachfiledesc(attachfilepath);
        fileName.Format( attachfilepath, &string );
        #endif
        /*End:-Added For Hard Code Path Elimanation*/
        mediaResolver = CMsgMediaResolver::NewL();
        TRAPD(err , fileHandle = mediaResolver->FileHandleL(fileName))
        if ( err != KErrNone )
            {
            if ( err == KErrNotFound )
                {
                iLog->Log( _L("Inside helper function -FileNotFound in TestPath"));
                }
            else
                {
                _LIT( KError, "Inside helper function -Returned error : %d" );
                iLog->Log( KError, err);
                }
            return err;
            }
        }
    CleanupClosePushL( fileHandle );

    TDataType mimeType;    
    mediaResolver->RecognizeL(fileHandle, mimeType);
    TMsgMediaType mediaType = mediaResolver->MediaType( mimeType.Des8());
    if( !iMediaInfo )
        {
        //Create mediaInfo object
        iMediaInfo = mediaResolver->CreateMediaInfoL( fileHandle );
        //iMediaInfo = CMsgMediaInfo::NewL(fileHandle, mimeType, mediaType);
        iMediaInfo->ParseInfoDetails( fileHandle, *( mediaResolver->DRMHelper() ), *mediaResolver);
        }
    CleanupStack::PopAndDestroy( &fileHandle );
    delete mediaResolver;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMMSConformanceAPITest::CleanUp
// Does cleanup , removes filehandle and deletes mediaInfo object
// -----------------------------------------------------------------------------
//
void CMMSConformanceAPITest::CleanUp()
    {
    if( iMediaInfo )
        {
        delete iMediaInfo;
        iMediaInfo = NULL;
        }
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove

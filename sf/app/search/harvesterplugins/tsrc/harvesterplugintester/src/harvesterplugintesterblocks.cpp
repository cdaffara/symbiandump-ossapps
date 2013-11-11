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
* Description: This file contains testclass implementation.
*
*/

// change the level of RVCT compiler warning 'non-POD class passed thru ellipses' to a remark.
#ifdef __MARM__
#pragma diag_remark 1446
#endif

// [INCLUDE FILES] - do not remove
#include "harvesterplugintester.h"
#include <e32svr.h>
#include <f32file.h> 
#include <stifparser.h>
#include <stiftestinterface.h>
#include <favouritesdb.h>
#include <msvapi.h>
#include <bautils.h>
#include <csearchdocument.h>
#include "harvesterobserver.h"
#include "cmessageplugin.h"
#include "cmessagemonitor.h"
#include "cmessageharvester.h"
#include "cmessagesessionobserver.h" //For CMsvSession
#include "ccpixsearcher.h"
#include "bookmarksplugin.h"
#include "applicationsplugin.h"
#include "ccontactsplugin.h"
#include "ccalendarplugin.h"
#include "ccalendarobserver.h"
#include "mediaplugin.h"
#include <harvesterclient.h>
#include "mdsitementity.h"
#include "cpixmdedbmanager.h"
#include "cpixindexerutils.h"
#include "cfolderrenamedharvester.h"
#include "cfilemonitor.h"
#include "cfileharvester.h"
#include "cmmcmonitor.h"
#include "videoplugin.h"
#include "imageplugin.h"
#include "cemailplugin.h"
#include "cmessagedatahandler.h"
#include <common.h>

_LIT(KAppBasePath,"@c:root file content");
_LIT(KEmailAppBasePath,"@c:root msg email");
_LIT(KAppBaseFolderFilePath,"@c:root file folder");
const TInt KMsgPluginBaseAppClassMaxLen = 64;

// For Notes --Start
#include <d32dbms.h>
#include <utf.h>
#include "notesplugin.h"

_LIT( KemailId, "1" );
_LIT(KMsgBaseAppClassGeneric, "root msg email");
#define MEDIA_QBASEAPPCLASS   "@c:root media audio"
#define LMEDIA_QBASEAPPCLASS  L"@c:root media audio"
#define VIDEO_QBASEAPPCLASS   "@c:root media video"
#define LVIDEO_QBASEAPPCLASS  L"@c:root media video"
#define IMAGE_QBASEAPPCLASS   "@c:root media image"
#define LIMAGE_QBASEAPPCLASS  L"@c:root media image"

// For Notes --End

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
void CHarvesterPluginTester::Delete() 
    {
    }

// -----------------------------------------------------------------------------
// Run specified method. Contains also table of test mothods and their names.
TInt CHarvesterPluginTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TestStartHarvester", CHarvesterPluginTester::TestStartHarvesterL ),        
        ENTRY( "TestFormBaseAppClass", CHarvesterPluginTester::TestFormBaseAppClassL ),
        ENTRY( "TestDatabasePath", CHarvesterPluginTester::TestDatabasePathL ),
        ENTRY( "TestCreateIndexItemL_Add", CHarvesterPluginTester::TestCreateIndexItemL ),
        ENTRY( "TestCreateIndexItemL_Update", CHarvesterPluginTester::TestCreateIndexItemL ),
        ENTRY( "TestCreateIndexItemL_Delete", CHarvesterPluginTester::TestCreateIndexItemL ),
        ENTRY( "TestFolderCreate", CHarvesterPluginTester::TestFolderCreateL ),
        ENTRY( "TestFolderRename", CHarvesterPluginTester::TestFolderRenameL ),
        ENTRY( "TestFileRunError", CHarvesterPluginTester::TestFileRunErrorL ),
        ENTRY( "TestFileDoCancel", CHarvesterPluginTester::TestFileDoCancelL ),        
        ENTRY( "TestHandleFileEngine", CHarvesterPluginTester::TestHandleFileEngineL ),
        ENTRY( "TestFileMonitorRunL", CHarvesterPluginTester::TestFileMonitorRunL ),
        ENTRY( "TestFileMmcMonitorRunL", CHarvesterPluginTester::TestFileMmcMonitorRunL ),
        ENTRY( "TestFileBaseAppNegative", CHarvesterPluginTester::TestFileBaseAppNegativeL ),        
        ENTRY( "TestMessaging", CHarvesterPluginTester::TestMessageHarvesterL ),
        ENTRY( "TestMessageHarvesting", CHarvesterPluginTester::TestMessageHarvesterWithMessageL ),
        ENTRY( "TestMessageDriveChange", CHarvesterPluginTester::TestMessageHarvesterChangeDriveL ),
        ENTRY( "TestMessageRunErrorL", CHarvesterPluginTester::TestMessageRunErrorL ),
        ENTRY( "TestMessageIsMediaRemovableL", CHarvesterPluginTester::TestMessageIsMediaRemovableL ),
        ENTRY( "TestMessageMountNoIndexerL", CHarvesterPluginTester::TestMessageMountNoIndexerL ),        
        ENTRY( "TestQualifiedBaseAppClass", CHarvesterPluginTester::TestQualifiedBaseAppClassL ),
        ENTRY( "TestHandleMsgMoved", CHarvesterPluginTester::TestHandleMsgMovedL ),
        ENTRY( "TestMessageDataHandler", CHarvesterPluginTester::TestMessageDataHandlerL ),
        ENTRY( "TestMsgUnMount", CHarvesterPluginTester::TestMsgUnMountL ),
        ENTRY( "TestFormBaseAppClass", CHarvesterPluginTester::TestFormFormBaseAppClassL ),
        ENTRY( "TestMsgDelete", CHarvesterPluginTester::TestMsgDeleteL ),
        ENTRY( "TestMsgHandlesession", CHarvesterPluginTester::TestMsgHandlesessionL ),
        ENTRY( "TestMsgHandlesessionPanic", CHarvesterPluginTester::TestMsgHandlesessionPanicL ),
        ENTRY( "TestMsgOverwriteOrAddToQuere", CHarvesterPluginTester::TestMsgOverwriteOrAddToQuereL ),
        ENTRY( "TestMsgMessageHarvester", CHarvesterPluginTester::TestMsgMessageHarvesterL ),
        ENTRY( "TestStartBookmarksHarvesterL", CHarvesterPluginTester::TestStartBookmarksHarvesterL ),
        ENTRY( "TestAddBookmarkL", CHarvesterPluginTester::TestAddBookmarkL ),
        ENTRY( "TestAddGetDomainL", CHarvesterPluginTester::TestAddGetDomainL ),
        ENTRY( "TestStartApplicationsHarvesterL", CHarvesterPluginTester::TestStartApplicationsHarvesterL ),        
        ENTRY( "TestCreateApplicationsIndexItemL", CHarvesterPluginTester::TestCreateApplicationsIndexItemL ),
		ENTRY( "TestStartNotesHarvesterL", CHarvesterPluginTester::TestStartNotesHarvesterL ),
        ENTRY( "TestAddNoteL", CHarvesterPluginTester::TestAddNoteL ),
        ENTRY( "TestAddLongNoteL", CHarvesterPluginTester::TestAddLongNoteL ),
        ENTRY( "TestAddAlphaNumericNoteL", CHarvesterPluginTester::TestAddAlphaNumericNoteL ),
        ENTRY( "TestDeleteNoteL", CHarvesterPluginTester::TestDeleteNoteL ),
        ENTRY( "TestUpdateNoteL", CHarvesterPluginTester::TestUpdateNoteL ),
        ENTRY( "TestNotesEntryNegative", CHarvesterPluginTester::TestNotesEntryNegativeL ),
        ENTRY( "TestNotesDestructor", CHarvesterPluginTester::TestNotesDestructorL ),
        ENTRY( "TestNotesDelayedError", CHarvesterPluginTester::TestNotesDelayedErrorL ),
        ENTRY( "TestNotesChangeEntry", CHarvesterPluginTester::TestNotesChangeEntryL),
        ENTRY( "TestNotesEntryNoIndexer", CHarvesterPluginTester::TestNotesEntryNoIndexerL),
        ENTRY( "TestContactsHarvesting", CHarvesterPluginTester::TestStartContactsHarvesterL ),
        ENTRY( "TestCreateContactIndexItemL_Add", CHarvesterPluginTester::TestCreateContactIndexItemL ),
        ENTRY( "TestCreateAllContactFields", CHarvesterPluginTester::TestCreateAllContactFieldsL ),
        ENTRY( "TestCreateContactIndexItemL_Edit", CHarvesterPluginTester::TestCreateContactIndexItemL ),
        ENTRY( "TestCreateContactIndexItemL_Delete", CHarvesterPluginTester::TestCreateContactIndexItemL ),
        ENTRY( "TestCreateContactIndexItemL_NoIndexer", CHarvesterPluginTester::TestCreateContactIndexItemNoIndexerL ),
        ENTRY( "TestContactGetDateL", CHarvesterPluginTester::TestContactGetDateL ),
        ENTRY( "TestContactDelayedErrorL", CHarvesterPluginTester::TestContactDelayedErrorL ),
        ENTRY( "TestContactDelayedCallbackNoIContactsL", CHarvesterPluginTester::TestContactDelayedCallbackNoIContactsL ),        
        ENTRY( "TestContactDelayedCallbackNoIObserverL", CHarvesterPluginTester::TestContactDelayedCallbackNoIObserverL ),        
        ENTRY( "TestContactDelayedCallbackWrongCountL", CHarvesterPluginTester::TestContactDelayedCallbackWrongCountL ),        
        ENTRY( "TestCreateContactGroup", CHarvesterPluginTester::TestCreateContactGroupL ),
        ENTRY( "TestCalenderHarvesting", CHarvesterPluginTester::TestStartCalenderHarvesterL ),
        ENTRY( "TestCalenderEntry",CHarvesterPluginTester::TestCalenderEntryL ),
        ENTRY( "TestCreateMMS",CHarvesterPluginTester::TestCreateMmsL ),        
        ENTRY( "Testcalenderdestructor",CHarvesterPluginTester::TestcalenderdestructorL ),
        ENTRY( "TestCalChangeEntry",CHarvesterPluginTester::TestCalChangeEntryL ),
        ENTRY( "TestCalCreateEntry",CHarvesterPluginTester::TestCalCreateEntryL ),
        ENTRY( "TestCreateEmail",CHarvesterPluginTester::TestCreateEmailL ),
		ENTRY( "TestAudioHarvesting",CHarvesterPluginTester::TestAudioHarvestingL ),        
		ENTRY( "TestMdsSyncController",CHarvesterPluginTester::TestMdsSyncControllerL ),
		ENTRY( "TestAudioHandleItem",CHarvesterPluginTester::TestAudioHandleItemL ),
		ENTRY( "TestAudioSyncDbManager",CHarvesterPluginTester::TestAudioSyncDbManagerL ),
        ENTRY( "TestAudioMMCEventL",CHarvesterPluginTester::TestAudioMMCEventL ),
        ENTRY( "TestAudioNoIndexer",CHarvesterPluginTester::TestAudioNoIndexerL ),
		//ENTRY( "TestBlacklistPlugin",CHarvesterPluginTester::TestBlacklistPluginL ),
		//ENTRY( "TestBlacklistPluginVersion",CHarvesterPluginTester::TestBlacklistPluginVersionL ),
		ENTRY( "TestVideoHarvesting",CHarvesterPluginTester::TestVideoHarvestingL ),		
        ENTRY( "TestVideoHandleItem",CHarvesterPluginTester::TestVideoHandleItemL ),
		ENTRY( "TestVideoSyncDbManager",CHarvesterPluginTester::TestVideoSyncDbManagerL ),
        ENTRY( "TestVideoMMCEventL",CHarvesterPluginTester::TestVideoMMCEventL ),
        ENTRY( "TestVideoNoIndexer",CHarvesterPluginTester::TestVideoNoIndexerL ),
		ENTRY( "TestImageHarvesting",CHarvesterPluginTester::TestImageHarvestingL ),				
		ENTRY( "TestImageHandleItem",CHarvesterPluginTester::TestImageHandleItemL ),
		ENTRY( "TestImageSyncDbManager",CHarvesterPluginTester::TestImageSyncDbManagerL ),
		ENTRY( "TestImageMMCEventL",CHarvesterPluginTester::TestImageMMCEventL ),
		ENTRY( "TestImageNoIndexer",CHarvesterPluginTester::TestImageNoIndexerL ),
		ENTRY( "TestStartEmailPlugin",CHarvesterPluginTester::TestStartEmailPluginL ),
		ENTRY( "TestHandleEmailDoc",CHarvesterPluginTester::TestHandleEmailDocL ),
		ENTRY( "TestEmailBaseappclass",CHarvesterPluginTester::TestEmailBaseappclassL ),
		ENTRY( "TestEmailHandleDoc",CHarvesterPluginTester::TestEmailHandleDocL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

_LIT( KNoErrorString, "No Error" );
//_LIT( KErrorString, " *** Error ***" );

void doLog( CStifLogger* logger, TInt error, const TDesC& errorString )
    {
    if( KErrNone == error ) logger->Log( KNoErrorString );
    else logger->Log( errorString );
    }

CSearchDocument* CHarvesterPluginTester::prepareemaildocument()
    {    
    CSearchDocument* doc = CSearchDocument::NewL( KemailId, 
            KMsgBaseAppClassGeneric ); 
    return doc;
    }
// Example test method function.
// -----------------------------------------------------------------------------
TInt CHarvesterPluginTester::TestStartHarvesterL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester: %S" );
    _LIT( KExample, "In TestStartHarvesterL" );
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );

    // Print to log file
    iLog->Log( KExample );
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    filePlugin->StartPluginL(); //Calls Add
    filePlugin->StartHarvestingL( KAppBasePath );
    iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, KErrNone, KNoErrorString );
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestFormBaseAppClassL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KFilePluginBaseAppClass, "FormBaseAppClass" );
    _LIT( KExample, "In TestFormBaseAppClass" );
    
    TestModuleIf().Printf( 0, KFilePluginBaseAppClass, KExample );
    
    TInt driveNumber;
    TBuf<100> formedBaseAppClass;
    TBuf<100> expectedBaseAppClass;
    TChar driveChar;
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    filePlugin->StartPluginL();

    TInt error = KErrNone;
    TInt errorNumber = 0;
    _LIT( KFileBaseAppClassFolder, "root file folder");
    _LIT( KBaseAppClassFormatString, "@%c:root file folder");
    _LIT( KTestFormBaseAppClassNoError, "TestFormBaseAppClass: No Error" );
    
    if( aItem.GetNextInt ( driveNumber ) == KErrNone  && error == KErrNone )
        {
        filePlugin->FormBaseAppClass(  static_cast<TDriveNumber>( driveNumber ),KFileBaseAppClassFolder, formedBaseAppClass );
        if( aItem.GetNextChar ( driveChar ) == KErrNone )
            {
            expectedBaseAppClass.Format( KBaseAppClassFormatString, driveChar );
            error = expectedBaseAppClass.Compare( formedBaseAppClass );
            if( aItem.GetNextInt ( errorNumber ) == KErrNone )
                error = ( KErrNone == error || -1 == errorNumber ) ? KErrNone : KErrGeneral ;
            }
        }
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, error, KTestFormBaseAppClassNoError );
    return error;
    }

TInt CHarvesterPluginTester::TestDatabasePathL( CStifItemParser& aItem )
    {
    TInt driveNumber;
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    filePlugin->StartPluginL();
    
    HBufC* buffer;
    TBuf<KMaxFileName> returnedDbPath;
    TBuf<KMaxFileName> expectedDbPath;
    TInt error = KErrNone;
    TInt errorNumber;
    TChar driveChar;
    _LIT( KTestDatabasePathNoError, "TestDatabasePathL: No Error" );
    _LIT(KPathFileContent, "\\root\\file\\content");
    _LIT( KDbFormatString, "%c:\\Private\\2001f6f7\\indexing\\indexdb\\root\\file\\content" );
    if( aItem.GetNextInt ( driveNumber ) == KErrNone  && error == KErrNone )
        {
        buffer = filePlugin->DatabasePathLC( static_cast<TDriveNumber>( driveNumber ), KPathFileContent );
        returnedDbPath.Copy( *buffer );
        if( aItem.GetNextChar ( driveChar ) == KErrNone )
            {
            expectedDbPath.Format( KDbFormatString, driveChar );
            error = expectedDbPath.Compare( returnedDbPath );
            if( aItem.GetNextInt ( errorNumber ) == KErrNone )
                error = ( KErrNone == error || -1 == errorNumber ) ? KErrNone : KErrGeneral ;
            }
        }
    CleanupStack::PopAndDestroy( buffer );
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, error, KTestDatabasePathNoError );
    return error;
    }

//Helper functions
enum TSearchType
    {
    ESearchTypeResultsExpected=0,
    ESearchTypeNoResultsExpected
    };

TInt doSearchL( const TDesC& aSearchString, const TDesC& aBaseAppClass, TSearchType aSearchType )
    {
    _LIT(KHeaderPrefix, "$prefix(\"");
    _LIT(KTrailerPrefix, "\")");
    RSearchServerSession session;
    User::LeaveIfError( session.Connect() );
    CCPixSearcher* searcher = CCPixSearcher::NewLC( session );
    
    //Form prefix search query
    searcher->OpenDatabaseL( aBaseAppClass );
    HBufC* searchbuf = HBufC::NewLC(KHeaderPrefix().Length() + aSearchString.Length() + KTrailerPrefix().Length());
    TPtr searchstring = searchbuf->Des();
    searchstring.Append(KHeaderPrefix);
    searchstring.Append(aSearchString);
    searchstring.Append(KTrailerPrefix);
    
    TInt docCount = searcher->SearchL( searchstring, KNullDesC);
    CleanupStack::PopAndDestroy( searchbuf );
    CleanupStack::PopAndDestroy( searcher );
    session.Close();
    
    if( aSearchType == ESearchTypeResultsExpected )
        {
        return docCount > 0 ? KErrNone : KErrNotFound;
        }
    else
        {
        return 0 == docCount ? KErrNone : KErrNotFound;
        }
    }

TInt CHarvesterPluginTester::TestCreateIndexItemL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    TPtrC fileName;
    TPtrC createIndexAction;
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* observer = CHarvesterObserver::NewL( filePlugin );
    filePlugin->StartPluginL();
    _LIT( KFileNameFormat, "C:\\Data\\" );
    _LIT( KActionAdd, "add" );
    _LIT( KActionDelete, "delete" );
    _LIT( KActionUpdate, "update" );
    _LIT( KCreateIndexItemNoError, "CreateIndexItemL: Error" );

    TBuf<KMaxFileName> filePathName( KFileNameFormat );
    
    if( aItem.GetNextString ( createIndexAction ) == KErrNone )
        {
        if( createIndexAction.Compare( KActionAdd ) == 0 )
            {
            if( aItem.GetNextString ( fileName ) == KErrNone  && error == KErrNone )
                {
                filePathName.Append( fileName );
                filePlugin->CreateContentIndexItemL( filePathName, ECPixAddAction );
                User::After( (TTimeIntervalMicroSeconds32)35000000 );
                TPtrC searchString;
                while( aItem.GetNextString ( searchString ) == KErrNone  && error == KErrNone )
                    {
                    error = doSearchL( searchString, KAppBasePath, ESearchTypeResultsExpected );
                    }
                filePlugin->CreateContentIndexItemL( filePathName, ECPixRemoveAction );
                }
            }
        if( createIndexAction.Compare( KActionDelete ) == 0 )
            {
            if( aItem.GetNextString ( fileName ) == KErrNone  && error == KErrNone )
                {
                filePathName.Append( fileName );
                filePlugin->CreateContentIndexItemL( filePathName, ECPixRemoveAction );
                TPtrC searchString;
                while( aItem.GetNextString ( searchString ) == KErrNone  && error == KErrNone )
                    {
                    error = doSearchL( searchString, KAppBasePath, ESearchTypeNoResultsExpected );
                    }
                }
            }
        if( createIndexAction.Compare( KActionUpdate ) == 0 )
            {
            if( aItem.GetNextString ( fileName ) == KErrNone  && error == KErrNone )
                {
                filePathName.Append( fileName );
                filePlugin->CreateContentIndexItemL( filePathName, ECPixUpdateAction );
                User::After( (TTimeIntervalMicroSeconds32)35000000 );
                TPtrC searchString;
                while( aItem.GetNextString ( searchString ) == KErrNone  && error == KErrNone )
                    {
                    error = doSearchL( searchString, KAppBasePath, ESearchTypeResultsExpected );
                    }
                }
            }
        }
    delete filePlugin;
    delete observer;
    doLog( iLog, error, KCreateIndexItemNoError );
    return error;
    }

TInt CHarvesterPluginTester::TestFolderCreateL( CStifItemParser& /*aItem*/ )
    {
    TInt error = KErrNone;
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    iPluginTester->SetWaitTime(2000000);
    filePlugin->StartPluginL();
    filePlugin->StartHarvestingL( KAppBasePath );
    
    _LIT( KDirectoryToCreate, "C:\\data\\TestFolder\\" );    
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    TBool folderExists = BaflUtils::FolderExists(fs, KDirectoryToCreate);
    if(!folderExists)
        {
        error = fs.MkDir(KDirectoryToCreate);
        }    
    //iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    TFastFindFSPStatus& status = filePlugin->iMonitor->iPckg();    
    status.iFileEventType = EFastFindDirCreated;
    status.iFileName.Copy(KDirectoryToCreate);    
    //Folder created event
    filePlugin->iMonitor->RunL();
    
    //Folder delete event
    status = filePlugin->iMonitor->iPckg();    
    status.iFileName.Copy(KDirectoryToCreate);
    status.iFileEventType = EFastFindDirDeleted;
    filePlugin->iMonitor->RunL();
    if(error == KErrNone)
        {
        error = doSearchL( _L("TestFolder"), KAppBaseFolderFilePath, ESearchTypeResultsExpected );
        fs.RmDir(KDirectoryToCreate);
        }
    fs.Close();
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, error, _L("Error: TestFolderCreateL") );
    
    return error;
    }

TInt CHarvesterPluginTester::TestFolderRenameL( CStifItemParser& /*aItem*/ )
    {
    TInt error = KErrNone;
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    
    CFilePlugin* filePlugin = CFilePlugin::NewL();    
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    
    filePlugin->StartPluginL();
    filePlugin->StartHarvestingL( KAppBasePath );    
    
    _LIT( KDirectoryToCreate, "C:\\data\\TestRenameFolder\\" );
    _LIT( KDirectoryRenamed, "C:\\data\\TestFolderRenamed\\" );
    
    TFileName oldFolderName(KDirectoryToCreate);
    TFileName newFolderName(KDirectoryRenamed);    
    
    if(!BaflUtils::FolderExists(fs, KDirectoryToCreate))
        {
        User::LeaveIfError(fs.MkDir(KDirectoryToCreate));        
        }
    if(BaflUtils::FolderExists(fs, KDirectoryRenamed))
        {
        User::LeaveIfError(fs.RmDir(KDirectoryRenamed));
        }
    filePlugin->CreateFolderFileIndexItemL( KDirectoryToCreate, ECPixAddAction );
    
    fs.Rename(KDirectoryToCreate, KDirectoryRenamed);
    filePlugin->iMonitor->iFolderRenamedHarvester->StartL( oldFolderName, newFolderName );
    //for code coverage
    filePlugin->iMonitor->iFolderRenamedHarvester->RunL();
    // To cover default case
    filePlugin->iMonitor->iFolderRenamedHarvester->iHarvestState = (CFolderRenamedHarvester::TFileHarvesterState)5;
    filePlugin->iMonitor->iFolderRenamedHarvester->RunL(); 
    //Search for the renamed directory
    error = doSearchL( _L("TestFolderRenamed"), KAppBaseFolderFilePath, ESearchTypeNoResultsExpected );    
    
    fs.RmDir(KDirectoryRenamed);    
    filePlugin->CreateFolderFileIndexItemL( KDirectoryRenamed, ECPixRemoveAction );
    delete filePlugin;
    
    delete iPluginTester;
    iPluginTester = NULL;
    fs.Close();
    doLog( iLog, error, _L("Error: TestFolderRenameL") );
    
    return error;
    }

TInt CHarvesterPluginTester::TestFileRunErrorL( CStifItemParser& /*aItem */)
    {
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    iPluginTester->SetWaitTime(2000000);
    filePlugin->StartPluginL();
    filePlugin->StartHarvestingL( KAppBasePath );
    filePlugin->iMonitor->iFolderRenamedHarvester->RunError(KErrNone);
    filePlugin->iMonitor->RunError( KErrNone );
    filePlugin->iMmcMonitor->RunError( KErrNone );
    filePlugin->iHarvester->RunError( KErrNone );
    
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestFileDoCancelL( CStifItemParser& /*aItem */)
    {
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    filePlugin->StartPluginL();     
    filePlugin->iHarvester->DoCancel();
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestHandleFileEngineL( CStifItemParser& /*aItem */)
    {
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    filePlugin->StartPluginL();
    filePlugin->iMonitor->Disable();
    filePlugin->iMonitor->Enable();    
    //filePlugin->iMonitor->Release();
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestFileMonitorRunL( CStifItemParser& /*aItem */)
    {
    _LIT( KOldFileName, "C:\\data\\testfiles.txt" );
    _LIT( KNewFileName, "C:\\data\\testfile.txt" );
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    filePlugin->StartPluginL();
    
    TFastFindFSPStatus& status = filePlugin->iMonitor->iPckg();    
    status.iFileEventType = EFastFindFileCreated;
    status.iFileName.Copy(KOldFileName);
    status.iNewFileName.Copy(KNewFileName);
    //File created event
    filePlugin->iMonitor->RunL(); 
    status = filePlugin->iMonitor->iPckg();    
    status.iFileName.Copy(KOldFileName);
    status.iNewFileName.Copy(KNewFileName);
    status.iFileEventType = EFastFindFileModified;
    //File Modified event
    filePlugin->iMonitor->RunL();
    
    status = filePlugin->iMonitor->iPckg();    
    status.iFileName.Copy(KOldFileName);
    status.iNewFileName.Copy(KNewFileName);
    status.iFileEventType = EFastFindFileRenamed;
    //Rename file event
    filePlugin->iMonitor->RunL();
    
    status = filePlugin->iMonitor->iPckg();    
    status.iFileName.Copy(KOldFileName);
    status.iNewFileName.Copy(KNewFileName);
    status.iFileEventType = EFastFindFileReplaced;
    //Replace file event
    filePlugin->iMonitor->RunL();
    
    status = filePlugin->iMonitor->iPckg();    
    status.iFileName.Copy(KOldFileName);
    status.iNewFileName.Copy(KNewFileName);
    status.iFileEventType = EFastFindFileDeleted;
    //Delete file event
    filePlugin->iMonitor->RunL();    
        
    status = filePlugin->iMonitor->iPckg();    
    status.iFileName.Copy(KOldFileName);
    status.iNewFileName.Copy(KNewFileName);
    status.iFileEventType = EFastFindFileUnknown;
    //For default test
    filePlugin->iMonitor->RunL();
    
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestFileMmcMonitorRunL( CStifItemParser& /*aItem */)
    {
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    iPluginTester->SetWaitTime(2000000);
    filePlugin->StartPluginL();
    filePlugin->StartHarvestingL( KAppBasePath );
    // cancel the subcribed request first
    filePlugin->iMmcMonitor->Cancel();
    filePlugin->iMmcMonitor->iProperty.Cancel(); 
    
    filePlugin->iMmcMonitor->RunL();
    
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;    
    }

TInt CHarvesterPluginTester::TestFileBaseAppNegativeL( CStifItemParser& /*aItem */)
    {
    _LIT(KIncorrectBaseApp,"c:root file content");
    CFilePlugin* filePlugin = CFilePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( filePlugin );
    filePlugin->StartPluginL();
    TRAPD(err,filePlugin->StartHarvestingL( KIncorrectBaseApp ));
    delete filePlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }
/**
* Message harvester test method.
* @since ?Series60_version
* @param aItem Script line containing parameters.
* @return Symbian OS error code.
*/
TInt CHarvesterPluginTester::TestMessageHarvesterL( CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester" );
    _LIT( KExample, "In TestMessageHarvesterL" );
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );
    // Print to log file
    iLog->Log( KExample );
    
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    CMessageSessionObserver* sessionobserver = CMessageSessionObserver::NewL();
    CMsvSession* msgSession = CMsvSession::OpenSyncL(*sessionobserver);
    // Form the baseappclass for this media
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    TInt drive = msgSession->CurrentDriveL();
    CMessagePlugin::FormBaseAppClass(TDriveNumber(drive),baseAppClass);
    //Get current base app class for drive
    //CMessagePlugin::FormBaseAppClass
    plugin->StartHarvestingL( baseAppClass );
    iPluginTester->iWaitForHarvester->Start();
    
    delete plugin; 
    delete iPluginTester;
    iPluginTester = NULL;
    delete sessionobserver;
    delete msgSession;
    doLog(iLog,KErrNone,KNoErrorString);
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestMessageHarvesterWithMessageL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester" );
    _LIT( KExample, "In TestMessageHarvesterWithMessageL" );
    TInt error(KErrNone);
    
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );
    // Print to log file
    iLog->Log( KExample );
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    CMessageSessionObserver* sessionobserver = CMessageSessionObserver::NewL();
    CMsvSession* msgSession = CMsvSession::OpenSyncL(*sessionobserver);
    // Form the baseappclass for this media
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    TInt drive = msgSession->CurrentDriveL();
    //Get current base app class for drive
    CMessagePlugin::FormBaseAppClass(TDriveNumber(drive),baseAppClass);
    plugin->StartHarvestingL( baseAppClass );
    //Create a new SMS message
    TMsvId msgid = MessagingUtils::CreateMessageL(msgSession,
                                                 _L("+3584400220055"),
                                                 _L("+919845062437"),
                                                 _L("This is test from bhuvnesh"));
    iPluginTester->iWaitForHarvester->Start();
    TInt count = SearchForTextL(_L("+919845062437"),baseAppClass,KNullDesC);
    MessagingUtils::RemoveEntryL(msgSession,msgid);
    if(count <= 0)
        {
        error = KErrNotFound;
        }
    
    delete plugin; 
    delete iPluginTester;
    iPluginTester = NULL;
    delete sessionobserver;
    delete msgSession;
    doLog(iLog,error,_L("Error in TestMessageHarvesterWithMessageL"));
    return error;
    }

TInt CHarvesterPluginTester::TestMessageHarvesterChangeDriveL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester" );
    _LIT( KExample, "In TestMessageHarvesterChangeDriveL" );
    TInt error(KErrNone);
    
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );
    // Print to log file
    iLog->Log( KExample );
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    CMessageSessionObserver* sessionobserver = CMessageSessionObserver::NewL();
    CMsvSession* msgSession = CMsvSession::OpenSyncL(*sessionobserver);
    // Form the baseappclass for this media
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    TInt drive = msgSession->CurrentDriveL();
    //Get current base app class for drive
    CMessagePlugin::FormBaseAppClass(TDriveNumber(drive),baseAppClass);
    plugin->StartHarvestingL( baseAppClass );
    iPluginTester->iWaitForHarvester->Start();
    //Harvesting completed change the drive and make sure new message harvested
    drive == EDriveC ? drive = EDriveE : drive = EDriveC;
    TRequestStatus status = KRequestPending;
    msgSession->ChangeDriveL(drive,status);
    User::WaitForAnyRequest();
    //User::WaitForRequest(status);//Wait to complete this request
    plugin->StartHarvestingL( baseAppClass );
    TMsvId msgid = MessagingUtils::CreateMessageL(msgSession,
                                                 _L("+3584400220055"),
                                                 _L("+919845062437"),
                                                 _L("This is test from bhuvnesh"));
    iPluginTester->iWaitForHarvester->Start();//Start again
    drive = msgSession->CurrentDriveL();
    CMessagePlugin::FormBaseAppClass(TDriveNumber(drive),baseAppClass);
    TInt count = SearchForTextL(_L("+919845062437"),baseAppClass,KNullDesC);
    if(count <= 0)
        {
        error = KErrNotFound;
        }
    MessagingUtils::RemoveEntryL(msgSession,msgid);
    
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    delete sessionobserver;
    delete msgSession;
    doLog(iLog,error,_L("Error in TestMessageHarvesterChangeDriveL"));
    return error;
    }

//Only for coverage.
TInt CHarvesterPluginTester::TestMessageRunErrorL( CStifItemParser& /*aItem */)
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    plugin->iMessageMonitor->RunError( KErrNone );
    delete plugin;
    return KErrNone;
    }
TInt CHarvesterPluginTester::TestQualifiedBaseAppClassL( CStifItemParser& )
    {
    //For coverage
    TDriveNumber drive ( EDriveA ); 
    _LIT( appcls1 ,"@c:root media image" );
    _LIT( appcls2 ,"@*:root media image" );
    _LIT( appcls3 ,"d:root media image" );    
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );    
    plugin->MountL( EDriveC );
    TRAPD(err, plugin->StartHarvestingL(appcls1));
    delete plugin->iIndexer[EDriveC];
    plugin->iIndexer[EDriveC] = NULL;
    TRAP(err, plugin->StartHarvestingL(appcls1));
    TRAP(err, plugin->StartHarvestingL(appcls2));
    TRAP(err, plugin->StartHarvestingL(appcls3));
    delete plugin;
    delete iPluginTester;
    return err;
    }
TInt CHarvesterPluginTester::TestMsgUnMountL( CStifItemParser& )
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->MountL(EDriveC);
    plugin->UnMount( EDriveC );
    plugin->MountL(EDriveC);
    plugin->UnMount( EDriveC , EFalse);
    delete plugin;
    delete iPluginTester;
    return KErrNone;
    }

TAny CHarvesterPluginTester::TestHandleMsgMovedL( CStifItemParser& )
    {
    const TMsvId id1 = 1;
    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    // Current entry will be the one to send
    selection->AppendL( id1 );
//    CMsvEntrySelection entry;
    const long int ref = 2;
    
    const TMsvId id2 = 2;
//    entry.AppendL(id1);
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    plugin->iMessageMonitor->HandleMsgMovedL(id1, id2, *selection);
    selection->Reset();
    delete plugin;
    }

TAny CHarvesterPluginTester::TestMessageDataHandlerL( CStifItemParser& )
    {
    CMessagePlugin* plugin3 = CMessagePlugin::NewL();
    _LIT8(Des1, "0xFFFFF");
    _LIT8(Des2, "0xF");
    _LIT8(Des3, "FFx0");
    _LIT8(Des4, "F");
    _LIT8(Des5, "0xFEFF");
    _LIT8(Des6, "0xFFFE");
    const TMsvId id1 = 1;
    const TMsvId id2 = 2;
    CMessageDataHandler* idatahandler = CMessageDataHandler::NewL( *plugin3, *(plugin3->iMsvSession) );
    idatahandler->CreateMessageIndexItemL(id1,ECPixAddAction,id1);
    idatahandler->IsTextUcs2(Des1);
    idatahandler->IsTextUcs2(Des2);
    idatahandler->IsTextUcs2(Des3);
    idatahandler->IsTextUcs2(Des4);
    idatahandler->IsTextUcs2(Des5);
    idatahandler->IsTextUcs2(Des6);
    const TInt KUtf8BomLength = 3;
    const TUint8 KUtf8Bom[KUtf8BomLength] = {0xEF, 0xBB, 0xBF};
    const TUint8 KUtf8Bom1[KUtf8BomLength+1] = {0xEF, 0x5, 0xF, 0xE};
    TPtrC8 ptr(KUtf8Bom, KUtf8BomLength);
    TPtrC8 ptr1(KUtf8Bom1, KUtf8BomLength+1); 
    idatahandler->TextContainsUtf8Bom(ptr);
    idatahandler->TextContainsUtf8Bom(ptr1);
    idatahandler->DoCancel();
    idatahandler->RunError(0);
    delete plugin3;

    }
TInt CHarvesterPluginTester::TestMessageMountNoIndexerL( CStifItemParser& )
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    RSearchServerSession session;
    User::LeaveIfError( session.Connect() );
    plugin->iIndexer[EDriveA] = CCPixIndexer::NewL( session );
    TRAPD( err, plugin->MountL( EDriveA, EFalse ) );
    TRAP( err, plugin->MountL( EDriveA, EFalse ) ); //For coverage
    delete plugin->iIndexer[EDriveA];
    plugin->iIndexer[EDriveA] = NULL;
    delete plugin;      
    return err;
    }

TInt CHarvesterPluginTester::TestMessageIsMediaRemovableL( CStifItemParser& aItem )
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    TInt error;
    TInt driveNumber = 0;
    //TODO add a while loop.
    while( aItem.GetNextInt( driveNumber ) == KErrNone )
        {
        error = 0;
        TDriveNumber number(static_cast<TDriveNumber>(driveNumber) );
        TRAP( error, plugin->IsMediaRemovableL( number ) );
        if( error ) break;
        }
    delete plugin;
    return error;
    }

TInt CHarvesterPluginTester::TestFormFormBaseAppClassL( CStifItemParser& /*aItem*/ )
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    TBuf<20> appclass;
    //To test proper case
    plugin->FormBaseAppClass( EDriveA , appclass);
    //To test failure case
    plugin->FormBaseAppClass( (TDriveNumber)29 , appclass);
    //cleanup
    delete plugin;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestMsgDeleteL( CStifItemParser& /*aItem*/ )
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CMessageDataHandler* idatahandler = CMessageDataHandler::NewL( *plugin, *(plugin->iMsvSession) );
    const TMsvId id1 = 1;
    RSearchServerSession session;
    User::LeaveIfError( session.Connect() );
    plugin->iIndexer[plugin->iCurrentDrive] = CCPixIndexer::NewL( session );       
    idatahandler->CreateMessageIndexItemL( id1, ECPixRemoveAction, id1 );
    delete idatahandler;
    delete plugin;
    session.Close();
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestMsgHandlesessionL( CStifItemParser& /*aItem*/ )
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    TMsvId id = 1;
    TDriveNumber drive1 = EDriveB;
    TDriveNumber drive2 = EDriveC;
    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    // Current entry will be the one to send
    selection->AppendL( id );    
    TRAPD(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvEntriesDeleted , (TAny*)selection ,NULL , NULL));
    TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvEntriesMoved , (TAny*)selection ,NULL , NULL));
    TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvEntriesMoved , (TAny*)selection ,(TAny*)selection , NULL));
    TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvEntriesMoved , (TAny*)selection ,(TAny*)selection , (TAny*)selection));
    TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvEntriesMoved , NULL ,(TAny*)selection , (TAny*)selection));
    TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvMediaChanged , (TAny*)&drive1 ,(TAny*)&drive2 , NULL));
    TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvMediaUnavailable , (TAny*)&drive2 ,NULL , NULL));
    TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvMediaAvailable , (TAny*)&drive1 ,NULL , NULL));
    selection->Reset();
    delete plugin;
    delete iPluginTester;
    return err;
    }

TInt CHarvesterPluginTester::TestMsgHandlesessionPanicL( CStifItemParser& aItem )
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    TInt num = 0;
    TInt err = KErrNone;
    aItem.GetNextInt( num );
    if ( num == 0)
        {
        TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvServerFailedToStart , NULL ,NULL , NULL));
        }
    else if ( num ==1 )
        {
        TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvCloseSession , NULL ,NULL , NULL));
        }
    else
        TRAP(err , plugin->HandleSessionEventL(MMsvSessionObserver::EMsvServerTerminated , NULL ,NULL , NULL));
    
    delete plugin;
    delete iPluginTester;
    return err;
    }

TInt CHarvesterPluginTester::TestMsgOverwriteOrAddToQuereL( CStifItemParser& /*aItem*/ )
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    plugin->iMessageMonitor->OverwriteOrAddToQuereL( 1 , ECPixAddAction , 1 );
    plugin->iMessageMonitor->OverwriteOrAddToQuereL( 1 , ECPixAddAction , 1 );
    plugin->iMessageMonitor->RunL();
    delete plugin;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestMsgMessageHarvesterL( CStifItemParser& /*aItem*/ )
    {
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CMessageHarvester* messageHarvester = CMessageHarvester::NewL( *plugin, *(plugin->iMsvSession) );
    messageHarvester->RunError(KErrNone);
    messageHarvester->HandleNextRequest();
    delete messageHarvester;
    delete plugin;
    return KErrNone;
    }

TInt CHarvesterPluginTester::SearchForTextL(const TDesC& aQueryString, const TDesC& aBaseAppclass,const TDesC& aDefaultField)
    {
    TInt DocumentCount(KErrNotFound);
    RSearchServerSession session;
    User::LeaveIfError(session.Connect());
    CCPixSearcher* searcher = CCPixSearcher::NewL(session);
    searcher->OpenDatabaseL(aBaseAppclass);
    // Send for indexing
    if ( searcher )
        {
        DocumentCount = searcher->SearchL(aQueryString, aDefaultField);
        }
    session.Close();
    return DocumentCount;
    }

TInt CHarvesterPluginTester::TestStartBookmarksHarvesterL( CStifItemParser& /*aItem */)
    {
    CBookmarksPlugin* plugin = CBookmarksPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    plugin->StartHarvestingL( _L(BOOKMARK_QBASEAPPCLASS) );
    iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, KErrNone, KNoErrorString );
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestAddBookmarkL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    CBookmarksPlugin* plugin = CBookmarksPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //starts the bookmarks db monitor.

    //now add a bookmark.
    CFavouritesItem* item = CFavouritesItem::NewL();
    RFavouritesDb favoritesDb;
    RFavouritesSession favSession;
    error = favSession.Connect();
    if( KErrNone == error ) error = favoritesDb.Open( favSession, KBrowserBookmarks ) ;
    if( KErrNone != error ) return error;
    
    item->SetType( CFavouritesItem::EItem );
    item->SetParentFolder( KFavouritesRootUid );
    
    TPtrC string;
    error = aItem.GetNextString( string );
    if( KErrNone == error ) 
        item->SetNameL( string );
    
    error = aItem.GetNextString( string );
    if( KErrNone == error ) 
        item->SetUrlL( string );
    
    if( KErrNone == error ) 
        {
        error = favoritesDb.Begin( ETrue );
        if( KErrNone == error ) error = favoritesDb.Add( *item, ETrue ); //ETrue for Autorename. We are searching only by URL so this is fine.
        if( KErrNone == error ) error = favoritesDb.Commit();
        plugin->StartHarvestingL( _L(BOOKMARK_QBASEAPPCLASS) );
        if( KErrNone == error ) iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
        }
    
    //now, search for URL.
    if( KErrNone == error )
        {
        error = aItem.GetNextString( string );
        if( KErrNone == error ) 
            error = doSearchL( string , _L( BOOKMARK_QBASEAPPCLASS ), ESearchTypeResultsExpected );
        }

    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    favoritesDb.Close();
    favSession.Close();
    
    doLog( iLog, error, _L("Error in AddBookmarkL") );
    return error;
    }

//Forward declare this function.
//It is a local function in the .cpp file and hence not listed in .h file.
//Forward declare it so that the compilation can go thru - the implementation
//will be picked up by the linker.
void GetDomainNameL( const TDesC& aUrl, TPtrC& aDomain );

TInt CHarvesterPluginTester::TestAddGetDomainL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    TPtrC inputString;
    TPtrC domain;
    while( aItem.GetNextString( inputString ) == KErrNone )
        {
        GetDomainNameL( inputString, domain );
        if( aItem.GetNextString( inputString ) == KErrNone )
            error = domain.Compare( inputString );
            if( error != KErrNone ) return error;
        }
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestStartApplicationsHarvesterL( CStifItemParser& /*aItem*/ )
    {
    CApplicationsPlugin* appsPlugin = CApplicationsPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( appsPlugin );
    appsPlugin->StartPluginL(); //Calls Add
    appsPlugin->StartHarvestingL( KNullDesC );
    iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    delete appsPlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, KErrNone, KNoErrorString );
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestCreateApplicationsIndexItemL( CStifItemParser& /*aItem*/ )
    {
    CApplicationsPlugin* appsPlugin = CApplicationsPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( appsPlugin );
    appsPlugin->StartPluginL();
    
    //Force reharvesting:
    appsPlugin->HandleAppListEvent( 1 ); //EListChanged = 1
    iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.

    delete appsPlugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, KErrNone, KNoErrorString );
    return KErrNone;
    }
	
	TInt CHarvesterPluginTester::TestStartNotesHarvesterL( CStifItemParser& /*aItem*/ )
    {	
    CNotesPlugin* plugin = CNotesPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    plugin->StartHarvestingL( _L(NOTES_QBASEAPPCLASS) );
    iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, KErrNone, KNoErrorString );	
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestAddNoteL( CStifItemParser& aItem )
    {    
    TPtrC string;
    TPtrC searchstring;
    TInt error = aItem.GetNextString( string );
    error = aItem.GetNextString( searchstring );
    error = PerformNotesTestL(string , searchstring);
    return error;
    }

TInt CHarvesterPluginTester::TestAddLongNoteL( CStifItemParser& aItem )
    {
    TPtrC string;
    TPtrC searchstring;
    TInt error = aItem.GetNextString( string );
    error = aItem.GetNextString( searchstring );
    error = PerformNotesTestL(string , searchstring);
    return error;
    }
TInt CHarvesterPluginTester::TestAddAlphaNumericNoteL( CStifItemParser& aItem )
    {
    TPtrC string;
    TPtrC searchstring;
    TInt error = aItem.GetNextString( string );
    error = aItem.GetNextString( searchstring );
    error = PerformNotesTestL(string , searchstring);
    return error;
    }
TInt CHarvesterPluginTester::PerformNotesTestL( TPtrC aString1 , TPtrC aString2)
    {	
    TInt error = KErrNone;	
    HBufC8* memo = NULL;    
    CNotesPlugin* plugin = CNotesPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //starts the Notes database notifier    
    if(aString1.Length() > 0)
        {
        RFs fssession;
        User::LeaveIfError( fssession.Connect() );
        RFile testfile;        
        error = testfile.Open(fssession,aString1,EFileRead);
        if(error == KErrNone)
            {
            TInt size(0);
            testfile.Size(size);
            memo = HBufC8::NewL(size);
            TPtr8 memoptr = memo->Des();
            User::LeaveIfError( testfile.Read(memoptr, size ) );
            //testfile.Read(memoptr,size);
            testfile.Close();                
            fssession.Close();
            }
        } 
    if(error == KErrNone)
        {
        CCalendarObserver* session = CCalendarObserver::NewL();
        session->AddNoteL(memo->Des());
        plugin->StartHarvestingL( _L(NOTES_QBASEAPPCLASS) );
        iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
        error = doSearchL( aString2, _L(NOTES_QBASEAPPCLASS), ESearchTypeResultsExpected );
        
        delete session;
        doLog( iLog, error, KNoErrorString );
        }
    delete memo;
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return error;
    }

TInt CHarvesterPluginTester::TestDeleteNoteL( CStifItemParser& /*aItem */)
    {	
    TInt error = KErrNone;
    _LIT( KSearchError, "Search Failed" );   
    CNotesPlugin* plugin = CNotesPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();   
    
    CCalendarObserver* session = CCalendarObserver::NewL();
    plugin->StartHarvestingL( _L(NOTES_QBASEAPPCLASS) );
    session->DeleteNoteEntryL();
    iPluginTester->iWaitForHarvester->Start();
    // Add a Note entry
    /*TPtrC searchstring;
    error = aItem.GetNextString( searchstring );
    HBufC8* buf8 = HBufC8::NewL(2*searchstring.Length());
    buf8->Des().Copy(searchstring);
    session->AddNoteL( buf8->Des());
    User::After( (TTimeIntervalMicroSeconds32)35000000 );
    delete buf8;    
    error = doSearchL( _L("TestNote"), _L( NOTES_QBASEAPPCLASS ), ESearchTypeResultsExpected );
    doLog( iLog, error, KSearchError );    
    if( error == KErrNone)
        {
        session->DeleteNoteEntryL();
        User::After( (TTimeIntervalMicroSeconds32)35000000 );
        //Delete the Note entry added previously and search for result    
        error = doSearchL( _L("TestNote"), _L( NOTES_QBASEAPPCLASS ), ESearchTypeResultsExpected );             
        //If the entery is succesfully deleted, make error to KErrNone.To show testcase success
        if(error == KErrNotFound)
            error = KErrNone;    
        }*/
    delete session;
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, error, KSearchError );
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestUpdateNoteL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    _LIT( KSearchError, "Search Failed" );
    TPtrC oldString;
    error = aItem.GetNextString( oldString );
    TPtrC newString;
    error = aItem.GetNextString( newString );
    CNotesPlugin* plugin = CNotesPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();     
    CCalendarObserver* session = CCalendarObserver::NewL();
    plugin->StartHarvestingL( _L(NOTES_QBASEAPPCLASS) );
    // Add a Note entry
    HBufC8* oldbuf8 = HBufC8::NewL(2*oldString.Length());
    oldbuf8->Des().Copy(oldString);
    session->AddNoteL( oldbuf8->Des());
    TBuf<20> oldsearchstring;
    oldsearchstring.Copy( oldString );
    iPluginTester->iWaitForHarvester->Start();
    error = doSearchL( oldsearchstring, _L( NOTES_QBASEAPPCLASS ), ESearchTypeResultsExpected );
    doLog( iLog, error, KSearchError );
    
    TBuf<20> newsearchstring;
    newsearchstring.Copy( newString );
    HBufC8* newbuf8 = HBufC8::NewL(2*newString.Length());
    newbuf8->Des().Copy(newString);
    //update the Note entry and search for result 
    session->UpdateNoteEntryL( oldbuf8->Des(), newbuf8->Des() );
    plugin->StartHarvestingL( _L(NOTES_QBASEAPPCLASS) );
    iPluginTester->iWaitForHarvester->Start();
    error = doSearchL( newsearchstring, _L( NOTES_QBASEAPPCLASS ), ESearchTypeResultsExpected );
    doLog( iLog, error, KSearchError );
    
    delete newbuf8;
    delete oldbuf8;
    delete session;
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return error;    
    }

TInt CHarvesterPluginTester::TestNotesEntryNegativeL( CStifItemParser& /*aItem*/ )
    {   
    CNotesPlugin* plugin = CNotesPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    plugin->StartHarvestingL( _L(NOTES_QBASEAPPCLASS) );
    iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    plugin->CreateNoteEntryL(0, ECPixUpdateAction);
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, KErrNone, KNoErrorString );    
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestNotesDestructorL( CStifItemParser& /*aItem*/ )
    {
    CNotesPlugin* plugin = CNotesPlugin::NewL();    
    plugin->iAsynchronizer->CancelCallback();
    plugin->iAsynchronizer = NULL;
    plugin->iSession->StopChangeNotification();
    plugin->iSession = NULL;
    delete plugin;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestNotesDelayedErrorL( CStifItemParser& /*aItem*/ )
    {
    CNotesPlugin* plugin = CNotesPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    iPluginTester->SetWaitTime(2000000);
    plugin->StartPluginL();
    TRAPD(err, plugin->DelayedError( KErrGeneral ));
    delete plugin;
    delete iPluginTester;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestNotesChangeEntryL( CStifItemParser& /*aItem*/ )
    {
    CNotesPlugin* plugin = CNotesPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    iPluginTester->SetWaitTime(2000000);
    plugin->StartPluginL();
    TCalChangeEntry calEntry;
    calEntry.iChangeType = MCalChangeCallBack2::EChangeUndefined; //undefined type
    plugin->HandleNoteChangedEntryL( calEntry );
    calEntry.iChangeType = MCalChangeCallBack2::EChangeTzRules; //default check
    plugin->HandleNoteChangedEntryL( calEntry );
    delete plugin;
    delete iPluginTester;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestNotesEntryNoIndexerL( CStifItemParser& /*aItem*/ )
    {    
    CNotesPlugin* plugin = CNotesPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    TRAPD(err, plugin->CreateNoteEntryL(0, ECPixUpdateAction) );
    TRAP(err, plugin->CreateNoteEntryL(0, (TCPixActionType)3) );
    iPluginTester->SetWaitTime(2000000);    
    CCalendarObserver* calobserver = CCalendarObserver::NewL();
    calobserver->AddEntryL();
    delete calobserver;
    delete plugin;
    delete iPluginTester;
    return err;
    }
	
TInt CHarvesterPluginTester::TestStartContactsHarvesterL( CStifItemParser& /*aItem*/ )
    {
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //start to moniter contacts db
    plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS) );
    iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, KErrNone, KNoErrorString );
    return KErrNone;
    }

TContactItemId CHarvesterPluginTester::CreateNewContactL( CContactDatabase& database,const TDesC& aName,const TDesC& aPhoneNumber )
    {
    // Create a  contact card to contain the data
    CContactCard* newCard = CContactCard::NewLC();
    
    // Create the firstName field and add the data to it
    CContactItemField* firstName = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldGivenName );
    firstName->TextStorage()->SetTextL(aName);    
    newCard->AddFieldL(*firstName);
    CleanupStack::Pop(firstName);
      
    // Create the phoneNo field and add the data to it
    CContactItemField* phoneNumber = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldPhoneNumber );
    phoneNumber->SetMapping(KUidContactFieldVCardMapTEL);
    phoneNumber ->TextStorage()->SetTextL(aPhoneNumber);
    newCard->AddFieldL(*phoneNumber);
    CleanupStack::Pop(phoneNumber);
    
    // Add newCard to the database     
    const TContactItemId contactId = database.AddNewContactL(*newCard);    
    CleanupStack::PopAndDestroy(newCard);    
    return contactId;
    }

TInt CHarvesterPluginTester::TestContactDelayedCallbackNoIContactsL( CStifItemParser& /*aItem*/ )
    {
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    //for coverage
    delete plugin->iContacts;
    plugin->iContacts = NULL;
    TRAPD( err, plugin->DelayedCallbackL( KErrNone ) );
    delete plugin;
    return err;
    }

TInt CHarvesterPluginTester::TestContactDelayedCallbackNoIObserverL( CStifItemParser& /*aItem*/ )
    {
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    //for coverage
    delete plugin->iObserver;
    plugin->iObserver = NULL;
    TRAPD( err, plugin->DelayedCallbackL( KErrNone ) );
    delete plugin;
    return err;
    }

TInt CHarvesterPluginTester::TestContactDelayedCallbackWrongCountL( CStifItemParser& /*aItem*/ )
    {
    //This case can happen if a contact was deleted while harvesting is going on.
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    plugin->iCurrentIndex = plugin->iContacts->Count()+1;
    TRAPD( err, plugin->DelayedCallbackL( KErrNone ) );
    delete plugin;
    return err;
    }

TInt CHarvesterPluginTester::TestContactDelayedErrorL(CStifItemParser& /*aItem*/)
    {
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    plugin->DelayedError( KErrGeneral );
    delete plugin;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestContactGetDateL( CStifItemParser& aItem )
    {
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    
    TPtrC dateString;
    TInt error = 0;
    if( aItem.GetNextString ( dateString ) == KErrNone )
        {
        TBuf<30> date; //Picked up 30 from contactsplugin.cpp file.
        TRAP(error, plugin->GetDateL(dateString, date) );
        }
    delete plugin;
    return error;
    }

TInt CHarvesterPluginTester::TestCreateContactIndexItemNoIndexerL( CStifItemParser& /*aItem*/ )
    {
    //Create contact plugin, call CreateContactIndexItemL without creating indexer.
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    TRAPD(err, plugin->CreateContactIndexItemL(0, ECPixUpdateAction) );
    delete plugin;
    return err;
    }

TInt CHarvesterPluginTester::TestCreateContactIndexItemL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    TPtrC IndexAction;
    TPtrC ContactName;
    TPtrC NewContactName;
    _LIT( KActionAdd, "add" );
    _LIT( KSearchError, "Search Failed" );
    _LIT( KActionDelete, "delete" );
    _LIT( KActionUpdate, "update" );
    
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //start to moniter contacts db
    CContactDatabase* db = CContactDatabase::OpenL();
    
    if( aItem.GetNextString ( IndexAction ) == KErrNone )
        {
         if( IndexAction.Compare( KActionAdd ) == 0 )
             {
             if( aItem.GetNextString ( ContactName ) == KErrNone )
                 {
                 TInt aContactId;
                 plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS) );
                 aContactId = CreateNewContactL( *db, ContactName, _L("1234") );                 
                 iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.                 
                                                       
                 error = doSearchL( ContactName, _L( CONTACT_QBASEAPPCLASS ), ESearchTypeResultsExpected ); 
                 db->DeleteContactL(aContactId);
                 }
             }
         if( IndexAction.Compare( KActionUpdate ) == 0 )
             {
             if( aItem.GetNextString ( ContactName ) == KErrNone )
                 {
                  TInt aContactId;
                  //Add a contact first and later edit the contact
                  plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS) );                                    
                  aContactId = CreateNewContactL( *db, ContactName, _L("123") );
                  iPluginTester->iWaitForHarvester->Start();
                  if( aItem.GetNextString ( NewContactName ) == KErrNone )
                      {
                      CContactCard* newCard = CContactCard::NewLC();                      
                      // Create the firstName field and add the data to it
                      CContactItemField* firstName = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldGivenName );
                      firstName->TextStorage()->SetTextL(NewContactName);    
                      newCard->AddFieldL(*firstName);
                      CleanupStack::Pop(firstName);
                        
                      // Create the phoneNo field and add the data to it
                      CContactItemField* phoneNumber = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldPhoneNumber );
                      phoneNumber->SetMapping(KUidContactFieldVCardMapTEL);
                      phoneNumber ->TextStorage()->SetTextL(_L("567"));
                      newCard->AddFieldL(*phoneNumber);
                      CleanupStack::Pop(phoneNumber);
                      
                      plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS) );
                      CContactItem* contactItem = db->UpdateContactLC(aContactId, newCard);
                      iPluginTester->iWaitForHarvester->Start();
                      
                      CleanupStack::PopAndDestroy(contactItem);
                      CleanupStack::PopAndDestroy(newCard);
                      
                      error = doSearchL( NewContactName, _L( CONTACT_QBASEAPPCLASS ), ESearchTypeResultsExpected );
                      }
                  }                 
             }
         if( IndexAction.Compare( KActionDelete ) == 0 )
             {
             if( aItem.GetNextString ( ContactName ) == KErrNone )
                 {
                 TInt aContactId;
                 plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS));
                 //Add a contact to contactdatabase and later delete it
                 aContactId = CreateNewContactL( *db, ContactName, _L("123455"));                 
                 iPluginTester->iWaitForHarvester->Start();
                 error = doSearchL( ContactName, _L( CONTACT_QBASEAPPCLASS ), ESearchTypeResultsExpected );
                 if(error == KErrNone)
                     {
                     plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS));
                     db->DeleteContactL(aContactId);
                     iPluginTester->iWaitForHarvester->Start();
                     error = doSearchL( ContactName, _L( CONTACT_QBASEAPPCLASS ), ESearchTypeNoResultsExpected );
                     }                                 
                 }             
             }
        }        
        delete db;
        delete plugin;
        delete iPluginTester;
        iPluginTester = NULL;
        doLog( iLog, error, KSearchError );
        return KErrNone;    
    }

TInt CHarvesterPluginTester::TestCreateAllContactFieldsL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    TPtrC ContactName;
    _LIT( KBirthDate, "19970107:");
    _LIT( KSearchError, "Search Failed" );
    
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    CContactDatabase* db = CContactDatabase::OpenL();
    if( aItem.GetNextString ( ContactName ) == KErrNone )
        {
        plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS) );
        CContactCard* newCard = CContactCard::NewLC();
        
        // Create the firstName field and add the data to it
        CContactItemField* firstName = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldGivenName );
        firstName->TextStorage()->SetTextL(ContactName);    
        newCard->AddFieldL(*firstName);
        CleanupStack::Pop(firstName);
        
        CContactItemField* suffix = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldSuffixName );        
        suffix->TextStorage()->SetTextL(_L("Mr"));
        newCard->AddFieldL(*suffix);
        CleanupStack::Pop(suffix);
        
        CContactItemField* assistant = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldAssistant );        
        assistant->TextStorage()->SetTextL(_L("Jane"));
        newCard->AddFieldL(*assistant);
        CleanupStack::Pop(assistant);
        
        CContactItemField* department = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldDepartmentName );
        department->TextStorage()->SetTextL(_L("Services"));
        newCard->AddFieldL(*department);
        CleanupStack::Pop(department);
        
        CContactItemField* birthday = CContactItemField::NewLC( KStorageTypeDateTime, KUidContactFieldBirthday );
        TTime time(KBirthDate);
        birthday->DateTimeStorage()->SetTime(time);
        newCard->AddFieldL(*birthday);
        CleanupStack::Pop(birthday);        

        CContactItemField*  anniversary = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldAnniversary );        
        anniversary->TextStorage()->SetTextL(_L("2002-10-10"));
        newCard->AddFieldL(*anniversary);
        CleanupStack::Pop(anniversary);      
        
        // Create the phoneNo field and add the data to it
        CContactItemField* phoneNumber = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldPhoneNumber );        
        phoneNumber->TextStorage()->SetTextL(_L("1234"));
        newCard->AddFieldL(*phoneNumber);
        CleanupStack::Pop(phoneNumber);
        
        // Add newCard to the database     
        const TContactItemId contactId = db->AddNewContactL(*newCard);    
        CleanupStack::PopAndDestroy(newCard);
        
        iPluginTester->iWaitForHarvester->Start();
        
        error = doSearchL( ContactName, _L( CONTACT_QBASEAPPCLASS ), ESearchTypeResultsExpected );
        if( error == KErrNone )
            {
            error = doSearchL( _L("jan"), _L( CONTACT_QBASEAPPCLASS ), ESearchTypeResultsExpected );
            }
        }
    delete db;
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, error, KSearchError );
    return error;
    }
TInt CHarvesterPluginTester::TestCreateContactGroupL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    _LIT( KSearchError, "Search Failed" );
    TPtrC GroupName;
    CContactsPlugin* plugin = CContactsPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //start to moniter contacts db
    CContactDatabase* db = CContactDatabase::OpenL();    
    
    if( aItem.GetNextString ( GroupName ) == KErrNone )
        {
        plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS) );
        TInt aContactId = CreateNewContactL( *db, _L("Group Contact"), _L("123455") );
        CContactItem* newitem = db->CreateContactGroupL( _L("TestGroup") );
        db->AddContactToGroupL( aContactId, newitem->Id() );       
        iPluginTester->iWaitForHarvester->Start();    
        error = doSearchL( GroupName, _L( CONTACT_QBASEAPPCLASS ), ESearchTypeResultsExpected );
        
        //update the group by adding a new contact, to get update event
        CContactGroup* group = static_cast<CContactGroup*>(db->OpenContactLX(newitem->Id()));        
        CleanupStack::PushL(group);
        plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS) );
        TInt aId = CreateNewContactL( *db, _L("Contact1"), _L("455") );        
        db->AddContactToGroupL( aId, group->Id());
        db->CommitContactL(*group);
        iPluginTester->iWaitForHarvester->Start();    
        CleanupStack::PopAndDestroy(2);
        
        //Delete the group and its contacts
        plugin->StartHarvestingL( _L(CONTACT_QBASEAPPCLASS) );
        db->DeleteContactL(aContactId);
        db->RemoveContactFromGroupL(aId, newitem->Id());
        db->DeleteContactL( newitem->Id() );
        iPluginTester->iWaitForHarvester->Start();
        }
    
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    delete db;
    doLog( iLog, error, KSearchError );
    return KErrNone;    
    }

TInt CHarvesterPluginTester::TestStartCalenderHarvesterL( CStifItemParser& /*aItem*/ )
    {
    CCalendarObserver* session = CCalendarObserver::NewL();
    // Add a calender entry
    session->AddEntryL();
    delete session;
    CCalendarPlugin* plugin = CCalendarPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //start to moniter contacts db
    plugin->StartHarvestingL( _L(CALENDAR_QBASEAPPCLASS) );
    iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, KErrNone, KNoErrorString );
    return KErrNone;    
    }

TInt CHarvesterPluginTester::TestCalenderEntryL( CStifItemParser& /*aItem*/ )
    {
    TInt error = KErrNone;
    _LIT( KSearchError, "Search Failed" );
    CCalendarPlugin* plugin = CCalendarPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();   
    
    CCalendarObserver* session = CCalendarObserver::NewL();
    plugin->StartHarvestingL( _L(CALENDAR_QBASEAPPCLASS) );
    // Add a calender entry
    session->AddEntryL();
    iPluginTester->iWaitForHarvester->Start();
    User::After((TTimeIntervalMicroSeconds32)30000000);
    error = doSearchL( _L("Meeting"), _L( CALENDAR_QBASEAPPCLASS ), ESearchTypeResultsExpected );
    doLog( iLog, error, KSearchError );
    
    // For update event
    session->UpdateCalenderEntryL();
    plugin->StartHarvestingL( _L(CALENDAR_QBASEAPPCLASS) );
    iPluginTester->iWaitForHarvester->Start();
    
    plugin->StartHarvestingL( _L(CALENDAR_QBASEAPPCLASS) );
    session->DeleteEntryL();
    iPluginTester->iWaitForHarvester->Start();
    //Delete the calender entry added previously and search for result    
    error = doSearchL( _L("scheduled"), _L( CALENDAR_QBASEAPPCLASS ), ESearchTypeResultsExpected );
        
    //If the entery is succesfully deleted, make error to KErrNone.To show testcase success
    if(error == KErrNotFound)
       error = KErrNone;    
    
    delete session;
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, error, KSearchError );     
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestCreateMmsL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    _LIT( KSearchError, "Search Failed" );
    TPtrC filepath;
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    CMessageSessionObserver* sessionobserver = CMessageSessionObserver::NewL();
    CMsvSession* msgSession = CMsvSession::OpenSyncL(*sessionobserver);    
    TInt found = aItem.GetNextString( filepath );
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    TInt drive = msgSession->CurrentDriveL();
    CMessagePlugin::FormBaseAppClass(TDriveNumber(drive),baseAppClass);
    
    plugin->StartHarvestingL( baseAppClass );
    
    //Create MMS with text attachment for UTF encoded data
    TMsvId msgid = MessagingUtils::CreateMmsMessageL(msgSession,
                                                 _L("+358440067886"),
                                                 _L("+919845062437"),
                                                 filepath);
    
    iPluginTester->iWaitForHarvester->Start();
    
    TInt count = SearchForTextL(_L("UTF"),baseAppClass,KNullDesC);
    if(count <= 0)
       {
       error = KErrNotFound;
       }
    MessagingUtils::RemoveMmsEntryL( msgSession, msgid );
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    delete sessionobserver;
    delete msgSession;    
    doLog(iLog,error,KSearchError);
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestcalenderdestructorL( CStifItemParser& /*aItem */ )
    {
    CCalendarPlugin* plugin = CCalendarPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //start to moniter contacts db   
    plugin->Progress(100);
    iPluginTester->SetWaitTime(2000000);
    plugin->StartHarvestingL( _L(CALENDAR_QBASEAPPCLASS) );
    plugin->DelayedError(KErrNone);
    iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    plugin->Completed(KErrGeneral);  
    plugin->iAsynchronizer->CancelCallback();
    plugin->iAsynchronizer = NULL;
    plugin->iSession->StopChangeNotification();
    plugin->iSession = NULL;
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, KErrNone, KNoErrorString );
    return KErrNone; 
    }

TInt CHarvesterPluginTester::TestCalChangeEntryL( CStifItemParser& /*aItem */ )
    {
    CCalendarPlugin* plugin = CCalendarPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    iPluginTester->SetWaitTime(2000000);
    plugin->StartPluginL();
    TCalChangeEntry calEntry;
    calEntry.iChangeType = MCalChangeCallBack2::EChangeAdd; //undefined type
    plugin->HandleChangedEntryL( calEntry );
    calEntry.iChangeType = MCalChangeCallBack2::EChangeUndefined; //undefined type
    plugin->HandleChangedEntryL( calEntry );
    calEntry.iChangeType = MCalChangeCallBack2::EChangeTzRules; //default check
    plugin->HandleChangedEntryL( calEntry );
    delete plugin;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestCalCreateEntryL( CStifItemParser& /*aItem */ )
    {
    _LIT8(KTestmemo , "Notesplugin");
    CCalendarPlugin* plugin = CCalendarPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    iPluginTester->SetWaitTime(2000000);
    plugin->CreateEntryL(0,ECPixAddAction );
    plugin->StartPluginL();
    plugin->CreateEntryL(0,ECPixAddAction );
    CCalendarObserver* calobserver = CCalendarObserver::NewL();
    HBufC8* memo = HBufC8::New(10);
    TPtr8 ptr = memo->Des();
    ptr.Copy(KTestmemo);
    calobserver->AddNoteL(ptr);
    delete calobserver;
    delete memo;
    plugin->CreateEntryL(0,(TCPixActionType )3);
    delete plugin;
    delete iPluginTester;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestCreateEmailL( CStifItemParser& /*aItem */)
    {
    TInt error = KErrNone;
    _LIT( KSearchError, "Search Failed" );
    CMessagePlugin* plugin = CMessagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    CMessageSessionObserver* sessionobserver = CMessageSessionObserver::NewL();
    CMsvSession* msgSession = CMsvSession::OpenSyncL( *sessionobserver );
    
    TBuf<KMsgPluginBaseAppClassMaxLen> baseAppClass;
    TInt drive = msgSession->CurrentDriveL();
    CMessagePlugin::FormBaseAppClass( TDriveNumber(drive), baseAppClass );        
    plugin->StartHarvestingL( baseAppClass );
    //Create Email entry using SMTP client 
    TMsvId msgid = MessagingUtils::CreateEmailEntryL( msgSession );
    iPluginTester->iWaitForHarvester->Start();
    TInt count = SearchForTextL( _L("SampleEmail"), baseAppClass, KNullDesC );
    if(count <= 0)
       {
       error = KErrNotFound;
       }
    MessagingUtils::RemoveMmsEntryL( msgSession, msgid );
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    delete sessionobserver;
    delete msgSession;    
    doLog(iLog,error,KSearchError);
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestAudioHarvestingL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester" );
    _LIT( KExample, "In TestAudioHarvestingL" );
    TInt error(KErrNone);
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );
    // Print to log file
    iLog->Log( KExample );
    CAudioPlugin* plugin = CAudioPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //Initialize the Plugin
    //Copy file path
    RFs fileSession;
    User::LeaveIfError( fileSession.Connect() );
    _LIT(KPathToMusic,"c:\\data\\music\\");
    fileSession.Delete(_L("c:\\data\\music\\Eagle_Landed.mp3")); //Delete if already exist
    //pause harvester while copy
    RHarvesterClient   harvester;
    User::LeaveIfError(harvester.Connect() );
    harvester.Pause();
    TRAP_IGNORE( BaflUtils::EnsurePathExistsL(fileSession,KPathToMusic) );//Create folder
    BaflUtils::CopyFile(fileSession,_L("c:\\data\\Eagle_Landed.mp3"),KPathToMusic);
    //Resume harvester for this harvesting
    harvester.Resume();
    harvester.Close();
    //Do harvesting and search for Eagle it should be available
    plugin->StartHarvestingL(_L(MEDIA_QBASEAPPCLASS));
    //Wait for one minutes after doc processing to Index and Flush to happen
    iPluginTester->SetWaitTime((TTimeIntervalMicroSeconds32)60000000);
    iPluginTester->iWaitForHarvester->Start(); //Start Wait AO and let it complete
    error = doSearchL( _L("Eagle"),_L(MEDIA_QBASEAPPCLASS), ESearchTypeResultsExpected ); 
    
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    fileSession.Close();    
    //End search
    doLog(iLog,error,_L("Error in TestAudioHarvestingL"));
    return error;
    }
        
TInt CHarvesterPluginTester::TestMdsSyncControllerL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester" );
    _LIT( KExample, "In TestMdsSyncControllerL" );
    TInt error(KErrNone);
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );
    // Print to log file
    iLog->Log( KExample );
    CMDSEntity* entity;
    CCPIXMDEDbManager* dbcontroller;
    entity = CMDSEntity::NewL();
    TUid uid = {0x101FB3E3};
    dbcontroller = CCPIXMDEDbManager::NewL(uid);
    entity->Setkey(1);
    entity->SetUri(_L("\\music\\bhuvnesh.mp3"));
    TDriveNumber drive = TDriveNumber(EDriveC);
    entity->SetDrive(drive);
    dbcontroller->AddL(entity->Key(),*entity);
    error = dbcontroller->IsAlreadyExistL(entity->Key());
    if( error )
        {
        iLog->Log( _L("Already exist") );
        }
    entity->Reset();
    dbcontroller->GetItemL(1,*entity);
    dbcontroller->RemoveL(entity->Key());
    error = dbcontroller->IsAlreadyExistL( entity->Key() );
    if( error )
        {
        iLog->Log( _L("Already exist") );
        }
    dbcontroller->ResetL();
    delete entity;
    delete dbcontroller;
    return error;
    }

TInt CHarvesterPluginTester::TestAudioHandleItemL( CStifItemParser& aItem)
    {
    TInt error = KErrNone;    
    TInt objId;    
    TInt actionType;
    aItem.GetNextInt ( objId );
    aItem.GetNextInt ( actionType );    
    CAudioPlugin* plugin = CAudioPlugin::NewL(); 
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    TRAPD( err , plugin->HandleMdeItemL(objId, (TCPixActionType)actionType) );
    doLog(iLog,error,_L("Error in TestAudioHandleItemL"));
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestAudioSyncDbManagerL( CStifItemParser& /* aItem */)
    {    
    CAudioPlugin* plugin = CAudioPlugin::NewL(); 
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    CMDSEntity* entity;    
    entity = CMDSEntity::NewL();    
    entity->Setkey(100);
    entity->SetUri(_L("\\music\\audio.mp3"));
    TDriveNumber drive = TDriveNumber(EDriveC);
    entity->SetDrive(drive);    
    plugin->iDBManager->AddL( entity->Key(),*entity );
    TRAPD( err , plugin->HandleMdeItemL(entity->Key(), ECPixAddAction));
    TRAP( err , plugin->HandleMdeItemL(entity->Key(), ECPixUpdateAction));
    TRAP( err , plugin->HandleMdeItemL(entity->Key(), ECPixRemoveAction));
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestAudioMMCEventL( CStifItemParser& aItem )
    {    
    TInt error(KErrNone);
    TInt drive;    
    TInt mmcstatus;
    aItem.GetNextInt ( drive );
    aItem.GetNextInt ( mmcstatus );    
    CAudioPlugin* plugin = CAudioPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //Initialize the Plugin
    TRAPD( err , plugin->HandleMMCEventL( (TDriveNumber)drive , mmcstatus) );
    //iPluginTester->iWaitForHarvester->Start(); //Start Wait AO and let it complete
    doLog(iLog,error,_L("Error in TestAudioMMCEventL"));
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    //End search
    return err;
    }

TInt CHarvesterPluginTester::TestAudioNoIndexerL( CStifItemParser& aItem )
    {
    TInt drive;
    TInt objId;
    aItem.GetNextInt ( drive );
    aItem.GetNextInt ( objId );
    CAudioPlugin* plugin = CAudioPlugin::NewL(); 
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    plugin->iIndexerUtil->iIndexer[drive]= NULL;
    TRAPD( err , plugin->HandleMdeItemL(objId, ECPixUpdateAction) );
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;    
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestBlacklistPluginL( CStifItemParser& /*aItem*/ )
    {
    //@todo: This test case shoud be in IDS middleware harvester STIF cases
    TInt err = KErrNone;
    /*
    CBlacklistMgr* blacklistmanager = CBlacklistMgr::NewL();
    CleanupStack::PushL( blacklistmanager );
    TInt version = 0;
    aItem.GetNextInt(version);
    //Add an Uid to Blacklist DB
    blacklistmanager->AddL( KTestUid , version );
    //Check if the Uid is added to database or not
    TBool found = blacklistmanager->FindL(KTestUid , version );
    
    if(!found) err = KErrNotFound;
    //clear the UID from the database
    blacklistmanager->Remove(KTestUid);
    CleanupStack::PopAndDestroy( blacklistmanager ); 
    doLog( iLog, err, KNoErrorString );
    */
    return err;
    }

TInt CHarvesterPluginTester::TestBlacklistPluginVersionL( CStifItemParser& /*aItem*/ )
    {
    //@todo: This test case shoud be in IDS middleware harvester STIF cases
    TInt err = KErrNone;
    /*	
    CBlacklistMgr* blacklistmanager = CBlacklistMgr::NewL();
    CleanupStack::PushL( blacklistmanager );
    TInt oldversion = 0;
    TInt newversion = 0;
    aItem.GetNextInt(oldversion);
    aItem.GetNextInt(newversion);
    //Add an Uid to Blacklist DB with old version
    blacklistmanager->AddL( KTestUid , oldversion );
    //Add an Uid to Blacklist DB with new version
    blacklistmanager->AddL( KTestUid , newversion );
    //Check if the Uid with old version exists
    TBool found = blacklistmanager->FindL(KTestUid , oldversion );
    if( found )
        {
        err = KErrNotFound;
        }
    else
        {
        //check with new version
        found = blacklistmanager->FindL(KTestUid , newversion );
        if(!found) err = KErrNotFound;
        }    
    //clear the UID from the database
    blacklistmanager->Remove(KTestUid);
    CleanupStack::PopAndDestroy( blacklistmanager );    
    doLog( iLog, err, KNoErrorString );*/
    return err;
    }
TInt CHarvesterPluginTester::TestVideoHarvestingL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    TPtrC filepath;
    TPtrC filename;
    TPtrC newFile;
    CVideoPlugin* plugin = CVideoPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    RFs fSession;
    User::LeaveIfError( fSession.Connect());    
    if((aItem.GetNextString(filepath)==KErrNone) && (aItem.GetNextString(filename) == KErrNone))
        {        
        TBuf<KMaxFileName> desPath(_L("c:\\data\\Videos\\"));
        desPath.Append( filename );
        //Delete the file if it already exists
        fSession.Delete( desPath ); 
        //To ensure video plugin is loaded, pause the harvester
        RHarvesterClient harvester;
        User::LeaveIfError(harvester.Connect());
        harvester.Pause();
        BaflUtils::EnsurePathExistsL( fSession, desPath );//Create folder
        BaflUtils::CopyFile( fSession, filepath, desPath );
        //Resume harvester for this harvesting
        harvester.Resume();
        harvester.Close();
        //Now start video plugin harvesting
        plugin->StartHarvestingL( _L(VIDEO_QBASEAPPCLASS) );
        //wait for index to flush
        iPluginTester->SetWaitTime( (TTimeIntervalMicroSeconds32)60000000 );
        //wait till video harvesting completes
        iPluginTester->iWaitForHarvester->Start();
        TInt count = SearchForTextL( filename, _L(VIDEO_QBASEAPPCLASS), KNullDesC );
        if(count <= 0)
           {
           error = KErrNotFound;
           }
        }        
    fSession.Close();
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    doLog( iLog, error, _L("Error in TestVideoHarvesting") );
    return error;
    }

TInt CHarvesterPluginTester::TestVideoHandleItemL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;    
    TInt objId;    
    TInt actionType;
    aItem.GetNextInt ( objId );
    aItem.GetNextInt ( actionType );    
    CVideoPlugin* plugin = CVideoPlugin::NewL(); 
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    TRAPD( err , plugin->HandleMdeItemL(objId, (TCPixActionType)actionType) );
    doLog(iLog,error,_L("Error in TestVideoHandleItemL"));
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestVideoSyncDbManagerL( CStifItemParser& /*aItem */)
    {
    CVideoPlugin* plugin = CVideoPlugin::NewL(); 
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    CMDSEntity* entity;    
    entity = CMDSEntity::NewL();    
    entity->Setkey(101);
    entity->SetUri(_L("\\video\\video.mpg"));
    TDriveNumber drive = TDriveNumber(EDriveC);
    entity->SetDrive(drive);    
    plugin->iDBManager->AddL( entity->Key(),*entity );
    TRAPD( err , plugin->HandleMdeItemL(entity->Key(), ECPixAddAction));
    TRAP( err , plugin->HandleMdeItemL(entity->Key(), ECPixUpdateAction));
    TRAP( err , plugin->HandleMdeItemL(entity->Key(), ECPixRemoveAction));
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestVideoNoIndexerL( CStifItemParser& aItem )
    {
    TInt drive;
    TInt objId;
    aItem.GetNextInt ( drive );
    aItem.GetNextInt ( objId );
    CVideoPlugin* plugin = CVideoPlugin::NewL(); 
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    plugin->iIndexerUtil->iIndexer[drive]= NULL;
    TRAPD( err , plugin->HandleMdeItemL(objId, ECPixUpdateAction) );
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;    
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestImageHarvestingL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;
    TPtrC filepath;
    TPtrC filename;
    TPtrC newFile;
    CImagePlugin* plugin = CImagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    RFs fSession;
    User::LeaveIfError( fSession.Connect());    
    if((aItem.GetNextString(filepath)==KErrNone) && (aItem.GetNextString(filename) == KErrNone))
        {        
        TBuf<KMaxFileName> desPath(_L("c:\\data\\Images\\"));
        desPath.Append( filename );
        //Delete the file if it already exists
        fSession.Delete( desPath ); 
        //To ensure video plugin is loaded, pause the harvester
        RHarvesterClient harvester;
        User::LeaveIfError(harvester.Connect());
        harvester.Pause();
        BaflUtils::EnsurePathExistsL( fSession, desPath );//Create folder
        BaflUtils::CopyFile( fSession, filepath, desPath );
        //Resume harvester for this harvesting
        harvester.Resume();
        harvester.Close();
        //Now start video plugin harvesting
        plugin->StartHarvestingL( _L(IMAGE_QBASEAPPCLASS) );
        //wait for index to flush
        iPluginTester->SetWaitTime( (TTimeIntervalMicroSeconds32)60000000 );
        iPluginTester->iWaitForHarvester->Start();
        TInt count = SearchForTextL( filename, _L(IMAGE_QBASEAPPCLASS), KNullDesC );
        if(count <= 0)
           {
           error = KErrNotFound;
           }
        }
        else
           error = KErrNotFound;
    doLog( iLog, error, _L("Error in TestImageHarvestingAddIndexL") );  
    fSession.Close();
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return error;
    }

TInt CHarvesterPluginTester::TestImageHandleItemL( CStifItemParser& aItem )
    {
    TInt error = KErrNone;    
    TInt objId;    
    TInt actionType;
    aItem.GetNextInt ( objId );
    aItem.GetNextInt ( actionType );    
    CImagePlugin* plugin = CImagePlugin::NewL(); 
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    TRAPD( err , plugin->HandleMdeItemL(objId, (TCPixActionType)actionType) );
    doLog(iLog,error,_L("Error in TestImageHandleItemL"));
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestImageSyncDbManagerL( CStifItemParser& /*aItem */)
    {
    CImagePlugin* plugin = CImagePlugin::NewL(); 
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    CMDSEntity* entity;    
    entity = CMDSEntity::NewL();    
    entity->Setkey(102);
    entity->SetUri(_L("\\image\\image.jpg"));
    TDriveNumber drive = TDriveNumber(EDriveC);
    entity->SetDrive(drive);    
    plugin->iDBManager->AddL( entity->Key(),*entity );
    TRAPD( err , plugin->HandleMdeItemL(entity->Key(), ECPixAddAction));
    TRAP( err , plugin->HandleMdeItemL(entity->Key(), ECPixUpdateAction));
    TRAP( err , plugin->HandleMdeItemL(entity->Key(), ECPixRemoveAction));
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestImageNoIndexerL( CStifItemParser& aItem )
    {
    TInt drive;
    TInt objId;
    aItem.GetNextInt ( drive );
    aItem.GetNextInt ( objId );
    CImagePlugin* plugin = CImagePlugin::NewL(); 
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL();
    plugin->iIndexerUtil->iIndexer[drive]= NULL;
    TRAPD( err , plugin->HandleMdeItemL(objId, ECPixUpdateAction) );
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;    
    return KErrNone;
    }        
TInt CHarvesterPluginTester::TestVideoMMCEventL( CStifItemParser& aItem )
    {
    TInt error(KErrNone);
    TInt drive;    
    TInt mmcstatus;
    aItem.GetNextInt ( drive );
    aItem.GetNextInt ( mmcstatus );    
    CVideoPlugin* plugin = CVideoPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //Initialize the Plugin
    TRAPD( err , plugin->HandleMMCEventL( (TDriveNumber)drive , mmcstatus) );
    //iPluginTester->iWaitForHarvester->Start(); //Start Wait AO and let it complete
    doLog(iLog,error,_L("Error in TestVideoMMCEventL"));
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    //End search
    return err;
    }
        
TInt CHarvesterPluginTester::TestImageMMCEventL( CStifItemParser& aItem )
    {
    TInt error(KErrNone);
    TInt drive;    
    TInt mmcstatus;
    aItem.GetNextInt ( drive );
    aItem.GetNextInt ( mmcstatus );    
    CImagePlugin* plugin = CImagePlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( plugin );
    plugin->StartPluginL(); //Initialize the Plugin
    TRAPD( err , plugin->HandleMMCEventL( (TDriveNumber)drive , mmcstatus) );
    //iPluginTester->iWaitForHarvester->Start(); //Start Wait AO and let it complete
    doLog(iLog,error,_L("Error in TestImageMMCEventL"));
    delete plugin;
    delete iPluginTester;
    iPluginTester = NULL;
    //End search
    return err;
    }

TInt CHarvesterPluginTester::TestStartEmailPluginL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester: %S" );
    _LIT( KExample, "In TestStartEmailPluginL" );
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );

    // Print to log file
    iLog->Log( KExample );
    CEmailPlugin* emailPlugin = CEmailPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( emailPlugin );    
    emailPlugin->StartPluginL();
    iPluginTester->SetWaitTime( (TTimeIntervalMicroSeconds32)5000000 );
    emailPlugin->StartHarvestingL( KEmailAppBasePath );
    emailPlugin->HarvestingCompleted();
    iPluginTester->iWaitForHarvester->Start();
    delete emailPlugin;
    delete iPluginTester;
    doLog( iLog, KErrNone, KNoErrorString );
    return KErrNone;
    }

TInt CHarvesterPluginTester::TestHandleEmailDocL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester: %S" );
    _LIT( KExample, "In TestHandleEmailDocL" );
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );

    // Print to log file
    iLog->Log( KExample );
    CEmailPlugin* emailPlugin = CEmailPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( emailPlugin );
    TRAPD(err , emailPlugin->StartPluginL());
    CSearchDocument* doc = prepareemaildocument();
    TRAP(err , emailPlugin->HandleDocumentL( doc , ECPixAddAction));    
    TRAP(err , emailPlugin->HandleDocumentL( doc , ECPixRemoveAction));
    //iPluginTester->iWaitForHarvester->Start();//Wait here till Harvesting is complete.
    delete doc;
    delete emailPlugin;
    delete iPluginTester;
    doLog( iLog, err, KNoErrorString );
    return err;
    }

TInt CHarvesterPluginTester::TestEmailBaseappclassL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester: %S" );
    _LIT( KExample, "In TestEmailBaseappclassL" );
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );
    
    TDriveNumber drive ( EDriveA ); 
    _LIT( appcls1 ,"@c:root msg email" );
    _LIT( appcls2 ,"@*:root msg email" );
    _LIT( appcls3 ,"d:root msg email" );    
    CEmailPlugin* emailPlugin = CEmailPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( emailPlugin );
    TRAPD(err , emailPlugin->StartPluginL());
    TRAP(err, emailPlugin->StartHarvestingL(appcls1));
    delete emailPlugin->iIndexer[EDriveC];
    emailPlugin->iIndexer[EDriveC] = NULL;
    TRAP(err, emailPlugin->StartHarvestingL(appcls1));
    TRAP(err, emailPlugin->StartHarvestingL(appcls2));
    TRAP(err, emailPlugin->StartHarvestingL(appcls3));
    delete emailPlugin;
    delete iPluginTester;
    return err;
    }

TInt CHarvesterPluginTester::TestEmailHandleDocL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KHarvesterPluginTester, "HarvesterPluginTester: %S" );
    _LIT( KExample, "In TestEmailHandleDocL" );
    TestModuleIf().Printf( 0, KHarvesterPluginTester, KExample );
    CEmailPlugin* emailPlugin = CEmailPlugin::NewL();
    CHarvesterObserver* iPluginTester = CHarvesterObserver::NewL( emailPlugin );
    CSearchDocument* doc = prepareemaildocument();
    TRAPD(err , emailPlugin->StartPluginL());
    TRAP(err, emailPlugin->StartHarvestingL( KEmailAppBasePath ));    
    delete emailPlugin->iIndexer[EDriveC];    
    TRAP(err,emailPlugin->HandleDocumentL(doc,ECPixRemoveAction));
    TRAP(err,emailPlugin->HandleDocumentL(doc,ECPixAddAction));
    emailPlugin->MountL(EDriveC);
    TRAP(err,emailPlugin->HandleDocumentL(doc,ECPixAddAction));
    TRAP(err,emailPlugin->HandleDocumentL(doc,ECPixUpdateAction));
    delete doc;
    delete emailPlugin;
    delete iPluginTester;
    return err;
    }

//  [End of File] - Do not remove

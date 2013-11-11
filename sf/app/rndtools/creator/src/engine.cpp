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
* Description: 
*
*/


#include <eikenv.h>
#include <s32file.h> 
#include <coemain.h>

#include <driveinfo.h>

#include <datacreator.rsg>

#include "mainview.h"
#include "engine.h"
#include "enginewrapper.h"


#include <cntdb.h>//For Math

#include "creator_scriptentry.h"
#include "creator_modulebase.h"
//#include "creator_browser.h"
#include "creator_calendar.h"
#include "creator_phonebookbase.h"
#include "creator_note.h"
#include "creator_log.h"
//#include "creator_connectionmethodbase.h"
#include "creator_mailbox.h"
//#include "creator_imps.h"
#include "creator_message.h"
#include "creator_landmark.h"


#include "creator_traces.h"
#include "creator_factory.h"
#include "creator_scriptparser.h"
#include "creator_file.h"
#include "creator_cmdscriptrun.h"
#include "creator_contactsetcache.h"

#include <apparc.h>
#include <eikappui.h>
#include <eikapp.h>
#include <bitmaptransforms.h>

_LIT(KTempPathDrive, "d");
_LIT(KTempPath, ":\\Creator\\");
const TInt KRegisterDrive = EDriveC;
_LIT(KRegisterFileName, "creator_created_items.dat");
_LIT(KResourceFileName, "z:\\Resource\\apps\\datacreator.RSC");


// ---------------------------------------------------------------------------

CCreatorEngine* CCreatorEngine::NewL(EngineWrapper *aEngineWrapper)
    {
    CCreatorEngine* self = CCreatorEngine::NewLC(aEngineWrapper);
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------------------------

CCreatorEngine* CCreatorEngine::NewLC(EngineWrapper *aEngineWrapper)
    {
    CCreatorEngine* self = new (ELeave) CCreatorEngine();
    CleanupStack::PushL(self);
    self->ConstructL(aEngineWrapper);
    return self;
	}
	
// ---------------------------------------------------------------------------

CCreatorEngine::CCreatorEngine() 
: 
CActive(0)
{}

// ---------------------------------------------------------------------------

void CCreatorEngine::ConstructL(EngineWrapper *aEngineWrapper)
    {
    LOGSTRING("Creator: CCreatorEngine::ConstructL");

    iEngineWrapper = aEngineWrapper;
	iMemoryDetails.iENotAvailable = EFalse;
	RequestMemoryDetails();
    iEnv = CEikonEnv::Static();
	// add second resource file to use for creator random data generation:
	iResourceFileId = iEnv->AddResourceFileL(KResourceFileName);
    User::LeaveIfError(iTimer.CreateLocal());

    // these are needed by the random data generator
    TTime now;
    now.HomeTime();
    iSeed=now.Int64();
    
    iTempPath = HBufC::NewL(KTempPathDrive().Length() + KTempPath().Length());
    iTempPath->Des().Copy(KTempPathDrive);
    iTempPath->Des().Append(KTempPath);
	
	iCmdScriptRun = CCreatorCmdScriptRun::NewL(this);

    GetRandomDataFromFileL(KNullDesC);
        
    CActiveScheduler::Add(this);

    }

// ---------------------------------------------------------------------------

CCreatorEngine::~CCreatorEngine()
    {
    LOGSTRING("Creator: CCreatorEngine::~CCreatorEngine");

    Cancel();
    
    for( TInt i = 0; i < iStringArrays.Count(); ++i )
        {
        delete iStringArrays[i].iArrayPtr;
        }
    iStringArrays.Close();
    iAllocatedFilePaths.ResetAndDestroy();
    iAllocatedFilePaths.Close();
    iTimer.Close();


	iEnv->DeleteResourceFile(iResourceFileId);
    
	iMemoryDetailsList.Reset();
    delete iPictureFileArray;
    delete iSoundFileArray;
    delete iTempPath;
    delete iFrameImageData;
    delete iEncoder;
    delete iScaler;
    delete iDecoder;
    delete iScaledBitmap;
    delete iBitmap;
    delete iBitmapData;
	
    delete iCmdScriptRun;
    iCmdScriptRun = NULL;
    
    delete iCommandParser;
    }

void CCreatorEngine::CopyFileL(const TFileName& aSourceFile, const TFileName& aTargetFile, TBool aOverwrite )
	{
	if( !aOverwrite && ConeUtils::FileExists(aTargetFile))
		{
		// File already exists
		return;
		}
	
	// Make sure that the path exists. Creates the directory if it does not exist already:
	ConeUtils::EnsurePathExistsL(aTargetFile);	
	User::LeaveIfError(BaflUtils::CopyFile(CEikonEnv::Static()->FsSession(), aSourceFile, aTargetFile));
	}

// ---------------------------------------------------------------------------
	
void CCreatorEngine::RequestMemoryDetails() 
    {

	TDriveList dl;
    // variables for memory and disk handling
    TVolumeInfo vinfo;

    //*************************************************************************
   
    //*************************************************************************
    // get an access to file server
    RFs& fsSession = CEikonEnv::Static()->FsSession();

    fsSession.DriveList(dl);
    
    for(TInt driveNum = EDriveC; driveNum <= EDriveZ ; driveNum ++ )
    	{
		if( dl[driveNum] )
			{
			TMemoryDetails *memDet = new TMemoryDetails;
			if(fsSession.Volume(vinfo, driveNum) == KErrNone)
				{
				memDet->iFree.Num(TInt64(vinfo.iFree/1024));
				memDet->iSize.Num(TInt64(vinfo.iSize/1024));
				}
			else
				{
				memDet->iFree.Num( 0 );
				memDet->iSize.Num( 0 );
				}
			User::LeaveIfError(fsSession.DriveToChar(driveNum,memDet->iDriveLetter));
			iMemoryDetailsList.AppendL( memDet );
			}
    	}
  
    // available work memory
    TMemoryInfoV1Buf memory;
    UserHal::MemoryInfo(memory);
    TInt64 freeMemBytes=(TInt64)(memory().iFreeRamInBytes);
    TInt64 sizeMemBytes=(TInt64)(memory().iTotalRamInBytes);
    TInt64 sizeWorkMemBytes = sizeMemBytes; 
    TInt64 sizeRomBytes = (TInt64)(memory().iTotalRomInBytes);
    iMemoryDetails.iRamFree.Num(TInt64(freeMemBytes/1024));
    iMemoryDetails.iRamSize.Num(TInt64(sizeWorkMemBytes/1024));
    iMemoryDetails.iRomSize.Num(TInt64(sizeRomBytes/1024));
    }

// ---------------------------------------------------------------------------
	
TBool CCreatorEngine::MMC_OK() const
	{
	TBool MMCstatus = EFalse;

	TDriveNumber eDrive=EDriveE;
    TVolumeInfo vinfo;
    TInt err = CEikonEnv::Static()->FsSession().Volume(vinfo, eDrive);
	// check if we can access the E-drive:
	if ( err == KErrNone)
		{
		MMCstatus = ETrue;
		}
	else
		{
		MMCstatus = EFalse;
		}

	// returns ETrue if MMC working properly
	return MMCstatus;
	}	
	
// ---------------------------------------------------------------------------

void CCreatorEngine::ExecuteFirstCommandL(const TDesC& aText)
    {
    LOGSTRING("Creator: CCreatorEngine::ExecuteFirstCommand");

    if( !iCommandArray->Count())
        {
        ShutDownEnginesL();
        return;
        }
    
    // show a progress dialog
    iEngineWrapper->ShowProgressBar(aText, CommandArrayCount());	

    iFailedCommands = 0;

    // starts executing commands
    ExecuteCommand();

	
    }

// ---------------------------------------------------------------------------

void CCreatorEngine::ExecuteCommand()
    {
    LOGSTRING("Creator: CCreatorEngine::ExecuteCommand");

    // make sure the engine isn't active, should never happen
    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("IsActive"), 500));

    // execute a command after a very short delay (75ms)
    iTimer.After(iStatus, 75);
    SetActive();
    }

// ---------------------------------------------------------------------------

void CCreatorEngine::RunL()
    {
    LOGSTRING("Creator: CCreatorEngine::RunL");
    LOGSTRING2("Creator: CCreatorEngine::RunL iCommandId=%d", iCommandArray->At(iCurrentEntry).iCommandId);
    
    if ( iUserCancelled ) 
    	{
    	return;
    	}
    
    if (!iCommandArray->At(iCurrentEntry).iParameters)
        {
        LOGSTRING("Creator: CCreatorEngine::RunL - iParameters==NULL !");
        }

    // launch a command

    TCreatorIds cmd = (TCreatorIds)iCommandArray->At(iCurrentEntry).iCommandId; 
    switch( cmd )
        {
/*        
	    case ECmdCreateBrowserBookmarkEntries: { iBrowser->CreateBookmarkEntryL(reinterpret_cast<CBrowserParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
	    case ECmdCreateBrowserBookmarkFolderEntries: { iBrowser->CreateBookmarkFolderEntryL(reinterpret_cast<CBrowserParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
	    case ECmdCreateBrowserSavedPageEntries: { iBrowser->CreateSavedDeckEntryL(reinterpret_cast<CBrowserParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
	    case ECmdCreateBrowserSavedPageFolderEntries: { iBrowser->CreateSavedDeckFolderEntryL(reinterpret_cast<CBrowserParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
*/	    
        case ECmdCreateCalendarEntryAppointments: { iCalendar->CreateAppointmentEntryL(iCommandArray->At(iCurrentEntry).iParameters); } break;
	    case ECmdCreateCalendarEntryEvents: { iCalendar->CreateEventEntryL(iCommandArray->At(iCurrentEntry).iParameters); } break;
	    case ECmdCreateCalendarEntryAnniversaries: { iCalendar->CreateAnniversaryEntryL(iCommandArray->At(iCurrentEntry).iParameters); } break;
	    case ECmdCreateCalendarEntryToDos: { iCalendar->CreateTodoEntryL(iCommandArray->At(iCurrentEntry).iParameters); } break;
        case ECmdCreateCalendarEntryReminders: { iCalendar->CreateReminderEntryL(iCommandArray->At(iCurrentEntry).iParameters); } break;

	    
	    case ECmdCreatePhoneBookEntryContacts: { iPhonebook->CreateContactEntryL(iCommandArray->At(iCurrentEntry).iParameters) ; } break;
	    case ECmdCreatePhoneBookEntryGroups: { iPhonebook->CreateGroupEntryL(iCommandArray->At(iCurrentEntry).iParameters) ; } break;
	    case ECmdCreatePhoneBookEntrySubscribedContacts: { iPhonebook->CreateSubscribedContactEntryL(iCommandArray->At(iCurrentEntry).iParameters) ; } break;

	    case ECmdCreateMiscEntryNotes: { iNotepad->CreateNoteEntryL(reinterpret_cast<CNotepadParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;

	    case ECmdCreateLogEntryMissedCalls: { iLogs->CreateMissedCallEntryL(reinterpret_cast<CLogsParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
	    case ECmdCreateLogEntryReceivedCalls: { iLogs->CreateReceivedCallEntryL(reinterpret_cast<CLogsParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
	    case ECmdCreateLogEntryDialledNumbers: { iLogs->CreateDialledNumberEntryL(reinterpret_cast<CLogsParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
/*
        case ECmdCreateMiscEntryAccessPoints: { iAccessPoints->CreateConnectionSettingsEntryL(iCommandArray->At(iCurrentEntry).iParameters); } break;
        case ECmdDeleteIAPs: { iAccessPoints->DeleteAllL(); } break;
        case ECmdDeleteCreatorIAPs: { iAccessPoints->DeleteAllCreatedByCreatorL(); } break;
*/
        case ECmdCreateMiscEntryLandmarks: { iLandmarks->CreateLandmarkEntryL(reinterpret_cast<CLandmarkParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
        
	    case ECmdCreateMessagingEntryMailboxes: { iMailboxes->CreateMailboxEntryL(reinterpret_cast<CMailboxesParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
/*
        #ifdef __PRESENCE
  	     case ECmdCreateMiscEntryIMPSServers: { iIMPS->CreateIMPSServerEntryL(reinterpret_cast<CIMPSParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;
        #endif
*/
	    case ECmdCreateMessagingEntryMessages:  { iMessages->CreateMessageEntryL(reinterpret_cast<CMessagesParameters*>(iCommandArray->At(iCurrentEntry).iParameters), ETrue); } break;
	    case ECmdCreateMessagingEntryMessagesViaScript:  { iMessages->CreateMessageEntryL(reinterpret_cast<CMessagesParameters*>(iCommandArray->At(iCurrentEntry).iParameters)); } break;

	    case ECmdCreateRandomEntrySMSInbox:
	    case ECmdCreateRandomEntrySMSDrafts:
	    case ECmdCreateRandomEntrySMSOutbox:
	    case ECmdCreateRandomEntrySMSSent:
	    case ECmdCreateRandomEntryMMSInbox:
	    case ECmdCreateRandomEntryMMSDrafts:
	    case ECmdCreateRandomEntryMMSOutbox:
	    case ECmdCreateRandomEntryMMSSent:
	    case ECmdCreateRandomEntryAMSInbox:
	    case ECmdCreateRandomEntryAMSDrafts:
	    case ECmdCreateRandomEntryAMSOutbox:
	    case ECmdCreateRandomEntryAMSSent:
	    case ECmdCreateRandomEntryEmailInbox:
	    case ECmdCreateRandomEntryEmailDrafts:
	    case ECmdCreateRandomEntryEmailOutbox:
	    case ECmdCreateRandomEntryEmailSent:
	    case ECmdCreateRandomEntryBIOInbox:
	    case ECmdCreateRandomEntryBIODrafts:
	    case ECmdCreateRandomEntryBIOOutbox:
	    case ECmdCreateRandomEntryBIOSent:
	    case ECmdCreateRandomEntryIRInbox:
	    case ECmdCreateRandomEntryIRDrafts:
	    case ECmdCreateRandomEntryIROutbox:
	    case ECmdCreateRandomEntryIRSent:
	    case ECmdCreateRandomEntryBTInbox:
	    case ECmdCreateRandomEntryBTDrafts:
	    case ECmdCreateRandomEntryBTOutbox:
	    case ECmdCreateRandomEntryBTSent:
	            { iMessages->CreateRandomMessageEntryL(iCommandArray->At(iCurrentEntry).iCommandId); } break;

        case ECmdCreateFileEntryEmptyFolder:
        case ECmdCreateFileEntry3GPP_70kB:
        case ECmdCreateFileEntryAAC_100kB:
        case ECmdCreateFileEntryAMR_20kB:
        case ECmdCreateFileEntryBMP_25kB:
        case ECmdCreateFileEntryDeck_1kB:
        case ECmdCreateFileEntryDOC_20kB:
        case ECmdCreateFileEntryGIF_2kB:
        case ECmdCreateFileEntryHTML_20kB:
        case ECmdCreateFileEntryJAD_1kB:
        case ECmdCreateFileEntryJAR_10kB:
        case ECmdCreateFileEntryJP2_65kB:
        case ECmdCreateFileEntryJPEG_200kB:
        case ECmdCreateFileEntryJPEG_25kB:
        case ECmdCreateFileEntryJPEG_500kB:
        case ECmdCreateFileEntryMIDI_10kB:
        case ECmdCreateFileEntryMP3_250kB:
        case ECmdCreateFileEntryMP4_200kB:
        case ECmdCreateFileEntryMXMF_40kB:
        case ECmdCreateFileEntryPNG_15kB:
        case ECmdCreateFileEntryPPT_40kB:
        case ECmdCreateFileEntryRAM_1kB:
        case ECmdCreateFileEntryRM_95kB:
        case ECmdCreateFileEntryRNG_1kB:
        case ECmdCreateFileEntrySVG_15kB:
        case ECmdCreateFileEntrySWF_15kB:
        case ECmdCreateFileEntryTIF_25kB:
        case ECmdCreateFileEntryTXT_10kB:
        case ECmdCreateFileEntryTXT_70kB:
        case ECmdCreateFileEntryVCF_1kB:
        case ECmdCreateFileEntryVCS_1kB:
        case ECmdCreateFileEntryWAV_20kB:
        case ECmdCreateFileEntryXLS_15kB:
        case ECmdCreateFileEntrySISX_10kB:
        case ECmdCreateFileEntryWMA_50kB:
        case ECmdCreateFileEntryWMV_200kB:
	            { iFiles->CreateFileEntryL( reinterpret_cast<CFilesParameters*>(iCommandArray->At(iCurrentEntry).iParameters), iCommandArray->At(iCurrentEntry).iCommandId ); } break;
        case ECmdDeleteContacts:
            {
            iPhonebook->DeleteAllL(); 
            break;
            }
        case ECmdDeleteContactGroups:
            {
            iPhonebook->DeleteAllGroupsL();
            break;            
            }
        case ECmdDeleteCreatorContacts:
            {
            iPhonebook->DeleteAllCreatedByCreatorL();
            break;
            }
        case ECmdDeleteCreatorContactGroups:
            {
            iPhonebook->DeleteAllGroupsCreatedByCreatorL();
            break;            
            }
        case ECmdDeleteCalendarEntries:
            {
            iCalendar->DeleteAllL();
            break;
            }
        case ECmdDeleteCreatorCalendarEntries:
            {
            iCalendar->DeleteAllCreatedByCreatorL();
            break;
            }
        case ECmdDeleteLogs:
            {
            iLogs->DeleteAllL();
            break;
            }
        case ECmdDeleteCreatorLogs:
            {
            iLogs->DeleteAllCreatedByCreatorL();
            break;
            }
        case ECmdDeleteMessages:
            {
            iMessages->DeleteAllL();
            break;
            }
        case ECmdDeleteCreatorMessages:
            {
            iMessages->DeleteAllCreatedByCreatorL();
            break;
            }
        case ECmdDeleteLandmarks:
            {
            iLandmarks->DeleteAllL();
            break;
            }
        case ECmdDeleteCreatorLandmarks:
            {
            iLandmarks->DeleteAllCreatedByCreatorL();
            break;
            }
        case ECmdDeleteMailboxes:
            {
            iMailboxes->DeleteAllL();
            break;
            }
        case ECmdDeleteCreatorMailboxes:
            {
            iMailboxes->DeleteAllCreatedByCreatorL();
            break;
            }

        case ECmdDeleteNotes:
            {
            iNotepad->DeleteAllL();
            break;
            }

        case ECmdDeleteIMPSs:
            {
//            iIMPS->DeleteAllL();
            break;
            }
        case ECmdDeleteCreatorIMPSs:
            {
//            iIMPS->DeleteAllCreatedByCreatorL();
            break;
            }
/*            
        case ECmdDeleteBrowserBookmarks:
            {
            iBrowser->DeleteAllBookmarksL();
            break;
            }
        case ECmdDeleteCreatorBrowserBookmarks:
            {
            iBrowser->DeleteAllBookmarksCreatedByCreatorL();
            break;
            }
        case ECmdDeleteBrowserBookmarkFolders:
            {
            iBrowser->DeleteAllBookmarkFoldersL();
            break;
            }
        case ECmdDeleteCreatorBrowserBookmarkFolders:
            {
            iBrowser->DeleteAllBookmarkFoldersCreatedByCreatorL();
            break;
            }
        case ECmdDeleteBrowserSavedPages:
            {
            iBrowser->DeleteAllSavedPagesL();
            break;
            }
        case ECmdDeleteCreatorBrowserSavedPages:
            {
            iBrowser->DeleteAllSavedPagesCreatedByCreatorL();
            break;
            }
        case ECmdDeleteBrowserSavedPageFolders:
            {
            iBrowser->DeleteAllSavedPageFoldersL();
            break;
            }
        case ECmdDeleteCreatorBrowserSavedPageFolders:
            {
            iBrowser->DeleteAllSavedPageFoldersCreatedByCreatorL();
            break;
            }
*/            
        case ECmdDeleteCreatorFiles:
            {
            iFiles->DeleteAllCreatedByCreatorL();
            break;
            }
        default:
            User::Panic (_L("ExecuteOptionsMenuCommandL"), 205);
            break;
        }

    TRAP_IGNORE( CheckForMoreCommandsL() );

    }
    
CDesCArrayFlat* CCreatorEngine::PictureFilesL()
    {
    if( iPictureFileArray == 0 )
        {
        _LIT(KImageFile, "JPEG*.jpg");
        _LIT(KImageFileDir, "C:\\Data\\");
        _LIT(KImageFileDir2, "Z:\\Data\\");
        iPictureFileArray = new (ELeave) CDesCArrayFlat(8);        
        TInt err = CreatorFileUtils::FindFilesRecursiveL(iPictureFileArray, KImageFile, KImageFileDir);
        if( iPictureFileArray->Count() == 0 )
            {
            err = CreatorFileUtils::FindFilesRecursiveL(iPictureFileArray, KImageFile, KImageFileDir2);
            }
        }
    return iPictureFileArray;
    }

CDesCArrayFlat* CCreatorEngine::SoundFilesL()
    {
    if( iSoundFileArray == 0 )
        {
        _LIT(KSoundFile, "*.aac");
        _LIT(KSoundFileDir, "Z:\\data\\Sounds\\");
        iSoundFileArray = new (ELeave) CDesCArrayFlat(8);                  
        TInt err = CreatorFileUtils::FindFilesRecursiveL(iSoundFileArray, KSoundFile, KSoundFileDir);
        }
    return iSoundFileArray;
    }
// ---------------------------------------------------------------------------

TInt CCreatorEngine::RunError(TInt aError)
    {
    LOGSTRING2("Creator: CCreatorEngine::RunError %d", aError);
    
    iFailedCommands++;
  
    _LIT(KMessage, "Command error %d");
    TBuf<128> noteMsg;
    noteMsg.Format(KMessage, aError);
//    iEnv->InfoMsg(noteMsg);
	iEngineWrapper->ShowNote(noteMsg);  
    TRAP_IGNORE( CheckForMoreCommandsL() );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------

void CCreatorEngine::CheckForMoreCommandsL()
    {
    LOGSTRING("Creator: CCreatorEngine::CheckForMoreCommandsL");
    if ( iUserCancelled ) 
		{
		return;
		}
    // update the progress bar
    //iProgressInfo->IncrementAndDraw(1);
	iEngineWrapper->IncrementProgressbarValue();

	// check if we have more commands to be executed
	if (iCurrentEntry >= CommandArrayCount() - 1)
		{
        LOGSTRING("Creator: CCreatorEngine::CheckForMoreCommandsL all done");

        // all done, free resources and show a note
        ShutDownEnginesL();

		//iProgressDialog->ProcessFinishedL();
		
		// remove the progress dialog from the screen
        iEngineWrapper->CloseProgressbar();
		
		if (iFailedCommands == 0)
		    {
		    iEngineWrapper->ShowNote(_L("Done"));
		    }
		else
		    {
			_LIT(KMessage, "Done, %d commands failed");
            TBuf<128> noteMsg;
            noteMsg.Format(KMessage, iFailedCommands);
			iEngineWrapper->ShowNote(noteMsg);
		    }
		
	    iCmdScriptRun->RunScriptDone();

		}
	else
		{
		// maintain requests
		iCurrentEntry++;

        LOGSTRING2("Creator: CCreatorEngine::CheckForMoreCommandsL iCurrentEntry=%d", iCurrentEntry);

        ExecuteCommand();
		}
    }

// ---------------------------------------------------------------------------
void CCreatorEngine::SortCommands()
	{
	TKeyArrayFix iBufKey(_FOFF(TCommand, iCommandId),ECmpTInt);
	iCommandArray->Sort(iBufKey);
	}
// ---------------------------------------------------------------------------

// This callback function is called when cancel button of the progress bar was pressed

void CCreatorEngine::ProgressDialogCancelledL()
    {
    LOGSTRING("Creator: CCreatorEngine::ProgressDialogCancelledL");

        iUserCancelled = ETrue;
        // cancel the active object, command executer 
        Cancel();

        // delete parameters from the command array, otherwise there might be memory leaks
        for (TInt i=iCurrentEntry; i<CommandArrayCount(); i++)
            {
            if ( iCommandArray->At(i).iParameters )
				{
                delete iCommandArray->At(i).iParameters;
                iCommandArray->At(i).iParameters = NULL;
				}
            }

        if ( iDecoder  )
            {
            iDecoder->Cancel();
            // CancelComplete() will be called from GenerateSourceImageFileL
            }
        else if ( iScaler )
            {
            iScaler->Cancel();
            // CancelComplete() will be called from GenerateSourceImageFileL
            }
        else if ( iEncoder  )
            {
            iEncoder->Cancel();
            // CancelComplete() will be called from GenerateSourceImageFileL
            }
        else
            {
            // free resources and show a note
            ShutDownEnginesL();
            iEngineWrapper->ShowNote(_L("Cancelled"));
            }
        
    }

// ---------------------------------------------------------------------------

void CCreatorEngine::StartEnginesL()
    {
    LOGSTRING("Creator: CCreatorEngine::StartEnginesL");
    
    // reset user cancel flag
    iUserCancelled = EFalse;
    
    // start from the beginning
    iCurrentEntry = 0;

    // init the command array
    __ASSERT_ALWAYS(!iCommandArray, User::Panic(_L("iCommandArray"), 701));
    iCommandArray = new(ELeave) CCommandArray(10000);

    // init all modules here
    TInt err(KErrNone);
//    TRAP(err, iBrowser = CCreatorBrowser::NewL(this));
    TRAP(err, iCalendar = CCreatorInterimCalendar::NewL(this));
    TRAP(err, iPhonebook = (CCreatorPhonebookBase*)TCreatorFactory::CreatePhoneBookL(this)); 
    TRAP(err, iNotepad = CCreatorNotepad::NewL(this));
    TRAP(err, iLogs = CCreatorLogs::NewL(this));
//    TRAP(err, iAccessPoints = (CCreatorConnectionSettingsBase*)TCreatorFactory::CreateConnectionSettingsL(this));
    TRAP(err, iMailboxes = CCreatorMailboxes::NewL(this));
    TRAP(err, iFiles = CCreatorFiles::NewL(this));
    TRAP(err, iMessages = CCreatorMessages::NewL(this));
    TRAP(err, iLandmarks = CCreatorLandmarks::NewL(this));
    ContactLinkCache::InitializeL();
/*
    #ifdef __PRESENCE
      TRAP(err, iIMPS = CCreatorIMPS::NewL(this));
    #endif 
*/
      
      
    // ...
    // ...
    // ...

    }
// ---------------------------------------------------------------------------

void CCreatorEngine::ShutDownEnginesL()
    {
    LOGSTRING("Creator: CCreatorEngine::ShutDownEnginesL");

        
    // delete all modules here and free resources which aren't anymore needed

/*
    #ifdef __PRESENCE
     delete iIMPS;
     iIMPS = NULL;
    #endif
*/
    delete iMessages;
    iMessages = NULL;
    delete iFiles;
    iFiles = NULL;
    delete iMailboxes;
    iMailboxes = NULL;
 //   delete iAccessPoints;
//    iAccessPoints = NULL;
    delete iLogs;
    iLogs = NULL;
    delete iNotepad;
    iNotepad = NULL;
    delete iPhonebook;
    iPhonebook = NULL;
    delete iCalendar;
    iCalendar = NULL;
 //   delete iBrowser;
 //   iBrowser = NULL;
    delete iLandmarks;
    iLandmarks = NULL;

    ContactLinkCache::DestroyL();
    // now delete the command array
    delete iCommandArray;
    iCommandArray = NULL;
	
	// delete parameter array
	iParameterArray.ResetAndDestroy();
	iParameterArray.Close();
	
	if(iCommandParser)
	    {
        delete iCommandParser;
	    iCommandParser = NULL;
	    }
    
    
	// clear temp drive
	CFileMan* fileMan = CFileMan::NewL( iEnv->FsSession() );
	CleanupStack::PushL( fileMan );
	fileMan->RmDir( iTempPath->Des() );
	CleanupStack::PopAndDestroy(); //fileMan
    }

// ---------------------------------------------------------------------------

void CCreatorEngine::DoCancel()
    {
    LOGSTRING("Creator: CCreatorEngine::DoCancel");

    iTimer.Cancel();
    }

void CCreatorEngine::ScriptChosenL( TBool aSuccess, const TDesC& aFileName )
    {
    if( aSuccess )
        {
        // open the file for reading
        RFile scriptFile;
        User::LeaveIfError( scriptFile.Open( iEnv->FsSession(), aFileName, EFileRead ) );
        CleanupClosePushL(scriptFile);
    
        
        // wait dialog
        /* TODO
        CAknGlobalNote* waitDialog = CAknGlobalNote::NewLC();
        waitDialog->SetSoftkeys( R_AVKON_SOFTKEYS_CANCEL );
        TInt dialogId = waitDialog->ShowNoteL( EAknGlobalWaitNote, _L("Parsing") );
        */
        TInt parseErr( KErrNone );
        CCreatorScriptParser* scriptParser = CCreatorScriptParser::NewLC(this);
        scriptParser->ParseL(scriptFile);
        parseErr = scriptParser->GetError();
        CleanupStack::PopAndDestroy( scriptParser );
    
        CleanupStack::PopAndDestroy( &scriptFile );
    
        //waitDialog->CancelNoteL( dialogId );
        //CleanupStack::PopAndDestroy( waitDialog );
    
        if(parseErr != KErrNone)
            {
            // show error note
            _LIT(KErrMsg, "Parser error: %d");
            TBuf<32> msgBuf;
            msgBuf.Format(KErrMsg, parseErr);
            iEngineWrapper->ShowErrorMessage(msgBuf);
            ShutDownEnginesL();
            return;
            }         
        }

    // start executing commands if commands in the command array
    if (CommandArrayCount() > 0)
        {
        ExecuteFirstCommandL( KSavingText );
        }
    else
        {
        ShutDownEnginesL();
        }
    }
// ---------------------------------------------------------------------------

void CCreatorEngine::RunScriptL()
    {
    LOGSTRING("Creator: CCreatorEngine::RunScriptL");

    // startup modules (also inits the command array):
    StartEnginesL();

    // use the command parser module to init the command array from a script file
    delete iCommandParser;
    iCommandParser = NULL;
    iCommandParser = CCommandParser::NewL(this);
    TBool ret = EFalse;
    TRAPD(err, ret = iCommandParser->OpenScriptL( this ) );
    if( err != KErrNone || ret == EFalse)
        {
        ShutDownEnginesL();
        User::LeaveIfError(err);
        }
    }

// ---------------------------------------------------------------------------

TInt CCreatorEngine::RunScriptL(const TDesC& aScriptFile)
    {
    LOGSTRING("Creator: CCreatorEngine::RunScriptL");

    // startup modules (also inits the command array):
    StartEnginesL();

    RFile scriptFile;
    if (scriptFile.Open(CEikonEnv::Static()->FsSession(), aScriptFile, EFileRead) != KErrNone)
    	{
    	ShutDownEnginesL();
    	return KErrNotFound; // file error
    	}
    
    CleanupClosePushL(scriptFile);
    CCreatorScriptParser* scriptParser = CCreatorScriptParser::NewLC(this);
    scriptParser->ParseL(scriptFile);
    TInt err = scriptParser->GetError();
    CleanupStack::PopAndDestroy(); //scriptParser
    CleanupStack::Pop(); // scriptFile
    scriptFile.Close();
        
    if (err != KErrNone)
        {
        ShutDownEnginesL();
        return KErrCorrupt; // parser error
        }         

    // start executing commands if commands in the command array
    if (CommandArrayCount() > 0)
        {
        ExecuteFirstCommandL( KSavingText );
        return KErrNone; // executing commands is started
        }
    else
        {
        ShutDownEnginesL();
        return KErrCompletion; // nothing to do
        }
    }

// ---------------------------------------------------------------------------
void CCreatorEngine::QueryDialogClosedL( TBool aPositiveAction, TInt aUserData )
    {
    LOGSTRING("Creator: CCreatorEngine::QueryDialogClosedL");
    
    if( aPositiveAction == EFalse )
        {
        ShutDownEnginesL();
        return;
        }

    switch(aUserData)
        {
        case ECreatorEngineDeleteAllEntries:
            AppendToCommandArrayL( ECmdDeleteContacts, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteContactGroups, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCalendarEntries, NULL, 1 );
            //AppendToCommandArrayL( ECmdDeleteBrowserBookmarks, NULL, 1 );
            //AppendToCommandArrayL( ECmdDeleteBrowserBookmarkFolders, NULL, 1 );
            //AppendToCommandArrayL( ECmdDeleteBrowserSavedPages, NULL, 1 );
            //AppendToCommandArrayL( ECmdDeleteBrowserSavedPageFolders, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteLogs, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteMessages, NULL, 1 );
            //AppendToCommandArrayL( ECmdDeleteIAPs, NULL, 1 );
            //AppendToCommandArrayL( ECmdDeleteIMPSs, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteNotes, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteLandmarks, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCreatorFiles, NULL, 1 );
            
            // started exucuting delete commands
            ExecuteFirstCommandL( KDeletingText );
            break;
        case ECreatorEngineDeleteAllCreatorEntries:
            AppendToCommandArrayL( ECmdDeleteCreatorContacts, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCreatorContactGroups, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCreatorCalendarEntries, NULL, 1 );
            /*
            AppendToCommandArrayL( ECmdDeleteCreatorBrowserBookmarks, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCreatorBrowserBookmarkFolders, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCreatorBrowserSavedPages, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCreatorBrowserSavedPageFolders, NULL, 1 );
            */
            AppendToCommandArrayL( ECmdDeleteCreatorFiles, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCreatorLogs, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCreatorMessages, NULL, 1 );
            //AppendToCommandArrayL( ECmdDeleteCreatorIAPs, NULL, 1 );
            //AppendToCommandArrayL( ECmdDeleteCreatorIMPSs, NULL, 1 );
            AppendToCommandArrayL( ECmdDeleteCreatorLandmarks, NULL, 1 );
            
            // started exucuting delete commands
            ExecuteFirstCommandL( KDeletingText );
            break;
        default:
            //some error
             ShutDownEnginesL();
            break;
        }
    }

// ---------------------------------------------------------------------------

void CCreatorEngine::ExecuteOptionsMenuCommandL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorEngine::ExecuteOptionsMenuCommandL");

    // startup modules (also inits the command array):
    StartEnginesL();

    // set the home module
    switch(aCommand)
        {
/*
        case ECmdCreateBrowserBookmarkEntries:
	    case ECmdCreateBrowserBookmarkFolderEntries:
	    case ECmdCreateBrowserSavedPageEntries:
	    case ECmdCreateBrowserSavedPageFolderEntries:
	    case ECmdDeleteBrowserBookmarks:
	    case ECmdDeleteBrowserBookmarkFolders:
	    case ECmdDeleteBrowserSavedPages:
	    case ECmdDeleteBrowserSavedPageFolders:
        case ECmdDeleteCreatorBrowserBookmarks:
        case ECmdDeleteCreatorBrowserBookmarkFolders:
        case ECmdDeleteCreatorBrowserSavedPages:
        case ECmdDeleteCreatorBrowserSavedPageFolders:
            {
            iUsedOptionsMenuModule = iBrowser;
            }
            break;
*/	    
        case ECmdCreateCalendarEntryAppointments:
	    case ECmdCreateCalendarEntryEvents:
	    case ECmdCreateCalendarEntryAnniversaries:
	    case ECmdCreateCalendarEntryToDos:
	    case ECmdCreateCalendarEntryReminders:
	    case ECmdDeleteCalendarEntries:
	    case ECmdDeleteCreatorCalendarEntries:
            {
            iUsedOptionsMenuModule = iCalendar;
            }
            break;

	    case ECmdCreatePhoneBookEntryContacts:
	    case ECmdCreatePhoneBookEntryGroups:
	    case ECmdCreatePhoneBookEntrySubscribedContacts:
	    case ECmdDeleteContacts:
	    case ECmdDeleteCreatorContacts:
        case ECmdDeleteContactGroups:
        case ECmdDeleteCreatorContactGroups:
            {
            iUsedOptionsMenuModule = iPhonebook;
            }
            break;

	    case ECmdCreateMiscEntryNotes:
	    case ECmdDeleteNotes:
            {
            iUsedOptionsMenuModule = iNotepad;
            }
            break;

	    case ECmdCreateLogEntryMissedCalls:
	    case ECmdCreateLogEntryReceivedCalls:
	    case ECmdCreateLogEntryDialledNumbers:
	    case ECmdDeleteLogs:
	    case ECmdDeleteCreatorLogs:
            {
            iUsedOptionsMenuModule = iLogs;
            }
            break;
/*
	    case ECmdCreateMiscEntryAccessPoints:
	    case ECmdDeleteIAPs:
	    case ECmdDeleteCreatorIAPs:
            {
            iUsedOptionsMenuModule = iAccessPoints;
            }
            break;
*/
	    case ECmdCreateMessagingEntryMailboxes:
	    case ECmdDeleteMailboxes:
	    case ECmdDeleteCreatorMailboxes:
            {
            iUsedOptionsMenuModule = iMailboxes;
            }
            break;
/*
        #ifdef __PRESENCE
	      case ECmdCreateMiscEntryIMPSServers:
	      case ECmdDeleteIMPSs:
	      case ECmdDeleteCreatorIMPSs:
            {
            iUsedOptionsMenuModule = iIMPS;
            }
            break;
        #endif
*/
	      case ECmdCreateFileEntryEmptyFolder:
	      case ECmdCreateFileEntry3GPP_70kB:
	      case ECmdCreateFileEntryAAC_100kB:
	      case ECmdCreateFileEntryAMR_20kB:
	      case ECmdCreateFileEntryBMP_25kB:
	      case ECmdCreateFileEntryDeck_1kB:
	      case ECmdCreateFileEntryDOC_20kB:
	      case ECmdCreateFileEntryGIF_2kB:
	      case ECmdCreateFileEntryHTML_20kB:
	      case ECmdCreateFileEntryJAD_1kB:
	      case ECmdCreateFileEntryJAR_10kB:
	      case ECmdCreateFileEntryJP2_65kB:
	      case ECmdCreateFileEntryJPEG_200kB:
	      case ECmdCreateFileEntryJPEG_25kB:
	      case ECmdCreateFileEntryJPEG_500kB:
	      case ECmdCreateFileEntryMIDI_10kB:
	      case ECmdCreateFileEntryMP3_250kB:
	      case ECmdCreateFileEntryMP4_200kB:
	      case ECmdCreateFileEntryMXMF_40kB:
	      case ECmdCreateFileEntryPNG_15kB:
	      case ECmdCreateFileEntryPPT_40kB:
	      case ECmdCreateFileEntryRAM_1kB:
	      case ECmdCreateFileEntryRM_95kB:
	      case ECmdCreateFileEntryRNG_1kB:
	      case ECmdCreateFileEntrySVG_15kB:
	      case ECmdCreateFileEntrySWF_15kB:
	      case ECmdCreateFileEntryTIF_25kB:
	      case ECmdCreateFileEntryTXT_10kB:
	      case ECmdCreateFileEntryTXT_70kB:
	      case ECmdCreateFileEntryVCF_1kB:
	      case ECmdCreateFileEntryVCS_1kB:
	      case ECmdCreateFileEntryWAV_20kB:
	      case ECmdCreateFileEntryXLS_15kB:
	      case ECmdCreateFileEntrySISX_10kB:
	      case ECmdCreateFileEntryWMA_50kB:
	      case ECmdCreateFileEntryWMV_200kB:
	      case ECmdDeleteCreatorFiles:

            {
            iUsedOptionsMenuModule = iFiles;
            }
            break;


        case ECmdCreateMessagingEntryMessages:
        case ECmdDeleteMessages:
        case ECmdDeleteCreatorMessages:
            {
            iUsedOptionsMenuModule = iMessages;
            }
            break;
            
        case ECmdCreateMiscEntryLandmarks:
        case ECmdDeleteLandmarks:
        case ECmdDeleteCreatorLandmarks:
            {
            iUsedOptionsMenuModule = iLandmarks;
            }
            break;
        case ECmdDeleteAllEntries:
        case ECmdDeleteAllCreatorEntries:
            {
            break;
            }
        default:
            User::Panic (_L("ExecuteOptionsMenuCommandL"), 201);
            break;
        }


    //If it's a delete command, asking do you really want to delete
    if( IsDeleteCommand( aCommand ) )
        {
        if ( aCommand == ECmdDeleteAllEntries )
            {
            if ( !iEngineWrapper->YesNoQueryDialog( _L("Delete all entries?"), this, ECreatorEngineDeleteAllEntries) )
                {
                // cancelled, free resources
                ShutDownEnginesL();
                }
            }
        else if ( aCommand == ECmdDeleteAllCreatorEntries )
            {
            if ( !iEngineWrapper->YesNoQueryDialog( _L("Delete all entries created with Creator?"), this, ECreatorEngineDeleteAllCreatorEntries ) )
                {
                // cancelled, free resources
                ShutDownEnginesL();
                }
            }
        else if ( !iUsedOptionsMenuModule->AskDataFromUserL( aCommand )  )
            {
            // cancelled or error, free resources
            ShutDownEnginesL();
            }
        }

    // ask user data, if query accepted start processing...
    else if (!iUsedOptionsMenuModule->AskDataFromUserL(aCommand))
        {
        // cancelled or error, free resources
        ShutDownEnginesL();
        }
    }



TBool CCreatorEngine::IsDeleteCommand(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorEngine::IsDeleteCommand");

    switch(aCommand)
        {
        //Add supported delete command here        
        case ECmdDeleteAllEntries:
        case ECmdDeleteAllCreatorEntries:
        case ECmdDeleteEntries:
        case ECmdDeleteContacts:
        case ECmdDeleteCreatorContacts:
        case ECmdDeleteContactGroups:
        case ECmdDeleteCreatorContactGroups:
        case ECmdDeleteCalendarEntries:
        case ECmdDeleteCreatorCalendarEntries:
        case ECmdDeleteBrowserBookmarks:
        case ECmdDeleteCreatorBrowserBookmarks:
        case ECmdDeleteBrowserBookmarkFolders:
        case ECmdDeleteCreatorBrowserBookmarkFolders:
        case ECmdDeleteBrowserSavedPages:
        case ECmdDeleteCreatorBrowserSavedPages:
        case ECmdDeleteBrowserSavedPageFolders:
        case ECmdDeleteCreatorBrowserSavedPageFolders:
        case ECmdDeleteCreatorFiles:
        case ECmdDeleteLogs:
        case ECmdDeleteCreatorLogs:
        case ECmdDeleteMessages:
        case ECmdDeleteCreatorMessages:
        case ECmdDeleteMailboxes:
        case ECmdDeleteCreatorMailboxes:
        case ECmdDeleteIAPs:
        case ECmdDeleteCreatorIAPs:
        case ECmdDeleteIMPSs:
        case ECmdDeleteCreatorIMPSs:
        case ECmdDeleteNotes:
        case ECmdDeleteLandmarks:
        case ECmdDeleteCreatorLandmarks:
            {
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
     
    }
// ---------------------------------------------------------------------------
void CCreatorEngine::RandomDataFileChosenL( TBool aSuccess, const TDesC& aFileName )
    {
    if( aSuccess )
        {
        GetRandomDataFromFileL( aFileName );
        }
    }

// ---------------------------------------------------------------------------

TBool CCreatorEngine::GetRandomDataL()
	{
    delete iCommandParser;
    iCommandParser = NULL;
    iCommandParser = CCommandParser::NewL( this );
	return iCommandParser->GetRandomDataFilenameL( this );
	}

// ---------------------------------------------------------------------------

TBool CCreatorEngine::GetRandomDataFromFileL(const TDesC& aFilename)
	{
    // wait dialog
	// TODO
    //CAknGlobalNote* waitDialog = CAknGlobalNote::NewLC();
    //waitDialog->SetSoftkeys(R_AVKON_SOFTKEYS_CANCEL);
    //TInt dialogId = waitDialog->ShowNoteL(EAknGlobalWaitNote, _L("Reading random data"));

    for (TInt i = 0; i < iStringArrays.Count(); ++i)
        {
        delete iStringArrays[i].iArrayPtr;
        }
    iStringArrays.Reset();

    TBool errorDetected = EFalse;

    // NOTE: These must be exactly in same order as in enumeration:

    if (ReadRandomDataL(EFirstName, R_DATA_FIRSTNAMES, aFilename, CCreatorRandomDataParser::EFirstName) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(ESurname, R_DATA_SURNAMES, aFilename, CCreatorRandomDataParser::ESurname) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(ECompany, R_DATA_COMPANIES, aFilename, CCreatorRandomDataParser::ECompany) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EAddress, R_DATA_ADDRESSES, aFilename, CCreatorRandomDataParser::EAddress) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EJobTitle, R_DATA_JOBTITLES, aFilename, CCreatorRandomDataParser::EJobTitle) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EPhoneNumber, R_DATA_PHONENUMBERS, aFilename, CCreatorRandomDataParser::EPhoneNumber) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EGroupName, R_DATA_GROUPNAMES, aFilename, CCreatorRandomDataParser::EGroupName) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EMeetingReason, R_DATA_MEETINGREASONS, aFilename, CCreatorRandomDataParser::EMeetingReason) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EMeetingPlace, R_DATA_MEETINGPLACES, aFilename, CCreatorRandomDataParser::EMeetingPlace) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EMemoText, R_DATA_MEMOS, aFilename, CCreatorRandomDataParser::EMemoText) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EAnniversaryReason, R_DATA_ANNIVERSARIES, aFilename, CCreatorRandomDataParser::EAnniversaryReason) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EToDoText, R_DATA_TODOS, aFilename, CCreatorRandomDataParser::EToDoText) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EReminderText, R_DATA_REMINDERS, aFilename, CCreatorRandomDataParser::EReminderText) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EMessageSubject, R_DATA_MESSAGESUBJECTS, aFilename, CCreatorRandomDataParser::EMessageSubject) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EMessageText, R_DATA_MESSAGETEXTS, aFilename, CCreatorRandomDataParser::EMessageText) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(ECity, R_DATA_CITIES, aFilename, CCreatorRandomDataParser::ECity) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(ECountry, R_DATA_COUNTRIES, aFilename, CCreatorRandomDataParser::ECountry) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EPostcode, R_DATA_POSTCODES, aFilename, CCreatorRandomDataParser::EPostcode) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EState, R_DATA_STATES, aFilename, CCreatorRandomDataParser::EState) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EPobox, R_DATA_POBOXES, aFilename, CCreatorRandomDataParser::EPobox) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(EPrefix, R_DATA_PREFIXES, aFilename, CCreatorRandomDataParser::EPrefix) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(ESuffix, R_DATA_SUFFIXES, aFilename, CCreatorRandomDataParser::ESuffix) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(ELandmarkName, R_DATA_LANDMARK_NAMES, aFilename, CCreatorRandomDataParser::ELandmarkName) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    if (ReadRandomDataL(ELandmarkDescription, R_DATA_LANDMARK_DESCRIPTIONS, aFilename, CCreatorRandomDataParser::ELandmarkDescription) != KErrNone)
    	{
    	errorDetected = ETrue;
    	}
    
    // remove the wait dialog
    //waitDialog->CancelNoteL(dialogId);
    //CleanupStack::PopAndDestroy(waitDialog);
    
    if (errorDetected)
    	{
		iEngineWrapper->ShowNote(_L("Error in reading random data."));
   	}
    return !errorDetected;
	}

// ---------------------------------------------------------------------------

TInt CCreatorEngine::ReadRandomDataL(const TRandomStringType aRandomStringType,
		                             const TInt aResourceId,
		                             const TDesC& aFilename,
		                             const CCreatorRandomDataParser::TRandomDataType aRandomDataType)
	{
	TInt error = KErrNone;
	if (aFilename == KNullDesC)
		{
        iStringArrays.AppendL(TStringArrayPtr(aRandomStringType, iEnv->ReadDesCArrayResourceL(aResourceId)));
		}
	else
		{
	    CCreatorRandomDataParser* dataParser = CCreatorRandomDataParser::NewLC();
		CDesCArrayFlat* array = NULL;
	
		TRAP(error, array = dataParser->ParseL(aFilename, aRandomDataType));
		if (error == KErrNone)
			{
			error = dataParser->GetError();
			}
	    if ((error == KErrNone) && (array->MdcaCount() > 0))
	    	{
	    	iStringArrays.AppendL(TStringArrayPtr(aRandomStringType, array));
	    	}
	    else
	    	{
	    	if (array)
	    		{
	    		array->Reset();
	    		delete array;
	    		}
	        iStringArrays.AppendL(TStringArrayPtr(aRandomStringType, iEnv->ReadDesCArrayResourceL(aResourceId)));
	    	}
		CleanupStack::PopAndDestroy(dataParser);
		}
	return error;
	}

// ---------------------------------------------------------------------------

void CCreatorEngine::AppendToCommandArrayL(TInt aCommand, CCreatorModuleBaseParameters* aParameters, TInt aNumberOfEntries)
    {
    LOGSTRING("Creator: CCreatorEngine::AppendToCommandArrayL");
    iParameterArray.AppendL(aParameters);

    for (TInt i=0; i<aNumberOfEntries; i++)
        iCommandArray->AppendL( TCommand(aCommand, aParameters) );
    }

// ---------------------------------------------------------------------------

TInt CCreatorEngine::CommandArrayCount()
    {
    TInt count(0);
    
    if (iCommandArray)
        count = iCommandArray->Count();
    
    LOGSTRING2("Creator: CCreatorEngine::CommandArrayCount returns %d", count);
    
    return count;
    }

// ---------------------------------------------------------------------------

//returs fileid by its name, eg. 1 in 
TInt CCreatorEngine::GetAttachmentIdL( const TDesC& aAttachmentIdStr ) const
    {
    TInt mapCount = sizeof(FileMap) / sizeof(FileMapping);
    
    for( TInt i = 0; i < mapCount; ++i )
        {
        if( FileMap[i].FileName() == aAttachmentIdStr )
            return FileMap[i].FileId();
        }
    LOGSTRING2("CCreatorEngine::GetAttachmentIdL: Unknown attachment file id: %s", &aAttachmentIdStr);
    User::Leave(KErrGeneral);
    return KErrNotFound; // Not reached, but disables a compiler warning
    }

// ---------------------------------------------------------------------------

// returns a random string from the resource files
TPtrC CCreatorEngine::TestDataPathL (enum TTestDataPath aTestDataPath )
    {
    LOGSTRING("Creator: CCreatorEngine::TestDataPathL");

    static TFileName filePath;

    switch (aTestDataPath )
        {
        case ESavedDeck_1kB:
            {
            filePath.Copy (_L("Deck-1kB.saveddeck") );
            }
            break;
    
        case EJPEG_25kB:
            {
            filePath.Copy (_L("JPEG-25kB.jpg") );
            }
            break;
    
        case EJPEG_200kB:
            {
            filePath.Copy (_L("JPEG-200kB.jpg") );
            }
            break;
    
        case EJPEG_500kB:
            {
            filePath.Copy (_L("JPEG-500kB.jpg") );
            }
            break;
    
        case EPNG_15kB:
            {
            filePath.Copy (_L("PNG-15kB.png") );
            }
            break;
    
        case EGIF_2kB:
            {
            filePath.Copy (_L("GIF-2kB.gif") );
            }
            break;
    
        case ERNG_1kB:
            {
            filePath.Copy (_L("RNG-1kB.rng") );
            }
            break;
    
        case EMIDI_10kB:
            {
            filePath.Copy (_L("MIDI-10kB.mid") );
            }
            break;
    
        case EWAVE_20kB:
            {
            filePath.Copy (_L("WAV-20kB.wav") );
            }
            break;
    
        case EAMR_20kB:
            {
            filePath.Copy (_L("AMR-20kB.amr") );
            }
            break;
    
        case EExcel_15kB:
            {
            filePath.Copy (_L("XLS-15kB.xls") );
            }
            break;
    
        case EWord_20kB:
            {
            filePath.Copy (_L("DOC-20kB.doc") );
            }
            break;
    
        case EPowerPoint_40kB:
            {
            filePath.Copy (_L("PPT-40kB.ppt") );
            }
            break;
    
        case EText_10kB:
            {
            filePath.Copy (_L("TXT-10kB.txt") );
            }
            break;
    
        case EText_70kB:
            {
            filePath.Copy (_L("TXT-70kB.txt") );
            }
            break;
    
        case E3GPP_70kB:
            {
            filePath.Copy (_L("3GPP-70kB.3gpp") );
            }
            break;
    
        case EMP3_250kB:
            {
            filePath.Copy (_L("MP3-250kB.mp3") );
            }
            break;
    
        case EAAC_100kB:
            {
            filePath.Copy (_L("AAC-100kB.aac") );
            }
            break;
    
        case ERM_95kB:
            {
            filePath.Copy (_L("RM-95kB.rm") );
            }
            break;
    
        case EBMP_25kB:
            {
            filePath.Copy (_L("BMP-25kB.bmp") );
            }
            break;
        case EHTML_20kB:
            {
            filePath.Copy (_L("HTML-20kB.html") );
            }
            break;
        case EJAD_1kB:
            {
            filePath.Copy (_L("JAD-1kB.jad") );
            }
            break;
        case EJAR_10kB:
            {
            filePath.Copy (_L("JAR-10kB.jar") );
            }
            break;
        case EJP2_65kB:
            {
            filePath.Copy (_L("JP2-65kB.jp2") );
            }
            break;
        case EMP4_200kB:
            {
            filePath.Copy (_L("MP4-200kB.mp4") );
            }
            break;
        case EMXMF_40kB:
            {
            filePath.Copy (_L("MXMF-40kB.mxmf") );
            }
            break;
        case ERAM_1kB:
            {
            filePath.Copy (_L("RAM-1kB.ram") );
            }
            break;
        case ESVG_15kB:
            {
            filePath.Copy (_L("SVG-15kB.svg") );
            }
            break;
        case ESWF_15kB:
            {
            filePath.Copy (_L("SWF-15kB.swf") );
            }
            break;
        case ETIF_25kB:
            {
            filePath.Copy (_L("TIF-25kB.tif") );
            }
            break;
        case EVCF_1kB:
            {
            filePath.Copy (_L("VCF-1kB.vcf") );
            }
            break;
        case EVCS_1kB:
            {
            filePath.Copy (_L("VCS-1kB.vcs") );
            }
            break;
        case ESISX_10kB:
            {
            filePath.Copy (_L("SISX-10kB.sisx") );
            }
            break;
        case EWMA_50kB:
            {
            filePath.Copy (_L("WMA-50kB.wma") );
            }
            break;
        case EWMV_200kB:
            {
            filePath.Copy (_L("WMV-200kB.wmv") );
            }
            break;

        default:
            User::Panic (_L("Test data"), 401 );
            break;
        }

    TFileName fullTargetPath;
    fullTargetPath.Copy( iTempPath->Des() );
    fullTargetPath.Append( filePath );
    if (ConeUtils::FileExists(fullTargetPath))
        {        
        // No need to copy, if the file already exists.
        LOGSTRING2("Creator: CCreatorEngine::TestDataPathL %S already exists", &fullTargetPath);
        return fullTargetPath;
        }

    // Define the path where the testdata can be retrieved
	// get the data from the private directory in secure platform
    TFileName fullAppPath = iEnv->EikAppUi()->Application()->AppFullName();
	TChar driveLetter = fullAppPath[0];
	TInt driveNumber;
	
	iEnv->FsSession().CharToDrive(driveLetter, driveNumber);
	iEnv->FsSession().SetSessionToPrivate( driveNumber );   // "\\private\\20011383\\"
 

    // check the file exists
    if (!ConeUtils::FileExists(filePath))
        {
        LOGSTRING2("Creator: CCreatorEngine::TestDataPathL %S not found", &filePath);

        TInt generatingError( KErrNone );
        
        // The file could be on C drive
        iEnv->FsSession().SetSessionToPrivate( KRegisterDrive );
        
        if ( !ConeUtils::FileExists( filePath ) )
            {
            // The file is not on C drive, see if we can generate it
            
            // Make sure that the private path of this app in c-drive exists
            iEnv->FsSession().CreatePrivatePath( KRegisterDrive );
            
            TFileName privatePath;
            iEnv->FsSession().PrivatePath( privatePath );
            privatePath.Append( filePath );
            TChar privateDriveChar;
            iEnv->FsSession().DriveToChar( KRegisterDrive, privateDriveChar );
            TFileName fullPrivatePath; // full path of a file to be generated
            fullPrivatePath.Append( privateDriveChar );
            fullPrivatePath.Append( ':' );
            fullPrivatePath.Append( privatePath );

            switch ( aTestDataPath )
                {
                case EJPEG_25kB:
                case EJPEG_200kB:
                case EJPEG_500kB:
                case EBMP_25kB:
                case EGIF_2kB:
                    {
                    TRAP( generatingError, GenerateSourceImageFileL( aTestDataPath, fullPrivatePath ) );
                    // GenerateSourceImageFileL calls TestDataPathL -> private session points to z
                    // change it back to KRegisterDrive, because generated source file is there
                    // and we need to copy it at the end of this function.
                    iEnv->FsSession().SetSessionToPrivate( KRegisterDrive );
                    break;
                    }
                case EText_10kB:
                    {
                    TRAP( generatingError, GenereteSourceTextFileL( fullPrivatePath, 10000 ) );
                    // GenerateSourceImageFileL calls TestDataPathL -> private session points to z
                    // change it back to KRegisterDrive, because generated source file is there
                    // and we need to copy it at the end of this function.
                    iEnv->FsSession().SetSessionToPrivate( KRegisterDrive );
                    break;
                    }
                case EText_70kB:
                    {
                    TRAP( generatingError, GenereteSourceTextFileL( fullPrivatePath, 70000 ) );
                    break;
                    }
                default:
                    {
                    generatingError = KErrNotSupported;
                    break;
                    }
                }
            }
        
        if ( iUserCancelled )
            {
            LOGSTRING("Creator: CCreatorEngine::TestDataPathL iUserCancelled" );
            User::Leave( KErrCancel );
            }
        
        if ( generatingError )
            {
            // Generating the file failed
            
            // cancel the dialog
            //iProgressDialog->ProcessFinishedL();
            //DialogDismissedL(EAknSoftkeyBack);

			// remove the progress dialog from the screen
			iEngineWrapper->CloseProgressbar();
			
			
            // show error note
            iEngineWrapper->ShowNote(_L("Test data missing"));
			
            User::Leave(KErrPathNotFound);
            }
        }

	// now copy the data to D-drive to make the file public
	iEnv->FsSession().MkDirAll( iTempPath->Des() );
	TInt copyErr = BaflUtils::CopyFile(iEnv->FsSession(), filePath, fullTargetPath);
	LOGSTRING4("Creator: CCreatorEngine::TestDataPathL copy %S to %S returns=%d", &filePath, &fullTargetPath, copyErr);
	
	// make sure that the file won't have a read only attribute
	TEntry fileEntry;
	iEnv->FsSession().Entry(fullTargetPath, fileEntry);
	iEnv->FsSession().SetEntry(fullTargetPath, fileEntry.iModified, NULL, KEntryAttReadOnly);
	
	filePath = fullTargetPath;  // change the reference to the temp path
    return filePath;
    }


// ---------------------------------------------------------------------------



// a method to return a default Internet Access Point
TUint32 CCreatorEngine::GetDefaultIAPL()
	{
    LOGSTRING("Creator: CCreatorEngine::GetDefaultIAPL");

	TUint32 lIAPid = 1; // Default value

	CCommsDatabase* cDatabase = CCommsDatabase::NewL();
	CleanupStack::PushL(cDatabase);

	CCommsDbTableView* tableView = cDatabase->OpenTableLC(TPtrC(IAP));

	TInt errorCode = tableView->GotoFirstRecord();
		
	if ( errorCode == KErrNone ) 
		{
		TUint32	iValue;
		tableView->ReadUintL(TPtrC(COMMDB_ID),iValue);
		lIAPid = iValue;
		}

	CleanupStack::PopAndDestroy(2); // cDatabase,tableView

	return lIAPid;
	}

// ---------------------------------------------------------------------------

TUint32 CCreatorEngine::AccessPointNameToIdL(const TDesC& /*aAPName*/, TBool /*aAnyIfNotFound*/ )
    {
    LOGSTRING("Creator: CCreatorEngine::AccessPointNameToIdL");
    // Accespoint impl moved from engine to accespoint implementations for supporting 3.0-3.1 and 3.2->
    //return iAccessPoints->AccessPointNameToIdL(aAPName, aAnyIfNotFound );
    return 0;
    }

static CCreatorEngine::TTestDataPath SoundFiles[] = {
        CCreatorEngine::EMIDI_10kB,
        CCreatorEngine::EWAVE_20kB,  
        CCreatorEngine::EMP3_250kB,
        CCreatorEngine::EAAC_100kB
        };

static CCreatorEngine::TTestDataPath PictureFiles[] = 
    {
    CCreatorEngine::EJPEG_25kB,
    CCreatorEngine::EJPEG_200kB,
    CCreatorEngine::EJPEG_500kB,
    CCreatorEngine::EPNG_15kB,
    CCreatorEngine::EGIF_2kB
    };

TPtrC CCreatorEngine::CreateSoundFileL(TTestDataPath aId)
{
	TPtrC tempPath(TestDataPathL(aId));
	// Copy file to permanent location:
    TParse parser;
    parser.Set(tempPath, NULL, NULL);
    TPtrC fileName(parser.NameAndExt());
    HBufC* fullTargetPath = HBufC::NewLC( KMaxFileName );
    fullTargetPath->Des().Copy( PathInfo::PhoneMemoryRootPath() );
    fullTargetPath->Des().Append( PathInfo::DigitalSoundsPath() );
    
    fullTargetPath->Des().Append(fileName);
    CopyFileL(tempPath, *fullTargetPath);
    iAllocatedFilePaths.AppendL(fullTargetPath);
    CleanupStack::Pop(); // fullTargetPath
    return fullTargetPath->Des(); 
}
TPtrC CCreatorEngine::CreateRandomSoundFileL()
{
    TInt numOfFiles = sizeof(SoundFiles) / sizeof(TTestDataPath);
    return CreateSoundFileL(SoundFiles[RandomNumber(0, numOfFiles-1)]);
}
TPtrC CCreatorEngine::RandomSoundFileL()
{
    TInt numOfFiles = sizeof(SoundFiles) / sizeof(TTestDataPath);
    return TestDataPathL(SoundFiles[RandomNumber(0, numOfFiles-1)]);
}
    
void CCreatorEngine::RandomPictureFileL(TDes& aPath)
{
    TInt numOfFiles = sizeof(PictureFiles) / sizeof(TTestDataPath);
    aPath = TestDataPathL(PictureFiles[RandomNumber(0, numOfFiles-1)]);
}
// ---------------------------------------------------------------------------

// returns a random string from the resource files
TPtrC CCreatorEngine::RandomString(enum TRandomStringType aRandomStringType)
    {
    LOGSTRING("Creator: CCreatorEngine::RandomString");
    
    // Let's see if the array is OK and the string arrays are in correct order:
    if( ((TInt) aRandomStringType) < iStringArrays.Count() && 
        iStringArrays[aRandomStringType].iStringType == aRandomStringType)
        {        
        TInt rand = (TInt) (iStringArrays[aRandomStringType].iArrayPtr->Count() * Math::FRand(iSeed));
        return (*iStringArrays[aRandomStringType].iArrayPtr)[rand];
        }
    
    // Now let's loop the arrays and try to find one with the type:
    for( TInt i = 0; i < iStringArrays.Count(); ++i )
        {
        if( iStringArrays[i].iStringType == aRandomStringType )
            {
            TInt rand = (TInt) (iStringArrays[i].iArrayPtr->Count() * Math::FRand(iSeed));
            return (*iStringArrays[i].iArrayPtr)[rand];
            }
        }

    User::Panic (_L("Random string"), 402);    
    return NULL;
    }

// ---------------------------------------------------------------------------

// returns a random number
TInt CCreatorEngine::RandomNumber(TInt aRange)
    {
    LOGSTRING2("Creator: CCreatorEngine::RandomNumber aRange=%d", aRange);

    return (TInt) (aRange*Math::FRand(iSeed));
    }

// ---------------------------------------------------------------------------

// returns a random number
TInt CCreatorEngine::RandomNumber(TInt aMin, TInt aMax)
    {
    LOGSTRING3("Creator: CCreatorEngine::RandomNumber aMin=%d aMax=%d", aMin, aMax);

    TInt range = (aMax > aMin ? aMax - aMin : aMin - aMax) + 1;
    TInt random = (TInt) (range*Math::FRand(iSeed));
    
    random = (random % range) + aMin;

    if (random > aMax)
        random = aMax;
    else if (random < aMin)
        random = aMin;
    
    return random;
    }

TTime CCreatorEngine::RandomTime(TTime baseTime, TRandomDateType type, TInt aMinHours, TInt aMaxHours, TInt aMinMinutes, TInt aMaxMinutes )
    {
    TInt randomHours = RandomNumber(aMinHours, aMaxHours);
    TInt randomMinutes = RandomNumber(aMinMinutes, aMaxMinutes);
    
    switch( type )
    {
    case EDatePast:
        return (baseTime - TTimeIntervalHours(randomHours)) - TTimeIntervalMinutes(randomMinutes);
    case EDateFuture:
        return (baseTime + TTimeIntervalHours(randomHours)) + TTimeIntervalMinutes(randomMinutes);
    default:
        break;
    }
    return baseTime;
    }

TTime CCreatorEngine::RandomDate(TTime baseTime, TRandomDateType type, TInt aMinDays, TInt aMaxDays)
    {
    TInt random = RandomNumber(aMinDays, aMaxDays);
    
    switch( type )
    {
    case EDatePast:
        return baseTime - TTimeIntervalDays(random);
    case EDateFuture:
        return baseTime + TTimeIntervalDays(random);
    default:
        break;
    }
        
    return baseTime;
    }

TTime CCreatorEngine::RandomDate(TRandomDateType type, TInt aMinDays, TInt aMaxDays)
    {    
    TTime time;
    // Set to current time:
    time.HomeTime();
    return RandomDate(time, type, aMinDays, aMaxDays);
    }

HBufC* CCreatorEngine::CreateEmailAddressLC()
    {
    _LIT(KCountry, "com");    
    return CreateEmailAddressLC(RandomString(EFirstName), RandomString(ESurname), RandomString(ECompany), KCountry);
    }

HBufC* CCreatorEngine::CreateHTTPUrlLC()
    {
    _LIT(KProtocol, "http://");
    _LIT(KPrefix, "www");
    _LIT(KCountry, "com");
    return CreateUrlLC(KProtocol, KPrefix, RandomString(ECompany), KCountry);
    }

HBufC* CCreatorEngine::CreateRandomStringLC(TInt aStrLen)
    {
    TInt minAscii = 65;
    TInt maxAscii = 90;
    HBufC* str = HBufC::NewL(aStrLen);
    CleanupStack::PushL(str);
    for( TInt i = 0; i < aStrLen; ++i )
        {
        TInt random = RandomNumber(minAscii, maxAscii);
        TChar randomChar = TChar((TUint) random);
        str->Des().Append(randomChar);
        }    
    return str;
    }

HBufC* CCreatorEngine::CreateEmailAddressLC(
        const TDesC& firstname,
        const TDesC& lastname,
        const TDesC& domain,
        const TDesC& country ) const
        {
        HBufC* email = HBufC::New(firstname.Length() + lastname.Length() + domain.Length() + country.Length() + 3);
        CleanupStack::PushL(email);        
        _LIT(KEmailAddr, "%S.%S@%S.%S");
        email->Des().Format(KEmailAddr, &firstname, &lastname, &domain, &country);
        return email;
        }
        
HBufC* CCreatorEngine::CreateUrlLC(
        const TDesC& protocol,
        const TDesC& prefix,
        const TDesC& domain,
        const TDesC& country) const
    {
    HBufC* url = HBufC::New(protocol.Size() + prefix.Size() + domain.Size() + country.Size() + 2);
    CleanupStack::PushL(url);
    _LIT(KUrl, "%S%S.%S.%S");
    url->Des().Format(KUrl, &protocol, &prefix, &domain, &country);    
    return url;
    }
// ---------------------------------------------------------------------------

void CCreatorEngine::SetDefaultPathForFileCommandL(TInt aCommand, TFileName& aPath)
	{
	aPath.Copy( PathInfo::PhoneMemoryRootPath() );
	switch (aCommand)
	{
	case ECmdCreateFileEntryJPEG_25kB:
	case ECmdCreateFileEntryJPEG_200kB:
	case ECmdCreateFileEntryJPEG_500kB:
	case ECmdCreateFileEntryPNG_15kB:
	case ECmdCreateFileEntryGIF_2kB:
	case ECmdCreateFileEntryBMP_25kB:
	case ECmdCreateFileEntrySVG_15kB:
	case ECmdCreateFileEntryTIF_25kB:
	case ECmdCreateFileEntryJP2_65kB:

		{  aPath.Append( PathInfo::ImagesPath() );  break;  }

	case ECmdCreateFileEntryRNG_1kB:
		{  aPath.Append( PathInfo::SimpleSoundsPath() );  break;  }

	case ECmdCreateFileEntryMIDI_10kB:
	case ECmdCreateFileEntryWAV_20kB:
	case ECmdCreateFileEntryAMR_20kB:
	case ECmdCreateFileEntryMP3_250kB:
	case ECmdCreateFileEntryAAC_100kB:
	case ECmdCreateFileEntryWMA_50kB:
		{  aPath.Append( PathInfo::DigitalSoundsPath() );  break;  }

	case ECmdCreateFileEntry3GPP_70kB:
	case ECmdCreateFileEntryRM_95kB:
	case ECmdCreateFileEntryMP4_200kB:
	case ECmdCreateFileEntryWMV_200kB:

		{  aPath.Append( PathInfo::VideosPath() );  break;  }

	case ECmdCreateFileEntryEmptyFolder:
		{  aPath.Append( _L("MyFolder\\") );  break;  }

	default:
		{  aPath.Append( PathInfo::OthersPath() );  break;  }

	}
	}


// ---------------------------------------------------------------------------

void CCreatorEngine::CancelComplete()
    {
    LOGSTRING("Creator: CCreatorEngine::CancelComplete");
    // free resources and show a note
    TRAP_IGNORE( ShutDownEnginesL() );

    TRAP_IGNORE(iEngineWrapper->ShowNote(_L("Cancelled")));
	
	}

// ---------------------------------------------------------------------------

TCommand::TCommand(TInt aCommandId, CCreatorModuleBaseParameters* aParameters)
    {
    iCommandId = aCommandId;
    iParameters = aParameters;
    }

// ---------------------------------------------------------------------------

TCommand::~TCommand()
    {
    }

// ---------------------------------------------------------------------------

CAsyncWaiter* CAsyncWaiter::NewL(TInt aPriority)
	{
	CAsyncWaiter* self = new(ELeave) CAsyncWaiter(aPriority);
	return self;
	}

CAsyncWaiter* CAsyncWaiter::NewLC(TInt aPriority)
	{
	CAsyncWaiter* self = new(ELeave) CAsyncWaiter(aPriority);
	CleanupStack::PushL(self);
	return self;
	}
	
CAsyncWaiter::CAsyncWaiter(TInt aPriority) : CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	}	

CAsyncWaiter::~CAsyncWaiter()
	{
	Cancel();
	}
	
void CAsyncWaiter::StartAndWait()
	{
    LOGSTRING("Creator: CAsyncWaiter::StartAndWait");
    iStatus = KRequestPending;
    SetActive();
    iWait.Start();
	}
	
TInt CAsyncWaiter::Result() const
	{
	return iError;
	}
	
void CAsyncWaiter::RunL()
	{
    LOGSTRING("Creator: CAsyncWaiter::RunL");

	iError = iStatus.Int();
	//CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
	iWait.AsyncStop();
	}
	
void CAsyncWaiter::DoCancel()
	{
    LOGSTRING("Creator: CAsyncWaiter::DoCancel");

	iError = KErrCancel;
    if( iStatus == KRequestPending )
        {
        TRequestStatus* s=&iStatus;
        User::RequestComplete( s, KErrCancel );
        }
    iWait.AsyncStop();
    //CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
	}
	
FileMapping::FileMapping( const TDesC& aAttName, TInt aAttId, TInt aCommandId )
    :
    iFileName(aAttName),
    iFileId(aAttId),
    iCommandId(aCommandId)
    {}

    const TDesC& FileMapping::FileName() const
    {
        return iFileName;
    }

    TInt FileMapping::FileId() const
    {
        return iFileId;
    }   

    TInt FileMapping::CommandId() const
    {
        return iCommandId;
    }
    
// ---------------------------------------------------------------------------  
CDictionaryFileStore* CCreatorEngine::FileStoreLC()
    {
    LOGSTRING("Creator: CCreatorEngine::FileStoreLC");
    CDictionaryFileStore* store;
    
    // make sure that the private path of this app in c-drive exists
    CEikonEnv::Static()->FsSession().CreatePrivatePath( KRegisterDrive ); // c:\\private\\20011383\\

    // handle register always in the private directory 
    User::LeaveIfError( CEikonEnv::Static()->FsSession().SetSessionToPrivate( KRegisterDrive ) );
    store = CDictionaryFileStore::OpenLC( CEikonEnv::Static()->FsSession(), KRegisterFileName, KUidCreatorApp );
    return store;
    }
    
// ---------------------------------------------------------------------------
void CCreatorEngine::ReadEntryIdsFromStoreL( RArray<TInt>& aEntryIds, const TUid aModuleUid )
    {
    LOGSTRING("Creator: CCreatorEngine::ReadEntryIdsFromStoreL");
    CDictionaryFileStore* store = FileStoreLC();
    User::LeaveIfNull( store );
    if ( store->IsPresentL( aModuleUid ) )
        {
        RDictionaryReadStream in;
        in.OpenLC( *store, aModuleUid );
        TFileName fullPath;
        TInt err( KErrNone );
        TInt id( KErrNotFound );
        while ( !err )
            {
            TRAP( err, id = in.ReadInt32L() );  // will leave with KErrEof
            if ( !err )
                {
                aEntryIds.AppendL( id );
                }
            }
        CleanupStack::PopAndDestroy( &in );
        }
    CleanupStack::PopAndDestroy( store );
    }

// ---------------------------------------------------------------------------
void CCreatorEngine::WriteEntryIdsToStoreL( RArray<TInt>& aEntryIds, const TUid aModuleUid )
    {
    LOGSTRING("Creator: CCreatorEngine::WriteEntryIdsToStoreL");
    CDictionaryFileStore* store = FileStoreLC();
    User::LeaveIfNull( store );
    
    // backup previous ids from store
    // otherwise they would be overwritten when calling out.WriteL
    RArray<TInt> previousIds;
    CleanupClosePushL( previousIds );
    
    if ( store->IsPresentL( aModuleUid ) )
        {
        RDictionaryReadStream in;
        in.OpenLC( *store, aModuleUid );        
        TInt err( KErrNone );
        TInt id( KErrNotFound );
        while ( !err ) 
            {
            TRAP( err, id = in.ReadInt32L() ); // will leave with KErrEof
            if ( !err )
                {
                previousIds.AppendL( id );
                }
            }
        CleanupStack::PopAndDestroy( &in );
        }
    
    RDictionaryWriteStream out;       
    out.AssignLC( *store, aModuleUid );
    
    // restore previous ids to store
    for ( TInt i = 0; i < previousIds.Count(); i++ )
        {
        out.WriteInt32L( previousIds[i] );            
        }

    // write new ids to store
    for ( TInt i = 0; i < aEntryIds.Count(); i++ )
        {
        out.WriteInt32L( aEntryIds[i] );        
        }
    
    out.CommitL();
    CleanupStack::PopAndDestroy( &out );
    
    store->CommitL();
    CleanupStack::PopAndDestroy( &previousIds );
    CleanupStack::PopAndDestroy( store );
    }

// ---------------------------------------------------------------------------
void CCreatorEngine::ReadEntryIdsFromStoreL( RArray<TUint32>& aEntryIds, const TUid aModuleUid )
    {
    LOGSTRING("Creator: CCreatorEngine::ReadEntryIdsFromStoreL");
    CDictionaryFileStore* store = FileStoreLC();
    User::LeaveIfNull( store );
    if ( store->IsPresentL( aModuleUid ) )
        {
        RDictionaryReadStream in;
        in.OpenLC( *store, aModuleUid );
        TFileName fullPath;
        TInt err( KErrNone );
        TUint32 id( KErrNone ); 
        while ( !err )
            {
            TRAP( err, id = in.ReadUint32L() );  // will leave with KErrEof
            if ( !err )
                {
                aEntryIds.AppendL( id );
                }
            }
        CleanupStack::PopAndDestroy( &in );
        }
    CleanupStack::PopAndDestroy( store );
    }

// ---------------------------------------------------------------------------
void CCreatorEngine::WriteEntryIdsToStoreL( RArray<TUint32>& aEntryIds, const TUid aModuleUid )
    {
    LOGSTRING("Creator: CCreatorEngine::WriteEntryIdsToStoreL");
    CDictionaryFileStore* store = FileStoreLC();
    User::LeaveIfNull( store );
    
    // backup previous ids from store
    // otherwise they would be overwritten when calling out.WriteL
    RArray<TUint32> previousIds;
    CleanupClosePushL( previousIds );
    
    if ( store->IsPresentL( aModuleUid ) )
        {
        RDictionaryReadStream in;
        in.OpenLC( *store, aModuleUid );
        TInt err( KErrNone );
        TUint32 id( KErrNone );
        while ( !err ) 
            {
            TRAP( err, id = in.ReadUint32L() ); // will leave with KErrEof
            if ( !err )
                {
                previousIds.AppendL( id );
                }
            }
        CleanupStack::PopAndDestroy( &in );
        }
    
    RDictionaryWriteStream out;       
    out.AssignLC( *store, aModuleUid );
    
    // restore previous ids to store
    for ( TInt i = 0; i < previousIds.Count(); i++ )
        {
        out.WriteUint32L( previousIds[i] );            
        }

    // write new ids to store
    for ( TInt i = 0; i < aEntryIds.Count(); i++ )
        {
        out.WriteUint32L( aEntryIds[i] );        
        }
    
    out.CommitL();
    CleanupStack::PopAndDestroy( &out );
    
    store->CommitL();
    CleanupStack::PopAndDestroy( &previousIds );
    CleanupStack::PopAndDestroy( store );
    }

// ---------------------------------------------------------------------------
void CCreatorEngine::RemoveStoreL( const TUid aModuleUid )
    {
    LOGSTRING("Creator: CCreatorEngine::RemoveStoreL");
    CDictionaryFileStore* store = FileStoreLC();
    User::LeaveIfNull( store );
    if ( store->IsPresentL( aModuleUid ) )
        {        
        // entries of this module should be deleted by now,
        // remove the Creator internal registry for this module
        store->Remove( aModuleUid );
        store->CommitL();
        }
    CleanupStack::PopAndDestroy( store );
    }

//----------------------------------------------------------------------------
void CCreatorEngine::GenerateSourceImageFileL(
    const CCreatorEngine::TTestDataPath aFileType,
    const TDesC& aFileName )
    {
    LOGSTRING("Creator: CCreatorEngine::GenerateSourceImageFileL");
    
    // Peek file size
    RFile file;
    TInt fileSize( 0 );
    // Using png file as the source file
    TFileName fileName = TestDataPathL( CCreatorEngine::EPNG_15kB );
    User::LeaveIfError( file.Open( iEnv->FsSession(),
                                   fileName,
                                   EFileRead | EFileShareAny ) );
    CleanupClosePushL( file );
    User::LeaveIfError( file.Size( fileSize ) );
    CleanupStack::PopAndDestroy( &file );
    
    // Using heap variables, because bitmaps 
    // may reserve large amount of memory.
    
    // Read data from the file
    RFileReadStream ws;    
    User::LeaveIfError( ws.Open( iEnv->FsSession(),
                                 fileName,
                                 EFileStream | EFileRead | EFileShareAny ) );
    CleanupClosePushL( ws );
    delete iBitmapData;
    iBitmapData = NULL;
    iBitmapData = HBufC8::NewL( fileSize );
    TPtr8 dataPtr = iBitmapData->Des();
    ws.ReadL( dataPtr, fileSize );
    CleanupStack::PopAndDestroy( &ws );
       
    // Create decoder for the data
    delete iDecoder;
    iDecoder = NULL;
    iDecoder = CImageDecoder::DataNewL( iEnv->FsSession(), dataPtr );
    iFrameinfo = iDecoder->FrameInfo();
    
    // Create bitmap handle for the source image
    iBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iBitmap->Create( iFrameinfo.iFrameCoordsInPixels.Size(), EColor16M ) );
    
    // Convert the data from the file into bitmap format (inmemory)  
    CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
    iDecoder->Convert( &waiter->iStatus, *iBitmap );
    waiter->StartAndWait();
    delete iDecoder;
    iDecoder = NULL;
    delete iBitmapData;
    iBitmapData = NULL;
    if ( iUserCancelled )
        {
        CancelComplete();
        User::Leave( KErrCancel );
        }
    User::LeaveIfError( waiter->Result() );
    
    // Define the target dimensions and image quality
    // so that the generated file size will match required size.
    // Quality and scaling factors are defined experimentally.
    delete iScaler;
    iScaler = NULL;
    iScaler = CBitmapScaler::NewL();
    iScaledBitmap = new (ELeave) CFbsBitmap();
    // use original size as base
    TSize scaledSize( iFrameinfo.iFrameCoordsInPixels.Size() );
    delete iFrameImageData;
    iFrameImageData = NULL;
    iFrameImageData = CFrameImageData::NewL();
    TBuf8<64> mimeType;
    TJpegImageData* jpegImageData( NULL );
    TBmpImageData* bmpImageData( NULL );
    switch ( aFileType )
        {
        case CCreatorEngine::EJPEG_25kB:
            // QualityFactor = 93 and size factor = 2 -> 25kB jpeg file
            mimeType.Copy( _L8("image/jpeg") );
            jpegImageData = new (ELeave) TJpegImageData;
            // Set some format specific data
            jpegImageData->iSampleScheme = TJpegImageData::EColor444;
            jpegImageData->iQualityFactor = 93; // 0..100
            // ownership passed to iFrameImageData after AppendImageData
            User::LeaveIfError( iFrameImageData->AppendImageData( jpegImageData ) );
            scaledSize.iHeight *= 2;
            scaledSize.iWidth *= 2;
            break;
        case CCreatorEngine::EJPEG_200kB:
            {
            mimeType.Copy( _L8("image/jpeg") );
            jpegImageData = new (ELeave) TJpegImageData;
            // Set some format specific data
            jpegImageData->iSampleScheme = TJpegImageData::EColor444;
#ifdef __WINS__
            // QualityFactor = 95 and size factor = 7 -> 196kB jpeg file
            jpegImageData->iQualityFactor = 95; // 0..100
            scaledSize.iHeight *= 7;
            scaledSize.iWidth *= 7;
#else
            // In hw the image compression seems to work
            // more efficiently. Need to set greater values.
            jpegImageData->iQualityFactor = 100; // 0..100
            scaledSize.iHeight *= 7;
            scaledSize.iWidth *= 7;
            // 100, 7, 7 -> 213kB
#endif
            // ownership passed to iFrameImageData after AppendImageData
            User::LeaveIfError( iFrameImageData->AppendImageData( jpegImageData ) );
            break;
            }
        case CCreatorEngine::EJPEG_500kB:
            {
            mimeType.Copy( _L8("image/jpeg") );
            jpegImageData = new (ELeave) TJpegImageData;
            // Set some format specific data
            jpegImageData->iSampleScheme = TJpegImageData::EColor444;
#ifdef __WINS__
            // QualityFactor = 99 and size factor = 8 -> 514kB jpeg file
            jpegImageData->iQualityFactor = 99; // 0..100
            scaledSize.iHeight *= 8;
            scaledSize.iWidth *= 8;
#else
            // In hw the image compression seems to work
            // more efficiently. Need to set greater values. 
            jpegImageData->iQualityFactor = 100; // 0..100
            scaledSize.iHeight *= 13;
            scaledSize.iWidth *= 13;
            // 100, 13, 13 -> 535kB
#endif
            // ownership passed to iFrameImageData after AppendImageData
            User::LeaveIfError( iFrameImageData->AppendImageData( jpegImageData ) );
            break;
            }
        case CCreatorEngine::EBMP_25kB:
            {
            // bit depyh 8 and 11/10 size factor -> 25kB bmp
            mimeType.Copy( _L8("image/bmp") );
            bmpImageData = new (ELeave) TBmpImageData;
            bmpImageData->iBitsPerPixel = 8;
            // ownership passed to iFrameImageData after AppendImageData
            User::LeaveIfError( iFrameImageData->AppendImageData( bmpImageData ) );
            scaledSize.iHeight *= 11;
            scaledSize.iWidth *= 11;
            scaledSize.iHeight /= 10;
            scaledSize.iWidth /= 10;
            break;
            }
        case CCreatorEngine::EGIF_2kB:
            {
            // size factor 1/2 -> 2560B gif
            mimeType.Copy( _L8("image/gif") );
            // GIF encoder is not configurable, only the size matters here
            scaledSize.iHeight /= 2;
            scaledSize.iWidth /= 2;
            break;
            }
        default: break;
        }

    // Scale to reach target size
    User::LeaveIfError( iScaledBitmap->Create( scaledSize, EColor16M ) );
    iScaler->Scale( &waiter->iStatus, *iBitmap, *iScaledBitmap );
    waiter->StartAndWait();
    delete iBitmap;
    iBitmap = NULL;
    delete iScaler;
    iScaler = NULL;
    if ( iUserCancelled )
        {
        CancelComplete();
        User::Leave( KErrCancel );
        }
    User::LeaveIfError( waiter->Result() );
    
    // Encode to target format
    delete iEncoder;
    iEncoder = NULL;
    // Creating CImageEncoder opens the target file
    iEncoder = CImageEncoder::FileNewL( iEnv->FsSession(), aFileName, mimeType );

    // Do the conversion to target format, this will write to the file
    iEncoder->Convert( &waiter->iStatus, *iScaledBitmap, iFrameImageData );
    waiter->StartAndWait();
    delete iEncoder;
    iEncoder = NULL;
    delete iFrameImageData;
    iFrameImageData = NULL;
    delete iScaledBitmap;
    iScaledBitmap = NULL;
    if ( iUserCancelled )
        {
        CancelComplete();
        User::Leave( KErrCancel );
        }
    User::LeaveIfError( waiter->Result() );
    CleanupStack::PopAndDestroy( waiter );
    }

//----------------------------------------------------------------------------
void CCreatorEngine::GenereteSourceTextFileL( const TDesC& aFileName, TInt aSize )
    {
    LOGSTRING("Creator: CCreatorEngine::GenereteSourceTextFileL");
    RFile txtFile;
    _LIT8( KTestContent, "Testing... ");
    _LIT8( KTestContentCRLF, "\r\n");
    txtFile.Create( iEnv->FsSession(),
                    aFileName,
                    EFileStreamText | EFileWrite | EFileShareAny );
    CleanupClosePushL( txtFile );
    for ( TInt i = 1; i*KTestContent().Length() < aSize; i++ )
        {
        User::LeaveIfError( txtFile.Write( KTestContent ) );
        if ( !( i % 10 ) ) // linefeed for every 10th
            {
            User::LeaveIfError( txtFile.Write( KTestContentCRLF ) );
            }
        }
    CleanupStack::PopAndDestroy( &txtFile );
    }

//----------------------------------------------------------------------------

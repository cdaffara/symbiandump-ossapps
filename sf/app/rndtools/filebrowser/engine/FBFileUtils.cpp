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

// INCLUDE FILES
#include "FBFileUtils.h"
#include "FBFileOps.h"
#include "engine.h"
#include "FB.hrh"
#include <filebrowser.rsg>

#include <textresolver.h> 
#include <eikprogi.h>
#include <e32svr.h>
#include <eikfutil.h>
#include <apparc.h>
#include <apaid.h>
#include <apaidpartner.h>
#include <eikenv.h>
#include <bautils.h>
#include <gulicon.h>
#include <babackup.h> 
//#include <aknmemorycardui.mbg>
#include <sendui.h>
#include <CMessageData.h>
#include <ezgzip.h>
#include <e32hal.h>
#include <pathinfo.h>
#include <apgcli.h>
#include <apmstd.h>
#include <apgtask.h>
#include <apacmdln.h>
#include <DocumentHandler.h>
#include <msvapi.h>
#include <msvids.h>
#include <msvstd.h>
#include <msvstore.h>
#include <mmsvattachmentmanager.h>
#include <cmsvattachment.h>
#include <hash.h>
//#include <sbeclient.h>
#include <e32property.h>
#if (!defined __SERIES60_30__ && !defined __SERIES60_31__) 
#include <driveinfo.h>
#endif // !defined __SERIES60_30__ && !defined __SERIES60_31__

#ifndef __SERIES60_30__
//  #include <akniconconfig.h>
#endif

//_LIT(KGZipExt, ".gz");
_LIT8(KFileNewLine, "\r\n");
const TInt KForcedFormatTimeout = 1000000;

// copied from coreapplicationuisdomainpskeys.h
const TUid KPSUidCoreApplicationUIs = { 0x101F8767 };
const TUint32 KCoreAppUIsMmcRemovedWithoutEject = 0x00000109;
enum TCoreAppUIsMmcRemovedWithoutEject
    {
    ECoreAppUIsEjectCommandUsedUninitialized = 0,
    ECoreAppUIsEjectCommandNotUsed,
    ECoreAppUIsEjectCommandUsed,
    ECoreAppUIsEjectCommandUsedToDrive // Low 16-bits contain this value and high 16-bits TDriveNumber to eject
    };
const TInt KDriveLetterStringLength = 3; // e.g. C:\

// ================= MEMBER FUNCTIONS =======================

CFileBrowserFileUtils* CFileBrowserFileUtils::NewL(CEngine* aEngine)
	{
	CFileBrowserFileUtils* self = new(ELeave) CFileBrowserFileUtils(aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// --------------------------------------------------------------------------------------------

CFileBrowserFileUtils::CFileBrowserFileUtils(CEngine* aEngine) : CActive(EPriorityStandard), iEngine(aEngine)
	{
	}

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ConstructL()
	{
    User::LeaveIfError(iTimer.CreateLocal());
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iTz.Connect());
    
    iState = EIdle;
    iListingMode = ENormalEntries;

    iSortMode = EFileBrowserCmdViewSortByName;
    iOrderMode = EFileBrowserCmdViewOrderAscending;
    iCurrentPath = KNullDesC;
    
    iFileOps = CFileBrowserFileOps::NewL(iEngine);
    
    iDriveEntryList = new(ELeave) CDriveEntryList(8);
    iFileEntryList = new(ELeave) CFileEntryList(64);
    iFindFileEntryList = new(ELeave) CFileEntryList(64);
    iAppIconList = new(ELeave) CAppIconList(16);
    iClipBoardList = new(ELeave) CFileEntryList(64);
    iCurrentSelectionList = new(ELeave) CFileEntryList(64);

    iDocHandler = CDocumentHandler::NewL();
//    iDocHandler->SetExitObserver(this);
    
    // set defaults to the search settings
    iSearchAttributes.iSearchDir = KNullDesC;
    iSearchAttributes.iWildCards = _L("*.jpg");
    iSearchAttributes.iTextInFile = KNullDesC;
    iSearchAttributes.iMinSize = 0;
    iSearchAttributes.iMaxSize = 999999999;
    iSearchAttributes.iMinDate = TDateTime(1980, EJanuary,   0,0,0,0,0);
    iSearchAttributes.iMaxDate = TDateTime(2060, EDecember, 30,0,0,0,0);
    iSearchAttributes.iRecurse = ETrue;
    
    // init the command array
    iCommandArray = new(ELeave) CCommandArray(64);
        
    // get current path from settings
    if (iEngine->Settings().iRememberLastPath)
        {
        TFileName settingPath = iEngine->Settings().iLastPath;
        if (BaflUtils::PathExists(iFs, settingPath))
            iCurrentPath = settingPath;
        
        if (iCurrentPath != KNullDesC)
            GetDriveListL();
        }

    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------------------------

CFileBrowserFileUtils::~CFileBrowserFileUtils()
	{
	Cancel();
	
	// save current path from settings
    if (iEngine->Settings().iRememberLastPath)
        {
        iEngine->Settings().iLastPath = iCurrentPath;
        TRAP_IGNORE(iEngine->SaveSettingsL(EFalse));
        }

//    if (iWaitDialog)
//        TRAP_IGNORE(iWaitDialog->ProcessFinishedL()); 
//     
//    if (iProgressDialog)
//        TRAP_IGNORE(iProgressDialog->ProcessFinishedL());
    
        delete iFileOps;

        delete iDocHandler;
	
//	if (iOpenFileService)
//	    delete iOpenFileService;
	
        delete iCurrentSelectionList;
        delete iClipBoardList;
        delete iAppIconList;
        delete iFindFileEntryList;
        delete iFileEntryList;
        delete iDriveEntryList;

        delete iFileMan;

        if (iCommandArray)
            {
            ResetCommandArray();
            delete iCommandArray;
            }

        iTz.Close();
        iFs.Close();
        iTimer.Close();
        }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::DoCancel()
	{
    iTimer.Cancel();
	}

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::StartExecutingCommandsL(const TDesC& aLabel)
    {
    if (iCommandArray->Count() >= 2)
        {
        // init progress bar
//        iProgressDialog = new(ELeave) CAknProgressDialog((reinterpret_cast<CEikDialog**>(&iProgressDialog)), ETrue);
//        iProgressDialog->SetCallback(this);
//        iProgressDialog->PrepareLC(R_GENERAL_PROGRESS_NOTE);
//        iProgressDialog->SetCurrentLabelL( EAknCtNote, aLabel );
//        iProgressInfo = iProgressDialog->GetProgressInfoL();
//        iProgressInfo->SetFinalValue( CommandArrayCount() );
//        iProgressDialog->RunLD();
//        iProgressDialog->MakeVisible( ETrue );        
        iEngine->FileBrowserUI()->ShowProgressDialog(aLabel, 0, CommandArrayCount());
        isProgressDialog = ETrue;
        }
    else if (iCommandArray->Count() >= 1)
        {
        // init wait dialog
//        iWaitDialog = new(ELeave) CAknWaitDialog((reinterpret_cast<CEikDialog**>(&iWaitDialog)), ETrue);
//        iWaitDialog->SetCallback(this);
//        iWaitDialog->PrepareLC(R_GENERAL_WAIT_NOTE);
//        iWaitDialog->SetTextL( aLabel );
//        iWaitDialog->RunLD();
        iEngine->FileBrowserUI()->ShowWaitDialog(aLabel);
        isWaitDialog = ETrue;
        }
    else
        {
        // no commands, just do nothing
        return;
        }


    // start with the first operation
    iCurrentEntry = 0;
    iSucceededOperations = 0;
    iFailedOperations = 0;

    // starts executing commands
    ExecuteCommand();
    }
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ExecuteCommand()
    {
    // make sure the engine isn't active, should never happen
    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("FileUtils:IsActive"), 333));

    // execute a command after a very short delay (25ms)
    if (isWaitDialog)
        {
        iEngine->FileBrowserUI()->ProcessEvents();
        }
    if (isProgressDialog)
        {
        TInt newValue = iCurrentEntry;
        iEngine->FileBrowserUI()->SetProgressValue(newValue);
        }
    iTimer.After(iStatus, 25);
    SetActive();
    }

// --------------------------------------------------------------------------------------------
    
void CFileBrowserFileUtils::RunL()
    {
    TInt err(KErrNone);
    TInt currentCommand = iCommandArray->At(iCurrentEntry).iCommandId;

    // execute a command from the queue
    switch(currentCommand)
        {
        case EFileBrowserFileOpCommandAttribs:
            {
            CCommandParamsAttribs* params = static_cast<CCommandParamsAttribs*>(iCommandArray->At(iCurrentEntry).iParameters);
            err = iFileOps->Attribs(params->iSourceEntry, params->iSetMask, params->iClearMask, params->iTime, params->iSwitch);
            }
            break;
            
        case EFileBrowserFileOpCommandCopy:
            {
            CCommandParamsCopyOrMove* params = static_cast<CCommandParamsCopyOrMove*>(iCommandArray->At(iCurrentEntry).iParameters);
            err = iFileOps->Copy(params->iSourceEntry, params->iTargetPath, params->iSwitch);
            }
            break;
            
        case EFileBrowserFileOpCommandMove:
            {
            CCommandParamsCopyOrMove* params = static_cast<CCommandParamsCopyOrMove*>(iCommandArray->At(iCurrentEntry).iParameters);
            err = iFileOps->Copy(params->iSourceEntry, params->iTargetPath, params->iSwitch, ETrue);
            }
            break;
                
        case EFileBrowserFileOpCommandRename:
            {
            CCommandParamsRename* params = static_cast<CCommandParamsRename*>(iCommandArray->At(iCurrentEntry).iParameters);
            err = iFileOps->Rename(params->iSourceEntry, params->iTargetPath, params->iSwitch);
            }
            break;

        case EFileBrowserFileOpCommandDelete:
            {
            CCommandParamsDelete* params = static_cast<CCommandParamsDelete*>(iCommandArray->At(iCurrentEntry).iParameters);
            err = iFileOps->Delete(params->iSourceEntry, params->iSwitch);
            }
            break;

        case EFileBrowserFileOpCommandDriveSnapShot:
            {
            CCommandParamsDriveSnapShot* params = static_cast<CCommandParamsDriveSnapShot*>(iCommandArray->At(iCurrentEntry).iParameters);
            err = iFileOps->DriveSnapShot(params->iSourceDriveLetter, params->iTargetDriveLetter);
            }
            break;
                                                
        default:
            User::Panic (_L("Unknown command"), 400);
            break;
        }

    
    if (err == KErrNone)
        {
        iSucceededOperations++;
        }
    else if ( err != KErrCancel )
        {
        iFailedOperations++;
        }
        
    iLastError = err;            

    // check for more commands in queue
    TRAP_IGNORE( CheckForMoreCommandsL() );
    }

// ---------------------------------------------------------------------------

TInt CFileBrowserFileUtils::RunError(TInt aError)
    {
    _LIT(KMessage, "Command error %d");
    TBuf<128> noteMsg;
    noteMsg.Format(KMessage, aError);
            
    iEngine->EikonEnv()->InfoMsg(noteMsg);
    
    TRAP_IGNORE( CheckForMoreCommandsL() );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------

void CFileBrowserFileUtils::CheckForMoreCommandsL()
    {
    //LOGSTRING("Creator: CCreatorEngine::CheckForMoreCommandsL");
    
    // update the progress bar
//    if (iProgressInfo)
//        iProgressInfo->IncrementAndDraw(1);

	// check if we have more commands to be executed
	if (iCurrentEntry >= CommandArrayCount() - 1)
		{
        //LOGSTRING("Creator: CCreatorEngine::CheckForMoreCommandsL all done");

        // all done, free resources
        ResetCommandArray();
        
        // deactive secure backup if it was activated by a file command
        iFileOps->DeActivateSecureBackUpViaFileOp();

        // dismiss any wait/progress dialogs        
        if (isWaitDialog)
            {
            iEngine->FileBrowserUI()->CancelWaitDialog();
            isWaitDialog = EFalse;
            }
        if (isProgressDialog)
            {
            iEngine->FileBrowserUI()->CancelProgressDialog();
            isProgressDialog = EFalse;
            }
//        if (iWaitDialog)
//            {
//            TRAP_IGNORE(iWaitDialog->ProcessFinishedL()); 
//            iWaitDialog = NULL;
//            }
//        if (iProgressDialog)
//            {
//            TRAP_IGNORE(iProgressDialog->ProcessFinishedL());
//            iProgressDialog = NULL;
//            }


        // show result note
        if ( iSucceededOperations == 0 && iFailedOperations == 0 )
            {
            // single operation cancelled -> show nothing
            }
        else if (iSucceededOperations == 1 && iFailedOperations == 0)    
            {
            _LIT(KMessage, "Operation succeeded");
            iEngine->FileBrowserUI()->ShowConfirmationNote(KMessage);
            }
        else if (iSucceededOperations > 0 && iFailedOperations == 0)    
            {
            _LIT(KMessage, "%d operations succeeded");
            TBuf<128> noteMsg;
            noteMsg.Format(KMessage, iSucceededOperations);
            iEngine->FileBrowserUI()->ShowConfirmationNote(noteMsg);
            }
        else if (iSucceededOperations == 0 && iFailedOperations > 1)    
            {
            _LIT(KMessage, "%d operations failed");
            TBuf<128> noteMsg;
            noteMsg.Format(KMessage, iFailedOperations);

            iEngine->FileBrowserUI()->ShowErrorNote(noteMsg);
            }
        else if (iSucceededOperations > 0 && iFailedOperations > 0)    
            {
            _LIT(KMessage, "%d operations succeeded, %d failed");
            TBuf<128> noteMsg;
            noteMsg.Format(KMessage, iSucceededOperations, iFailedOperations);

            iEngine->FileBrowserUI()->ShowInformationNote(noteMsg, _L(""));
            }
        else    
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(iLastError, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }

        RefreshViewL();
        iEngine->FileBrowserUI()->NotifyModelHasChanged();

            }
	else
            {
            // maintain requests
            iCurrentEntry++;

            //LOGSTRING2("Creator: CCreatorEngine::CheckForMoreCommandsL iCurrentEntry=%d", iCurrentEntry);

            ExecuteCommand();
            }
    }    
// --------------------------------------------------------------------------------------------

// This callback function is called when cancel button of the progress bar was pressed
void CFileBrowserFileUtils::DialogDismissedL(/*TInt aButtonId*/)
    {
//    iProgressDialog = NULL;
//    iProgressInfo = NULL;
//    iWaitDialog = NULL;
    
    // check if cancel button was pressed
//    if (aButtonId == EAknSoftkeyCancel)
//        {
        // cancel the active object, command executer 
        Cancel();
        
        iFileOps->CancelOp();
        
        ResetCommandArray();

        isProgressDialog = EFalse;

        iEngine->FileBrowserUI()->ShowInformationNote(_L("Cancelled"), _L(""));
//        }
    }
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::AppendToCommandArrayL(TInt aCommand, CCommandParamsBase* aParameters)
    {
    if (iCommandArray)
        iCommandArray->AppendL( TCommand(aCommand, aParameters) );
    else
        User::Leave(KErrNotReady);
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::CommandArrayCount() const
    {
    TInt count(0);
    
    if (iCommandArray)
        count = iCommandArray->Count();
    
    return count;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ResetCommandArray()
    {
    // delete params first
    for (TInt i=0; i<CommandArrayCount(); i++)
        {
        if ( iCommandArray->At(i).iParameters )
            {
            delete iCommandArray->At(i).iParameters;
            iCommandArray->At(i).iParameters = NULL;
            }
        }

    // reset the entries
    iCommandArray->Reset();
    }
                    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::HandleSettingsChangeL()
    {
	// TODO

//    if (iEngine->FileListContainer())
//        {
//        iEngine->FileListContainer()->SetScreenLayoutL(iEngine->Settings().iDisplayMode);
//        iEngine->FileListContainer()->CreateListBoxL(iEngine->Settings().iFileViewMode);
        //iEngine->FileListContainer()->SetNaviPaneTextL(iCurrentPath);
        RefreshViewL();
//        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SetSortModeL(TInt aSortMode)
    {
    iSortMode = aSortMode;
    RefreshViewL();
    }
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SetOrderModeL(TInt aOrderMode)
    {
    iOrderMode = aOrderMode;
    RefreshViewL();
    }

// --------------------------------------------------------------------------------------------

TKeyResponse CFileBrowserFileUtils::HandleOfferKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
	// TODO
	/*
    if(aType != EEventKey)
        return EKeyWasNotConsumed;
    
    TBool shiftKeyPressed = (aKeyEvent.iModifiers & EModifierShift) ||
                            (aKeyEvent.iModifiers & EModifierLeftShift) ||
                            (aKeyEvent.iModifiers & EModifierRightShift);
    TBool controlKeyPressed = (aKeyEvent.iModifiers & EModifierCtrl) || 
                              (aKeyEvent.iModifiers & EModifierRightCtrl); 
    
    // handle left arrow key press
    if (aKeyEvent.iCode == EKeyLeftArrow)
        {
        MoveUpOneLevelL();
        return EKeyWasConsumed;
        }
    
    // handle right arrow key press
    else if (aKeyEvent.iCode == EKeyRightArrow)
        {
        if (IsDriveListViewActive() && iDriveEntryList->Count() > 0)
            {
            MoveDownToDirectoryL();
            return EKeyWasConsumed;
            }
        else if (iFileEntryList->Count() > 0)
            {
            MoveDownToDirectoryL();
            return EKeyWasConsumed;
            }        
        }    

    // skip OK/Enter keys when shift or ctrl is pressed because those are needed by the listbox
    else if ((aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter) && (shiftKeyPressed || controlKeyPressed))
        {
        return EKeyWasNotConsumed;
        }
    
    // handle OK/Enter keys
    else if (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
        {
        if (IsDriveListViewActive() && iDriveEntryList->Count() > 0)
            {
            MoveDownToDirectoryL();
            return EKeyWasConsumed;
            }
        else if (iFileEntryList->Count() > 0)
            {
            TFileEntry fileEntry = iFileEntryList->At(iEngine->FileListContainer()->CurrentListBoxItemIndex());
            
            if (fileEntry.iEntry.IsDir())
                MoveDownToDirectoryL();
            else
                OpenCommonFileActionQueryL();
            
            return EKeyWasConsumed;
            } 
        }
    
    // handle Delete/Backspace keys
    else if (!iEngine->FileListContainer()->SearchField() && (aKeyEvent.iCode == EKeyBackspace || aKeyEvent.iCode == EKeyDelete) &&
             !iEngine->FileUtils()->IsCurrentDriveReadOnly() && !iEngine->FileUtils()->IsDriveListViewActive())
        {
        DeleteL();
        return EKeyWasConsumed;
        }
    */                      
    return EKeyWasNotConsumed;
    }        
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::RefreshViewL()
    {
//    if (iEngine->FileListContainer())
//        {
//
//        // make sure that the search field is disabled
//        iEngine->FileListContainer()->DisableSearchFieldL();
        
        // read directory listing
        GenerateDirectoryDataL();
        
//        // set an icon array
//        iEngine->FileListContainer()->SetListBoxIconArrayL(GenerateIconArrayL());
//
//        // set text items
//        iEngine->FileListContainer()->SetListBoxTextArrayL(GenerateItemTextArrayL());
//        }
    }
    
// --------------------------------------------------------------------------------------------

//CAknIconArray* CFileBrowserFileUtils::GenerateIconArrayL(TBool aGenerateNewBasicIconArray)
//    {
//	// TODO
//
//    CAknIconArray* iconArray = NULL;
//
//    if (aGenerateNewBasicIconArray)
//        {
//        iconArray = new(ELeave) CAknIconArray(16);
//        }
//    else
//        {
//        if (iEngine->FileListContainer())
//            iconArray = iEngine->FileListContainer()->ListBoxIconArrayL();
//
//        if (!iconArray)
//            iconArray = new(ELeave) CAknIconArray(16);
//        }
//
//
//    CleanupStack::PushL(iconArray);
//
//    // generate basic items if not already existing
//    if (iconArray->Count() < EFixedIconListLength)
//        {
//        // reset arrays
//        iconArray->Reset();
//        iAppIconList->Reset();
//
//        // get default control color
//        TRgb defaultColor;
//        defaultColor = iEngine->EikonEnv()->Color(EColorControlText);
//
//        // create a color icon of the marking indicator
//        CFbsBitmap* markBitmap = NULL;
//        CFbsBitmap* markBitmapMask = NULL;
//
//        AknsUtils::CreateColorIconL(AknsUtils::SkinInstance(),
//                KAknsIIDQgnIndiMarkedAdd,
//                KAknsIIDQsnIconColors,
//                EAknsCIQsnIconColorsCG13,
//                markBitmap,
//                markBitmapMask,
//                AknIconUtils::AvkonIconFileName(),
//                EMbmAvkonQgn_indi_marked_add,
//                EMbmAvkonQgn_indi_marked_add_mask,
//                defaultColor
//                );
//
//        // 0 marking indicator
//        CGulIcon* markIcon = CGulIcon::NewL(markBitmap, markBitmapMask);
//        iconArray->AppendL(markIcon);
//
//        // 1 empty
//        AppendGulIconToIconArrayL(iconArray, AknIconUtils::AvkonIconFileName(),
//                                  EMbmAvkonQgn_prop_empty, EMbmAvkonQgn_prop_empty_mask, KAknsIIDQgnPropEmpty);
//        // 2 phone memory
//        AppendGulIconToIconArrayL(iconArray, AknIconUtils::AvkonIconFileName(),
//                                  EMbmAvkonQgn_prop_phone_memc, EMbmAvkonQgn_prop_phone_memc_mask, KAknsIIDQgnPropPhoneMemc);
//        // 3 memory card
//        AppendGulIconToIconArrayL(iconArray, KMemoryCardUiBitmapFile,
//                                  EMbmAknmemorycarduiQgn_prop_mmc_memc, EMbmAknmemorycarduiQgn_prop_mmc_memc_mask, KAknsIIDQgnPropMmcMemc);
//        // 4 memory card disabled
//        AppendGulIconToIconArrayL(iconArray, KMemoryCardUiBitmapFile,
//                                  EMbmAknmemorycarduiQgn_prop_mmc_non, EMbmAknmemorycarduiQgn_prop_mmc_non_mask, KAknsIIDQgnPropMmcNon);
//        // 5 folder
//        AppendGulIconToIconArrayL(iconArray, AknIconUtils::AvkonIconFileName(),
//                                  EMbmAvkonQgn_prop_folder_small, EMbmAvkonQgn_prop_folder_small_mask, KAknsIIDQgnPropFolderSmall);
//        // 6 folder with subfolders
//        AppendGulIconToIconArrayL(iconArray, AknIconUtils::AvkonIconFileName(),
//                                  EMbmAvkonQgn_prop_folder_sub_small, EMbmAvkonQgn_prop_folder_sub_small_mask, KAknsIIDQgnPropFolderSubSmall);
//        // 7 current folder
//        AppendGulIconToIconArrayL(iconArray, AknIconUtils::AvkonIconFileName(),
//                                  EMbmAvkonQgn_prop_folder_current, EMbmAvkonQgn_prop_folder_current_mask, KAknsIIDQgnPropFolderCurrent);
//        }
//
//    // append custom icons if not in drive list view and setting is enabled
//    if (!aGenerateNewBasicIconArray && !IsDriveListViewActive() && iEngine->Settings().iShowAssociatedIcons)
//        {
//        // init id counter
//        TInt idCounter(EFixedIconListLength + iAppIconList->Count());
//
//        // loop all items in the file list
//        for (TInt i=0; i<iFileEntryList->Count(); i++)
//            {
//            TFileEntry& fileEntry = iFileEntryList->At(i);
//
//            // just check for normal files
//            if (!fileEntry.iEntry.IsDir())
//                {
//                TUid appUid = GetAppUid(fileEntry);
//                TInt iconId = AppIconIdForUid(appUid);
//
//                if (appUid != KNullUid && iconId == KErrNotFound)
//                    {
//                    // icon not found from the icon array, generate it
//                    CFbsBitmap* bitmap = NULL;
//                    CFbsBitmap* mask = NULL;
//                    CGulIcon* appIcon = NULL;
//
//                    TRAPD(err,
//                        AknsUtils::CreateAppIconLC(AknsUtils::SkinInstance(), appUid, EAknsAppIconTypeList, bitmap, mask);
//                        appIcon = CGulIcon::NewL(bitmap, mask);
//                        CleanupStack::Pop(2); //bitmap, mask
//                        );
//
//                    if (err == KErrNone)
//                        {
//                        TAppIcon appIconEntry;
//                        appIconEntry.iId = idCounter;
//                        appIconEntry.iUid = appUid;
//
//                        appIcon->SetBitmapsOwnedExternally(EFalse);
//                        iconArray->AppendL(appIcon);
//                        iAppIconList->AppendL(appIconEntry);
//
//                        fileEntry.iIconId = idCounter;
//
//                        idCounter++;
//                        }
//                    else
//                        {
//                        delete bitmap;
//                        delete mask;
//
//                        TAppIcon appIconEntry;
//                        appIconEntry.iId = EFixedIconEmpty;
//                        appIconEntry.iUid = appUid;
//
//                        iAppIconList->AppendL(appIconEntry);
//                        }
//                    }
//
//                else if (appUid != KNullUid && iconId >= 0)
//                    {
//                    // we already have already generated an icon for this uid, use it
//                    fileEntry.iIconId = iconId;
//                    }
//                }
//            }
//        }
//
//    CleanupStack::Pop(); //iconArray
//	return iconArray;
//
//	}
//
// --------------------------------------------------------------------------------------------

//void CFileBrowserFileUtils::AppendGulIconToIconArrayL(CAknIconArray* aIconArray, const TDesC& aIconFile, TInt aIconId, TInt aMaskId, const TAknsItemID aAknsItemId)
//    {
//    CGulIcon* icon = AknsUtils::CreateGulIconL(
//            AknsUtils::SkinInstance(),
//            aAknsItemId,
//            aIconFile,
//            aIconId,
//            aMaskId);
//    CleanupStack::PushL(icon);
//	
//	icon->SetBitmapsOwnedExternally(EFalse);
//	aIconArray->AppendL(icon);
//	
//	CleanupStack::Pop(); //icon          
//    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::AppIconIdForUid(TUid aUid)
    {
    TInt result(KErrNotFound);
    
    for (TInt i=0; i<iAppIconList->Count(); i++)
        {
        if (iAppIconList->At(i).iUid == aUid)
            {
            result = iAppIconList->At(i).iId;
            break;
            }
        }
    
    return result;
    }

// --------------------------------------------------------------------------------------------

TUid CFileBrowserFileUtils::GetAppUid(TFileEntry aFileEntry)
    {
    TFileName fullPath;
    fullPath.Copy(aFileEntry.iPath);
    fullPath.Append(aFileEntry.iEntry.iName);
    
    TParse parsedName;
    parsedName.Set(fullPath, NULL, NULL);

    // this will boost performance in \sys\bin folder    
    if (parsedName.Ext().CompareF(_L(".dll")) == 0 || parsedName.Ext().CompareF(_L(".rsc")) == 0)
        return KNullUid;
    
    TInt err1(KErrNone), err2(KErrNone);
    TUid appUid;
    TDataType dataType;
    TBool isProgram;
    
    err1 = iEngine->LsSession().AppForDocument(fullPath, appUid, dataType);
    err2 = iEngine->LsSession().IsProgram(fullPath, isProgram);
        
    if (err1 == KErrNone && err2 == KErrNone)
        {
        if (isProgram)
            {
            // get the real app Uid from the app list
            TApaAppInfo appInfo;
            iEngine->LsSession().GetAllApps();

            while (iEngine->LsSession().GetNextApp(appInfo) == KErrNone)
                {
                if (fullPath.CompareF(appInfo.iFullName) == 0)
                    return appInfo.iUid;
                }
            }
        else
            {
            // return the app Uid associated for it
            return appUid;
            }            
        }

    return KNullUid;
    }


CFileEntryList* CFileBrowserFileUtils::FileEntries() const
{
    return iFileEntryList;
}
CDriveEntryList* CFileBrowserFileUtils::DriveEntries() const
{
    return iDriveEntryList;
}

//// --------------------------------------------------------------------------------------------
//
//CDesCArray* CFileBrowserFileUtils::GenerateItemTextArrayL()
//    {
//    iTextArray = new(ELeave) CDesCArrayFlat(64);
////    CleanupStack::PushL(textArray);
//
//    if (IsDriveListViewActive())
//        {
//        _LIT(KSimpleDriveEntry, "%d\t%c: <%S>\t\t");
//        _LIT(KExtendedDriveEntry, "%d\t%c: <%S>\t%LD / %LD kB\t");
//
//        for (TInt i=0; i<iDriveEntryList->Count(); i++)
//            {
//            TFileName textEntry;
//            TDriveEntry driveEntry = iDriveEntryList->At(i);
//
//            if (iEngine->Settings().iFileViewMode == EFileViewModeSimple)
//                {
//                textEntry.Format(KSimpleDriveEntry, driveEntry.iIconId, TUint(driveEntry.iLetter), &driveEntry.iMediaTypeDesc);
//                }
//            else if (iEngine->Settings().iFileViewMode == EFileViewModeExtended)
//                {
//                textEntry.Format(KExtendedDriveEntry, driveEntry.iIconId, TUint(driveEntry.iLetter), &driveEntry.iMediaTypeDesc, driveEntry.iVolumeInfo.iFree/1024, driveEntry.iVolumeInfo.iSize/1024);
//                }
//
//            iTextArray->AppendL(textEntry);
//            }
//        }
//
//    else
//        {
//        _LIT(KSimpleFileEntry, "%d\t%S\t\t");
//        _LIT(KExtendedFileEntry, "%d\t%S\t%S\t");
//
//        for (TInt i=0; i<iFileEntryList->Count(); i++)
//            {
//            TFileName textEntry;
//            TFileEntry fileEntry = iFileEntryList->At(i);
//
//            // format text line entries
//            if (iEngine->Settings().iFileViewMode == EFileViewModeSimple)
//                {
//                textEntry.Format(KSimpleFileEntry, fileEntry.iIconId, &fileEntry.iEntry.iName);
//                }
//            else if (iEngine->Settings().iFileViewMode == EFileViewModeExtended)
//                {
//                TBuf<128> extraData;
//
//                // append entry date
//                TTime entryModified = fileEntry.iEntry.iModified;
//                if ( iTz.ConvertToLocalTime( entryModified ) == KErrNone )
//                    {
//                    entryModified = fileEntry.iEntry.iModified;
//                    }
//
//                _LIT(KCurrentDate,"%D%M%*Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
//                entryModified.FormatL(extraData, KCurrentDate);
//
//                // for a directory append number of entries and for a file the file size
//                if (fileEntry.iEntry.IsDir() && fileEntry.iDirEntries >= 0)
//                    {
//                    extraData.Append(_L(" - "));
//                    extraData.AppendNum(fileEntry.iDirEntries);
//
//                    if (fileEntry.iDirEntries == 1)
//                        extraData.Append(_L(" entry"));
//                    else
//                        extraData.Append(_L(" entries"));
//                    }
//
//                // normal file entry
//                else if (!fileEntry.iEntry.IsDir())
//                    {
//                    extraData.Append(_L(" - "));
//
//                    if (fileEntry.iEntry.iSize < 1024)
//                        {
//                        extraData.AppendNum(fileEntry.iEntry.iSize);
//                        extraData.Append(_L(" B"));
//                        }
//                    else if (fileEntry.iEntry.iSize < 1024*1024)
//                        {
//                        TReal sizeKB = (TReal)fileEntry.iEntry.iSize / 1024;
//                        extraData.AppendNum(sizeKB, TRealFormat(KDefaultRealWidth, 1));
//                        extraData.Append(_L(" kB"));
//                        }
//                    else
//                        {
//                        TReal sizeMB = (TReal)fileEntry.iEntry.iSize / (1024*1024);
//                        extraData.AppendNum(sizeMB, TRealFormat(KDefaultRealWidth, 1));
//                        extraData.Append(_L(" MB"));
//                        }
//                    }
//
//                // append attribute flags if any
//                TBuf<4> atts;
//                if (fileEntry.iEntry.IsArchive())
//                    atts.Append(_L("A"));
//                if (fileEntry.iEntry.IsHidden())
//                    atts.Append(_L("H"));
//                if (fileEntry.iEntry.IsReadOnly())
//                    atts.Append(_L("R"));
//                if (fileEntry.iEntry.IsSystem())
//                    atts.Append(_L("S"));
//
//                if (atts.Length() > 0)
//                    {
//                    extraData.Append(_L(" - ["));
//                    extraData.Append(atts);
//                    extraData.Append(_L("]"));
//                    }
//
//                // format
//                textEntry.Format(KExtendedFileEntry, fileEntry.iIconId, &fileEntry.iEntry.iName, &extraData);
//                }
//
//            iTextArray->AppendL(textEntry);
//            }
//        }
//
////    CleanupStack::Pop(); //textArray
//    return iTextArray;
//    }
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::GenerateDirectoryDataL()
    {
    if (iListingMode == ENormalEntries)
        {
        if (IsDriveListViewActive())
            GetDriveListL();    
        else
            GetDirectoryListingL();        
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::GetDriveListL()
    {
    TDriveList driveList;
    
    // get drive listing depending of the support for network drives
    if (iEngine->Settings().iSupportNetworkDrives)
        {
        #ifndef __SERIES60_30__
            #ifndef __SERIES60_31__
                User::LeaveIfError(iFs.DriveList(driveList, KDriveAttAll));
            #endif
        #else    
            User::LeaveIfError(iFs.DriveList(driveList));
        #endif        
        }
    else
        {
        User::LeaveIfError(iFs.DriveList(driveList));
        }
    
    iDriveEntryList->Reset();        
    
    for (TInt i=0; i<KMaxDrives; i++)
        {
        if (driveList[i])
            {
            TDriveEntry driveEntry;
            
            // set default icon
            driveEntry.iIconId = EFixedIconPhoneMemory;

            // get drive letter and number 
            driveEntry.iLetter = 'A' + i;
            iFs.CharToDrive(driveEntry.iLetter, driveEntry.iNumber);
            
            // get volume info and check errors
            if (iFs.Volume(driveEntry.iVolumeInfo, driveEntry.iNumber) == KErrNone)
                {
                // set media type descriptor
                TInt mediaType = driveEntry.iVolumeInfo.iDrive.iType;
                TBool extMountable( EFalse );
                
                if (mediaType == EMediaNotPresent)
                    driveEntry.iMediaTypeDesc = _L("Not present");
                else if (mediaType ==EMediaUnknown )
                    driveEntry.iMediaTypeDesc = _L("Unknown");
                else if (mediaType ==EMediaFloppy )
                    driveEntry.iMediaTypeDesc = _L("Floppy");
                else if (mediaType == EMediaHardDisk)
                    driveEntry.iMediaTypeDesc = _L("Mass storage");
                else if (mediaType == EMediaCdRom)
                    driveEntry.iMediaTypeDesc = _L("CD-ROM");
                else if (mediaType == EMediaRam)
                    driveEntry.iMediaTypeDesc = _L("RAM");
                else if (mediaType == EMediaFlash)
                    driveEntry.iMediaTypeDesc = _L("Flash");
                else if (mediaType == EMediaRom)
                    driveEntry.iMediaTypeDesc = _L("ROM");
                else if (mediaType == EMediaRemote)
                    driveEntry.iMediaTypeDesc = _L("Remote");
                else if (mediaType == EMediaNANDFlash)
                    driveEntry.iMediaTypeDesc = _L("NAND flash");
                
                // get real size of the ROM drive
                if (mediaType == EMediaRom)
                    {
                    TMemoryInfoV1Buf ramMemory;
                    UserHal::MemoryInfo(ramMemory);
                    driveEntry.iVolumeInfo.iSize = ramMemory().iTotalRomInBytes;
                    }
                
                // set attribute descripitions
                if (driveEntry.iVolumeInfo.iDrive.iBattery == EBatNotSupported)
                    driveEntry.iAttributesDesc.Append(_L("Battery not supported"));
                else if (driveEntry.iVolumeInfo.iDrive.iBattery == EBatGood)
                    driveEntry.iAttributesDesc.Append(_L("Battery good"));
                else if (driveEntry.iVolumeInfo.iDrive.iBattery == EBatLow)
                    driveEntry.iAttributesDesc.Append(_L("Battery low"));
                else
                    driveEntry.iAttributesDesc.Append(_L("Battery state unknown"));
                
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttLocal)
                    driveEntry.iAttributesDesc.Append(_L(" + Local"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRom)
                    driveEntry.iAttributesDesc.Append(_L(" + ROM"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRedirected)
                    driveEntry.iAttributesDesc.Append(_L("+ Redirected"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttSubsted)
                    driveEntry.iAttributesDesc.Append(_L(" + Substed"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttInternal)
                    driveEntry.iAttributesDesc.Append(_L(" + Internal"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRemovable)
                    {
                    driveEntry.iAttributesDesc.Append(_L(" + Removable"));
#if (!defined __SERIES60_30__ && !defined __SERIES60_31__)
                    TUint drvStatus( 0 );
                    if ( !DriveInfo::GetDriveStatus( iFs, driveEntry.iNumber, drvStatus ) &&
                         drvStatus & DriveInfo::EDriveExternallyMountable &&
                         drvStatus & DriveInfo::EDriveInternal )
                        {
                        extMountable = ETrue;
                        // iMediaTypeDesc already set as "Mass storage" 
                        }
                    else
                        {
                        driveEntry.iMediaTypeDesc = _L("Memory card");
                        }
#else
                    driveEntry.iMediaTypeDesc = _L("Memory card");
#endif
                    }
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRemote)
                    driveEntry.iAttributesDesc.Append(_L(" + Remote"));
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttTransaction)
                    driveEntry.iAttributesDesc.Append(_L(" + Transaction"));
                                
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttVariableSize)
                    driveEntry.iAttributesDesc.Append(_L(" + Variable size"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttDualDensity)
                    driveEntry.iAttributesDesc.Append(_L(" + Dual density"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttFormattable)
                    driveEntry.iAttributesDesc.Append(_L(" + Formattable"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttWriteProtected)
                    driveEntry.iAttributesDesc.Append(_L(" + Write protected"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttLockable)
                    driveEntry.iAttributesDesc.Append(_L(" + Lockable"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttLocked)
                    driveEntry.iAttributesDesc.Append(_L(" + Locked"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttHasPassword)
                    driveEntry.iAttributesDesc.Append(_L(" + Has password"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttReadWhileWrite)
                    driveEntry.iAttributesDesc.Append(_L(" + Read while write"));
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttDeleteNotify)
                    driveEntry.iAttributesDesc.Append(_L(" + Supports DeleteNotify"));
                
                
                // mark a removable media with memory card icon
                if (driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAttRemovable && !extMountable)
                    {
                    if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttLocked || driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAbsent)
                        {                
                        driveEntry.iIconId = EFixedIconMemoryCardDisabled;
                        }                 
                    else
                        {                
                        driveEntry.iIconId = EFixedIconMemoryCard;
                        } 
                    }
                }   
            
            // if this fails, likely it's a memory card which is not present
            else
                {
                TVolumeInfo volumeInfo;
                volumeInfo.iSize = 0;
                volumeInfo.iFree = 0;
                volumeInfo.iDrive.iDriveAtt = KDriveAttRemovable;
                volumeInfo.iDrive.iMediaAtt = KMediaAttWriteProtected;
                driveEntry.iVolumeInfo = volumeInfo;

                driveEntry.iMediaTypeDesc = _L("Not ready");
                driveEntry.iIconId = EFixedIconMemoryCardDisabled;
                }
            
            if ( iPrevFolderName != KNullDesC && iPrevFolderName[0] == driveEntry.iLetter )
                {
                iPrevFolderIndex = iDriveEntryList->Count();
                }
            
            iDriveEntryList->AppendL(driveEntry);
            }
        }
    
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::GetDirectoryListingL()
    {
    iFileEntryList->Reset();

    TInt sortMode(ESortByName);
    if (iSortMode == EFileBrowserCmdViewSortByExtension)    
        sortMode = ESortByExt; 
    else if (iSortMode == EFileBrowserCmdViewSortBySize)    
        sortMode = ESortBySize; 
    else if (iSortMode == EFileBrowserCmdViewSortByDate)    
        sortMode = ESortByDate; 
    
    TInt orderMode(EAscending);
    if (iOrderMode == EFileBrowserCmdViewOrderDescending)    
        orderMode = EDescending; 
    
    CDir* dir = NULL;
    if (iFs.GetDir(iCurrentPath, KEntryAttMatchMask, sortMode | orderMode | EDirsFirst, dir) == KErrNone)
        {
        CleanupStack::PushL(dir);
        
        for (TInt i=0; i<dir->Count(); i++)
            {
            TFileEntry fileEntry;
            fileEntry.iPath = iCurrentPath;
            fileEntry.iEntry = (*dir)[i];
            fileEntry.iDirEntries = KErrNotFound;
            fileEntry.iIconId = EFixedIconEmpty;
            
            if ( iPrevFolderName != KNullDesC && iPrevFolderName == fileEntry.iEntry.iName )
                {
                iPrevFolderIndex = i;
                }
            
            // check for directory entries
            if (fileEntry.iEntry.IsDir())
                {
                fileEntry.iIconId = EFixedIconFolder;

                TFileName subPath = fileEntry.iPath;
                subPath.Append(fileEntry.iEntry.iName);
                subPath.Append(_L("\\"));
                                
                // get number of entries in this directory if extended info about sub directories enabled
                if (iEngine->Settings().iShowSubDirectoryInfo && iEngine->Settings().iFileViewMode == EFileViewModeExtended)
                    {
                    CDir* subDir = NULL;
                    if (iFs.GetDir(subPath, KEntryAttMatchMask, ESortNone | EDirsFirst, subDir) == KErrNone)
                        {
                        fileEntry.iDirEntries = subDir->Count();
                        delete subDir;    
                        }
                    }
                
                // apply subfolder icon for known directories (speed improvement)
                if (subPath[0]=='Z' && (subPath.CompareF(_L("Z:\\data\\"))==0 || subPath.CompareF(_L("Z:\\Nokia\\"))==0
                    || subPath.Compare(_L("Z:\\private\\"))==0 || subPath.Compare(_L("Z:\\resource\\"))==0
                    || subPath.Compare(_L("Z:\\sys\\"))==0 || subPath.Compare(_L("Z:\\system\\"))==0))
                    {
                    fileEntry.iIconId = EFixedIconFolderSub;
                    }
                else if (subPath[0]=='Z' && (subPath.CompareF(_L("Z:\\sys\\bin\\"))==0))
                    {
                    // normal folder icon for these ones
                    fileEntry.iIconId = EFixedIconFolder;
                    }
                else
                    {
                    // check if this folder has subdirectories
                    CDir* subDir = NULL;
                    if (iFs.GetDir(subPath, KEntryAttDir|KEntryAttMatchMask, ESortNone | EDirsFirst, subDir) == KErrNone)
                        {
                        for (TInt j=0; j<subDir->Count(); j++)
                            {
                            TEntry entry = (*subDir)[j];

                            if (entry.IsDir())
                                {
                                fileEntry.iIconId = EFixedIconFolderSub;
                                break;    
                                }
                            }

                        delete subDir;    
                        }                    
                    }
                }

            iFileEntryList->AppendL(fileEntry);
            }
        
        CleanupStack::PopAndDestroy(); //dir    
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::MoveUpOneLevelL()
    {
    iPrevFolderIndex = KErrNotFound;
    iPrevFolderName.Zero();
    
    // change back to normal mode or move up
    if (iListingMode != ENormalEntries)
        {
        iListingMode = ENormalEntries;
        }
    else
        {
        // do nothing if displaying drive list view
        if (IsDriveListViewActive())
            return;
        
        if (iCurrentPath.Length() <= KDriveLetterStringLength)
            {
            if ( iCurrentPath.Length() > 0 )
                {
                iPrevFolderName.Append( iCurrentPath[0] );
                }
            // move to drive list view if the current path is already short enough
            iCurrentPath = KNullDesC;
            }
        else
            {
            // move one directory up
            TInt marker(iCurrentPath.Length());
            
            // find second last dir marker
            for (TInt i=iCurrentPath.Length()-2; i>=0; i--)
                {
                if (iCurrentPath[i] == '\\')
                    {
                    marker = i;
                    break;
                    }
                    
                }
            
            // update iPrevDir with the directory name that we just left 
            iPrevFolderName.Copy( iCurrentPath.RightTPtr( iCurrentPath.Length() -  marker - 1 ) );
            iPrevFolderName.Delete( iPrevFolderName.Length() -1, 2 ); // remove extra '\\'
            
            iCurrentPath = iCurrentPath.LeftTPtr(marker+1);
            }    
        }

//    if (iEngine->FileListContainer())
//        {
        //iEngine->FileListContainer()->DisableSearchFieldL();
        //iEngine->FileListContainer()->ListBox()->ClearSelection();
        //iEngine->FileListContainer()->ListBox()->SetCurrentItemIndex(0);
        //iEngine->FileListContainer()->SetNaviPaneTextL(iCurrentPath);
//        }
    
    // update view
    RefreshViewL();
    
    if (iEngine->Settings().iRememberFolderSelection
        //&& iEngine->FileListContainer()
        && iPrevFolderIndex > KErrNotFound )
        {
        
//        TInt visibleItems = iEngine->FileListContainer()->ListBox()->Rect().Height() /
//                            iEngine->FileListContainer()->ListBox()->ItemHeight()
//                            - 1; // for the title row
//
//        // By default, the selected item would be the last visible item on the listbox.
//        // We want the selected item be displayed at the middle of the listbox, so we
//        // select one of the successor items first to scroll down a bit
//        iEngine->FileListContainer()->ListBox()->SetCurrentItemIndex(
//            Min( iEngine->FileListContainer()->ListBox()->Model()->ItemTextArray()->MdcaCount() -1,
//                 iPrevFolderIndex + visibleItems / 2 ) );
//
//        // ...and after that we select the the correct item.
//        iEngine->FileListContainer()->ListBox()->SetCurrentItemIndex( iPrevFolderIndex );
        
        // update view
        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::MoveDownToDirectoryL(TInt aIndex)
    {

    // change back to normal mode
    if (iListingMode != ENormalEntries)
        {
        iListingMode = ENormalEntries;
        }
    
    if (aIndex >= 0)
        {
        TBool needsRefresh(EFalse);

        if (IsDriveListViewActive())
            {
            // currently in a drive list view, move to root of selected drive
            if (iDriveEntryList->Count() > aIndex)
                {
                TDriveEntry driveEntry = iDriveEntryList->At(aIndex);
                
                iCurrentPath.Append(driveEntry.iLetter);    
                iCurrentPath.Append(_L(":\\"));
                
                needsRefresh = ETrue;    
                }
            }
        else
            {
            // otherwise just append the new directory
            if (iFileEntryList->Count() > aIndex)
                {
                TFileEntry fileEntry = iFileEntryList->At(aIndex);
                
                if (fileEntry.iEntry.IsDir())
                    {
                    iCurrentPath.Copy(fileEntry.iPath);
                    iCurrentPath.Append(fileEntry.iEntry.iName);
                    iCurrentPath.Append(_L("\\"));

                    needsRefresh = ETrue;    
                    }
                }
            }

        if (needsRefresh /*&& iEngine->FileListContainer()*/)
            {
            //iEngine->FileListContainer()->DisableSearchFieldL();
            //iEngine->FileListContainer()->ListBox()->ClearSelection();
            //iEngine->FileListContainer()->ListBox()->SetCurrentItemIndex(0);
            //iEngine->FileListContainer()->SetNaviPaneTextL(iCurrentPath);

            // update view
            RefreshViewL();
            }
        }
    }
    
// --------------------------------------------------------------------------------------------

TBool CFileBrowserFileUtils::IsCurrentDriveReadOnly()
    {
    if (iListingMode !=ENormalEntries || iCurrentPath.Length() < 2)
        return EFalse;
    else
        {
        for (TInt i=0; i<iDriveEntryList->Count(); i++)
            {
            TDriveEntry driveEntry = iDriveEntryList->At(i);
            
            if (driveEntry.iLetter == iCurrentPath[0])
                {
                if (driveEntry.iVolumeInfo.iDrive.iMediaAtt == KMediaAttWriteProtected || driveEntry.iVolumeInfo.iDrive.iMediaAtt == KMediaAttLocked || driveEntry.iVolumeInfo.iDrive.iDriveAtt == KDriveAbsent)
                    return ETrue;    
                else
                    return EFalse;
                }
            }
        }

    return EFalse;
    }

// --------------------------------------------------------------------------------------------

TBool CFileBrowserFileUtils::IsItemDirectory(TInt aCurrentItemIndex)
    {
    if (aCurrentItemIndex < 0)
        {
        return EFalse;
        }
    else
        {
        if (iFileEntryList->Count() > aCurrentItemIndex)
            {
            TFileEntry fileEntry = iFileEntryList->At(aCurrentItemIndex) ;
            return fileEntry.iEntry.IsDir();
            }
        else
            {
            return EFalse;
            }
        }
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::SetSelectedItemsOrCurrentItemL(const CArrayFix<TInt>* selectionIndexes,
                                                           CFileEntryList* aFileEntryList)
    {
    aFileEntryList->Reset();

    // by default use selected items
    if (selectionIndexes && selectionIndexes->Count() > 0)
        {
        TInt ref(0);
        TKeyArrayFix key(0, ECmpTUint16);
        TInt index(0);

        for (TInt i=0; i<iFileEntryList->Count(); i++)
            {
            ref = i;

            if (selectionIndexes->Find(ref, key, index) == 0)
                {
                aFileEntryList->AppendL(iFileEntryList->At(i));
                }
            }
        }
    return aFileEntryList->Count();
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::ClipboardCutL(const CArrayFix<TInt>* aSelectionIndices)
    {
	
    iClipBoardMode = EClipBoardModeCut;
    TInt operations = SetSelectedItemsOrCurrentItemL(aSelectionIndices, iClipBoardList);
    return operations;
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::ClipboardCopyL(const CArrayFix<TInt>* aSelectionIndices)
    {
    iClipBoardMode = EClipBoardModeCopy;
    TInt operations = SetSelectedItemsOrCurrentItemL(aSelectionIndices, iClipBoardList);
    return operations;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ClipboardPasteL(const TOverwriteOptions &aOverwriteOptions)
    {
    if (iClipBoardMode == EClipBoardModeCut)
        {
        DoCopyToFolderL(iClipBoardList, iCurrentPath, aOverwriteOptions, ETrue );
        iClipBoardList->Reset();
        }
    else if (iClipBoardMode == EClipBoardModeCopy)
        {
        DoCopyToFolderL(iClipBoardList, iCurrentPath, aOverwriteOptions, EFalse);
        }
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::SetCurrentSelection(const CArrayFix<TInt>* aSelectionIndices)
    {
    TInt operations = SetSelectedItemsOrCurrentItemL(aSelectionIndices, iCurrentSelectionList);
    return operations;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::CopyToFolderL(TFileName aTargetDir,
                                          const TOverwriteOptions &aOverwriteOptions,
                                          TBool aMove)
    {

    //	TFileName destinationFolder;

    // generate an icon array
    // CAknIconArray* iconArray = GenerateIconArrayL(ETrue);
    //    CleanupStack::PushL(iconArray);

    // run folder selection dialog
    //    CFileBrowserDestinationFolderSelectionDlg* dlg = CFileBrowserDestinationFolderSelectionDlg::NewL(destinationFolder, iDriveEntryList, iconArray);

    // do the file operations
    //DoCopyToFolderL(entryList, aTargetDir, aOverwriteOptions, aMove);
    DoCopyToFolderL(iCurrentSelectionList, aTargetDir, aOverwriteOptions, aMove);

    //    CleanupStack::Pop(); //iconArray
    }

// --------------------------------------------------------------------------------------------

TBool CFileBrowserFileUtils::IsDestinationEntriesExists(const TDesC& aTargetDir)
    {
    TBool someEntryExists(EFalse);
    if (iCurrentSelectionList && iCurrentSelectionList->Count() > 0)
        {
        // check if some destination entries also exists
        for (TInt i=0; i<iCurrentSelectionList->Count(); i++)
            {
            TFileEntry fileEntry = iCurrentSelectionList->At(i);

            TFileName targetEntry = aTargetDir;
            targetEntry.Append(fileEntry.iEntry.iName);

            if (fileEntry.iEntry.IsDir())
                {
                targetEntry.Append(_L("\\"));

                if (BaflUtils::PathExists(iFs, targetEntry))
                    {
                    someEntryExists = ETrue;
                    break;
                    }
                }
            else
                {
                if (BaflUtils::FileExists(iFs, targetEntry))
                    {
                    someEntryExists = ETrue;
                    break;
                    }
                }
            }
        }
    return someEntryExists;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::DoCopyToFolderL(CFileEntryList* aEntryList,
                                            const TDesC& aTargetDir,
                                            const TOverwriteOptions &aOverwriteOptions,
                                            TBool aDeleteSource)
    {
    if (aEntryList && aEntryList->Count() > 0)
        {
        if (aOverwriteOptions.iDoFileOperations)
            {
            // set correct command id depending if we are copying or moving
            TInt commandId(EFileBrowserFileOpCommandCopy);
            if (aDeleteSource)
                commandId = EFileBrowserFileOpCommandMove;
            
            // do the file operations for each entry
            for (TInt i=0; i<aEntryList->Count(); i++)
                {
                TFileEntry fileEntry = aEntryList->At(i);
                
                TFileName targetEntry = aTargetDir;
                targetEntry.Append(fileEntry.iEntry.iName);
                
                if (aOverwriteOptions.iQueryIndex == EFileActionGenerateUniqueFilenames)
                    {
                    CApaApplication::GenerateFileName(iFs, targetEntry);
                    }
                else if (aOverwriteOptions.iQueryIndex == EFileActionQueryPostFix)
                    {
                    targetEntry.Append(aOverwriteOptions.iPostFix);
                    }

                
                // append the new command to the command array
                if (fileEntry.iEntry.IsDir())
                    {
                    AppendToCommandArrayL(commandId,
                                          new(ELeave)CCommandParamsCopyOrMove(fileEntry, targetEntry, aOverwriteOptions.iOverWriteFlags|CFileMan::ERecurse)
                                         );
                    }
                else
                    {
                    AppendToCommandArrayL(commandId,
                                          new(ELeave)CCommandParamsCopyOrMove(fileEntry, targetEntry, aOverwriteOptions.iOverWriteFlags)
                                         );
                    }

                }
            }
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::DeleteL()
    {
    // do the file operations for each entry
    for (TInt i=0; i<iCurrentSelectionList->Count(); i++)
        {
        TFileEntry fileEntry = iCurrentSelectionList->At(i);

        // append the new command to the command array
        if (fileEntry.iEntry.IsDir())
            {
            AppendToCommandArrayL(EFileBrowserFileOpCommandDelete,
                                  new(ELeave)CCommandParamsDelete(fileEntry, CFileMan::ERecurse)
                                 );
            }
        else
            {
            AppendToCommandArrayL(EFileBrowserFileOpCommandDelete,
                                  new(ELeave)CCommandParamsDelete(fileEntry, 0)
                                 );
            }

        }
    }


// --------------------------------------------------------------------------------------------

TBool CFileBrowserFileUtils::SelectionHasDirs()
{
    TBool isDir(EFalse);

    // check if any directories and ask recursion
    for (TInt i=0; i<iCurrentSelectionList->Count(); i++)
        {
        TFileEntry fileEntry = iCurrentSelectionList->At(i);

        if (fileEntry.iEntry.IsDir())
            {
            isDir = ETrue;
            break;
            }
        }
    return isDir;
}

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::TouchL(TBool aRecurse)
    {
    // now go through all entries
    for (TInt i=0; i<iCurrentSelectionList->Count(); i++)
        {
        TFileEntry fileEntry = iCurrentSelectionList->At(i);

        // touch to current time
        TTime now;
        now.UniversalTime();


        // append the new command to the command array
        if (fileEntry.iEntry.IsDir() && aRecurse)
            {
            AppendToCommandArrayL(EFileBrowserFileOpCommandAttribs,
                                  new(ELeave)CCommandParamsAttribs(fileEntry, NULL, NULL, now, CFileMan::ERecurse)
                                 );
            }
        else
            {
            AppendToCommandArrayL(EFileBrowserFileOpCommandAttribs,
                                  new(ELeave)CCommandParamsAttribs(fileEntry, NULL, NULL, now, 0)
                                 );
            }
        }
    }

TBool CFileBrowserFileUtils::TargetExists(const TInt aIndex, const TFileName &newName)
    {
    TBool alreadyExists(EFalse);
    if (iFileEntryList->Count() > aIndex && aIndex >= 0)
        {

        TFileEntry fileEntry = iFileEntryList->At(aIndex);
        TFileName targetEntry = fileEntry.iPath;
        targetEntry.Append(newName);
        if (fileEntry.iEntry.IsDir())
            {
            //targetEntry.Append(_L("\\"));
            alreadyExists = BaflUtils::PathExists(iFs, targetEntry);
            }
        else
            {
            alreadyExists = BaflUtils::FileExists(iFs, targetEntry);
            }
        }
    return alreadyExists;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::RenameL(const TInt aIndex, const TFileName &newName)
    {
        if (iFileEntryList->Count() > aIndex && aIndex >= 0)
        {
            TFileEntry fileEntry = iFileEntryList->At(aIndex);

            TFileName targetEntry = fileEntry.iPath;
            targetEntry.Append(newName);

            // append the new command to the command array
            AppendToCommandArrayL(EFileBrowserFileOpCommandRename,
                                  new(ELeave)CCommandParamsRename(fileEntry, targetEntry, CFileMan::EOverWrite)
                                 );
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SetAttributesL(TUint &aSetAttMask, TUint &aClearAttMask, TBool &aRecurse)
    {
    for (TInt i=0; i<iCurrentSelectionList->Count(); i++)
        {
        TFileEntry fileEntry = iCurrentSelectionList->At(i);

        // append the new command to the command array
        if (fileEntry.iEntry.IsDir() && aRecurse)
            {
            AppendToCommandArrayL(EFileBrowserFileOpCommandAttribs,
                                  new(ELeave)CCommandParamsAttribs(fileEntry, aSetAttMask, aClearAttMask, fileEntry.iEntry.iModified, CFileMan::ERecurse)
                                 );
            }
        else
            {
            AppendToCommandArrayL(EFileBrowserFileOpCommandAttribs,
                                  new(ELeave)CCommandParamsAttribs(fileEntry, aSetAttMask, aClearAttMask, fileEntry.iEntry.iModified, 0)
                                 );
            }
        }

    // execute all operations
    StartExecutingCommandsL(_L("Changing attributes"));
    }
       
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SearchL()
    {
//    iEngine->FileListContainer()->SetScreenLayoutL(EDisplayModeNormal);
//    iEngine->FileListContainer()->SetNaviPaneTextL(KNullDesC);
//
//    iSearchAttributes.iSearchDir = iCurrentPath;
//
//    CFileBrowserSearchQueryDlg* dlg = CFileBrowserSearchQueryDlg::NewL(iSearchAttributes);
//    TBool dlgResult = dlg->RunQueryLD();
//
//    iEngine->FileListContainer()->SetScreenLayoutL(iEngine->Settings().iDisplayMode);
//    iEngine->FileListContainer()->SetNaviPaneTextL(iCurrentPath);
//
//    if (dlgResult)
//        {
//        iEngine->EikonEnv()->BusyMsgL(_L("** Searching **"), TGulAlignment(EHCenterVTop));


        iEngine->FileBrowserUI()->ShowWaitDialog(_L("Searching"));
        iFileEntryList->Reset();

        // if search dir is empty, find from all drives
        if (iSearchAttributes.iSearchDir == KNullDesC)
            {
            for (TInt i=0; i<iDriveEntryList->Count(); i++)
                {
                TDriveEntry driveEntry = iDriveEntryList->At(i);

                TBuf<10> driveRoot;
                driveRoot.Append(driveEntry.iLetter);
                driveRoot.Append(_L(":\\"));

                DoSearchFiles(iSearchAttributes.iWildCards, driveRoot);

                if (iSearchAttributes.iRecurse)
                    DoSearchFilesRecursiveL(iSearchAttributes.iWildCards, driveRoot);
                }

            }

        // otherwise just search from the selected directory
        else
            {
            DoSearchFiles(iSearchAttributes.iWildCards, iSearchAttributes.iSearchDir);

            if (iSearchAttributes.iRecurse)
                DoSearchFilesRecursiveL(iSearchAttributes.iWildCards, iSearchAttributes.iSearchDir);
            }

//        iEngine->EikonEnv()->BusyMsgCancel();
        iEngine->FileBrowserUI()->CancelWaitDialog();

//        TInt operations = iFileEntryList->Count();

        iListingMode = ESearchResults;
        // TODO iEngine->FileListContainer()->ListBox()->SetCurrentItemIndex(0);
        RefreshViewL();

//        _LIT(KMessage, "%d entries found");
//        TFileName noteMsg;
//        noteMsg.Format(KMessage, operations);
//
//        iEngine->FileBrowserUI()->ShowInformationNote(noteMsg, _L(""));
//        }
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::DoSearchFiles(const TDesC& aFileName, const TDesC& aPath)
    {
    TFindFile fileFinder(iFs);
    CDir* dir;
    TInt err = fileFinder.FindWildByPath(aFileName, &aPath, dir);

    while (err == KErrNone && iAllowProcessing)
        {
        iEngine->FileBrowserUI()->ProcessEvents();
        for (TInt i=0; i<dir->Count(); i++)
            {
            TEntry entry = (*dir)[i];
            
            TTime entryModified = entry.iModified;
            // convert from universal time
            if ( iTz.ConvertToLocalTime( entryModified ) == KErrNone )
                {
                entryModified = entry.iModified;
                }
            if ((entry.IsDir() && entryModified >= iSearchAttributes.iMinDate && entryModified <= iSearchAttributes.iMaxDate) ||
                (!entry.IsDir() &&entry.iSize >= iSearchAttributes.iMinSize && entry.iSize <= iSearchAttributes.iMaxSize &&
                 entryModified >= iSearchAttributes.iMinDate && entryModified <= iSearchAttributes.iMaxDate))
                {
                TParse parsedName;
                parsedName.Set(entry.iName, &fileFinder.File(), NULL);
                
                if (parsedName.Drive().Length() && aPath.Length() && parsedName.Drive()[0] == aPath[0])
                    {
                    TFileEntry fileEntry;
                    fileEntry.iPath = parsedName.DriveAndPath();
                    fileEntry.iEntry = entry;
                    fileEntry.iDirEntries = KErrNotFound;
                    
                    TBool appendToArray(EFalse);
                    
                    // append directories always
                    if (entry.IsDir() && !iSearchAttributes.iTextInFile.Length())
                        {
                        fileEntry.iIconId = EFixedIconFolder;
                        appendToArray = ETrue;    
                        }
                    
                    // normal file
                    else
                        {
                        fileEntry.iIconId = EFixedIconEmpty;
                        
                        // check if a string needs to be found inside the file
                        if (iSearchAttributes.iTextInFile.Length())
                            {
                            // currently only 8-bit searching, so convert from 16-bit
                            TBuf8<256> searchText;
                            searchText.Copy(iSearchAttributes.iTextInFile);
                            
                            // specify buffer size and create a buffer
                            const TInt KReadBufSize = 1024*1024;
                            HBufC8* buffer = HBufC8::NewLC(KReadBufSize);
                            TPtr8 bufferPtr = buffer->Des();

                            // open the file for reading
                            RFile file;
                            if (file.Open(iFs, parsedName.FullName(), EFileRead) == KErrNone)
                                {
                                TInt currentPos(0);
                                
                                for (;;)
                                    {
                                    // read from the file
                                    file.Read(currentPos, bufferPtr, KReadBufSize);

                                    // stop looping if the read buffer isn't long enough
                                    if (bufferPtr.Length() < searchText.Length())
                                        {
                                        break;
                                        }
                                        
                                    // try to find the text
                                    if (bufferPtr.FindF(searchText) >= 0)
                                        {
                                        // match found!
                                        appendToArray = ETrue;
                                        break;
                                        }
                                        
                                    // calculate the next position
                                    currentPos += KReadBufSize - searchText.Length();    
                                    }
                                
                                file.Close();
                                }
                            
                            CleanupStack::PopAndDestroy(); //buffer
                            }
                        else
                            {
                            appendToArray = ETrue;
                            }
                        }

                    if (appendToArray)
                        TRAP(err, iFileEntryList->AppendL(fileEntry));    
                    }
                }
            }

        delete dir;
        err = fileFinder.FindWild(dir);
        }

    return err;
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::DoSearchFilesRecursiveL(const TDesC& aFileName, const TDesC& aPath)
	{
    TInt err(KErrNone);
    CDirScan* scan = CDirScan::NewLC(iFs);
    scan->SetScanDataL(aPath, KEntryAttDir|KEntryAttMatchMask, ESortByName | EAscending | EDirsFirst);
    CDir* dir = NULL;

    for(;;)
        {
        TRAP(err, scan->NextL(dir));
        if (!dir  || (err != KErrNone))
            break;

        for (TInt i=0; i<dir->Count(); i++)
            {
            TEntry entry = (*dir)[i];
            
            if (entry.IsDir())
                {
                TFileName path(scan->FullPath());
                
                if (path.Length())
                    {
                    path.Append(entry.iName);
                    path.Append(_L("\\"));
                    DoSearchFiles(aFileName, path);
                    }
                }
            }
        delete(dir);
        }

    CleanupStack::PopAndDestroy(scan);
    return err;
    }
         
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::NewFileL(const TFileName &aNewFileName)
    {
    TFileName fullPath = iCurrentPath;
    fullPath.Append(aNewFileName);

    TParse nameParser;
    TInt err = nameParser.SetNoWild(fullPath, NULL, NULL);

    if (err == KErrNone)
        {
        // do not try to recreate the file if it already exists
        if (BaflUtils::PathExists(iFs, nameParser.DriveAndPath()))
            {
            err = iFileOps->CreateEmptyFile(fullPath);
            }
        else
            err = KErrAlreadyExists;
        }

    if (err == KErrNone)
        {
        iEngine->FileBrowserUI()->ShowConfirmationNote(_L("New file created"));
        }

    else if (err == KErrAlreadyExists)
        {
        iEngine->FileBrowserUI()->ShowInformationNote(_L("File already exists"), _L(""));
        }

    else
        {
        CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

        iEngine->FileBrowserUI()->ShowErrorNote( textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

        CleanupStack::PopAndDestroy();  //textResolver
        }

    RefreshViewL();
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::NewDirectoryL(const TFileName &aNewDirectoryName)
    {       
    TFileName fullPath = iCurrentPath;
    fullPath.Append(aNewDirectoryName);
    fullPath.Append(_L("\\"));

    TInt err = iFileOps->MkDirAll(fullPath, 0, ETrue);

    if (err == KErrNone)
        {
        iEngine->FileBrowserUI()->ShowConfirmationNote(_L("New directory created"));
        }

    else if (err == KErrAlreadyExists)
        {
        iEngine->FileBrowserUI()->ShowInformationNote(_L("Directory already exists"), _L(""));
        }

    else
        {
        CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

        iEngine->FileBrowserUI()->ShowErrorNote( textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

        CleanupStack::PopAndDestroy();  //textResolver
        }

    RefreshViewL();
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SendToL()
    {
//    CFileEntryList* entries = new(ELeave) CFileEntryList(32);
//    GetSelectedItemsOrCurrentItemL(entries);
//    CleanupStack::PushL(entries);
//
//    TInt operations(0);
//
//    CSendUi* sendUi = CSendUi::NewL();
//    CleanupStack::PushL(sendUi);
//    CMessageData* messageData = CMessageData::NewL();
//    CleanupStack::PushL(messageData);
//
//    for (TInt i=0; i<entries->Count(); i++)
//        {
//        TFileEntry fileEntry = entries->At(i);
//
//        // only supported for normal file entries
//        if (!fileEntry.iEntry.IsDir())
//            {
//            TFileName fullPath = fileEntry.iPath;
//            fullPath.Append(fileEntry.iEntry.iName);
//
//            messageData->AppendAttachmentL(fullPath);
//            operations++;
//
//            }
//        }
//
//    if (operations > 0)
//        {
//        sendUi->ShowQueryAndSendL(messageData, TSendingCapabilities(0, 0, TSendingCapabilities::ESupportsAttachments));
//
//        // I guess this note is not needed..
////        _LIT(KMessage, "%d entries sent");
////        TFileName noteMsg;
////        noteMsg.Format(KMessage, operations);
////
////        iEngine->FileBrowserUI()->ShowConfirmationNote(noteMsg);
//        }
//    else
//        {
//        _LIT(KMessage, "Nothing to send");
//        iEngine->FileBrowserUI()->ShowInformationNote(KMessage, _L(""));
//        }
//
//    CleanupStack::PopAndDestroy(3); // entries, sendUi, messageData
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::CompressL()
    {
//    CFileEntryList* entries = new(ELeave) CFileEntryList(16);
//    GetSelectedItemsOrCurrentItemL(entries);
//    CleanupStack::PushL(entries);
//
//    for (TInt i=0; i<entries->Count(); i++)
//        {
//        TFileEntry fileEntry = entries->At(i);
//
//        TFileName sourceEntry = fileEntry.iPath;
//        sourceEntry.Append(fileEntry.iEntry.iName);
//
//        TFileName targetEntry = sourceEntry;
//        targetEntry.Append(KGZipExt);
//
//        TFileName targetEntryShort;
//        targetEntryShort.Copy(fileEntry.iEntry.iName);
//        targetEntryShort.Append(KGZipExt);
//
//        if (BaflUtils::FileExists(iFs, targetEntry))
//            {
//            _LIT(KMessage, "%S already exists");
//            TFileName noteMsg;
//            noteMsg.Format(KMessage, &targetEntryShort);
//
//            iEngine->FileBrowserUI()->ShowInformationNote(noteMsg, _L(""));
//            }
//        else
//            {
//            TRAPD(err,
//                RFile inputFile;
//                User::LeaveIfError(inputFile.Open(iFs, sourceEntry, EFileStream | EFileRead | EFileShareAny));
//                CleanupClosePushL(inputFile);
//
//                CEZFileToGZip* gZip = CEZFileToGZip::NewLC(iFs, targetEntry, inputFile);
//
//                while (gZip->DeflateL())
//                    ;
//
//                CleanupStack::PopAndDestroy(2); //inputFile, gZip
//                );
//
//             if (err == KErrNone)
//                {
//                _LIT(KMessage, "%S created succesfully");
//                TFileName noteMsg;
//                noteMsg.Format(KMessage, &targetEntryShort);
//
//                iEngine->FileBrowserUI()->ShowConfirmationNote(noteMsg);
//                }
//             else
//                {
//                _LIT(KMessage, "Unable to create %S");
//                TFileName noteMsg;
//                noteMsg.Format(KMessage, &targetEntryShort);
//
//                iEngine->FileBrowserUI()->ShowErrorNote(noteMsg);
//                }
//
//            RefreshViewL();
//            }
//        }
//
//    CleanupStack::PopAndDestroy(); //entries
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::DecompressL()
    {
//    CFileEntryList* entries = new(ELeave) CFileEntryList(16);
//    GetSelectedItemsOrCurrentItemL(entries);
//    CleanupStack::PushL(entries);
//
//    for (TInt i=0; i<entries->Count(); i++)
//        {
//        TFileEntry fileEntry = entries->At(i);
//
//        TFileName sourceEntry = fileEntry.iPath;
//        sourceEntry.Append(fileEntry.iEntry.iName);
//
//        TFileName targetEntry;
//        TFileName targetEntryShort;
//
//        TInt sourceNameLen = fileEntry.iEntry.iName.Length();
//        TInt gZipExtLen = KGZipExt().Length();
//
//        if (sourceNameLen > gZipExtLen && sourceEntry.Right(gZipExtLen).CompareF(KGZipExt) == 0)
//            {
//            targetEntry = sourceEntry.Left(sourceEntry.Length() - gZipExtLen);
//            targetEntryShort = fileEntry.iEntry.iName.Left(sourceNameLen - gZipExtLen);
//
//            if (BaflUtils::FileExists(iFs, targetEntry))
//                {
//                _LIT(KMessage, "%S already exists");
//                TFileName noteMsg;
//                noteMsg.Format(KMessage, &targetEntryShort);
//
//                iEngine->FileBrowserUI()->ShowInformationNote(noteMsg, _L(""));
//                }
//            else
//                {
//                TRAPD(err,
//                    RFile outputFile;
//                    User::LeaveIfError(outputFile.Create(iFs, targetEntry, EFileStream | EFileWrite | EFileShareExclusive));
//                    CleanupClosePushL(outputFile);
//
//                    CEZGZipToFile* gZip = CEZGZipToFile::NewLC(iFs, sourceEntry, outputFile);
//
//                    while (gZip->InflateL())
//                        ;
//
//                    CleanupStack::PopAndDestroy(2); //outputFile, gZip
//                    );
//
//                 if (err == KErrNone)
//                    {
//                    _LIT(KMessage, "%S decompressed succesfully");
//                    TFileName noteMsg;
//                    noteMsg.Format(KMessage, &fileEntry.iEntry.iName);
//
//                    iEngine->FileBrowserUI()->ShowConfirmationNote(noteMsg);
//                    }
//                 else
//                    {
//                    _LIT(KMessage, "Unable to decompress %S");
//                    TFileName noteMsg;
//                    noteMsg.Format(KMessage, &fileEntry.iEntry.iName);
//
//                    iEngine->FileBrowserUI()->ShowErrorNote(noteMsg);
//                    }
//
//                RefreshViewL();
//                }
//            }
//        else
//            {
//            _LIT(KMessage, "%S does not have %S extension");
//            TFileName noteMsg;
//            noteMsg.Format(KMessage, &fileEntry.iEntry.iName, &KGZipExt);
//
//            iEngine->FileBrowserUI()->ShowInformationNote(noteMsg, _L(""));
//            }
//        }
//
//    CleanupStack::PopAndDestroy(); //entries
    }

// --------------------------------------------------------------------------------------------

bool CFileBrowserFileUtils::PropertiesL(TInt aCurrentItemIndex, CDesCArray* entryLines, TFileName &titleText)
    {

    TBool showDialog(EFalse);
    
    _LIT(KPropertiesEntryStr,       "%S\t%S");
    _LIT(KPropertiesEntryInt,       "%S\t%d");
    _LIT(KPropertiesEntryUintHex,   "%S\t0x%x");
    
    _LIT(KDateFormat,               "%D%M%Y%/0%1%/1%2%/2%3%/3");
    _LIT(KTimeFormat,               "%-B%:0%J%:1%T%:2%S%:3%+B");
    
    _LIT(KMediaType,                "Media");
    _LIT(KAttributes,               "Atts");
    _LIT(KVolumeName,               "Name");
    _LIT(KUniqueID,                 "ID");
    _LIT(KNumberOfEntries,          "Entries");
    _LIT(KNumberOfFiles,            "Files");
    _LIT(KUsed,                     "Used (B)");
    _LIT(KFree,                     "Free (B)");
    _LIT(KSize,                     "Size (B)");
    _LIT(KPath,                     "Path");
    _LIT(KDate,                     "Date");
    _LIT(KTime,                     "Time");
    _LIT(KMimeType,                 "Mime");
    _LIT(KOpensWith,                "Opens");
 

    // create an array for the items
//    CDesCArray* entryLines = new(ELeave) CDesCArrayFlat(16);
//    CleanupStack::PushL(entryLines);
//    TFileName titleText;
    
    // just get current item
//    TInt currentItemIndex = iEngine->QueryCurrentItemIndex();
    
    if (IsDriveListViewActive() && iDriveEntryList->Count() > aCurrentItemIndex && aCurrentItemIndex >= 0)
        {
        // it is a drive entry
        TDriveEntry driveEntry = iDriveEntryList->At(aCurrentItemIndex);
        TFileName textEntry;
        
        // set title
        titleText.Append(driveEntry.iLetter);
        titleText.Append(_L(":"));
        
        // media type
        textEntry.Format(KPropertiesEntryStr, &KMediaType, &driveEntry.iMediaTypeDesc);
        entryLines->AppendL(textEntry);

        // attributes
        textEntry.Format(KPropertiesEntryStr, &KAttributes, &driveEntry.iAttributesDesc);
        entryLines->AppendL(textEntry);

        // volume name
        if (driveEntry.iVolumeInfo.iName.Length())
            {
            textEntry.Format(KPropertiesEntryStr, &KVolumeName, &driveEntry.iVolumeInfo.iName);
            entryLines->AppendL(textEntry);
            }

        // unique id
        textEntry.Format(KPropertiesEntryUintHex, &KUniqueID, driveEntry.iVolumeInfo.iUniqueID);
        entryLines->AppendL(textEntry);
        
        // number of files
        if (iEngine->Settings().iShowSubDirectoryInfo)
            {
            iFindFileEntryList->Reset();
            
            TBuf<10> driveRoot;
            driveRoot.Append(driveEntry.iLetter);
            driveRoot.Append(_L(":\\"));

            DoFindFiles(_L("*"), driveRoot);
            DoFindFilesRecursiveL(_L("*"), driveRoot);
            
            textEntry.Format(KPropertiesEntryInt, &KNumberOfFiles, iFindFileEntryList->Count());
            entryLines->AppendL(textEntry);
            
            iFindFileEntryList->Reset();
            }
      
        // used
        TBuf<32> usedBuf;
        usedBuf.AppendNum(driveEntry.iVolumeInfo.iSize-driveEntry.iVolumeInfo.iFree, TRealFormat(KDefaultRealWidth, 0));
        textEntry.Format(KPropertiesEntryStr, &KUsed, &usedBuf);
        entryLines->AppendL(textEntry);        

        // free
        TBuf<32> freeBuf;
        freeBuf.AppendNum(driveEntry.iVolumeInfo.iFree, TRealFormat(KDefaultRealWidth, 0));
        textEntry.Format(KPropertiesEntryStr, &KFree, &freeBuf);
        entryLines->AppendL(textEntry);
                
        // size
        TBuf<32> sizeBuf;
        sizeBuf.AppendNum(driveEntry.iVolumeInfo.iSize, TRealFormat(KDefaultRealWidth, 0));
        textEntry.Format(KPropertiesEntryStr, &KSize, &sizeBuf);
        entryLines->AppendL(textEntry);   

        showDialog = ETrue;
        }
    
    else if (iFileEntryList->Count() > aCurrentItemIndex && aCurrentItemIndex >= 0)
        {
        // it is a file or a directory entry
        TFileEntry fileEntry = iFileEntryList->At(aCurrentItemIndex);
        TFileName textEntry;
        
        // set title
        titleText.Append(fileEntry.iEntry.iName);
        
        // path
        textEntry.Format(KPropertiesEntryStr, &KPath, &fileEntry.iPath);
        entryLines->AppendL(textEntry);

        // date
        TTime entryModified = fileEntry.iEntry.iModified;
        
        // convert from universal time
        if ( iTz.ConvertToLocalTime( entryModified ) == KErrNone )
            {
            entryModified = fileEntry.iEntry.iModified; // use universal time
            }
        
        TBuf<32> dateBuf;
        entryModified.FormatL(dateBuf, KDateFormat);
        textEntry.Format(KPropertiesEntryStr, &KDate, &dateBuf);
        entryLines->AppendL(textEntry);

        // time
        TBuf<32> timeBuf;
        entryModified.FormatL(timeBuf, KTimeFormat);
        textEntry.Format(KPropertiesEntryStr, &KTime, &timeBuf);
        entryLines->AppendL(textEntry);

        if (!fileEntry.iEntry.IsDir())
            {
            // size
            TBuf<32> sizeBuf;
            sizeBuf.AppendNum(fileEntry.iEntry.iSize, TRealFormat(KDefaultRealWidth, 0));
            textEntry.Format(KPropertiesEntryStr, &KSize, &sizeBuf);
            entryLines->AppendL(textEntry);            
            }
        else if (fileEntry.iEntry.IsDir() && iEngine->Settings().iShowSubDirectoryInfo)
            {
            iFindFileEntryList->Reset();
            
            TFileName fullPath;
            fullPath.Append(fileEntry.iPath);
            fullPath.Append(fileEntry.iEntry.iName);
            fullPath.Append(_L("\\"));
            
            // number of entries
            CDir* subDir = NULL;
            if (iFs.GetDir(fullPath, KEntryAttMatchMask, ESortNone | EDirsFirst, subDir) == KErrNone)
                {
                CleanupStack::PushL(subDir);
                textEntry.Format(KPropertiesEntryInt, &KNumberOfEntries, subDir->Count());
                entryLines->AppendL(textEntry);
                CleanupStack::PopAndDestroy(); //subDir 
                }
            
            // number of files
            DoFindFiles(_L("*"), fullPath);
            DoFindFilesRecursiveL(_L("*"), fullPath);
            textEntry.Format(KPropertiesEntryInt, &KNumberOfFiles, iFindFileEntryList->Count());
            entryLines->AppendL(textEntry);
            
            // size
            TInt64 size(0);
            for (TInt i=0; i<iFindFileEntryList->Count(); i++)
                {
                TFileEntry fileEntry = iFindFileEntryList->At(i);
                size += fileEntry.iEntry.iSize;
                }
            
            TBuf<32> sizeBuf;
            sizeBuf.AppendNum(size, TRealFormat(KDefaultRealWidth, 0));
            textEntry.Format(KPropertiesEntryStr, &KSize, &sizeBuf);
            entryLines->AppendL(textEntry); 
            
            iFindFileEntryList->Reset();            
            }
        
        // attributes
        TBuf<32> attBuf;
        if (fileEntry.iEntry.IsArchive())
            attBuf.Append(_L("Arc "));
        if (fileEntry.iEntry.IsHidden())
            attBuf.Append(_L("Hid "));
        if (fileEntry.iEntry.IsReadOnly())
            attBuf.Append(_L("R/O "));
        if (fileEntry.iEntry.IsSystem())
            attBuf.Append(_L("Sys"));
        textEntry.Format(KPropertiesEntryStr, &KAttributes, &attBuf);
        entryLines->AppendL(textEntry);            

        if (!fileEntry.iEntry.IsDir())
            {
            // mime type
            TFileName fullPath = fileEntry.iPath;
            fullPath.Append(fileEntry.iEntry.iName);
            TDataType dataType;
            TUid appUid;
            if (iEngine->LsSession().AppForDocument(fullPath, appUid, dataType) == KErrNone)
                {
                TBuf<128> mimeTypeBuf;
                mimeTypeBuf.Copy(dataType.Des8());
                if (mimeTypeBuf == KNullDesC)
                    mimeTypeBuf.Copy(_L("N/A"));
                textEntry.Format(KPropertiesEntryStr, &KMimeType, &mimeTypeBuf);
                entryLines->AppendL(textEntry); 
                }

            // opens with
            TApaAppInfo appInfo;
            if (iEngine->LsSession().GetAppInfo(appInfo, appUid) == KErrNone)
                {
                TFileName opensWithBuf;
                _LIT(KOpensWithFormat, "%S (0x%08X)");
                opensWithBuf.Format(KOpensWithFormat, &appInfo.iCaption, appInfo.iUid);
                textEntry.Format(KPropertiesEntryStr, &KOpensWith, &opensWithBuf);
                entryLines->AppendL(textEntry); 
                }
            }
        
        showDialog = ETrue;
        }   
    return showDialog;

//    if (showDialog)
//        {
//        CAknSingleHeadingPopupMenuStyleListBox* listBox = new(ELeave) CAknSingleHeadingPopupMenuStyleListBox;
//        CleanupStack::PushL(listBox);
//
//        CAknPopupList* popupList = CAknPopupList::NewL(listBox, R_AVKON_SOFTKEYS_OK_EMPTY, AknPopupLayouts::EMenuDoubleWindow);
//        CleanupStack::PushL(popupList);
//
//        listBox->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
//        listBox->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
//        listBox->CreateScrollBarFrameL(ETrue);
//        listBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
//        listBox->Model()->SetItemTextArray(entryLines);
//        listBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
//        listBox->HandleItemAdditionL();
//
//        popupList->SetTitleL(titleText);
//        popupList->ExecuteLD();
//
//        CleanupStack::Pop(); // popupList
//        CleanupStack::PopAndDestroy(); // listBox
//        }
//
//        CleanupStack::PopAndDestroy(); //entryLines
	}
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::OpenWithApparcL(TFileName aFileName)
    {
       
        TFileName fullPath = aFileName;
        TInt err1(KErrNone), err2(KErrNone);
        TUid appUid;
        TDataType dataType;
        TBool isProgram;
        
	
        err1 = iEngine->LsSession().AppForDocument(fullPath, appUid, dataType);
        err2 = iEngine->LsSession().IsProgram(fullPath, isProgram);
        
        if (err1 == KErrNone && err2 == KErrNone)
            {
            if (appUid != KNullUid)
                {
                // found an app, run using it
                TApaTaskList taskList(CEikonEnv::Static()->WsSession());
                TApaTask task = (taskList.FindApp(appUid));
                if (task.Exists())
                    {
                    User::LeaveIfError(task.SwitchOpenFile(fullPath));
                    task.BringToForeground();
                    }
                else
                    {
                    TThreadId id;
                    iEngine->LsSession().StartDocument(fullPath, appUid, id, RApaLsSession::ESwitchFiles);
                    }
                }
            else if (isProgram)
                {
                LaunchProgramL(fullPath);
                }            
            }
        //}   
    }
	
// --------------------------------------------------------------------------------------------

//void CFileBrowserFileUtils::HandleServerAppExit(TInt aReason)
//	{
//	if (iOpenFileService)
//		{
//		delete iOpenFileService;
//		iOpenFileService = NULL;
//		}
//	MAknServerAppExitObserver::HandleServerAppExit(aReason);
//	}

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::OpenWithDocHandlerL(TFileName aFileName, TBool aEmbed)
    {
        TFileName fullPath = aFileName;
        RFile sharableFile;
        iDocHandler->OpenTempFileL(fullPath, sharableFile);
        CleanupClosePushL(sharableFile);
        TDataType dataType;
        if (aEmbed) 
            {
    	    iDocHandler->OpenFileEmbeddedL(sharableFile, dataType);
            }
        else
            {
    	    iDocHandler->OpenFileL(sharableFile, dataType);
            }
        CleanupStack::PopAndDestroy();  //sharableFile
    //    }   
    }

	// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::OpenWithFileServiceL(TInt aCurrentItemIndex)
    {
    if (iFileEntryList->Count() > aCurrentItemIndex && aCurrentItemIndex >= 0)
        {
        TFileEntry fileEntry = iFileEntryList->At(aCurrentItemIndex);
        
        TFileName fullPath = fileEntry.iPath;
        fullPath.Append(fileEntry.iEntry.iName);

    	TBool itemSelected(EFalse);
        RFile sharableFile;
//        TUid appUidExtracted;
        
        TRAPD(err,
           
        	// use doc handler to get a sharable file handle
            iDocHandler->OpenTempFileL(fullPath, sharableFile);
            CleanupClosePushL(sharableFile);
            
            // find the data type
            TDataRecognitionResult dataType;
        	User::LeaveIfError(iEngine->LsSession().RecognizeData(sharableFile, dataType));

        	// get all apps that support OpenFileService for this datatype
        	CApaAppServiceInfoArray* apps = iEngine->LsSession().GetServiceImplementationsLC(KOpenServiceUid, dataType.iDataType);
        	TArray<TApaAppServiceInfo> appServiceInfos = apps->Array();
    	
        	if (appServiceInfos.Count() > 0)
        		{
        		// build an array of the app names
        		CDesCArray* names = new (ELeave) CDesCArrayFlat(16);
        		CleanupStack::PushL(names);
        		for (TInt ii=0; ii<appServiceInfos.Count(); ii++)
        			{
        			TUid appUid = appServiceInfos[ii].Uid();
        			TApaAppInfo appInfo;
        			if (iEngine->LsSession().GetAppInfo(appInfo, appUid) == KErrNone)
        				{
        				names->AppendL(appInfo.iCaption);
        				}
        			else
        				{
        				TBuf<10> buf;
        				buf.Format(_L("0x%08x"), appUid);
        				names->AppendL(buf);
        				}
        			}
    		
        		// use a list query to select the app
//        		TInt index(0);
//        		CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog(&index);
//        		dlg->PrepareLC(R_APP_SELECTION_QUERY);
//        		dlg->SetItemTextArray(names);
//        		dlg->SetOwnershipType(ELbmDoesNotOwnItemArray);
//
//        		if(dlg->RunLD())
//        			{
//        			// extract the chosen UID
//        			appUidExtracted = appServiceInfos[index].Uid();
//        			itemSelected = ETrue;
//        			}
    		
        		CleanupStack::PopAndDestroy(); // names
        		}

            if (itemSelected)
                {
                // start OpenFileService for the selected app
                CAiwGenericParamList& paramList = iDocHandler->InParamListL();
                
//                if (iOpenFileService)
//                    {
//                    delete iOpenFileService;
//                    iOpenFileService = NULL;
//                    }

//                iOpenFileService = CAknOpenFileService::NewL(appUidExtracted, sharableFile, this, &paramList);
                }

            CleanupStack::PopAndDestroy(2); // sharableFile, apps

            ); // TRAPD

        if (err != KErrNone)
            {
            _LIT(KMessage, "No file service available for %S");
            TFileName noteMsg;
            noteMsg.Format(KMessage, &fileEntry.iEntry.iName);
            
            iEngine->FileBrowserUI()->ShowInformationNote(noteMsg, _L(""));
            }
        }   
    }
    
// --------------------------------------------------------------------------------------------

TBool CFileBrowserFileUtils::FileExists(const TDesC& aPath)
    {
    return BaflUtils::FileExists(iFs, aPath);
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::LaunchProgramL(const TDesC& aPath)
    {
    CApaCommandLine* commandLine = CApaCommandLine::NewLC();
    commandLine->SetExecutableNameL(aPath);
    commandLine->SetCommandL(EApaCommandRun);
    TInt err = iEngine->LsSession().StartApp(*commandLine);
    CleanupStack::PopAndDestroy(); //commandLine
    return err;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::MemoryInfoPopupL()
    {
    _LIT(KUsedBytesEntry, "Used: %S bytes\n");
    _LIT(KFreeBytesEntry, "Free: %S bytes\n");
    _LIT(KSizeBytesEntry, "Size: %S bytes\n");
        
    HBufC* messageBuf = HBufC::NewLC(2048);
    TPtr messagePtr = messageBuf->Des();
    TBuf<128> entryLine;

    // RAM
    TMemoryInfoV1Buf ramMemory;
    UserHal::MemoryInfo(ramMemory);
    messagePtr.Append(_L("RAM\n"));

    // used
    TBuf<32> usedBuf;
    usedBuf.AppendNum(ramMemory().iMaxFreeRamInBytes-ramMemory().iFreeRamInBytes, TRealFormat(KDefaultRealWidth, 0));
    entryLine.Format(KUsedBytesEntry, &usedBuf);
    messagePtr.Append(entryLine);        

    // free
    TBuf<32> freeBuf;
    freeBuf.AppendNum(ramMemory().iFreeRamInBytes, TRealFormat(KDefaultRealWidth, 0));
    entryLine.Format(KFreeBytesEntry, &freeBuf);
    messagePtr.Append(entryLine);
            
    // size
    TBuf<32> sizeBuf;
    sizeBuf.AppendNum(ramMemory().iMaxFreeRamInBytes, TRealFormat(KDefaultRealWidth, 0));
    entryLine.Format(KSizeBytesEntry, &sizeBuf);
    messagePtr.Append(entryLine); 
    
    // All drives
    for (TInt i=0; i<iDriveEntryList->Count(); i++)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(i);

        messagePtr.Append(_L("\n"));
        messagePtr.Append(driveEntry.iLetter);
        messagePtr.Append(_L(":\n"));

        // used
        TBuf<32> usedBuf;
        usedBuf.AppendNum(driveEntry.iVolumeInfo.iSize-driveEntry.iVolumeInfo.iFree, TRealFormat(KDefaultRealWidth, 0));
        entryLine.Format(KUsedBytesEntry, &usedBuf);
        messagePtr.Append(entryLine);        

        // free
        TBuf<32> freeBuf;
        freeBuf.AppendNum(driveEntry.iVolumeInfo.iFree, TRealFormat(KDefaultRealWidth, 0));
        entryLine.Format(KFreeBytesEntry, &freeBuf);
        messagePtr.Append(entryLine);
                
        // size
        TBuf<32> sizeBuf;
        sizeBuf.AppendNum(driveEntry.iVolumeInfo.iSize, TRealFormat(KDefaultRealWidth, 0));
        entryLine.Format(KSizeBytesEntry, &sizeBuf);
        messagePtr.Append(entryLine); 
        }

//    CAknMessageQueryDialog* dialog = new(ELeave) CAknMessageQueryDialog;
//    dialog->PrepareLC(R_MEMORYINFO_DIALOG);
//    dialog->SetMessageTextL(messagePtr);
//    dialog->RunLD();
    
    CleanupStack::PopAndDestroy(); //messageBuf
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ShowFileCheckSumsL(TInt aCurrentItemIndex, TInt aType)
    {
    if (iFileEntryList->Count() > aCurrentItemIndex && aCurrentItemIndex >= 0)
        {
        TFileEntry fileEntry = iFileEntryList->At(aCurrentItemIndex);
        TFileName fullPath = fileEntry.iPath;
        fullPath.Append(fileEntry.iEntry.iName);
        
        // try to open the file for reading
        RFile fileP;
        TInt err = fileP.Open(iEngine->EikonEnv()->FsSession(), fullPath, EFileRead|EFileShareReadersOnly);
        
        if (err == KErrNone)
            {
            CleanupClosePushL(fileP);

            TBuf<128> checkSum;
            HBufC* messageBuf = HBufC::NewLC(2048);
            TPtr messagePtr = messageBuf->Des();
            
            // get MD5 checksum
            if (aType == EFileChecksumsMD5)
                {
                CMD5* md5 = CMD5::NewL();
                CleanupStack::PushL(md5);
                HBufC8* md5Buf = MessageDigestInHexLC(md5, fileP);
                checkSum.Copy(*md5Buf);
                messagePtr.Append(_L("MD5\n"));
                messagePtr.Append(checkSum);
                CleanupStack::PopAndDestroy(2); // md5, md5Buf
                }

            // get MD2 checksum
            else if (aType == EFileChecksumsMD2)
                {                
                CMD2* md2 = CMD2::NewL();
                CleanupStack::PushL(md2);
                HBufC8* md2Buf = MessageDigestInHexLC(md2, fileP);
                checkSum.Copy(*md2Buf);
                messagePtr.Append(_L("MD2\n"));
                messagePtr.Append(checkSum);
                CleanupStack::PopAndDestroy(2); // md2, md2Buf
                }

            // get SHA-1 checksum
            else if (aType == EFileChecksumsSHA1)
                {
                CSHA1* sha1 = CSHA1::NewL();
                CleanupStack::PushL(sha1);
                HBufC8* sha1Buf = MessageDigestInHexLC(sha1, fileP);
                checkSum.Copy(*sha1Buf);
                messagePtr.Append(_L("SHA-1\n"));
                messagePtr.Append(checkSum);
                CleanupStack::PopAndDestroy(2); // sha1, sha1Buf
                }
            
            else
                {
                User::Panic(_L("Inv.CS.Type"), 723);
                }
                            
            // show dialog
            iEngine->FileBrowserUI()->ShowInformationNote(messagePtr, fileEntry.iEntry.iName);

            CleanupStack::PopAndDestroy(2); // messageBuf, fileP
            }
        else
            {
            // open failed, show an error note
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }
        }
    }

// --------------------------------------------------------------------------------------------

HBufC8* CFileBrowserFileUtils::MessageDigestInHexLC(CMessageDigest* aMD, RFile& aFile)
    {
    // seek to file start
    TInt startPos(0);
    aFile.Seek(ESeekStart, startPos);

    // create result buffer
    HBufC8* result = HBufC8::NewLC(128);

    const TInt KBufSize(1024);
    TInt fileSize(0);
    TInt fileOffset(0);

    // get file size
    aFile.Size(fileSize);

    HBufC8* buf = HBufC8::NewMaxLC(KBufSize);
    TPtr8 bufPtr(buf->Des());

    // read to buffer
    while (fileOffset < fileSize - KBufSize)
        {
        aFile.Read(bufPtr, KBufSize);
        aMD->Hash(bufPtr);
        fileOffset += bufPtr.Length();
        }

    aFile.Read(bufPtr, fileSize - fileOffset);
    bufPtr.SetLength(fileSize - fileOffset);
    
    // get final message digest
    TPtrC8 hashedSig(aMD->Final(bufPtr));
    
    // change size of the result buffer
    result->ReAllocL(hashedSig.Length() * 2);
    TPtr8 resultPtr = result->Des();

    // convert to hex format
    for (TInt i=0; i<hashedSig.Length(); i++)
        {
        resultPtr.AppendFormat(_L8("%+02x"), hashedSig[i]);
        }
                    
    CleanupStack::PopAndDestroy(); // buf
    
    return result;
    }
        
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SetErrRdL(TBool aEnable)
    {
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {
        TInt err(KErrNone);
        
        if (aEnable)
            {
            // make sure that the directory exists
            iFileOps->MkDirAll(KErrRdPath);        

            // create the file
            err = iFileOps->CreateEmptyFile(KErrRdPath);
            }
        else    
            {
            // get TEntry of ErrRd
            TEntry entry;
            err = iFs.Entry(KErrRdPath, entry);
            
            if (err == KErrNone)
                {
                TFileEntry fileEntry;
                fileEntry.iPath = KErrRdDir;
                fileEntry.iEntry = entry;
                
                err = iFileOps->Delete(fileEntry);                
                }
            }        

        // update view
        RefreshViewL(); 
    
        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("State changed"));
            }
        else    
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Cannot change the state"));
            }        
        }
    else
        {
        iEngine->FileBrowserUI()->ShowInformationNote(_L("Enable \"bypass platform security\" from the settings first"), _L(""));
        }
    #else
        aEnable = aEnable;
        iEngine->FileBrowserUI()->ShowInformationNote(_L("Not supported in lite version"), _L(""));
    #endif
    }

// --------------------------------------------------------------------------------------------

//void CFileBrowserFileUtils::EnableAvkonIconCacheL(TBool aEnable)
//    {
//#ifndef __SERIES60_30__
//    if (AknIconConfig::EnableAknIconSrvCache(aEnable) == KErrNone)
//        {
//        iEngine->FileBrowserUI()->ShowConfirmationNote(_L("State changed"));
//        }
//    else
//        {
//        iEngine->FileBrowserUI()->ShowErrorNote(_L("Unknown error occured"));
//        }
// #else
//    aEnable = aEnable;
//    iEngine->FileBrowserUI()->ShowInformationNote(_L("Not supported in S60 3.0"), _L(""));
// #endif
// 
//    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SimulateLeaveL(TInt aLeaveCode)
    {
    User::Leave(aLeaveCode);
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SimulatePanicL(const TDesC& aPanicCategory, TInt aPanicCode)
    {
    User::Panic(aPanicCategory, aPanicCode);
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SimulateExceptionL(TInt aExceptionCode)
    {
    User::RaiseException((TExcType)aExceptionCode);
    }

// --------------------------------------------------------------------------------------------

TUint32 CFileBrowserFileUtils::GetDebugMask()
    {
    return UserSvr::DebugMask();
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SetDebugMaskL(TUint32 aDbgMask)
    {
    User::SetDebugMask(aDbgMask);
    }
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::WriteAllAppsL()
    {
    _LIT(KAllAppsFileName, "AllApps.txt");
    TFileName allAppsPath = PathInfo::PhoneMemoryRootPath();
    allAppsPath.Append(KAllAppsFileName);

    RFile file;
    if (file.Replace(iFs, allAppsPath, EFileWrite) == KErrNone)
        {
        CleanupClosePushL(file);
        
        CDesC8Array* appsArray = new(ELeave) CDesC8ArrayFlat(192);
        CleanupStack::PushL(appsArray);
        
        TBuf8<KMaxFileName> fileEntry;
        TBuf8<KMaxFileName> appFullPath;
        _LIT8(KAppEntryFormat, "%S (0x%08X)%S");
        TApaAppInfo appInfo;

        iEngine->LsSession().GetAllApps();        

        while (iEngine->LsSession().GetNextApp(appInfo) == KErrNone)
            {
            appFullPath.Copy(appInfo.iFullName);
            fileEntry.Format(KAppEntryFormat, &appFullPath, appInfo.iUid, &KFileNewLine);
            appsArray->AppendL(fileEntry);    
            }
        
        appsArray->Sort();
        
        for (TInt i=0; i<appsArray->Count(); i++)
            {
            file.Write(appsArray->MdcaPoint(i));
            }
        
        CleanupStack::PopAndDestroy(2); //appsArray, file
        
        _LIT(KMessage, "App list written to %S");
        TFileName noteMsg;
        noteMsg.Format(KMessage, &allAppsPath);

        iEngine->FileBrowserUI()->ShowConfirmationNote(noteMsg, ETrue); // NoTimeout
        }
    else
        {
        _LIT(KMessage, "Failed writing to %S");
        TFileName noteMsg;
        noteMsg.Format(KMessage, &allAppsPath);

        iEngine->FileBrowserUI()->ShowErrorNote(noteMsg, ETrue); // NoTimeout
        }    
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::WriteAllFilesL()
    {
    _LIT(KAllFilesFileName, "AllFiles.txt");
    TFileName allFilesPath = PathInfo::PhoneMemoryRootPath();
    allFilesPath.Append(KAllFilesFileName);

    RFile file;
    if (file.Replace(iFs, allFilesPath, EFileWrite) == KErrNone)
        {
        CleanupClosePushL(file);
        iFindFileEntryList->Reset();
        
        iEngine->FileBrowserUI()->ShowWaitDialog(_L("Generating"));
        
        for (TInt i=0; i<iDriveEntryList->Count(); i++)
            {
            TDriveEntry driveEntry = iDriveEntryList->At(i);

            TBuf<10> driveRoot;
            driveRoot.Append(driveEntry.iLetter);
            driveRoot.Append(_L(":\\"));

            // current dir
            DoFindFiles(_L("*"), driveRoot);
            
            // recurse into sub directories
            DoFindFilesRecursiveL(_L("*"), driveRoot);
            }
        
        // write entries
        TBuf8<KMaxFileName> writeBuf;
        for (TInt i=0; i<iFindFileEntryList->Count(); i++)
            {
            TFileEntry fileEntry = iFindFileEntryList->At(i);
            
            writeBuf.Copy(fileEntry.iPath);
            writeBuf.Append(fileEntry.iEntry.iName);
            writeBuf.Append(_L(","));
            writeBuf.AppendNum(fileEntry.iEntry.iSize);
            writeBuf.Append(_L(" B"));

//            // date
//            TTime entryModified = fileEntry.iEntry.iModified;
//            // convert from universal time
//            if ( iTz.ConvertToLocalTime( entryModified ) == KErrNone )
//                {
//                entryModified = fileEntry.iEntry.iModified; // use universal time
//                }
//
//            _LIT(KDateFormat,               "%D%M%Y%/0%1%/1%2%/2%3%/3");
//            TBuf<32> dateBuf;
//            entryModified.FormatL(dateBuf, KDateFormat);
//            writeBuf.Append(dateBuf);
//
//            writeBuf.Append(_L(" "));
//            // time
//            _LIT(KTimeFormat, "%-B%:0%J%:1%T%:2%S%:3%+B");
//            TBuf<32> timeBuf;
//            entryModified.FormatL(timeBuf, KTimeFormat);
//            writeBuf.Append(timeBuf);
//
            writeBuf.Append(KFileNewLine);
            file.Write(writeBuf);
            }
        
        iEngine->FileBrowserUI()->CancelWaitDialog();
        
        CleanupStack::PopAndDestroy(); //file
        iFindFileEntryList->Reset();
        
        _LIT(KMessage, "File list written to %S");
        TFileName noteMsg;
        noteMsg.Format(KMessage, &allFilesPath);

        iEngine->FileBrowserUI()->ShowConfirmationNote(noteMsg, EFalse); // NoTimeout
        }
    else
        {
        _LIT(KMessage, "Failed writing to %S");
        TFileName noteMsg;
        noteMsg.Format(KMessage, &allFilesPath);

        iEngine->FileBrowserUI()->ShowErrorNote(noteMsg, ETrue); // NoTimeout
        }    
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::DoFindFiles(const TDesC& aFileName, const TDesC& aPath)
    {
    TFindFile fileFinder(iFs);
    CDir* dir;
    TInt err = fileFinder.FindWildByPath(aFileName, &aPath, dir);

    while (err == KErrNone && iAllowProcessing)
        {
        iEngine->FileBrowserUI()->ProcessEvents();
        for (TInt i=0; i<dir->Count(); i++)
            {
            TEntry entry = (*dir)[i];

            // ignore any directory entries
            if (!entry.IsDir() && entry.iName.Length() && aPath.Length())
                {
                TParse parsedName;
                parsedName.Set(entry.iName, &fileFinder.File(), NULL);
                
                if (parsedName.Drive().Length() && aPath.Length() && parsedName.Drive()[0] == aPath[0])
                    {
                    TFileEntry fileEntry;
                    fileEntry.iPath = parsedName.DriveAndPath();
                    fileEntry.iEntry = entry;
                    fileEntry.iDirEntries = KErrNotFound;
                    fileEntry.iIconId = EFixedIconEmpty;
                    
                    TRAP(err, iFindFileEntryList->AppendL(fileEntry));             
                    }
                }
            }

        delete dir;
        dir = NULL;
        err = fileFinder.FindWild(dir);
        }

    return err;
    }

// --------------------------------------------------------------------------------------------

TInt CFileBrowserFileUtils::DoFindFilesRecursiveL(const TDesC& aFileName, const TDesC& aPath)
	{
    TInt err(KErrNone);
    CDirScan* scan = CDirScan::NewLC(iFs);
    scan->SetScanDataL(aPath, KEntryAttDir|KEntryAttMatchMask, ESortByName | EAscending | EDirsFirst);
    CDir* dir = NULL;

    for(;;)
        {
        TRAP(err, scan->NextL(dir));
        if (!dir  || (err != KErrNone))
            break;

        for (TInt i=0; i<dir->Count(); i++)
            {
            TEntry entry = (*dir)[i];
            
            if (entry.IsDir())
                {
                TFileName path(scan->FullPath());
                
                if (path.Length())
                    {
                    path.Append(entry.iName);
                    path.Append(_L("\\"));
                    DoFindFiles(aFileName, path);
                    }
                }
            }
        delete(dir);
        }

    CleanupStack::PopAndDestroy(scan);
    return err;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ListOpenFilesL()
    {
    iFileEntryList->Reset();

    CFileList* dir = NULL;
    TOpenFileScan fileScan(iFs);
    fileScan.NextL(dir);
    
    while (dir)
        {
        for (TInt i=0; i<dir->Count(); i++)
            {
            TEntry entry = (*dir)[i];
            TFileName fullPath;

            // TOpenFileScan does not return drive letters for the entries, so try to guess it
            if (entry.iName.Length() > 2 && entry.iName[1] != ':')
                {
                for (TInt i=0; i<iDriveEntryList->Count(); i++)
                    {
                    TDriveEntry driveEntry = iDriveEntryList->At(i);
                    
                    TFileName guessPath;
                    guessPath.Append(driveEntry.iLetter);
                    guessPath.Append(_L(":"));
                    guessPath.Append(entry.iName);
                    
                    // check if the file exists
                    if (BaflUtils::FileExists(iFs, guessPath))
                        {
                        // if it's on read only drive, then take it or check if it's really in use
                        TBool readOnlyDrive(ETrue);
                        BaflUtils::DiskIsReadOnly(iFs, guessPath, readOnlyDrive);
                        
                        if (readOnlyDrive || iFs.SetEntry(guessPath, entry.iModified, NULL, NULL) == KErrInUse)
                            {
                            fullPath = guessPath;
                            break;
                            }
                        }
                    }
                }
            else
                {
                fullPath = entry.iName;   
                }

            TParse nameParser;
            if (fullPath.Length() && nameParser.SetNoWild(fullPath, NULL, NULL) == KErrNone)
                {
                entry.iName = nameParser.NameAndExt();
                
                TFileEntry fileEntry;
                fileEntry.iPath = nameParser.DriveAndPath();
                fileEntry.iEntry = entry;
                fileEntry.iDirEntries = KErrNotFound;
                fileEntry.iIconId = EFixedIconEmpty;
                
                iFileEntryList->AppendL(fileEntry);                
                }
            }
            
        delete dir;
        dir = NULL;
        fileScan.NextL(dir);
        }

    TInt operations = iFileEntryList->Count();
    
    iListingMode = EOpenFiles;
	// TODO
    //iEngine->FileListContainer()->ListBox()->SetCurrentItemIndex(0);
    RefreshViewL();            

    _LIT(KMessage, "%d open files found");
    TFileName noteMsg;
    noteMsg.Format(KMessage, operations);

    iEngine->FileBrowserUI()->ShowInformationNote(noteMsg, _L(""));
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ListMessageAttachmentsL(TInt aType)
    {
    iFileEntryList->Reset();

    CMsvSession* session = CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL(session);
    
    CMsvEntry* context = NULL;
    
    if (aType == EFileBrowserCmdToolsMsgAttachmentsInbox)
        context = session->GetEntryL(KMsvGlobalInBoxIndexEntryId);
    else if (aType == EFileBrowserCmdToolsMsgAttachmentsDrafts)
        context = session->GetEntryL(KMsvDraftEntryId);
    else if (aType == EFileBrowserCmdToolsMsgAttachmentsSentItems)
        context = session->GetEntryL(KMsvSentEntryId);
    else if (aType == EFileBrowserCmdToolsMsgAttachmentsOutbox)
        context = session->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
    else
        User::Panic(_L("MsgAtt.Mode"), 100);
    
    CleanupStack::PushL(context);
    
    CDesCArray* attPaths = new(ELeave) CDesCArrayFlat(128);
    CleanupStack::PushL(attPaths);
    
    // read attachment paths
    ReadAttachmentPathsRecursiveL(session, context, attPaths);
    
    // create file entries of the paths
    for (TInt i=0; i<attPaths->MdcaCount(); i++)
        {
        TFileEntry fileEntry;
        TEntry entry;
        TParse nameParser;
        
        if (attPaths->MdcaPoint(i).Length() &&
            nameParser.SetNoWild(attPaths->MdcaPoint(i), NULL, NULL) == KErrNone &&
            iFs.Entry(attPaths->MdcaPoint(i), entry) == KErrNone)
            {
            TFileEntry fileEntry;
            fileEntry.iPath = nameParser.DriveAndPath();
            fileEntry.iEntry = entry;
            fileEntry.iDirEntries = KErrNotFound;
            fileEntry.iIconId = EFixedIconEmpty;

            iFileEntryList->AppendL(fileEntry);                
            }
        }
    
    CleanupStack::PopAndDestroy(3); //session, context, attPaths
            
    TInt operations = iFileEntryList->Count();
    
    if (aType == EFileBrowserCmdToolsMsgAttachmentsInbox)
        iListingMode = EMsgAttachmentsInbox;
    else if (aType == EFileBrowserCmdToolsMsgAttachmentsDrafts)
        iListingMode = EMsgAttachmentsDrafts;
    else if (aType == EFileBrowserCmdToolsMsgAttachmentsSentItems)
        iListingMode = EMsgAttachmentsSentItems;
    else if (aType == EFileBrowserCmdToolsMsgAttachmentsOutbox)
        iListingMode = EMsgAttachmentsOutbox;
	
    // TODO
    //iEngine->FileListContainer()->ListBox()->SetCurrentItemIndex(0);
    RefreshViewL();            

    _LIT(KMessage, "%d files found");
    TFileName noteMsg;
    noteMsg.Format(KMessage, operations);

    iEngine->FileBrowserUI()->ShowInformationNote(noteMsg, _L(""));
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ReadAttachmentPathsRecursiveL(CMsvSession* aSession, CMsvEntry* aContext, CDesCArray* aAttPaths)
    {
    CMsvEntrySelection* entrySelection = aContext->ChildrenL(); 
    CleanupStack::PushL(entrySelection);
    
    for (TInt i=0; i<entrySelection->Count(); i++)
        {
        CMsvEntry* entry = aSession->GetEntryL((*entrySelection)[i]);
        CleanupStack::PushL(entry);
        
        CMsvStore* store = NULL;
        TRAPD(err, store = entry->ReadStoreL());
        
        if (err == KErrNone)
            {
            CleanupStack::PushL(store);

            for (TInt j=0; j<store->AttachmentManagerL().AttachmentCount(); j++)
                {
                CMsvAttachment* attachment = store->AttachmentManagerL().GetAttachmentInfoL(j);
                CleanupStack::PushL(attachment);
                
                aAttPaths->AppendL(attachment->FilePath());
                
                CleanupStack::PopAndDestroy(); //attachment
                }
            CleanupStack::PopAndDestroy(); // store
            }

        // recurse into children
        if (entry->Count() > 0)
            {
            ReadAttachmentPathsRecursiveL(aSession, entry, aAttPaths);
            }

        CleanupStack::PopAndDestroy(); // entry
        }      
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::WriteMsgStoreWalkL()
    {
    _LIT(KAllAppsFileName, "MsgStoreWalk.txt");
    TFileName allAppsPath = PathInfo::PhoneMemoryRootPath();
    allAppsPath.Append(KAllAppsFileName);

    if (iMsgStoreWalkFile.Replace(iFs, allAppsPath, EFileWrite) == KErrNone)
        {
        CMsvSession* session = CMsvSession::OpenSyncL(*this);
        CleanupStack::PushL(session);
        
        CMsvEntry* context = session->GetEntryL(KMsvRootIndexEntryId);
        CleanupStack::PushL(context);
        
        iMsgStoreWalkFile.Write(_L8("id  service_id  related_id  type  mtm  date  size  error  biotype  mtmdata1  mtmdata2  mtmdata3  description  details\r\n\r\n"));
        
        // write details about the root entry
        DoWriteMessageEntryInfoL(context, iMsgStoreWalkFile, 0);
        
        // get and write info recursively
        TInt level(1);        
        WriteMessageEntryInfoRecursiveL(session, context, iMsgStoreWalkFile, level);

        CleanupStack::PopAndDestroy(2); //session, context
        
        iMsgStoreWalkFile.Flush();
        iMsgStoreWalkFile.Close();
        
        _LIT(KMessage, "Msg. store walk written to %S");
        TFileName noteMsg;
        noteMsg.Format(KMessage, &allAppsPath);

        iEngine->FileBrowserUI()->ShowConfirmationNote(noteMsg, ETrue); // NoTimeout
        }
    else
        {
        _LIT(KMessage, "Failed writing to %S");
        TFileName noteMsg;
        noteMsg.Format(KMessage, &allAppsPath);

        iEngine->FileBrowserUI()->ShowErrorNote(noteMsg, ETrue); // NoTimeout
        }    
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::WriteMessageEntryInfoRecursiveL(CMsvSession* aSession, CMsvEntry* aContext, RFile& aFile, TInt& aLevel)
    {
    CMsvEntrySelection* entrySelection = aContext->ChildrenL(); 
    CleanupStack::PushL(entrySelection);
    
    for (TInt i=0; i<entrySelection->Count(); i++)
        {
        CMsvEntry* entry = aSession->GetEntryL((*entrySelection)[i]);
        CleanupStack::PushL(entry);
        
        DoWriteMessageEntryInfoL(entry, aFile, aLevel);

        // recurse into children
        if (entry->Count() > 0)
            {
            aLevel++;
            WriteMessageEntryInfoRecursiveL(aSession, entry, aFile, aLevel);
            aLevel--;
            }

        CleanupStack::PopAndDestroy(); // entry
        }      
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::DoWriteMessageEntryInfoL(CMsvEntry* aContext, RFile& aFile, TInt aLevel)
    {
    TMsvEntry messageEntry = aContext->Entry();
    
    for (TInt j=0; j<aLevel; j++)
        {
        aFile.Write(_L8("     "));
        }

    TBuf8<256> desc;
    desc.Copy(messageEntry.iDescription);

    TBuf8<256> details;
    details.Copy(messageEntry.iDetails);

    _LIT8(KLine, "-> %d  %d  %d  %d  %d  %Ld  %d  %d  %d  %d  %d  %d  %S  %S\r\n");
    TBuf8<1024> buf;
    buf.Format(KLine, messageEntry.Id(), messageEntry.iServiceId, messageEntry.iRelatedId, messageEntry.iType.iUid,
        messageEntry.iMtm.iUid, messageEntry.iDate.Int64(), messageEntry.iSize, messageEntry.iError, messageEntry.iBioType,
        messageEntry.iMtmData1, messageEntry.iMtmData2, messageEntry.iMtmData3, &desc, &details);

    aFile.Write(buf);
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::FileEditorL(TInt aCurrentItemIndex, TInt /*aType*/)
    {
    // TODO
    // just get current item 
    //TInt currentItemIndex = iEngine->QueryCurrentItemIndex();
    
    if (iFileEntryList->Count() > aCurrentItemIndex && aCurrentItemIndex >= 0)
        {
        TFileEntry fileEntry = iFileEntryList->At(aCurrentItemIndex);
        
        // only valid for files
        if (!fileEntry.iEntry.IsDir())
            {
            TFileName fullPath = fileEntry.iPath;
            fullPath.Append(fileEntry.iEntry.iName);

//            iEngine->FileListContainer()->SetNaviPaneTextL(fileEntry.iEntry.iName);
//
//            TInt editorMode(0);
//
//            if (aType == EFileBrowserCmdFileViewText)
//                editorMode = EFileEditorViewAsText;
//            else if (aType == EFileBrowserCmdFileViewHex)
//                editorMode = EFileEditorViewAsHex;
//            else if (aType == EFileBrowserCmdFileEditText)
//                editorMode = EFileEditorEditAsText;
//            else if (aType == EFileBrowserCmdFileEditHex)
//                editorMode = EFileEditorEditAsHex;
//            else
//                User::Panic(_L("Inv.Ed.Mode"), 843);
//
//            // launch dialog
//            CFileBrowserFileEditorDlg* dlg = CFileBrowserFileEditorDlg::NewL(fullPath, editorMode);
//            dlg->RunDlgLD();
//
//            iEngine->FileListContainer()->SetNaviPaneTextL(iCurrentPath);
//
//            if (aType == EFileBrowserCmdFileEditText || aType == EFileBrowserCmdFileEditHex)
//                {
//                RefreshViewL();
//                }
            }
        }
    }

// --------------------------------------------------------------------------------------------

TBool CFileBrowserFileUtils::DriveSnapShotPossible()
    {
//    // TODO
//    // check that E-drive is available
//    TBool EDriveIsOK(EFalse);
//
//    for (TInt i=0; i<iDriveEntryList->Count(); i++)
//        {
//        TDriveEntry driveEntry = iDriveEntryList->At(i);
//
//        if (driveEntry.iNumber == EDriveE)
//            {
//            if (driveEntry.iVolumeInfo.iDrive.iMediaAtt & KMediaAttLocked || driveEntry.iVolumeInfo.iDrive.iDriveAtt & KDriveAbsent)
//                EDriveIsOK = EFalse;
//            else
//                EDriveIsOK = ETrue;
//            }
//        }
//
//    if (!EDriveIsOK)
//        return EFalse;
//
//    // get current item
//    TInt currentItemIndex = iEngine->QueryCurrentItemIndex();
//
//    if (iDriveEntryList->Count() > currentItemIndex && currentItemIndex >= 0)
//        {
//        TDriveEntry driveEntry = iDriveEntryList->At(currentItemIndex);
//
//        // drive snapshot not possible when E-drive is highlighted, since that's where we are copying
//        if (driveEntry.iNumber == EDriveE)
//            return EFalse;
//        else
//            return ETrue;
//        }
    return EFalse;
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::DriveSnapShotL()
    {
//    if (iEngine->Settings().iBypassPlatformSecurity)
//        {
//        // get current item
//        TInt currentItemIndex = iEngine->QueryCurrentItemIndex();
//
//        if (iDriveEntryList->Count() > currentItemIndex && currentItemIndex >= 0)
//            {
//            TDriveEntry driveEntry = iDriveEntryList->At(currentItemIndex);
//
//            TChar sourceDriveLetter = driveEntry.iLetter;
//            TChar targetDriveLetter = 'E';  // hardcoded drive letter, could be better if would be queried from the user
//
//            // append to the command array
//            AppendToCommandArrayL(EFileBrowserFileOpCommandDriveSnapShot,
//                                  new(ELeave)CCommandParamsDriveSnapShot(sourceDriveLetter, targetDriveLetter)
//                                 );
//
//            // execute the operation
//            StartExecutingCommandsL(_L("Copying"));
//            }
//        }
//    else
//        {
//        iEngine->FileBrowserUI()->ShowInformationNote(_L("Enable \"bypass platform security\" from the settings first"), _L(""));
//        }
    }
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SetDrivePasswordL(TInt aIndex,
                                              const TFileName &aOldPassword,
                                              const TFileName &aNewPassword)
    {    
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TMediaPassword oldPassword;
        TMediaPassword newPassword;

        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        ConvertCharsToPwd(aOldPassword, oldPassword);
        ConvertCharsToPwd(aNewPassword, newPassword);

        // set the password, does not actually lock the drive
        TInt err = iFs.LockDrive(driveEntry.iNumber, oldPassword, newPassword, ETrue);

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Password set"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }

        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::UnlockDriveL(TInt aIndex, const TFileName &aOldPassword)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        TMediaPassword oldPassword;
        ConvertCharsToPwd(aOldPassword, oldPassword);

        // unlock the drive
        TInt err = iFs.UnlockDrive(driveEntry.iNumber, oldPassword, ETrue) == KErrNone;

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Drive unlocked"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote( textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }

        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ClearDrivePasswordL(TInt aIndex, const TFileName &aOldPassword)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        TMediaPassword oldPassword;

        ConvertCharsToPwd(aOldPassword, oldPassword);

        // clear the password from the drive
        TInt err = iFs.ClearPassword(driveEntry.iNumber, oldPassword);

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Password cleared"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }

        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::EraseDrivePasswordL(TInt aIndex)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);
        
        // erase the password from the drive
        TInt err(KErrNone);
        err = iFs.ErasePassword(driveEntry.iNumber);

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Password erased"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Cannot erase, you may have to format the drive first"));
            }

        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::FormatDriveL(TInt aIndex, TBool aQuickFormat)
    {    
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        TInt err(KErrNone);
        TInt formatCount(0);

        TBuf<10> driveRoot;
        driveRoot.Append(driveEntry.iLetter);
        driveRoot.Append(_L(":"));

        TUint formatMode(0);
        if (aQuickFormat)
            formatMode = ESpecialFormat|EQuickFormat;
        else
            formatMode = ESpecialFormat|EFullFormat;

        // set as system application to prevent getting shut down events
        iEngine->EikonEnv()->SetSystem(ETrue);

        // first close any open applications
        CBaBackupSessionWrapper* BSWrapper = CBaBackupSessionWrapper::NewL();
        CleanupStack::PushL(BSWrapper);

        TBackupOperationAttributes atts(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
        BSWrapper->NotifyBackupOperationL(atts);

        CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
        BSWrapper->CloseAll( MBackupObserver::EReleaseLockNoAccess, waiter->iStatus );
        waiter->StartAndWait();
        CleanupStack::PopAndDestroy(); //waiter

        // some delay to make sure all applications have been closed
        User::After(1000000);

        // format the drive
        RFormat format;
        err = format.Open(iFs, driveRoot, formatMode, formatCount);

        // Forced format for locked card
        if ( err == KErrLocked )
            {
            // Erase password and try again
            err = iFs.ErasePassword( driveEntry.iNumber );
            if ( !err )
                {
                err = format.Open(iFs, driveRoot, formatMode, formatCount);
                }
            }

        if( err != KErrNone &&  err != KErrLocked )
            {
//            CAknQueryDialog* lockQuery = CAknQueryDialog::NewL();
//            if ( lockQuery->ExecuteLD(R_GENERAL_CONFIRMATION_QUERY, _L("Memory card in use by other application. Proceed anyway?")))
            // TODO Dialog should be opened in UI part
            if (iEngine->FileBrowserUI()->ShowConfirmationQuery(_L("Memory card in use by other application. Proceed anyway?")))
                {
                // If format could not be started, someone is still keeping
                // files open on media. Let's try to dismount file system, then
                // remount with own extension embedded and try again.

                TFullName fsName;
                err = iFs.FileSystemName( fsName, driveEntry.iNumber );

                if ( err == KErrNone && fsName.Length() > 0 )
                    {
                    // Prevent SysAp shutting down applications
                    RProperty::Set(
                        KPSUidCoreApplicationUIs,
                        KCoreAppUIsMmcRemovedWithoutEject,
                        ECoreAppUIsEjectCommandUsed );

                    TRequestStatus stat;
                    iFs.NotifyDismount( driveEntry.iNumber, stat, EFsDismountForceDismount );
                    User::WaitForRequest( stat );

                    // Unfortunately, at the moment we have to wait until clients have received
                    // notification about card dismount. Otherwise at least causes problems with
                    // theme selected from card. In future clients should use new notify-API.
                    User::After( KForcedFormatTimeout );

                    // Let's set priority higher than normally. This is done to decrease the chance
                    // that someone reopens files on memory card after mandatory file system
                    // remounting hence preventing formatting again.
                    TThreadPriority priority( RThread().Priority() );
                    RThread().SetPriority( EPriorityAbsoluteHigh );

                    // Mount file system back...
                    err = iFs.MountFileSystem( fsName, driveEntry.iNumber );
                    err = format.Open(iFs, driveRoot, formatMode, formatCount);
                    RThread().SetPriority( priority );
                    }
                }
            }


        if (err == KErrNone)
            {
            // needs to be implemented with active objects
            // CAknProgressDialog* dlg = new(ELeave) CAknProgressDialog(formatCount, 1, 1, NULL);
            // dlg->SetTone(CAknNoteDialog::ENoTone);
            // dlg->ExecuteLD(R_FORMAT_PROGRESS_NOTE);

            while (formatCount && err == KErrNone)
                {
                err = format.Next(formatCount);
                }
            }

        format.Close();

        // restart closed applications
        TBackupOperationAttributes atts2(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
        BSWrapper->NotifyBackupOperationL(atts2);
        BSWrapper->RestartAll();
        CleanupStack::PopAndDestroy(); //BSWrapper

        // system status not needed anymore
        iEngine->EikonEnv()->SetSystem(EFalse);


        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Format succeeded"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }

        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::EraseMBRL(TInt aIndex)
    {
    // TODO
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {        
        if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
            {
            TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

            TInt err(KErrNone);

            iEngine->FileBrowserUI()->ShowInformationNote(_L("Please wait, this may take 30 seconds"), _L(""));

            // set as system application to prevent getting shut down events
            iEngine->EikonEnv()->SetSystem(ETrue);

            // first close any open applications
            CBaBackupSessionWrapper* BSWrapper = CBaBackupSessionWrapper::NewL();
            CleanupStack::PushL(BSWrapper);

            TBackupOperationAttributes atts(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
            BSWrapper->NotifyBackupOperationL(atts);

            CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
            BSWrapper->CloseAll( MBackupObserver::EReleaseLockNoAccess, waiter->iStatus );
            waiter->StartAndWait();
            CleanupStack::PopAndDestroy(); //waiter

            // some delay to make sure all applications have been closed
            User::After(1000000);


            // do the erase MBR operation
            err = iFileOps->EraseMBR(driveEntry.iNumber);


            // restart closed applications
            TBackupOperationAttributes atts2(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
            BSWrapper->NotifyBackupOperationL(atts2);
            BSWrapper->RestartAll();
            CleanupStack::PopAndDestroy(); //BSWrapper

            // system status not needed anymore
            iEngine->EikonEnv()->SetSystem(EFalse);


            if (err == KErrNone)
                {
                iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Erase MBR succeeded"));
                }
            else if (err == KErrNotSupported)
                {
                iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
                }
            else
                {
                CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

                iEngine->FileBrowserUI()->ShowErrorNote(
                        textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

                CleanupStack::PopAndDestroy();  //textResolver
                }

            RefreshViewL();
            }
        }
    else
        {
        iEngine->FileBrowserUI()->ShowInformationNote(_L("Enable \"bypass platform security\" from the settings first"), _L(""));
        }
    #else
        iEngine->FileBrowserUI()->ShowInformationNote(_L("Not supported in lite version"), _L(""));
    #endif
    }
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::PartitionDriveL(TInt aIndex, TBool aEraseMBR, TInt aAmountOfPartitions)
    {
    // TODO
    #ifndef FILEBROWSER_LITE
    if (iEngine->Settings().iBypassPlatformSecurity)
        {
        if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
            {
            TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

            TInt err(KErrNone);

            iEngine->FileBrowserUI()->ShowInformationNote(_L("Please wait, this may take 30 seconds"), _L(""));

            // set as system application to prevent getting shut down events
            iEngine->EikonEnv()->SetSystem(ETrue);

            // first close any open applications
            CBaBackupSessionWrapper* BSWrapper = CBaBackupSessionWrapper::NewL();
            CleanupStack::PushL(BSWrapper);

            TBackupOperationAttributes atts(MBackupObserver::EReleaseLockNoAccess, MBackupOperationObserver::EStart);
            BSWrapper->NotifyBackupOperationL(atts);

            CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
            BSWrapper->CloseAll( MBackupObserver::EReleaseLockNoAccess, waiter->iStatus );
            waiter->StartAndWait();
            CleanupStack::PopAndDestroy(); //waiter

            // some delay to make sure all applications have been closed
            User::After(1000000);

            // do the erase operation
            if (aEraseMBR)
                {
                err = iFileOps->EraseMBR(driveEntry.iNumber);

                if (err != KErrNone)
                    {
                    iEngine->FileBrowserUI()->ShowErrorNote(_L("Erase MBR failed"));
                    }

                User::After(500000);
                }


            // do the partition operation
            err = iFileOps->PartitionDrive(driveEntry.iNumber, aAmountOfPartitions);


            // restart closed applications
            TBackupOperationAttributes atts2(MBackupObserver::ETakeLock, MBackupOperationObserver::EEnd);
            BSWrapper->NotifyBackupOperationL(atts2);
            BSWrapper->RestartAll();
            CleanupStack::PopAndDestroy(); //BSWrapper

            // system status not needed anymore
            iEngine->EikonEnv()->SetSystem(EFalse);


            if (err == KErrNone)
                {
                iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Partition succeeded"));
                }
            else if (err == KErrNotSupported)
                {
                iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
                }
            else
                {
                CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

                iEngine->FileBrowserUI()->ShowErrorNote(
                        textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

                CleanupStack::PopAndDestroy();  //textResolver
                }

            RefreshViewL();
            }
        }
    else
        {
        iEngine->FileBrowserUI()->ShowInformationNote(_L("Enable \"bypass platform security\" from the settings first"), _L(""));
        }
    #else
        iEngine->FileBrowserUI()->ShowInformationNote(_L("Not supported in lite version"), _L(""));
    #endif
	}
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::CheckDiskL(TInt aIndex)
    {    
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);
        
        TBuf<10> driveRoot;
        driveRoot.Append(driveEntry.iLetter);
        driveRoot.Append(_L(":"));
        
        // check disk
        TInt err = iFs.CheckDisk(driveRoot);

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Integrity of the disk is ok"));
            }
        else if (err == KErrNotReady)
            {
            iEngine->FileBrowserUI()->ShowInformationNote(_L("Disk is empty"), _L(""));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Disk is corrupted"));
            }        

        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ScanDriveL(TInt aIndex)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        TBuf<10> driveRoot;
        driveRoot.Append(driveEntry.iLetter);
        driveRoot.Append(_L(":"));

        // scan disk
        TInt err = iFs.ScanDrive(driveRoot);

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Run succesfully"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }

        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::GetDriveName(TInt aIndex, TFileName &aDriveName)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        // get existing drive name
        iFs.GetDriveName(driveEntry.iNumber, aDriveName);
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SetDriveNameL(TInt aIndex, const TFileName &aDriveName)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        // set drive name
        TInt err = iFs.SetDriveName(driveEntry.iNumber, aDriveName);

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Name changed"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }

        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::GetDriveVolumeLabel(TInt aIndex, TFileName &aVolumeLabel)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        // get existing volume label
        aVolumeLabel.Copy(driveEntry.iVolumeInfo.iName);
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SetDriveVolumeLabelL(TInt aIndex, const TFileName &aVolumeLabel)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        // set volume label
        TInt err = iFs.SetVolumeLabel(aVolumeLabel, driveEntry.iNumber);

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Volume label changed"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }

        RefreshViewL();
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::EjectDriveL(TInt aIndex)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);
        
        TInt err(KErrNone);
        
        // get current filesystem name
        TFileName fileSystemName;
        err = iFs.FileSystemName(fileSystemName, driveEntry.iNumber);
        
        if (err == KErrNone)
            {
            // Prevent SysAp shutting down applications
            RProperty::Set(
                KPSUidCoreApplicationUIs,
                KCoreAppUIsMmcRemovedWithoutEject,
                ECoreAppUIsEjectCommandUsed );
            
            // dismount the file system
            err = iFs.DismountFileSystem(fileSystemName, driveEntry.iNumber);
            
            if (err == KErrNone)
                {
                // remount the file system
                err = iFs.MountFileSystem(fileSystemName, driveEntry.iNumber);
                
                if (err == KErrInUse)
                    {
                    // try to remount after a while if locked
                    User::After(1000000);
                    err = iFs.MountFileSystem(fileSystemName, driveEntry.iNumber);
                    }                
                }
            }

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Ejected succesfully"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }              

        RefreshViewL();
        }
    }
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::DismountFileSystemL(TInt aIndex)
    {
    if (iDriveEntryList->Count() > aIndex && aIndex >= 0)
        {
        TDriveEntry driveEntry = iDriveEntryList->At(aIndex);

        TInt err(KErrNone);

        // get current filesystem name
        TFileName fileSystemName;
        err = iFs.FileSystemName(fileSystemName, driveEntry.iNumber);

        if (err == KErrNone)
            {
            // Prevent SysAp shutting down applications
            RProperty::Set(
                KPSUidCoreApplicationUIs,
                KCoreAppUIsMmcRemovedWithoutEject,
                ECoreAppUIsEjectCommandUsed );

            // dismount the file system
            err = iFs.DismountFileSystem(fileSystemName, driveEntry.iNumber);
            }

        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Dismounted succesfully"));
            }
        else if (err == KErrNotSupported)
            {
            iEngine->FileBrowserUI()->ShowErrorNote(_L("Not supported for this drive"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }

        RefreshViewL();
        }
    }
    
// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::ConvertCharsToPwd(const TDesC& aWord, TDes8& aConverted) const
    {
    aConverted.FillZ(aConverted.MaxLength());
    aConverted.Zero();
    
    if (aWord.Length())
        {
        aConverted.Copy( (TUint8*)(&aWord[0]), aWord.Size() );
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::EditDataTypesL()
    {
	// TODO
	/*
    iEngine->FileListContainer()->SetScreenLayoutL(EDisplayModeNormal);
    iEngine->FileListContainer()->SetNaviPaneTextL(KNullDesC);
    
    CFileBrowserDataTypesDlg* dlg = CFileBrowserDataTypesDlg::NewL();
    dlg->RunQueryLD();

    iEngine->FileListContainer()->SetScreenLayoutL(iEngine->Settings().iDisplayMode);
    iEngine->FileListContainer()->SetNaviPaneTextL(iCurrentPath);  
	*/	
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::SecureBackupL(TInt aType)
    {
    TInt err(KErrNone);
    TInt showStatus(EFalse);
    
    if (aType == EFileBrowserCmdToolsSecureBackupStartBackup || aType == EFileBrowserCmdToolsSecureBackupStartRestore)
        {
//        TInt queryIndexState(0);
//        TInt queryIndexType(0);
        
//        CAknListQueryDialog* listQueryDlgState = new(ELeave) CAknListQueryDialog(&queryIndexState);
//        if (listQueryDlgState->ExecuteLD(R_SECUREBACKUP_STATE_QUERY))
//            {
//
//            CAknListQueryDialog* listQueryDlgType = new(ELeave) CAknListQueryDialog(&queryIndexType);
//            if (listQueryDlgType->ExecuteLD(R_SECUREBACKUP_TYPE_QUERY))
//                {
//                conn::TBURPartType partType = conn::EBURNormal;
//                conn::TBackupIncType backupIncType = conn::ENoBackup;
//                
//                if (aType == EFileBrowserCmdToolsSecureBackupStartBackup)
//                    {
//                    if (queryIndexState == ESecureBackupStateFull)
//                        partType = conn::EBURBackupFull;
//                    else if (queryIndexState == ESecureBackupStatePartial)
//                        partType = conn::EBURBackupPartial;
//                    }
//                else if (aType == EFileBrowserCmdToolsSecureBackupStartRestore)
//                    {
//                    if (queryIndexState == ESecureBackupStateFull)
//                        partType = conn::EBURRestoreFull;
//                    else if (queryIndexState == ESecureBackupStatePartial)
//                        partType = conn::EBURRestorePartial;
//                    }
//                    
//                if (queryIndexType == ESecureBackupTypeBase)
//                    backupIncType = conn::EBackupBase;
//                else if (queryIndexType == ESecureBackupTypeIncremental)    
//                    backupIncType = conn::EBackupIncrement;    
//
//                // start secure backup
//                err = iFileOps->ActivateSecureBackUp(partType, backupIncType);
//                showStatus = ETrue;
//                }
//            }
        }

    else if (aType == EFileBrowserCmdToolsSecureBackupStop)
        {
        err = iFileOps->DeActivateSecureBackUp();
        showStatus = ETrue;
        }

    else
        User::Panic(_L("Sec.br.mode"), 101);
    
    
    if (showStatus)
        {
        if (err == KErrNone)
            {
            iEngine->FileBrowserUI()->ShowConfirmationNote(_L("Succeeded"));
            }
        else
            {
            CTextResolver* textResolver = CTextResolver::NewLC(*iEngine->EikonEnv());

            iEngine->FileBrowserUI()->ShowErrorNote(
                    textResolver->ResolveErrorString(err, CTextResolver::ECtxNoCtxNoSeparator) );

            CleanupStack::PopAndDestroy();  //textResolver
            }  
        }
    }

// --------------------------------------------------------------------------------------------

void CFileBrowserFileUtils::OpenCommonFileActionQueryL(TInt aCurrentItemIndex)
    {
    // TODO
    // just get current item
//    TInt currentItemIndex = iEngine->QueryCurrentItemIndex();

    if (iFileEntryList->Count() > aCurrentItemIndex && aCurrentItemIndex >= 0)
        {
//        TInt queryIndex(0);
//
//        CAknListQueryDialog* listQueryDlg = new(ELeave) CAknListQueryDialog(&queryIndex);
//
//        if (listQueryDlg->ExecuteLD(R_COMMON_FILE_ACTION_QUERY))
//            {
//            if (queryIndex == ECommonFileActionViewAsTextHex)
//                {
//                TFileEntry fileEntry = iFileEntryList->At(aCurrentItemIndex);
//
//                TFileName fullPath = fileEntry.iPath;
//                fullPath.Append(fileEntry.iEntry.iName);
//
//                TInt viewerType(EFileBrowserCmdFileViewHex);
//
//                // check from mime type if it's text
//                TDataType dataType;
//                TUid appUid;
//                if (iEngine->LsSession().AppForDocument(fullPath, appUid, dataType) == KErrNone)
//                    {
//                    TBuf<128> mimeTypeBuf;
//                    mimeTypeBuf.Copy(dataType.Des8());
//                    if (mimeTypeBuf == KNullDesC)
//                        mimeTypeBuf.Copy(_L("N/A"));
//
//                    if (mimeTypeBuf.CompareF(_L("text/plain")) == 0)
//                        viewerType = EFileBrowserCmdFileViewText;
//                    }
//
//                FileEditorL(viewerType);
//                }
//
//            else if (queryIndex == ECommonFileActionOpenWithApparc)
//                {
//                // TODO
//                //OpenWithApparcL();
//                }
//
//            else if (queryIndex == ECommonFileActionOpenWithDocHandlerEmbed)
//                {
//                // TODO
//                //OpenWithDocHandlerL(ETrue);
//                }
//
//            else
//                User::Panic(_L("Unk.Com.Act"), 221);
//            }
        }
    }
    
// --------------------------------------------------------------------------------------------
	            
// End of File

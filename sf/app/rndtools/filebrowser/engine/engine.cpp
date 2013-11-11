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

#include "engine.h"

//#include "FBApp.h"
#include "FB.hrh"
#include "FBFileUtils.h"
#include "FBStd.h"
//#include <filebrowser.rsg>

#include <eikenv.h>
#include <coeutils.h>
#include <bautils.h>
#include <apaid.h>
#include <s32file.h>

// CONSTANTS
// UID of the application
const TUid KUidFileBrowser = { 0x102828D6 };

const TInt KSettingsDrive = EDriveC;
_LIT(KSettingsFileName, "filebrowser_settings.ini");

// ===================================== MEMBER FUNCTIONS =====================================

CEngine* CEngine::NewL(MFileBrowserUI *aFileBrowserUI)
    {
    CEngine* self = new(ELeave) CEngine;
    CleanupStack::PushL(self);
    self->ConstructL(aFileBrowserUI);
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------

CEngine::CEngine()
    : iFileUtils(NULL)
    {
    }

// --------------------------------------------------------------------------------------------

void CEngine::ConstructL(MFileBrowserUI *aFileBrowserUI)
    {
    iFileBrowserUI = aFileBrowserUI;
    iEnv = CEikonEnv::Static();
    User::LeaveIfError(iLs.Connect());
    }

// --------------------------------------------------------------------------------------------

CEngine::~CEngine()
    {
	if (iFileUtils != NULL)
	    delete iFileUtils;

	iLs.Close();
    }

// ---------------------------------------------------------------------------

void CEngine::ActivateEngineL()
    {
    TRAP_IGNORE( LoadSettingsL() );

    iFileUtils = CFileBrowserFileUtils::NewL(this);
    }

// ---------------------------------------------------------------------------

void CEngine::DeActivateEngineL()
    {
    }
	
// ---------------------------------------------------------------------------

void CEngine::LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TInt& aValue)
    {
    if (aDicFS->IsPresentL(aUid))
        {
        RDictionaryReadStream in;
        in.OpenLC(*aDicFS, aUid);
        aValue = in.ReadInt16L();
        CleanupStack::PopAndDestroy(); // in        
        }
    }

// ---------------------------------------------------------------------------

void CEngine::LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TDes& aValue)
    {
    if (aDicFS->IsPresentL(aUid))
        {
        RDictionaryReadStream in;
        in.OpenLC(*aDicFS, aUid);
        TInt bufLength = in.ReadInt16L();   // get length of descriptor
        in.ReadL(aValue, bufLength);        // get the descriptor itself
        CleanupStack::PopAndDestroy(); // in
        }
    }

// ---------------------------------------------------------------------------

void CEngine::SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TInt& aValue)
    {
    RDictionaryWriteStream out;
    out.AssignLC(*aDicFS, aUid);
    out.WriteInt16L(aValue);
    out.CommitL(); 	
    CleanupStack::PopAndDestroy(1);// out
    }

// ---------------------------------------------------------------------------

void CEngine::SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TDes& aValue)
    {
    RDictionaryWriteStream out;
    out.AssignLC(*aDicFS, aUid);
    out.WriteInt16L(aValue.Length());       // write length of the descriptor
    out.WriteL(aValue, aValue.Length());    // write the descriptor itself
    out.CommitL(); 	
    CleanupStack::PopAndDestroy(1);// out
    }
        
// --------------------------------------------------------------------------------------------

void CEngine::LoadSettingsL()
    {
    const TSize screenSize = iEnv->ScreenDevice()->SizeInPixels();

    // set defaults
    iSettings.iDisplayMode = EDisplayModeFullScreen;
    iSettings.iFileViewMode = IsQHD(screenSize) ? EFileViewModeExtended : EFileViewModeSimple;
    iSettings.iShowSubDirectoryInfo = EFalse;
    iSettings.iShowAssociatedIcons = EFalse;
    iSettings.iRememberLastPath = EFalse;
    iSettings.iLastPath = KNullDesC;
    iSettings.iRememberFolderSelection = ETrue;
//#if 0 // TODO
//#if(!defined __SERIES60_30__ && !defined __SERIES60_31__ && !defined __S60_32__)
//    if ( AknLayoutUtils::PenEnabled() )
//        {
//        iSettings.iEnableToolbar = ETrue;
//        }
//    else
//        {
//        iSettings.iEnableToolbar = EFalse;
//        }
//#else
//    iSettings.iEnableToolbar = EFalse;
//#endif
//#endif // TODO

    iSettings.iSupportNetworkDrives = EFalse;
    iSettings.iBypassPlatformSecurity = EFalse;
    iSettings.iRemoveFileLocks = ETrue;
    iSettings.iIgnoreProtectionsAtts = ETrue;
    iSettings.iRemoveROMWriteProrection = ETrue;

    // build specific defaults
#if(!defined __SERIES60_30__ && !defined __SERIES60_31__)
    iSettings.iSupportNetworkDrives = ETrue;
#endif 


    // make sure that the private path of this app in c-drive exists
    iEnv->FsSession().CreatePrivatePath( KSettingsDrive ); // c:\\private\\102828d6\\
    
    // handle settings always in the private directory 
    if (iEnv->FsSession().SetSessionToPrivate( KSettingsDrive ) == KErrNone)
        {
        // open or create a dictionary file store
        CDictionaryFileStore* settingsStore = CDictionaryFileStore::OpenLC(iEnv->FsSession(), KSettingsFileName, KUidFileBrowser);

        LoadDFSValueL(settingsStore, KFBSettingDisplayMode,                 iSettings.iDisplayMode);
        LoadDFSValueL(settingsStore, KFBSettingFileViewMode,                iSettings.iFileViewMode);
        LoadDFSValueL(settingsStore, KFBSettingShowSubDirectoryInfo,        iSettings.iShowSubDirectoryInfo);
        LoadDFSValueL(settingsStore, KFBSettingShowAssociatedIcons,         iSettings.iShowAssociatedIcons);
        LoadDFSValueL(settingsStore, KFBSettingRememberLastPath,            iSettings.iRememberLastPath);
        LoadDFSValueL(settingsStore, KFBSettingLastPath,                    iSettings.iLastPath);
        LoadDFSValueL(settingsStore, KFBSettingFolderSelection,             iSettings.iRememberFolderSelection);
        LoadDFSValueL(settingsStore, KFBSettingEnableToolbar,               iSettings.iEnableToolbar);

        LoadDFSValueL(settingsStore, KFBSettingSupportNetworkDrives,        iSettings.iSupportNetworkDrives);
        LoadDFSValueL(settingsStore, KFBSettingBypassPlatformSecurity,      iSettings.iBypassPlatformSecurity);
        LoadDFSValueL(settingsStore, KFBSettingRemoveFileLocks,             iSettings.iRemoveFileLocks);
        LoadDFSValueL(settingsStore, KFBSettingIgnoreProtectionsAtts,       iSettings.iIgnoreProtectionsAtts);
        LoadDFSValueL(settingsStore, KFBSettingRemoveROMWriteProtection,    iSettings.iRemoveROMWriteProrection);

        CleanupStack::PopAndDestroy(); // settingsStore         
        }
    }

// --------------------------------------------------------------------------------------------

void CEngine::SaveSettingsL(TBool aNotifyModules)
    {
    // handle settings always in c:\\private\\102828d6\\
    if (iEnv->FsSession().SetSessionToPrivate( KSettingsDrive ) == KErrNone)
        {
        // delete existing store to make sure that it is clean and not eg corrupted
        if (BaflUtils::FileExists(iEnv->FsSession(), KSettingsFileName))
            {
            iEnv->FsSession().Delete(KSettingsFileName);
            }
        
        // create a dictionary file store
        CDictionaryFileStore* settingsStore = CDictionaryFileStore::OpenLC(iEnv->FsSession(), KSettingsFileName, KUidFileBrowser);

        SaveDFSValueL(settingsStore, KFBSettingDisplayMode,                 iSettings.iDisplayMode);
        SaveDFSValueL(settingsStore, KFBSettingFileViewMode,                iSettings.iFileViewMode);
        SaveDFSValueL(settingsStore, KFBSettingShowSubDirectoryInfo,        iSettings.iShowSubDirectoryInfo);
        SaveDFSValueL(settingsStore, KFBSettingShowAssociatedIcons,         iSettings.iShowAssociatedIcons);
        SaveDFSValueL(settingsStore, KFBSettingRememberLastPath,            iSettings.iRememberLastPath);
        SaveDFSValueL(settingsStore, KFBSettingLastPath,                    iSettings.iLastPath);
        SaveDFSValueL(settingsStore, KFBSettingFolderSelection,             iSettings.iRememberFolderSelection);
        SaveDFSValueL(settingsStore, KFBSettingEnableToolbar,               iSettings.iEnableToolbar);

        SaveDFSValueL(settingsStore, KFBSettingSupportNetworkDrives,        iSettings.iSupportNetworkDrives);
        SaveDFSValueL(settingsStore, KFBSettingBypassPlatformSecurity,      iSettings.iBypassPlatformSecurity);
        SaveDFSValueL(settingsStore, KFBSettingRemoveFileLocks,             iSettings.iRemoveFileLocks);
        SaveDFSValueL(settingsStore, KFBSettingIgnoreProtectionsAtts,       iSettings.iIgnoreProtectionsAtts);
        SaveDFSValueL(settingsStore, KFBSettingRemoveROMWriteProtection,    iSettings.iRemoveROMWriteProrection);
        
        settingsStore->CommitL();
        CleanupStack::PopAndDestroy(); // settingsStore             
        }

    // update changes to modules
    if (aNotifyModules)
        {
        //iScreenCapture->HandleSettingsChangeL();
        if (iFileUtils != NULL) 
			{
			iFileUtils->HandleSettingsChangeL();
			}
        //iFileListContainer->HandleSettingsChangeL();
        }
    }

// --------------------------------------------------------------------------------------------

TInt CEngine::LaunchSettingsDialogL()
    {
	TInt retValue = KErrNone;
	// TODO:
    // set to normal mode
    //iFileListContainer->SetScreenLayoutL(EDisplayModeNormal);
    //iFileListContainer->DeleteNaviPane();
    //iFileListContainer->HideToolbar();
    
    // launch the dialog and save settings
    //CFileBrowserSettingViewDlg* dlg = CFileBrowserSettingViewDlg::NewL(iSettings);
    //TInt retValue = dlg->ExecuteLD(R_FILEBROWSER_SETTINGS_DIALOG);        
    //FileListContainer()->CreateEmptyNaviPaneLabelL();
    TRAP_IGNORE(SaveSettingsL());
    return retValue;
    }
	
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------

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
	iError = iStatus.Int();
	//CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
	iWait.AsyncStop();
	}
	
void CAsyncWaiter::DoCancel()
	{
	iError = KErrCancel;
    if( iStatus == KRequestPending )
        {
        TRequestStatus* s=&iStatus;
        User::RequestComplete( s, KErrCancel );
        }

        //CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
	iWait.AsyncStop();
	}

// --------------------------------------------------------------------------------------------

void CEngine::OpenWithApparcL(TFileName aFileName)
        {
	if (iFileUtils != NULL) 
		{
		iFileUtils->OpenWithApparcL(aFileName);
		}
	}

// --------------------------------------------------------------------------------------------

void CEngine::OpenWithDocHandlerL(TFileName aFileName, TBool aEmbed)
        {
	if (iFileUtils != NULL) 
		{
		iFileUtils->OpenWithDocHandlerL(aFileName, aEmbed);
		}
	}

// --------------------------------------------------------------------------------------------

//TInt CEngine::QueryCurrentItemIndex()
//	{
//        return iFileBrowserUI->QueryCurrentItemIndex();
//	}

// --------------------------------------------------------------------------------------------	

TSearchAttributes CEngine::GetSearchAttributes()
	{
	if (iFileUtils != NULL) 
		{
		return iFileUtils->GetSearchAttributes();
		} 
	else
		{
		return TSearchAttributes(); 
		}
	}

// --------------------------------------------------------------------------------------------	

void CEngine::ChangeAttributes(TSearchAttributes attributes) 
	{ 
	if (iFileUtils != NULL) 
		{
		iFileUtils->ChangeAttributes(attributes); 
		}
	}

// --------------------------------------------------------------------------------------------	

TSearchResults CEngine::SearchResults() 
	{ 
	if (iFileUtils != NULL) 
		{
		return iFileUtils->SearchResults(); 
		}
	else
		{
		return TSearchResults();
		}
	}

// --------------------------------------------------------------------------------------------	

CFileEntryList* CEngine::FoundFiles() 
	{ 
	if (iFileUtils != NULL) 
		{
		return iFileUtils->FoundFiles(); 
		}
	else 
		{
		return NULL;
		}
	}
	
// --------------------------------------------------------------------------------------------	

void CEngine::SearchL() 
	{ 
	if (iFileUtils != NULL) 
		{
		TRAP_IGNORE(iFileUtils->SearchL()); 
		}
	}
	
// --------------------------------------------------------------------------------------------	
	
// End of File

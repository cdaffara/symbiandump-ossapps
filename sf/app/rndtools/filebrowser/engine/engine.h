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

#ifndef __ENGINE_H__
#define __ENGINE_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <apgcli.h>

#include "FBFileUtils.h"

// setting keys (do not change uids of existing keys to maintain compatibility to older versions!)
const TUid KFBSettingDisplayMode                   = { 0x00 };
const TUid KFBSettingFileViewMode                  = { 0x01 };
const TUid KFBSettingShowSubDirectoryInfo          = { 0x02 };
const TUid KFBSettingShowAssociatedIcons           = { 0x03 };
const TUid KFBSettingRememberLastPath              = { 0x04 };
const TUid KFBSettingLastPath                      = { 0x05 };

const TUid KFBSettingSupportNetworkDrives          = { 0x06 };
const TUid KFBSettingBypassPlatformSecurity        = { 0x07 };
const TUid KFBSettingRemoveFileLocks               = { 0x08 };
const TUid KFBSettingIgnoreProtectionsAtts         = { 0x09 };
const TUid KFBSettingRemoveROMWriteProtection      = { 0x0A };

const TUid KFBSettingFolderSelection               = { 0x0B };
const TUid KFBSettingEnableToolbar                 = { 0x0C };


// FORWARD DECLARATIONS
class CFileBrowserFileListContainer;
class CFileBrowserScreenCapture;
class CFileBrowserFileUtils;
class CEikonEnv;
class CDictionaryFileStore;
class MFileBrowserUI;

// CLASS DECLARATIONS

class TFileBrowserSettings
    {
public:
    TInt                                        iDisplayMode;
    TInt                                        iFileViewMode;
    TBool                                       iShowSubDirectoryInfo;
    TBool                                       iShowAssociatedIcons;
    TBool                                       iRememberLastPath;
    TFileName                                   iLastPath;
    TBool                                       iRememberFolderSelection;
    TBool                                       iEnableToolbar;

    TBool                                       iSupportNetworkDrives;
    TBool                                       iBypassPlatformSecurity;
    TBool                                       iRemoveFileLocks;
    TBool                                       iIgnoreProtectionsAtts;
    TBool                                       iRemoveROMWriteProrection;
    };

// INFERFACE FOR COMMUNICATION WITH UI

class MFileBrowserUI
{
public:
//    /**
//      * Method from getting current index
//      * @return current index.
//      */
//    virtual TInt QueryCurrentItemIndex() = 0;

    /**
     * Shows info message with text
     * @param A aText text to be shown in message body.
     * @param A aTitle text to be shown in message title.
     * @return None.
     */
    virtual void ShowInformationNote(const TDesC &aDescText, const TDesC &aDescTitle) = 0;

    /**
     * Shows error message with error text
     * @param aText An error text to be shown in message.
     * @return None.
     */
    virtual void ShowErrorNote(const TDesC& aText, TBool aNoTimeout = EFalse) = 0;

    /**
     * Shows confirmation message with text
     * @param aText An text to be shown in message.
     * @return None.
     */
    virtual void ShowConfirmationNote(const TDesC& aText, TBool aNoTimeout = EFalse) = 0;

    /**
     * Shows progress bar with text
     * @param aText A text to be shown at top of the progress bar.
     * @param aMinimum A minimum progress bar value.
     * @param aMaximum A maximum progress bar value.
     * @return None.
     */
    virtual void ShowProgressDialog(const TDesC& aDescText, TInt aMinimum, TInt aMaximum ) = 0;

    /**
     * Cancel progress dialog
     * @return None.
     */
    virtual void CancelProgressDialog() = 0;

    /**
      * Set progress dialog value
      * @param aValue A vaule to be shown at top of the progress dialog.
      */
    virtual void SetProgressValue(TInt aValue) = 0;
//
//    /**
//     * Sets progress bar value
//     * @param aValue A value between min and max value of the progress bar range
//     * @return None.
//     */
//    virtual void SetProgressBarValue(TInt aValue) = 0;
//
//    /**
//     * Hides progress bar
//     * @return None.
//     */
//    virtual void HideProgressBar() = 0;
//
    /**
     * Shows wait dialog with text
     * @param aText A text to be shown at top of the wait bar.
     * @return None.
     */
    virtual void ShowWaitDialog(const TDesC& aText) = 0;

    /**
     * Cancel wait dialog
     * @return None.
     */
    virtual void CancelWaitDialog() = 0;

    /**
     * Processes all pending events to allow wait/progresa dialog to update itself
     * @return None.
     */
    virtual void ProcessEvents() = 0;

    /**
     * Shows confirmation dialog
     * @param aFileName String contaning file name and path
     * @return ETrue if user pressed OK button, otherwise EFalse.
     */
    virtual TBool ShowConfirmationQuery(const TDesC& aDescText) = 0;

    /**
      * Notify wrapping model data has changed
      */
    virtual void NotifyModelHasChanged() = 0;

};

class CEngine : public CBase
    {
public:
    static CEngine* NewL(MFileBrowserUI *aFileBrowserUI);
    ~CEngine();

private:
    CEngine();
    void ConstructL(MFileBrowserUI *aFileBrowserUI);
    void LoadSettingsL();
    void LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TInt& aValue);
    void LoadDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, TDes& aValue);
    void SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TInt& aValue);
    void SaveDFSValueL(CDictionaryFileStore* aDicFS, const TUid& aUid, const TDes& aValue);

public:
    void ActivateEngineL();
    void DeActivateEngineL();
    void SaveSettingsL(TBool aNotifyModules=ETrue);
    TInt LaunchSettingsDialogL();
    inline TFileBrowserSettings& Settings() { return iSettings; }
    inline CEikonEnv* EikonEnv() { return iEnv; }
    inline RApaLsSession& LsSession() { return iLs; }
    inline CFileBrowserScreenCapture* ScreenCapture() { return iScreenCapture; }
    inline CFileBrowserFileUtils* FileUtils() { return iFileUtils; }
    inline MFileBrowserUI* FileBrowserUI() { return iFileBrowserUI; }

    void OpenWithApparcL(TFileName aFileName);
    void OpenWithDocHandlerL(TFileName aFileName, TBool aEmbed);
//    TInt QueryCurrentItemIndex();
    TSearchAttributes GetSearchAttributes();
    void ChangeAttributes(TSearchAttributes attributes);
    TSearchResults SearchResults();
    CFileEntryList* FoundFiles();	
    void SearchL();

private:
    MFileBrowserUI                  *iFileBrowserUI;
    CFileBrowserScreenCapture       *iScreenCapture;
    CFileBrowserFileUtils           *iFileUtils;
    CEikonEnv*                      iEnv;
    TFileBrowserSettings            iSettings;
    RApaLsSession                   iLs;
    TBool                           iIsHashKeySelectionInUse; 
    };
   

#endif // __ENGINE_H__

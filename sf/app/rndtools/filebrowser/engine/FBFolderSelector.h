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
* Description:  
*
*/

#ifndef FILEBROWSER_FILEDLGS_H
#define FILEBROWSER_FILEDLGS_H

#include "FBFileUtils.h"

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <apgcli.h>

/**
  * Interface to communicate UI with
  */
class MFolderSelectorUI
{
public:
    /**
     * Shows info message with text
//     * @param A aText text to be shown in message body.
//     * @param A aTitle text to be shown in message title.
     * @return None.
     */
    virtual void InformFolderSelectionChanged() = 0;
};

/**
  * Backend class for destination folder selection dialog
  */
class CFileBrowserFolderSelector
    {
public:
    static CFileBrowserFolderSelector *NewL(TDes& aReturnPath, MFolderSelectorUI *);
    virtual ~CFileBrowserFolderSelector();

    inline TFileName CurrentPath() const { return iCurrentPath; }
    inline TBool IsDriveListViewActive() const { return iCurrentPath==KNullDesC; }
    inline const CDriveEntryList *DriveEntryList() { return iDriveEntryList; }
    inline const CFileEntryList *FileEntryList() {return iFileEntryList; }

    void MoveDownToDirectoryL(TInt index);
    void MoveUpOneLevelL();

private: // Constructors
    CFileBrowserFolderSelector(TDes& aReturnPath, MFolderSelectorUI *);
    void ConstructL();

protected:  // From CEikDialog
    TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);
    void PreLayoutDynInitL();
    void PostLayoutDynInitL();
    TBool OkToExitL(TInt aButtonId);    

protected:  // From CCoeControl
    //void HandlePointerEventL(const TPointerEvent &aPointerEvent); 

private: // New methods
    void RefreshViewL();

    void GetDriveListL();
    void GetFileListL();

private: // Data
    TInt                            iSelectedIndex;
    TBuf<16>                        iLSKActionText;
    TDes                           &iReturnPath;
    RFs                             iFs;
    CDriveEntryList                *iDriveEntryList;
    CFileEntryList                 *iFileEntryList;
    TFileName                       iCurrentPath;
    MFolderSelectorUI               *iFolderSelectorUI;
    };

#endif

// End of File


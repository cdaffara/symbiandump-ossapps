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

#include "fbfolderselectorwrapper.h"
#include "FBFolderSelector.h"

#include <QString>

// ---------------------------------------------------------------------------

/**
 * Constructor
 */
FbFolderSelectorWrapper::FbFolderSelectorWrapper()
    : mDestinationFolderSelector(0)
{
}

/**
 * Destructor
 */
FbFolderSelectorWrapper::~FbFolderSelectorWrapper()
{
    if (mDestinationFolderSelector)
        delete mDestinationFolderSelector;
}

/**
 * Initializes Engine Wrapper
 * @return true if engine was started successfully
 */
bool FbFolderSelectorWrapper::init()
{
    TFileName destinationFolder;

    TRAPD(err, mDestinationFolderSelector = CFileBrowserFolderSelector::NewL(destinationFolder, this));
    if (err != KErrNone) {
        return false;
    } else {
        //TRAP_IGNORE(mEngine->ActivateEngineL());
        //mSettings = FileBrowserSettings(&mEngine->Settings());
        return true;
    }
}

/**
  * Return current path
  */
QString FbFolderSelectorWrapper::currentPath() const
{
    return QString::fromUtf16(mDestinationFolderSelector->CurrentPath().Ptr(),
                              mDestinationFolderSelector->CurrentPath().Length());
}

/**
  * Return whether drive list view is active
  */
bool FbFolderSelectorWrapper::isDriveListViewActive() const
{
    return mDestinationFolderSelector->IsDriveListViewActive();
}

/**
  * Returns number of either drives or files depending on current view
  */
int FbFolderSelectorWrapper::itemCount() const
{
    if (isDriveListViewActive()) {
        return mDestinationFolderSelector->DriveEntryList()->Count();
    } else {
        return mDestinationFolderSelector->FileEntryList()->Count();
    }
}

/**
  * Returns drive entry for given \a index
  */
FbDriveEntry FbFolderSelectorWrapper::getDriveEntry(const int index) const
{
    TDriveEntry driveEntry;
    if (mDestinationFolderSelector->DriveEntryList()->Count() > index && index >= 0) {
        driveEntry = mDestinationFolderSelector->DriveEntryList()->At(index);
    }
    return FbDriveEntry(driveEntry);
}

/**
  * Returns file entry for given \a index
  */
FbFileEntry FbFolderSelectorWrapper::getFileEntry(const int index) const
{
    TFileEntry fileEntry;
    if (mDestinationFolderSelector->FileEntryList()->Count() > index && index >= 0) {
        fileEntry = mDestinationFolderSelector->FileEntryList()->At(index);
    }
    return FbFileEntry(fileEntry);
}

/**
  * Move down to selected item by \a index
  */
void FbFolderSelectorWrapper::moveDownToDirectory(int index)
{
    mDestinationFolderSelector->MoveDownToDirectoryL(index);
}

/**
  * Move up from folder
  */
void FbFolderSelectorWrapper::moveUpOneLevel()
{
    mDestinationFolderSelector->MoveUpOneLevelL();
}

void FbFolderSelectorWrapper::InformFolderSelectionChanged()
{
    emit FolderSelectionChanged();
}

// ---------------------------------------------------------------------------

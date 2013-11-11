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

#ifndef FILEBROWSERSETTINGS_H_
#define FILEBROWSERSETTINGS_H_

#include "engine.h"
#include "enginewrapper.h"

#include <QString>

/**
  * Settings class that is used for ui settings handling
  */
class FileBrowserSettings
{
public:
    FileBrowserSettings(TFileBrowserSettings *aSettings) : mSettings(aSettings) { }
    ~FileBrowserSettings() { }

    int displayMode() const { return mSettings->iDisplayMode; }
    void setDisplayMode(int displayMode) { mSettings->iDisplayMode = displayMode; }

    int fileViewMode() const { return mSettings->iFileViewMode; }
    void setFileViewMode(int fileViewMode) { mSettings->iFileViewMode = fileViewMode; }

    bool showSubDirectoryInfo() const { return mSettings->iShowSubDirectoryInfo; }
    void setShowSubDirectoryInfo(bool showSubDirectoryInfo) { mSettings->iShowSubDirectoryInfo = showSubDirectoryInfo; }

    bool showAssociatedIcons() const { return mSettings->iShowAssociatedIcons; }
    void setShowAssociatedIcons(bool showAssociatedIcons) { mSettings->iShowAssociatedIcons = showAssociatedIcons; }

    bool rememberLastPath() const { return mSettings->iRememberLastPath; }
    void setRememberLastPath(bool rememberLastPath) { mSettings->iRememberLastPath = rememberLastPath; }

    QString lastPath() const { return QString((QChar*)mSettings->iLastPath.Ptr(),mSettings->iLastPath.Length()); }
    void lastPath(const QString& lastPath) { mSettings->iLastPath = lastPath.utf16(); }

    bool rememberFolderSelection() const { return mSettings->iRememberFolderSelection; }
    void setRememberFolderSelection(bool rememberFolderSelection) { mSettings->iRememberFolderSelection = rememberFolderSelection; }

    bool enableToolbar() const { return mSettings->iEnableToolbar; }
    void setEnableToolbar(bool enableToolbar) { mSettings->iEnableToolbar = enableToolbar; }

    bool supportNetworkDrives() const { return mSettings->iSupportNetworkDrives; }
    void setSupportNetworkDrives(bool supportNetworkDrives) { mSettings->iSupportNetworkDrives = supportNetworkDrives; }

    bool bypassPlatformSecurity() const { return mSettings->iBypassPlatformSecurity; }
    void setBypassPlatformSecurity(bool bypassPlatformSecurity) { mSettings->iBypassPlatformSecurity = bypassPlatformSecurity; }

    bool removeFileLocks() const { return mSettings->iRemoveFileLocks; }
    void setRemoveFileLocks(bool removeFileLocks) { mSettings->iRemoveFileLocks = removeFileLocks; }

    bool ignoreProtectionsAttributes() const { return mSettings->iIgnoreProtectionsAtts; }
    void setIgnoreProtectionsAttributes(bool ignoreProtectionsAttributes) { mSettings->iIgnoreProtectionsAtts = ignoreProtectionsAttributes; }

    bool removeROMWriteProrection() const { return mSettings->iRemoveROMWriteProrection; }
    void setRemoveROMWriteProrection(bool removeROMWriteProrection) { mSettings->iRemoveROMWriteProrection = removeROMWriteProrection; }

private:
    TFileBrowserSettings *mSettings;
};

#endif /* FILEBROWSERSETTINGS_H_ */

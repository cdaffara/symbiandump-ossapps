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

#ifndef FBSETTINGSVIEW_H
#define FBSETTINGSVIEW_H

#include <hbview.h>

class HbDataForm;
class HbDataFormModelItem;
class EngineWrapper;
class FileBrowserSettings;

class FbSettingsView : public HbView
{
    Q_OBJECT

public:
    FbSettingsView(EngineWrapper &engineWrapper);
    virtual ~FbSettingsView();

    void initDataForm();
signals:
    void finished(bool ok);

public slots:
//    void displayModeChanged();
//    void fileViewChanged();
//    void subDirectoryInfoChanged();
//    void associatedIconsChanged();
//    void rememberFolderOnExitChanged();
//    void rememberLastFolderChanged();
//    void showToolbarChanged();

//    void toggleChange(QModelIndex, QModelIndex);
    void accept();
    void reject();

private:
    void createToolbar();
    void constructMenu();
    void loadSettings(const FileBrowserSettings &settings);
    void saveSettings(FileBrowserSettings &settings);

private:
    EngineWrapper &mEngineWrapper;

    HbDataForm *mForm;
    HbDataFormModelItem *mDisplayModeItem;
    HbDataFormModelItem *mFileViewItem;
    HbDataFormModelItem *mShowDirectoryInfoItem;
    HbDataFormModelItem *mShowAssociatedIconsItem;
    HbDataFormModelItem *mRememberFolderOnExitItem;
    HbDataFormModelItem *mRememberLastFolderItem;
    HbDataFormModelItem *mShowToolbarItem;

    HbDataFormModelItem *mSupportNetDrivesItem; //"Support net drives"
    HbDataFormModelItem *mBypassPlatformSecurityItem; //"Bypass plat.security"
    HbDataFormModelItem *mUnlockFilesViaSBItem; //"Unlock files via SB"
    HbDataFormModelItem *mIgnoreProtectionAttributesItem; //"Ign. protection atts"
    HbDataFormModelItem *mNoROAttributeCopyFromZItem; //"No RO-att copy from Z:"

};

#endif // FBSETTINGSVIEW_H

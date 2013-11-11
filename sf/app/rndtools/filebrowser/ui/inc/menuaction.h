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

#ifndef MENUACTION_H
#define MENUACTION_H

class HbMenu;
class HbAction;

class OptionMenuActions
{
public:
    OptionMenuActions() :
            mFileMenu(0),
            mFileBackMoveUp(0),
            mFileOpenDrive(0),
            mFileOpenDirectory(0),
            mFileSearch(0),
            mFileNewMenu(0),
            mFileNewFile(0),
            mFileNewDirectory(0),
            mFileDelete(0),
            mFileRename(0),
            mFileTouch(0),
            mFileProperties(0),
            mFileChecksumsMenu(0),
            mFileChecksumsMD5(0),
            mFileChecksumsMD2(0),
            mFileChecksumsSHA1(0),
            mFileSetAttributes(0),
            //Edit -
            mEditMenu(0),
            mEditSnapShotToE(0),
            mEditCut(0),
            mEditCopy(0),
            mEditPaste(0),
            mEditCopyToFolder(0),
            mEditMoveToFolder(0),
            mEditSelect(0),
            mEditUnselect(0),
            mEditSelectAll(0),
            mEditUnselectAll(0),
            // View -
            mViewMenu(0),
            mViewFilterEntries(0),
            mViewRefresh(0),
            // Disk admin -
            mDiskAdminMenu(0),
            mDiskAdminSetDrivePassword(0),
            mDiskAdminUnlockDrive(0),
            mDiskAdminClearDrivePassword(0),
            mDiskAdminEraseDrivePassword(0),
            mDiskAdminFormatDrive(0),
            mDiskAdminQuickFormatDrive(0),
            mDiskAdminCheckDisk(0),
            mDiskAdminScanDrive(0),
            mDiskAdminSetDriveName(0),
            mDiskAdminSetDriveVolumeLabel(0),
            mDiskAdminEjectDrive(0),
            mDiskAdminDismountDrive(0),
            mDiskAdminEraseMBR(0),
            mDiskAdminPartitionDrive(0),
            // Tools -
            mToolsMenu(0),
            mToolsAllAppsToTextFile(0),
            mToolsAllFilesToTextFile(0),
            mToolsAvkonIconCacheMenu(0),
            mToolsAvkonIconCacheEnable(0),
            mToolsAvkonIconCacheDisable(0),
            mToolsDisableExtendedErrors(0),
            mToolsDumpMsgStoreWalk(0),
            mToolsEditDataTypes(0),
            mToolsEnableExtendedErrors(0),
            mToolsErrorSimulateMenu(0),
            mToolsErrorSimulateLeave(0),
            mToolsErrorSimulatePanic(0),
            mToolsErrorSimulateException(0),
            mToolsMessageAttachmentsMenu(0),
            mToolsMessageInbox(0),
            mToolsMessageDrafts(0),
            mToolsMessageSentItems(0),
            mToolsMessageOutbox(0),
            mToolsLocalConnectivityMenu(0),
            mToolsLocalConnectivityActivateInfrared(0),
            mToolsLocalConnectivityLaunchBTUI(0),
            mToolsLocalConnectivityLaunchUSBUI(0),
            mToolsMemoryInfo(0),
            mToolsSecureBackupMenu(0),
            mToolsSecureBackStart(0),
            mToolsSecureBackRestore(0),
            mToolsSecureBackStop(0),
            mToolsSetDebugMask(0),
            mToolsShowOpenFilesHere(0),
            mSelection(0),
            mSetting(0),
            mAbout(0),
            mExit(0)
    {

    }

    ~OptionMenuActions() { }
public:
    //File menu operations
    HbMenu *mFileMenu;
    HbAction *mFileBackMoveUp;
    HbAction *mFileOpenDrive;
    HbAction *mFileOpenDirectory;
    HbAction *mFileSearch;
    HbMenu *mFileNewMenu;
    HbAction *mFileNewFile;
    HbAction *mFileNewDirectory;
    HbAction *mFileDelete;
    HbAction *mFileRename;
    HbAction *mFileTouch;
    HbAction *mFileProperties;
    HbMenu *mFileChecksumsMenu;
    HbAction *mFileChecksumsMD5;
    HbAction *mFileChecksumsMD2;
    HbAction *mFileChecksumsSHA1;
    HbAction *mFileSetAttributes;

    //Edit -
    HbMenu *mEditMenu;
    HbAction *mEditSnapShotToE;
    HbAction *mEditCut;
    HbAction *mEditCopy;
    HbAction *mEditPaste;
    HbAction *mEditCopyToFolder;
    HbAction *mEditMoveToFolder;
    HbAction *mEditSelect;
    HbAction *mEditUnselect;
    HbAction *mEditSelectAll;
    HbAction *mEditUnselectAll;

    // View -
    HbMenu *mViewMenu;
    HbAction *mViewFilterEntries;
    HbAction *mViewRefresh;

    // Disk admin -
    HbMenu *mDiskAdminMenu;
    HbAction *mDiskAdminSetDrivePassword;
    HbAction *mDiskAdminUnlockDrive;
    HbAction *mDiskAdminClearDrivePassword;
    HbAction *mDiskAdminEraseDrivePassword;
    HbAction *mDiskAdminFormatDrive;
    HbAction *mDiskAdminQuickFormatDrive;
    HbAction *mDiskAdminCheckDisk;
    HbAction *mDiskAdminScanDrive;
    HbAction *mDiskAdminSetDriveName;
    HbAction *mDiskAdminSetDriveVolumeLabel;
    HbAction *mDiskAdminEjectDrive;
    HbAction *mDiskAdminDismountDrive;
    HbAction *mDiskAdminEraseMBR;
    HbAction *mDiskAdminPartitionDrive;

    // Tools -
    HbMenu *mToolsMenu;
    HbAction *mToolsAllAppsToTextFile;
    HbAction *mToolsAllFilesToTextFile;

    HbMenu *mToolsAvkonIconCacheMenu;
    HbAction *mToolsAvkonIconCacheEnable;
    HbAction *mToolsAvkonIconCacheDisable;

    HbAction *mToolsDisableExtendedErrors;
    HbAction *mToolsDumpMsgStoreWalk;
    HbAction *mToolsEditDataTypes;

    HbAction *mToolsEnableExtendedErrors;

    HbMenu *mToolsErrorSimulateMenu;    
    HbAction *mToolsErrorSimulateLeave;
    HbAction *mToolsErrorSimulatePanic;
    HbAction *mToolsErrorSimulateException;

    HbMenu *mToolsMessageAttachmentsMenu;
    HbAction *mToolsMessageInbox;
    HbAction *mToolsMessageDrafts;
    HbAction *mToolsMessageSentItems;
    HbAction *mToolsMessageOutbox;

    HbMenu *mToolsLocalConnectivityMenu;
    HbAction *mToolsLocalConnectivityActivateInfrared;
    HbAction *mToolsLocalConnectivityLaunchBTUI;
    HbAction *mToolsLocalConnectivityLaunchUSBUI;

    HbAction *mToolsMemoryInfo;

    HbMenu *mToolsSecureBackupMenu;
    HbAction *mToolsSecureBackStart;
    HbAction *mToolsSecureBackRestore;
    HbAction *mToolsSecureBackStop;

    HbAction *mToolsSetDebugMask;
    HbAction *mToolsShowOpenFilesHere;

    HbAction *mSelection;
    HbAction *mSetting;
    HbAction *mAbout;
    HbAction *mExit;
};


class ContextMenuActions
{
public:
    ContextMenuActions() :
            mOpenDirectory(0),
            mSearch(0),
            //File -
            mFileMenu(0),
            mFileBackMoveUp(0),
            mFileOpenDrive(0),
            mFileOpenDirectory(0),
            mFileSearch(0),
            mFileNewMenu(0),
            mFileNewFile(0),
            mFileNewDirectory(0),
            mFileDelete(0),
            mFileRename(0),
            mFileTouch(0),
            mFileProperties(0),
            mFileChecksumsMenu(0),
            mFileChecksumsMD5(0),
            mFileChecksumsMD2(0),
            mFileChecksumsSHA1(0),
            mFileSetAttributes(0),
            //Edit -
            mEditMenu(0),
//            mEditSnapShotToE(0),
            mEditCut(0),
            mEditCopy(0),
            mEditPaste(0),
            mEditCopyToFolder(0),
            mEditMoveToFolder(0),
            // View -
//            mViewMenu(0),
//            mViewFilterEntries(0),
//            mViewRefresh(0),
            // Disk admin -
            mDiskAdminMenu(0),
            mDiskAdminSetDrivePassword(0),
            mDiskAdminUnlockDrive(0),
            mDiskAdminClearDrivePassword(0),
            mDiskAdminEraseDrivePassword(0),
            mDiskAdminFormatDrive(0),
            mDiskAdminQuickFormatDrive(0),
            mDiskAdminCheckDisk(0),
            mDiskAdminScanDrive(0),
            mDiskAdminSetDriveName(0),
            mDiskAdminSetDriveVolumeLabel(0),
            mDiskAdminEjectDrive(0),
            mDiskAdminDismountDrive(0),
            mDiskAdminEraseMBR(0),
            mDiskAdminPartitionDrive(0)
    {

    }

    ~ContextMenuActions() { }
public:
    HbAction *mOpenDirectory;
    HbAction *mSearch;
    //File menu operations
    HbMenu *mFileMenu;
    HbAction *mFileBackMoveUp;
    HbAction *mFileOpenDrive;
    HbAction *mFileOpenDirectory;
    HbAction *mFileSearch;
    HbMenu *mFileNewMenu;
    HbAction *mFileNewFile;
    HbAction *mFileNewDirectory;
    HbAction *mFileDelete;
    HbAction *mFileRename;
    HbAction *mFileTouch;
    HbAction *mFileProperties;
    HbMenu *mFileChecksumsMenu;
    HbAction *mFileChecksumsMD5;
    HbAction *mFileChecksumsMD2;
    HbAction *mFileChecksumsSHA1;
    HbAction *mFileSetAttributes;

    //Edit -
    HbMenu *mEditMenu;
//    HbAction *mEditSnapShotToE;
    HbAction *mEditCut;
    HbAction *mEditCopy;
    HbAction *mEditPaste;
    HbAction *mEditCopyToFolder;
    HbAction *mEditMoveToFolder;

//    // View -
//    HbMenu *mViewMenu;
//    HbAction *mViewFilterEntries;
//    HbAction *mViewRefresh;

    // Disk admin -
    HbMenu *mDiskAdminMenu;
    HbAction *mDiskAdminSetDrivePassword;
    HbAction *mDiskAdminUnlockDrive;
    HbAction *mDiskAdminClearDrivePassword;
    HbAction *mDiskAdminEraseDrivePassword;
    HbAction *mDiskAdminFormatDrive;
    HbAction *mDiskAdminQuickFormatDrive;
    HbAction *mDiskAdminCheckDisk;
    HbAction *mDiskAdminScanDrive;
    HbAction *mDiskAdminSetDriveName;
    HbAction *mDiskAdminSetDriveVolumeLabel;
    HbAction *mDiskAdminEjectDrive;
    HbAction *mDiskAdminDismountDrive;
    HbAction *mDiskAdminEraseMBR;
    HbAction *mDiskAdminPartitionDrive;
};

#endif // MENUACTION_H

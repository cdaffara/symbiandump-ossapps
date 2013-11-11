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

#ifndef FBDRIVEVIEWH_H_
#define FBDRIVEVIEWH_H_

#include "menuaction.h"
#include "enginewrapper.h"

#include <HbView>
#include <HbApplication>

#include <QModelIndexList>

// Forward declarations
class QFileInfo;
class QSignalMapper;
class QItemSelection;
class QString;
class QGraphicsLinearLayout;

class HbListView;
class HbListWidget;
class HbToolBar;
class HbLabel;
class HbDialog;
class HbAbstractViewItem;
class HbMenu;
class HbSearchPanel;

class EditorView;
class SearchView;
class SettingsView;
class EngineWrapper;
class FbDriveModel;
class FileBrowserSortFilterProxyModel;

class FbDriveView : public HbView
{
    Q_OBJECT

public:
    explicit FbDriveView();
    virtual ~FbDriveView();
    void init(EngineWrapper *engineWrapper);

public slots:
    void refreshList();

private:
    void openListDialog(const QStringList &items, const QString &titleText, QObject *receiver, const char *member);

    void openPropertyDialog(const QStringList &propertyList, const QString &title);

    QModelIndex currentItemIndex();
    void storeSelectedItemsOrCurrentItem();

    // Menu related methods
    void createMenu();
    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createToolsMenu();

    void createSettingsMenuItem();
    void createAboutMenuItem();
    void createExitMenuItem();

    void createContextMenu();
    void createFileContextMenu();
    void createEditContextMenu();
    void createViewContextMenu();
    void createDiskAdminContextMenu();

private slots:
    // menu action slots
    // file menu
    void fileOpenDrive();
    void fileSearch();

    void fileProperties();

    void fileSetAttributes();

    // edit menu
    void editSnapShotToE();

    // view menu
    void viewRefresh();

    // disk admin menu
    void diskAdminSetDrivePassword();
    void diskAdminSetDrivePasswordNew(HbAction *);
    void doDiskAdminSetDrivePassword(HbAction *);

    void diskAdminUnlockDrive();
    void doDiskAdminUnlockDrive(HbAction *);

    void diskAdminClearDrivePassword();
    void doDiskAdminClearDrivePassword(HbAction *);

    void diskAdminEraseDrivePassword();
    void doDiskAdminEraseDrivePassword(int);

    void diskAdminFormatDrive();
    void doDiskAdminFormatDrive(int);

    void diskAdminQuickFormatDrive();
    void doDiskAdminQuickFormatDrive(int);

    void diskAdminCheckDisk();

    void diskAdminScanDrive();
    void doDiskAdminScanDrive(int);

    void diskAdminSetDriveName();
    void doDiskAdminSetDriveName(HbAction *);

    void diskAdminSetDriveVolumeLabel();
    void doDiskAdminSetDriveVolumeLabel(HbAction *);

    void diskAdminEjectDrive();
    void diskAdminDismountDrive();
    void doDiskAdminDismountDrive(int);

    void diskAdminEraseMBR();
    void doDiskAdminEraseMBR(int);
    void doDiskAdminReallyEraseMBR(int);
    void doDiskAdminNotRemovableReallyEraseMBR(int);
    
    void diskAdminPartitionDrive();
    void diskAdminPartitionDriveProceed(int);
    void diskAdminPartitionDriveReallyProceed(int);
    void diskAdminPartitionDriveIsNotRemovable(int);
    void diskAdminPartitionDriveEraseMbr(int);
    void diskAdminPartitionDriveGetCount(HbAction *);

    // tools menu
    void toolsAllAppsToTextFile();
    void toolsAllFilesToTextFile();
//    void toolsAvkonIconCacheEnable();
//    void toolsAvkonIconCacheDisable();

    void toolsDisableExtendedErrors();
    void toolsDumpMsgStoreWalk();
    void toolsEditDataTypes();
    void toolsEnableExtendedErrors();

    void toolsErrorSimulateLeave();
    void doToolsErrorSimulateLeave(HbAction *);

    void toolsErrorSimulatePanic();
    void doToolsErrorSimulatePanicCode(HbAction *);
    void doToolsErrorSimulatePanic(HbAction *);

    void toolsErrorSimulateException();
    void doToolsErrorSimulateException(HbAction *);

//    void toolsLocalConnectivityActivateInfrared();
//    void toolsLocalConnectivityLaunchBTUI();
//    void toolsLocalConnectivityLaunchUSBUI();
    void toolsMessageInbox();
    void toolsMessageDrafts();
    void toolsMessageSentItems();
    void toolsMessageOutbox();
    void toolsMemoryInfo();
    void toolsSecureBackStart();
    void toolsSecureBackRestore();
    void toolsSecureBackStop();
    void toolsSetDebugMaskQuestion();
    void toolsSetDebugMask(HbAction *);
    void toolsShowOpenFilesHere();

    // main menu items
    void about();
    
signals:
    void aboutToShowFileView();
    void aboutToShowSettingsView();
    void aboutToShowEditorView(const QString &, bool);
    void aboutToShowSearchView(const QString &);
    void aboutToSimulateLeave(int);

private slots:
    void updateOptionMenu();
    void updateContextMenu();
    void selectionChanged(const QItemSelection &, const QItemSelection &);
    void activated(const QModelIndex& index);

    void onLongPressed(HbAbstractViewItem *, QPointF);

private:
    EngineWrapper *mEngineWrapper;

    HbListView *mListView;
    HbLabel *mNaviPane;

    QGraphicsLinearLayout *mMainLayout;

    // file info contains all needed information of selected file from file model
    QModelIndexList mClipboardIndexes;
    QModelIndexList mSelectionIndexes;

    FbDriveModel *mFbDriveModel;
    OptionMenuActions mOptionMenuActions;
    ContextMenuActions mContextMenuActions;
    HbMenu *mContextMenu;

    // flags
    bool mLocationChanged;
    // flag for removing source file after copied to target file
    bool mRemoveFileAfterCopied;
    bool mClipBoardInUse;
    bool mFolderContentChanged;
    QModelIndex mCurrentIndex;

    // temporarily storage
    QString mOldPassword;
    QString mPanicCategory;
    QString mAbsoluteFilePath;
    OverwriteOptions mOverwriteOptions;
    QModelIndex mModelIndex;
    QString mNewFileName;
    bool mProceed;
    bool mEraseMBR;
};



#endif /* FBDRIVEVIEWH_H_ */

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

#include "fbdriveview.h"
#include "fbsettingsview.h"
#include "fbeditorview.h"
#include "fbsearchview.h"
#include "enginewrapper.h"
#include "notifications.h"

#include "fbdrivemodel.h"
#include "filebrowsersortfilterproxymodel.h"
#include "fbdrivelistviewitem.h"

#include <HbMenu>
#include <HbPopup>
#include <HbView>
#include <HbMessageBox>
#include <HbAction>
#include <HbLabel>
#include <HbListView>
#include <HbListViewItem>
#include <HbListWidget>
#include <HbLineEdit>
#include <HbAbstractViewItem>
#include <HbSelectionDialog>
#include <HbValidator>
#include <HbInputDialog>

#include <QString>
#include <QGraphicsLinearLayout>
#include <QItemSelection>
#include <QDebug>
//TODO check if needed to do this way
#include <FB.hrh>

// ---------------------------------------------------------------------------

FbDriveView::FbDriveView() :
    mEngineWrapper(0),
    mListView(0),
    mNaviPane(0),
    mMainLayout(0),
    mFbDriveModel(0),
    mOptionMenuActions(),
    mContextMenuActions(),
    mContextMenu(0),
    mLocationChanged(false),
    mRemoveFileAfterCopied(false),
//    mClipBoardInUse(false),
    mFolderContentChanged(false),
    mCurrentIndex(),
    mOldPassword(),
    mPanicCategory(),
    mAbsoluteFilePath(),
    mOverwriteOptions(),
    mModelIndex(),
    mNewFileName(),
    mProceed(false),
    mEraseMBR(false)
{
    setTitle("File Browser");

    createMenu();
    createContextMenu();
}

// ---------------------------------------------------------------------------	

void FbDriveView::init(EngineWrapper *engineWrapper)
{
    mEngineWrapper = engineWrapper;

    mListView = new HbListView(this);
    mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);

    mListView->setItemPrototype(new FbDriveListViewItem(mListView));

    mFbDriveModel = new FbDriveModel(mEngineWrapper);
    if (!mListView->model()) {
        mEngineWrapper->refreshView();
        mListView->setModel(mFbDriveModel);
    }

    //mListView->setRootIndex(mFileSystemModel->index(startPath));
    //mListView->setRootIndex(model->index());

//    mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);

    connect(mListView, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));
    connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)),
            this, SLOT(onLongPressed(HbAbstractViewItem*, QPointF)));

    mNaviPane = new HbLabel(this);
    mNaviPane->setPlainText(QString(" ")); // TODO get from settings or default
    //mNaviPane->setPlainText(QString(mEngineWrapper->currentPath()));
    HbFontSpec fontSpec(HbFontSpec::PrimarySmall);
    mNaviPane->setFontSpec(fontSpec);

    // Create layout and add list view and naviPane into layout:
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mMainLayout->addItem(mNaviPane);
    mMainLayout->addItem(mListView);
    setLayout(mMainLayout);
}

// ---------------------------------------------------------------------------

FbDriveView::~FbDriveView()
{  
//    if (mEngineWrapper) {
//        delete mEngineWrapper;
//    }
    if (mContextMenu) {
        mContextMenu->deleteLater();
    }

    delete mFbDriveModel;
    delete mListView;
}

/**
  Initial setup for options menu.
  Dynamic menu update during the runtime is performed by updateOptionMenu() which
  to menu's aboutToShow() signal.
  */
void FbDriveView::createMenu()
{
    createFileMenu();
    createEditMenu();
    createViewMenu();
    createToolsMenu();

    createSettingsMenuItem();
    createAboutMenuItem();
    createExitMenuItem();

    // menu dynamic update
    connect(menu(), SIGNAL(aboutToShow()), this, SLOT(updateOptionMenu()));
}

/**
  Initial setup for File submenu
  */
void FbDriveView::createFileMenu()
{
    mOptionMenuActions.mFileMenu = menu()->addMenu("File");

    mOptionMenuActions.mFileSearch = mOptionMenuActions.mFileMenu->addAction("Search...", this, SLOT(fileSearch()));
    //mOptionMenuActions.mFileSearch->setVisible(false);

    //mOptionMenuActions.mFileNewMenu = mOptionMenuActions.mFileMenu->addMenu("New");

    mOptionMenuActions.mFileSetAttributes = mOptionMenuActions.mFileMenu->addAction("Set attributes...", this, SLOT(fileSetAttributes()));
    mOptionMenuActions.mFileSetAttributes->setVisible(false);
}

/**
  Initial setup for Edit submenu
  */
void FbDriveView::createEditMenu()
{
    mOptionMenuActions.mEditMenu = menu()->addMenu("Edit");

    mOptionMenuActions.mEditSnapShotToE = mOptionMenuActions.mEditMenu->addAction("Snap shot to E:", this, SLOT(editSnapShotToE()));
    mOptionMenuActions.mEditSnapShotToE->setVisible(false);
}

/**
  Initial setup for View submenu
  */
void FbDriveView::createViewMenu()
{
    mOptionMenuActions.mViewMenu = menu()->addMenu("View");
    //mOptionMenuActions.mViewMenu->menuAction()->setVisible(false);

    //mOptionMenuActions.mViewFilterEntries = mOptionMenuActions.mViewMenu->addAction("Filter entries", this, SLOT(viewFilterEntries()));
    mOptionMenuActions.mViewRefresh = mOptionMenuActions.mViewMenu->addAction("Refresh", this, SLOT(viewRefresh()));
}

/**
  Initial setup for Tools submenu
  */
void FbDriveView::createToolsMenu()
{
    mOptionMenuActions.mToolsMenu = menu()->addMenu("Tools");

    mOptionMenuActions.mToolsAllAppsToTextFile = mOptionMenuActions.mToolsMenu->addAction("All apps to a text file", this, SLOT(toolsAllAppsToTextFile()));
    mOptionMenuActions.mToolsAllAppsToTextFile->setVisible(false);
    mOptionMenuActions.mToolsAllFilesToTextFile = mOptionMenuActions.mToolsMenu->addAction("All files to a text file", this, SLOT(toolsAllFilesToTextFile()));
    //mOptionMenuActions.mToolsAllFilesToTextFile->setVisible(false);

//    mOptionMenuActions.mToolsAvkonIconCacheMenu = mOptionMenuActions.mToolsMenu->addMenu("Avkon icon cache");
//    mOptionMenuActions.mToolsAvkonIconCacheMenu->menuAction()->setVisible(false);
//    mOptionMenuActions.mToolsAvkonIconCacheEnable = mOptionMenuActions.mToolsAvkonIconCacheMenu->addAction("Enable", this, SLOT(toolsAvkonIconCacheEnable()));
//    mOptionMenuActions.mToolsAvkonIconCacheDisable = mOptionMenuActions.mToolsAvkonIconCacheMenu->addAction("Clear and disable", this, SLOT(toolsAvkonIconCacheDisable()));

    mOptionMenuActions.mToolsDisableExtendedErrors = mOptionMenuActions.mToolsMenu->addAction("Disable extended errors", this, SLOT(toolsDisableExtendedErrors()));
    mOptionMenuActions.mToolsDumpMsgStoreWalk = mOptionMenuActions.mToolsMenu->addAction("Dump msg. store walk", this, SLOT(toolsDumpMsgStoreWalk()));
    mOptionMenuActions.mToolsDumpMsgStoreWalk->setVisible(false);
    mOptionMenuActions.mToolsEditDataTypes = mOptionMenuActions.mToolsMenu->addAction("Edit data types", this, SLOT(toolsEditDataTypes()));
    mOptionMenuActions.mToolsEditDataTypes->setVisible(false);
    mOptionMenuActions.mToolsEnableExtendedErrors = mOptionMenuActions.mToolsMenu->addAction("Enable extended errors", this, SLOT(toolsEnableExtendedErrors()));

    mOptionMenuActions.mToolsErrorSimulateMenu = mOptionMenuActions.mToolsMenu->addMenu("Error simulate");
    mOptionMenuActions.mToolsErrorSimulateLeave = mOptionMenuActions.mToolsErrorSimulateMenu->addAction("Leave", this, SLOT(toolsErrorSimulateLeave()));
    mOptionMenuActions.mToolsErrorSimulatePanic = mOptionMenuActions.mToolsErrorSimulateMenu->addAction("Panic", this, SLOT(toolsErrorSimulatePanic()));
    mOptionMenuActions.mToolsErrorSimulatePanic->setVisible(false);
    mOptionMenuActions.mToolsErrorSimulateException = mOptionMenuActions.mToolsErrorSimulateMenu->addAction("Exception", this, SLOT(toolsErrorSimulateException()));

//    mOptionMenuActions.mLocalConnectivityMenu = mOptionMenuActions.mToolsMenu->addMenu("Local connectivity");
//    mOptionMenuActions.mToolsLocalConnectivityActivateInfrared = mOptionMenuActions.mLocalConnectivityMenu->addAction("Activate infrared", this, SLOT(toolsLocalConnectivityActivateInfrared()));
//    mOptionMenuActions.mToolsLocalConnectivityLaunchBTUI = mOptionMenuActions.mLocalConnectivityMenu->addAction("Launch BT UI", this, SLOT(toolsLocalConnectivityLaunchBTUI()));
//    mOptionMenuActions.mToolsLocalConnectivityLaunchUSBUI = mOptionMenuActions.mLocalConnectivityMenu->addAction("Launch USB UI", this, SLOT(toolsLocalConnectivityLaunchUSBUI()));

    mOptionMenuActions.mToolsMessageAttachmentsMenu = mOptionMenuActions.mToolsMenu->addMenu("Message attachments");
    mOptionMenuActions.mToolsMessageAttachmentsMenu->menuAction()->setVisible(false);
    mOptionMenuActions.mToolsMessageInbox = mOptionMenuActions.mToolsMessageAttachmentsMenu->addAction("Inbox", this, SLOT(toolsMessageInbox()));
    mOptionMenuActions.mToolsMessageDrafts = mOptionMenuActions.mToolsMessageAttachmentsMenu->addAction("Drafts", this, SLOT(toolsMessageDrafts()));
    mOptionMenuActions.mToolsMessageSentItems = mOptionMenuActions.mToolsMessageAttachmentsMenu->addAction("Sent items", this, SLOT(toolsMessageSentItems()));
    mOptionMenuActions.mToolsMessageOutbox = mOptionMenuActions.mToolsMessageAttachmentsMenu->addAction("Outbox", this, SLOT(toolsMessageOutbox()));

    mOptionMenuActions.mToolsMemoryInfo = mOptionMenuActions.mToolsMenu->addAction("Memory info", this, SLOT(toolsMemoryInfo()));
    mOptionMenuActions.mToolsMemoryInfo->setVisible(false);

    mOptionMenuActions.mToolsSecureBackupMenu = mOptionMenuActions.mToolsMenu->addMenu("Secure backup");
    mOptionMenuActions.mToolsSecureBackupMenu->menuAction()->setVisible(false);
    mOptionMenuActions.mToolsSecureBackStart = mOptionMenuActions.mToolsSecureBackupMenu->addAction("Start backup", this, SLOT(toolsSecureBackStart()));
    mOptionMenuActions.mToolsSecureBackRestore = mOptionMenuActions.mToolsSecureBackupMenu->addAction("Start restore", this, SLOT(toolsSecureBackRestore()));
    mOptionMenuActions.mToolsSecureBackStop = mOptionMenuActions.mToolsSecureBackupMenu->addAction("Stop", this, SLOT(toolsSecureBackStop()));

    mOptionMenuActions.mToolsSetDebugMask = mOptionMenuActions.mToolsMenu->addAction("Set debug mask", this, SLOT(toolsSetDebugMaskQuestion()));
    mOptionMenuActions.mToolsShowOpenFilesHere = mOptionMenuActions.mToolsMenu->addAction("Show open files here", this, SLOT(toolsShowOpenFilesHere()));
    mOptionMenuActions.mToolsShowOpenFilesHere->setVisible(false);
}

/**
  Creates Setting menu item in option menu
  */
void FbDriveView::createSettingsMenuItem()
{
    mOptionMenuActions.mSetting = menu()->addAction("Settings...");
    connect(mOptionMenuActions.mSetting, SIGNAL(triggered()), this, SIGNAL(aboutToShowSettingsView()));
}


/**
  Creates About menu item in option menu
  */
void FbDriveView::createAboutMenuItem()
{
    // about note
    mOptionMenuActions.mAbout = menu()->addAction("About");
    connect(mOptionMenuActions.mAbout, SIGNAL(triggered()), this, SLOT(about()));
}

/**
  Creates Exit menu item in option menu
  */
void FbDriveView::createExitMenuItem()
{
    // application exit
    mOptionMenuActions.mExit = menu()->addAction("Exit");
    connect(mOptionMenuActions.mExit, SIGNAL(triggered()), qApp, SLOT(quit()));
}

/**
  update menu: disk admin available only in device root view. edit available only in folder view
  when file or folder content exist in current folder, or clipboard has copied item.
  file and view menus updated every time regarding the folder content.
  tools, settings, about, exit always available.
  If there's remove and add operations at same time, always remove first
  to keep to the correct menu items order.
  */
void FbDriveView::updateOptionMenu()
{
    bool isFileItemListEmpty = mFbDriveModel->rowCount() == 0;
//    bool isDriveListViewActive = true; //mEngineWrapper->isDriveListViewActive();
    bool isNormalModeActive = true;       //iModel->FileUtils()->IsNormalModeActive();
//    bool isCurrentDriveReadOnly = mEngineWrapper->isCurrentDriveReadOnly();   //iModel->FileUtils()->IsCurrentDriveReadOnly();
//    bool isCurrentItemDirectory = mEngineWrapper->getFileEntry(currentItemIndex()).isDir();
    // bool currentSelected = true;    //iContainer->ListBox()->View()->ItemIsSelected(iContainer->ListBox()->View()->CurrentItemIndex());
//    bool isAllSelected = mListView->selectionModel()->selection().count() == mFbDriveModel->rowCount();
    //bool isNoneSelected = mListView->selectionModel()->selection().count() != 0;
//    bool hasSelectedItems = mListView->selectionModel()->selection().count() != 0;
    //bool isSelectionMode = mOptionMenuActions.mSelection && mOptionMenuActions.mSelection->isChecked();
    bool isClipBoardEmpty = !mEngineWrapper->isClipBoardListInUse();
    bool showSnapShot = false;           //iModel->FileUtils()->DriveSnapShotPossible();

    bool showEditMenu(true);
    if (!showSnapShot || isFileItemListEmpty && isClipBoardEmpty)
        showEditMenu = false;
    else
        showEditMenu = true;

    mOptionMenuActions.mEditMenu->menuAction()->setVisible(showEditMenu);

    //aMenuPane->SetItemDimmed(EFileBrowserCmdFileOpen, isFileItemListEmpty || isDriveListViewActive || isCurrentItemDirectory);

    //aMenuPane->SetItemDimmed(EFileBrowserCmdFileView, isFileItemListEmpty || !hasSelectedItems || isCurrentItemDirectory || isDriveListViewActive);
    //aMenuPane->SetItemDimmed(EFileBrowserCmdFileEdit, isFileItemListEmpty || !hasSelectedItems || isCurrentItemDirectory || isDriveListViewActive);
    //aMenuPane->SetItemDimmed(EFileBrowserCmdFileSendTo, isFileItemListEmpty || driveListActive || isCurrentItemDirectory);

    // TODO mOptionMenuActions.mFileCompress->setVisible(!(isCurrentDriveReadOnly || isFileItemListEmpty || !hasSelectedItems || isCurrentItemDirectory || isDriveListViewActive));
    // TODO mOptionMenuActions.mFileDecompress->setVisible(!(isCurrentDriveReadOnly || isFileItemListEmpty || !hasSelectedItems || isCurrentItemDirectory || isDriveListViewActive));

    // TODO mOptionMenuActions.mEditSnapShotToE->setVisible(isDriveListViewActive);

    // TODO mOptionMenuActions.mViewSort->setVisible(!(!isNormalModeActive || isDriveListViewActive || isFileItemListEmpty));
    // TODO mOptionMenuActions.mViewOrder->setVisible(!(!isNormalModeActive || isDriveListViewActive || isFileItemListEmpty));
    mOptionMenuActions.mViewRefresh->setVisible(isNormalModeActive);
    //mOptionMenuActions.mViewFilterEntries->setVisible(!isFileItemListEmpty);

    // TODO R_FILEBROWSER_VIEW_SORT_SUBMENU
    // aMenuPane->SetItemButtonState(iModel->FileUtils()->SortMode(), EEikMenuItemSymbolOn);

    // TODO R_FILEBROWSER_VIEW_ORDER_SUBMENU
    // aMenuPane->SetItemButtonState(iModel->FileUtils()->OrderMode(), EEikMenuItemSymbolOn);

    // aResourceId == R_FILEBROWSER_TOOLS_SUBMENU
    bool noExtendedErrorsAllowed = mEngineWrapper->ErrRdFileExists();
    mOptionMenuActions.mToolsDisableExtendedErrors->setVisible(noExtendedErrorsAllowed);
    mOptionMenuActions.mToolsEnableExtendedErrors->setVisible(!noExtendedErrorsAllowed);

//    bool infraRedAllowed = mEngineWrapper->FileExists(KIRAppPath);
//    bool bluetoothAllowed = mEngineWrapper->FileExists(KBTAppPath);
//    bool usbAllowed = mEngineWrapper->FileExists(KUSBAppPath);
//
//    bool noLocalCon = !infraRedAllowed && !bluetoothAllowed && !usbAllowed;
//    mOptionMenuActions.mToolsLocalConnectivityMenu->menuAction()->setVisible(!noLocalCon);
//
//    mOptionMenuActions.mToolsLocalConnectivityActivateInfrared->setVisible(infraRedAllowed);
//    mOptionMenuActions.mToolsLocalConnectivityLaunchBTUI->setVisible(bluetoothAllowed);
//    mOptionMenuActions.mToolsLocalConnectivityLaunchUSBUI->setVisible(usbAllowed);
}

void FbDriveView::createContextMenu()
{
    mContextMenu = new HbMenu();
    connect(mContextMenu, SIGNAL(aboutToShow()), this, SLOT(updateContextMenu()));

    createFileContextMenu();
    createEditContextMenu();
    createViewContextMenu();
    createDiskAdminContextMenu();
}


void FbDriveView::createFileContextMenu()
{
    //mContextMenuActions.mFileMenu = mContextMenu->addMenu("File");

    //mContextMenuActions.mFileOpenDrive = mContextMenuActions.mFileMenu->addAction("Open drive (->)", this, SLOT(fileOpenDrive()));
//    mContextMenuActions.mFileSearch = mContextMenuActions.mFileMenu->addAction("Search...", this, SLOT(fileSearch()));
    //mContextMenuActions.mFileSearch->setVisible(false);

    //mContextMenuActions.mFileProperties = mContextMenuActions.mFileMenu->addAction("Properties", this, SLOT(fileProperties()));
    mContextMenuActions.mFileProperties = mContextMenu->addAction("Properties", this, SLOT(fileProperties()));
    mContextMenuActions.mFileSearch = mContextMenu->addAction("Search...", this, SLOT(fileSearch()));

//    mContextMenuActions.mFileSetAttributes = mContextMenuActions.mFileMenu->addAction("Set attributes...", this, SLOT(fileSetAttributes()));
//    mContextMenuActions.mFileSetAttributes->setVisible(false);
}

void FbDriveView::createEditContextMenu()
{
//    mContextMenuActions.mEditMenu = mContextMenu->addMenu("Edit");

    //mContextMenuActions.mEditSnapShotToE = mContextMenuActions.mEditMenu->addAction("Snap shot to E:", this, SLOT(editSnapShotToE()));
//    mContextMenuActions.mEditSnapShotToE->setVisible(false);
//    mContextMenuActions.mEditCut = mContextMenuActions.mEditMenu->addAction("Cut", this, SLOT(editCut()));
//    mContextMenuActions.mEditCopy = mContextMenuActions.mEditMenu->addAction("Copy", this, SLOT(editCopy()));
//    mContextMenuActions.mEditPaste = mContextMenuActions.mEditMenu->addAction("Paste", this, SLOT(editPaste()));
//    mContextMenuActions.mEditCopyToFolder = mContextMenuActions.mEditMenu->addAction("Copy to folder...", this, SLOT(editCopyToFolder()));
//    mContextMenuActions.mEditMoveToFolder = mContextMenuActions.mEditMenu->addAction("Move to folder...", this, SLOT(editMoveToFolder()));
}

void FbDriveView::createViewContextMenu()
{

}

/**
  Initial setup for Disk Admin submenu
  */
void FbDriveView::createDiskAdminContextMenu()
{
    mContextMenuActions.mDiskAdminMenu = mContextMenu->addMenu("Disk admin");
    //mContextMenuActions.mDiskAdminMenu->menuAction()->setVisible(false);

    mContextMenuActions.mDiskAdminSetDrivePassword = mContextMenuActions.mDiskAdminMenu->addAction("Set drive password", this, SLOT(diskAdminSetDrivePassword()));
    mContextMenuActions.mDiskAdminUnlockDrive = mContextMenuActions.mDiskAdminMenu->addAction("Unlock drive", this, SLOT(diskAdminUnlockDrive()));
    mContextMenuActions.mDiskAdminClearDrivePassword = mContextMenuActions.mDiskAdminMenu->addAction("Clear drive password", this, SLOT(diskAdminClearDrivePassword()));
    mContextMenuActions.mDiskAdminEraseDrivePassword = mContextMenuActions.mDiskAdminMenu->addAction("Erase drive password", this, SLOT(diskAdminEraseDrivePassword()));

    mContextMenuActions.mDiskAdminFormatDrive = mContextMenuActions.mDiskAdminMenu->addAction("Format drive", this, SLOT(diskAdminFormatDrive()));
    mContextMenuActions.mDiskAdminFormatDrive->setVisible(false);
    mContextMenuActions.mDiskAdminQuickFormatDrive = mContextMenuActions.mDiskAdminMenu->addAction("Quick format drive", this, SLOT(diskAdminQuickFormatDrive()));
    mContextMenuActions.mDiskAdminQuickFormatDrive->setVisible(false);

    mContextMenuActions.mDiskAdminCheckDisk = mContextMenuActions.mDiskAdminMenu->addAction("Check disk", this, SLOT(diskAdminCheckDisk()));
    mContextMenuActions.mDiskAdminScanDrive = mContextMenuActions.mDiskAdminMenu->addAction("Scan drive", this, SLOT(diskAdminScanDrive()));
    mContextMenuActions.mDiskAdminSetDriveName = mContextMenuActions.mDiskAdminMenu->addAction("Set drive name", this, SLOT(diskAdminSetDriveName()));
    mContextMenuActions.mDiskAdminSetDriveVolumeLabel = mContextMenuActions.mDiskAdminMenu->addAction("Set drive volume label", this, SLOT(diskAdminSetDriveVolumeLabel()));
    mContextMenuActions.mDiskAdminEjectDrive = mContextMenuActions.mDiskAdminMenu->addAction("Eject drive", this, SLOT(diskAdminEjectDrive()));
    mContextMenuActions.mDiskAdminDismountDrive = mContextMenuActions.mDiskAdminMenu->addAction("Dismount drive", this, SLOT(diskAdminDismountDrive()));
    mContextMenuActions.mDiskAdminEraseMBR = mContextMenuActions.mDiskAdminMenu->addAction("Erase MBR", this, SLOT(diskAdminEraseMBR()));
    mContextMenuActions.mDiskAdminPartitionDrive = mContextMenuActions.mDiskAdminMenu->addAction("Partition drive", this, SLOT(diskAdminPartitionDrive()));
}

void FbDriveView::updateContextMenu()
{
    bool isFileItemListEmpty = mFbDriveModel->rowCount() == 0;
    mContextMenuActions.mFileProperties->setVisible(!isFileItemListEmpty);
}

// ---------------------------------------------------------------------------

void FbDriveView::onLongPressed(HbAbstractViewItem *listViewItem, QPointF coords)
{
    mCurrentIndex = listViewItem->modelIndex();

    mContextMenu->setPreferredPos(coords);
    mContextMenu->show();
}

/**
  Refresh FileBrowser view
  */
void FbDriveView::refreshList()
{
    mEngineWrapper->refreshView();
    mListView->reset();

    TListingMode listingMode = mEngineWrapper->listingMode();
    if (listingMode == ENormalEntries)
        mNaviPane->setPlainText(QString(mEngineWrapper->currentPath()));
    else if (listingMode == ESearchResults)
        mNaviPane->setPlainText(QString(tr("Search results")));
    else if (listingMode == EOpenFiles)
        mNaviPane->setPlainText(QString(tr("Open files")));
    else if (listingMode == EMsgAttachmentsInbox)
        mNaviPane->setPlainText(QString(tr("Attachments in Inbox")));
    else if (listingMode == EMsgAttachmentsDrafts)
        mNaviPane->setPlainText(QString(tr("Attachments in Drafts")));
    else if (listingMode == EMsgAttachmentsSentItems)
        mNaviPane->setPlainText(QString(tr("Attachments in Sent Items")));
    else if (listingMode == EMsgAttachmentsOutbox)
        mNaviPane->setPlainText(QString(tr("Attachments in Outbox")));
}

// ---------------------------------------------------------------------------
/**
  Show a list dialog
  \param List aList of item to select item from.
  \param Title text titleText of a dialog heading widget
  \return None
  */
void FbDriveView::openListDialog(const QStringList& items, const QString &titleText, QObject* receiver, const char* member)
{
    // Create a list and some simple content for it
    HbSelectionDialog *dlg = new HbSelectionDialog();
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    // Set items to be popup's content
    dlg->setStringItems(items);
    dlg->setSelectionMode(HbAbstractItemView::SingleSelection);

    HbLabel *title = new HbLabel(dlg);
    title->setPlainText(titleText);
    dlg->setHeadingWidget(title);

    // Launch popup and handle the user response:
    dlg->open(receiver, member);
}

// ---------------------------------------------------------------------------

void FbDriveView::openPropertyDialog(const QStringList& propertyList, const QString& title)
{
    HbDialog *dialog = new HbDialog();
    dialog->setDismissPolicy(HbPopup::TapOutside);
    dialog->setTimeout(HbPopup::NoTimeout);

    HbLabel *titleWidget = new HbLabel();
    titleWidget->setPlainText(title);
    dialog->setHeadingWidget(titleWidget);

    // Create a list and some simple content for it
    HbListWidget *list = new HbListWidget();
    QString str;
    foreach (str, propertyList) {
        list->addItem(str);
    }

    // Connect list item activation signal to close the popup
    connect(list, SIGNAL(activated(HbListWidgetItem*)), dialog, SLOT(close()));

    HbAction *cancelAction = new HbAction("Close");
    dialog->setPrimaryAction(cancelAction);

    // Set listwidget to be popup's content
    dialog->setContentWidget(list);
    // Launch popup and handle the user response:
    dialog->open();
}

void FbDriveView::storeSelectedItemsOrCurrentItem()
{
    QItemSelectionModel *selectionIndexes = mListView->selectionModel();

    // by default use selected items
    if (selectionIndexes) {
        if (selectionIndexes->hasSelection()) {
            mSelectionIndexes = mListView->selectionModel()->selectedIndexes();
        } else { // or if none selected, use the current item index
            mSelectionIndexes.clear();
            mSelectionIndexes.append(mCurrentIndex);
//            QModelIndex currentIndex = currentItemIndex();
//            if (mFbDriveModel->rowCount(currentItemIndex) > currentItemIndex && currentItemIndex >= 0)
//            {
//                modelIndexList.append(currentIndex);
//            }
        }
    }
//    mClipBoardInUse = true;
}

// ---------------------------------------------------------------------------

QModelIndex FbDriveView::currentItemIndex()
{
    return mCurrentIndex;//mListView->selectionModel()->currentIndex();
}

// ---------------------------------------------------------------------------
// operations in File Menu
// ---------------------------------------------------------------------------

void FbDriveView::fileOpenDrive()
{
    // TODO make a separate function to be called from here and fileOpenDirectory()
    mLocationChanged = true;
    // get selected drive or directory from list view model and open it:
    //if (mListView->selectionModel()->hasSelection()) {
//    if (mListView->selectionModel()->selection().count() != 0) {
//        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->moveDownToDirectory(mCurrentIndex);
        mListView->setRootIndex(mCurrentIndex);
        refreshList();
//    } else {
//        Notifications::showErrorNote("not selected item!");
//    }
}

void FbDriveView::fileSearch()
{
    QString searchPath;
    HbAction *contextrMenuAction = static_cast<HbAction *>(sender());
    if (contextrMenuAction
        && contextrMenuAction->text().compare(QString("Search..."), Qt::CaseInsensitive) == 0
        && contextrMenuAction == mContextMenuActions.mFileSearch) {
        FbDriveEntry driveEntry(mEngineWrapper->getDriveEntry(currentItemIndex()));
        searchPath = driveEntry.driveLetter() + QString(":\\");
    } else {
        searchPath = mEngineWrapper->currentPath();
    }
    emit aboutToShowSearchView(searchPath);
}

/**
  Show file properties
  */
void FbDriveView::fileProperties()
{
    QModelIndex currentIndex = currentItemIndex();
    QStringList propertyList;
    QString titleText;
    mEngineWrapper->properties(currentIndex, propertyList, titleText);
    openPropertyDialog(propertyList, titleText);
}

void FbDriveView::fileSetAttributes()
{

}

// edit menu
void FbDriveView::editSnapShotToE()
{

}

// ---------------------------------------------------------------------------
// view menu
// ---------------------------------------------------------------------------

/**
  Refresh view
  */
void FbDriveView::viewRefresh()
{
    refreshList();
}

// ---------------------------------------------------------------------------
// disk admin menu
// ---------------------------------------------------------------------------

/**
  Open old password for the selected drive dialog.
  */
void FbDriveView::diskAdminSetDrivePassword()
{
    QModelIndex currentIndex = currentItemIndex();
    // check if the drive has a password
    if (mEngineWrapper->hasDrivePassword(currentIndex)) {
        QString heading = QString("Existing password");
        HbInputDialog::queryText(heading, this, SLOT(diskAdminSetDrivePasswordNew(HbAction*)), QString(), scene());
    } else {
        QString heading = QString("New password");
        HbInputDialog::queryText(heading, this, SLOT(doDiskAdminSetDrivePassword(HbAction*)), mOldPassword, scene());
    }
}

/**
   Open new password for the selected drive dialog.
  */
void FbDriveView::diskAdminSetDrivePasswordNew(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        mOldPassword = dlg->value().toString();
        QString heading = QString("New password");
        HbInputDialog::queryText(heading, this, SLOT(doDiskAdminSetDrivePassword(HbAction*)), mOldPassword, scene());
    }
}

/**
   Set password for the selected drive.
  */
void FbDriveView::doDiskAdminSetDrivePassword(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        QString newPassword = dlg->value().toString();
        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminSetDrivePassword(currentIndex, mOldPassword, newPassword);
        refreshList();
        mOldPassword = QString();
        newPassword = QString();
    }
}

/**
  Open Unlock the selected drive dialog.
  */
void FbDriveView::diskAdminUnlockDrive()
{
    QModelIndex currentIndex = currentItemIndex();
    // check if the drive is locked
    if (mEngineWrapper->isDriveLocked(currentIndex)) {
        QString heading = QString("Existing password");
        HbInputDialog::queryText(heading, this, SLOT(doDiskAdminUnlockDrive(HbAction*)), QString(), scene());
    } else {
        Notifications::showInformationNote(QString("This drive is not locked"));
    }
}

/**
  Unlock the selected drive.
  */
void FbDriveView::doDiskAdminUnlockDrive(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        QString oldPassword = dlg->value().toString();
        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminUnlockDrive(currentIndex, oldPassword);
        refreshList();
    }
}

/**
  Open clear password of the selected drive dialog.
  */
void FbDriveView::diskAdminClearDrivePassword()
{
    QModelIndex currentIndex = currentItemIndex();
    // check if the drive has a password
    if (mEngineWrapper->hasDrivePassword(currentIndex)) {
        QString heading = QString("Existing password");
        HbInputDialog::queryText(heading, this, SLOT(doDiskAdminClearDrivePassword(HbAction*)), QString(), scene());
    } else {
        Notifications::showInformationNote(QString("This drive has no password"));
    }
}

/**
  Clear password of the selected drive.
  */
void FbDriveView::doDiskAdminClearDrivePassword(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        QString oldPassword = dlg->value().toString();
        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminClearDrivePassword(currentIndex, oldPassword);
        refreshList();
    }
}


/**
  Question for erase password of the selected drive
  */
void FbDriveView::diskAdminEraseDrivePassword()
{
    // check if the drive has a password
    QModelIndex currentIndex = currentItemIndex();
    if (mEngineWrapper->hasDrivePassword(currentIndex)) {
        HbMessageBox::question(QString("Are you sure? All data can be lost!"),
                               this,
                               SLOT(doDiskAdminEraseDrivePassword(int)),
                               HbMessageBox::Yes | HbMessageBox::No);
    } else {
        Notifications::showInformationNote(QString("This drive has no password"));
    }
}

/**
  Erase password of the selected drive
  */
void FbDriveView::doDiskAdminEraseDrivePassword(int action)
{
    if (action == HbMessageBox::Yes) {
        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminEraseDrivePassword(currentIndex);
        refreshList();
    }
}

/**
  Performs format on the selected drive
  */
void FbDriveView::diskAdminFormatDrive()
{
    HbMessageBox::question(QString("Are you sure? All data will be lost!"),
                           this,
                           SLOT(doDiskAdminFormatDrive(int)),
                           HbMessageBox::Yes | HbMessageBox::No);
}

/**
  Performs format on the selected drive
  */
void FbDriveView::doDiskAdminFormatDrive(int action)
{
    if (action == HbMessageBox::Yes) {
        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminFormatDrive(currentIndex, false);
    }
}

/**
  Performs quick format on the selected drive
  */
void FbDriveView::diskAdminQuickFormatDrive()
{
    HbMessageBox::question(QString("Are you sure? All data will be lost!"),
                           this,
                           SLOT(doDiskAdminQuickFormatDrive(int)),
                           HbMessageBox::Yes | HbMessageBox::No);
}

/**
  Performs quick format on the selected drive
  */
void FbDriveView::doDiskAdminQuickFormatDrive(int action)
{
    if (action == HbMessageBox::Yes) {
        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminFormatDrive(currentIndex, true);
    }
}

/**
    Checks the disk integrity on the selected drive
  */
void FbDriveView::diskAdminCheckDisk()
{
    QModelIndex currentIndex = currentItemIndex();
    mEngineWrapper->DiskAdminCheckDisk(currentIndex);
}

/**
  Checks the selected drive for errors and corrects them
  */
void FbDriveView::diskAdminScanDrive()
{
    HbMessageBox::question(QString("This finds errors on disk and corrects them. Proceed?"),
                           this,
                           SLOT(doDiskAdminScanDrive(int)),
                           HbMessageBox::Yes | HbMessageBox::No);
}

/**
  Checks the selected drive for errors and corrects them
  */
void FbDriveView::doDiskAdminScanDrive(int action)
{
    if (action == HbMessageBox::Yes) {
        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminScanDrive(currentIndex);
        refreshList();
    }
}

/**
  Open drive name dialog
  */
void FbDriveView::diskAdminSetDriveName()
{
    QString driveName;

    // get existing drive name
    QModelIndex currentIndex = currentItemIndex();
    mEngineWrapper->GetDriveName(currentIndex, driveName);

    QString heading = QString("New name");
    HbInputDialog::queryText(heading, this, SLOT(doDiskAdminSetDriveName(HbAction*)), driveName, scene());
}

/**
  Set drive name.
  */
void FbDriveView::doDiskAdminSetDriveName(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        QString driveName = dlg->value().toString();

        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminSetDriveName(currentIndex, driveName);

        refreshList();
    }
}

/**
  Open drive volume label dialog
  */
void FbDriveView::diskAdminSetDriveVolumeLabel()
{
    QString volumeLabel;

    // get existing drive name
    QModelIndex currentIndex = currentItemIndex();
    mEngineWrapper->GetDriveVolumeLabel(currentIndex, volumeLabel);

    QString heading = QString("New volume label");
    HbInputDialog::queryText(heading, this, SLOT(doDiskAdminSetDriveVolumeLabel(HbAction*)), volumeLabel, scene());
}

/**
  Set drive volume label.
  */
void FbDriveView::doDiskAdminSetDriveVolumeLabel(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        QString volumeLabel = dlg->value().toString();

        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminSetDriveVolumeLabel(currentIndex, volumeLabel);

        refreshList();
    }
}

/**
  Eject the selected drive
  */
void FbDriveView::diskAdminEjectDrive()
{
    QModelIndex currentIndex = currentItemIndex();
    mEngineWrapper->DiskAdminEjectDrive(currentIndex);
    refreshList();
}

/**
  Dismount the selected drive
  */
void FbDriveView::diskAdminDismountDrive()
{
    HbMessageBox::question(QString("Are you sure you know what are you doing?"),
                           this,
                           SLOT(doDiskAdminDismountDrive(int)),
                           HbMessageBox::Yes | HbMessageBox::No);
}

void FbDriveView::doDiskAdminDismountDrive(int action)
{
    if (action == HbMessageBox::Yes) {
        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminDismountDrive(currentIndex);
        refreshList();
    }
}

/**
  Erase Master Boot Record of the selected drive
  */
void FbDriveView::diskAdminEraseMBR()
{
    // TODO What to do with FB LITE macros?
    HbMessageBox::question(QString("Are you sure? Your media driver must support this!"),
                           this,
                           SLOT(doDiskAdminEraseMBR(int)),
                           HbMessageBox::Yes | HbMessageBox::No);
}

void FbDriveView::doDiskAdminEraseMBR(int action)
{
    // TODO What to do with FB LITE macros?
    if (action == HbMessageBox::Yes) {
        HbMessageBox::question(QString("Are you really sure you know what are you doing ?!?"),
                               this,
                               SLOT(doDiskAdminReallyEraseMBR(int)),
                               HbMessageBox::Yes | HbMessageBox::No);
    }
}

void FbDriveView::doDiskAdminReallyEraseMBR(int action)
{
    if (action == HbMessageBox::Yes) {
        QModelIndex currentIndex = currentItemIndex();
        // warn if the selected drive is not detected as removable
        if (mEngineWrapper->isDriveRemovable(currentIndex)) {
            mEngineWrapper->DiskAdminEraseMBR(currentIndex);
            refreshList();
        } else {
            HbMessageBox::question(QString("Selected drive is not removable, really continue?"),
                                   this,
                                   SLOT(doDiskAdminNotRemovableReallyEraseMBR(int)),
                                   HbMessageBox::Yes | HbMessageBox::No);
        }
    }
}

void FbDriveView::doDiskAdminNotRemovableReallyEraseMBR(int action)
{
    if (action == HbMessageBox::Yes) {
        QModelIndex currentIndex = currentItemIndex();
        mEngineWrapper->DiskAdminEraseMBR(currentIndex);
        refreshList();
    }

}

/**
  Partition the selected drive
  */
void FbDriveView::diskAdminPartitionDrive()
{
    HbMessageBox::question(QString("Are you sure? Your media driver must support this!"),
                           this,
                           SLOT(diskAdminPartitionDriveProceed(int)),
                           HbMessageBox::Yes | HbMessageBox::No);
}

/**
  Partition the selected drive if user is sure
  */
void FbDriveView::diskAdminPartitionDriveProceed(int action)
{
    if (action == HbMessageBox::Yes) {
        HbMessageBox::question(QString("Are you really sure you know what are you doing ?!?"),
                               this,
                               SLOT(diskAdminPartitionDriveReallyProceed(int)),
                               HbMessageBox::Yes | HbMessageBox::No);
    }
}

/**
  Partition the selected drive if user is really sure
  */
void FbDriveView::diskAdminPartitionDriveReallyProceed(int action)
{
    if (action == HbMessageBox::Yes) {
        QModelIndex currentIndex = currentItemIndex();
        mEraseMBR = false;
        // warn if the selected drive is not detected as removable
        mProceed = false;
        if (mEngineWrapper->isDriveRemovable(currentIndex)) {
            mProceed = true;
        } else {
            HbMessageBox::question(QString("Selected drive is not removable, really continue?"),
                                   this,
                                   SLOT(diskAdminPartitionDriveIsNotRemovable(int)),
                                   HbMessageBox::Yes | HbMessageBox::No);
        }

        if (mProceed) {
            // query if erase mbr
            mEraseMBR = false;

            HbMessageBox::question(QString("Erase MBR first (normally needed)?"),
                                   this,
                                   SLOT(diskAdminPartitionDriveEraseMbr(int)),
                                   HbMessageBox::Yes | HbMessageBox::No);

            // TODO use HbListDialog
            QStringList list;
            list << "1" << "2" << "3" << "4";
            openListDialog(list, QString("Partitions?"), this, SLOT(diskAdminPartitionDriveGetCount(HbAction*)));
        }
    }
}

/**
  Store result of user query about proceeding when drive is not removable.
  */
void FbDriveView::diskAdminPartitionDriveIsNotRemovable(int action)
{
    if (action == HbMessageBox::Yes) {
        mProceed = true;
    } else {
        mProceed = false;
    }
}

/**
  Store result of user query about erase MBR
  */
void FbDriveView::diskAdminPartitionDriveEraseMbr(int action)
{
    if (action == HbMessageBox::Yes) {
        mEraseMBR = true;
    }
}

/**
  Partition the selected drive
  */
void FbDriveView::diskAdminPartitionDriveGetCount(HbAction* action)
{
    HbSelectionDialog *dlg = static_cast<HbSelectionDialog*>(sender());
    if(!action && dlg && dlg->selectedItems().count()){
        int selectionIndex = dlg->selectedItems().at(0).toInt();
        QModelIndex currentIndex = currentItemIndex();
        int amountOfPartitions = selectionIndex + 1;
        mEngineWrapper->DiskAdminPartitionDrive(currentIndex, mEraseMBR, amountOfPartitions);
        refreshList();
    }
}

// ---------------------------------------------------------------------------
// tools menu
// ---------------------------------------------------------------------------
void FbDriveView::toolsAllAppsToTextFile()
{

}

/**
  Write all files to text file
  */
void FbDriveView::toolsAllFilesToTextFile()
{
    mEngineWrapper->toolsWriteAllFiles();
}

//void FbDriveView::toolsAvkonIconCacheEnable()
//{
//
//}
//void FbDriveView::toolsAvkonIconCacheDisable()
//{
//
//}

/**
  Disable extended errors
  */
void FbDriveView::toolsDisableExtendedErrors()
{
    mEngineWrapper->ToolsSetErrRd(false);
}

void FbDriveView::toolsDumpMsgStoreWalk()
{

}
void FbDriveView::toolsEditDataTypes()
{

}

/**
  Enable extended errors
  */
void FbDriveView::toolsEnableExtendedErrors()
{
    mEngineWrapper->ToolsSetErrRd(true);
}

/**
  Open simulate leave dialog
  */
void FbDriveView::toolsErrorSimulateLeave()
{
    int leaveCode = -6;
    QString heading = QString("Leave code");
    HbInputDialog::queryText(heading, this, SLOT(doToolsErrorSimulateLeave(HbAction*)), QString::number(leaveCode), scene());
}


/**
  Simulate leave.
  */
void FbDriveView::doToolsErrorSimulateLeave(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        bool ok;
        int leaveCode = dlg->value().toString().toInt(&ok);
        if (leaveCode != 0 || ok) {
            mEngineWrapper->ToolsErrorSimulateLeave(leaveCode);
        }
    }
}

/**
  Open simulate panic dialog.
  */
void FbDriveView::toolsErrorSimulatePanic()
{
    mPanicCategory = QString ("Test Category");
    QString heading = QString("Panic category");
    HbInputDialog::queryText(heading, this, SLOT(doToolsErrorSimulatePanicCode(HbAction*)), mPanicCategory, scene());
}

/**
  Simulate panic.
  */
void FbDriveView::doToolsErrorSimulatePanicCode(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        mPanicCategory = dlg->value().toString();
        int panicCode(555);
        QString heading = QString("Panic code");
        HbInputDialog::queryInt(heading, this, SLOT(doToolsErrorSimulatePanic(HbAction*)), panicCode, scene());
    }
}

/**
  Simulate panic.
  */
void FbDriveView::doToolsErrorSimulatePanic(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        bool ok;
        int panicCode = dlg->value().toInt(&ok);
        if (panicCode != 0 || ok) {
            mEngineWrapper->ToolsErrorSimulatePanic(mPanicCategory, panicCode);
        }
    }
}

/**
  Open simulate exception dialog.
  */
void FbDriveView::toolsErrorSimulateException()
{
    int exceptionCode = 0;
    QString heading = QString("Exception code");
    HbInputDialog::queryInt(heading, this, SLOT(doToolsErrorSimulateException(HbAction*)), exceptionCode, scene());
}

/**
  Simulate exception.
  */
void FbDriveView::doToolsErrorSimulateException(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        bool ok;
        int exceptionCode = dlg->value().toInt(&ok);
        if (exceptionCode != 0 || ok) {
            mEngineWrapper->ToolsErrorSimulateException(exceptionCode);
        }
    }
}

//    void FbDriveView::toolsLocalConnectivityActivateInfrared()
//{
//
//}
//    void FbDriveView::toolsLocalConnectivityLaunchBTUI()
//{
//
//}
//    void FbDriveView::toolsLocalConnectivityLaunchUSBUI()
//{
//
//}
void FbDriveView::toolsMessageInbox()
{

}
void FbDriveView::toolsMessageDrafts()
{

}
void FbDriveView::toolsMessageSentItems()
{

}
void FbDriveView::toolsMessageOutbox()
{

}
void FbDriveView::toolsMemoryInfo()
{

}
void FbDriveView::toolsSecureBackStart()
{

}
void FbDriveView::toolsSecureBackRestore()
{

}
void FbDriveView::toolsSecureBackStop()
{

}

/**
  Open debug mask dialog
  */
void FbDriveView::toolsSetDebugMaskQuestion()
{
    quint32 dbgMask = mEngineWrapper->getDebugMask();
    QString dbgMaskText = QString("0x").append(QString::number(dbgMask, 16));
    QString heading = QString("Kernel debug mask in hex format");
    HbInputDialog::queryText(heading, this, SLOT(toolsSetDebugMask(HbAction*)), dbgMaskText, scene());
}

/**
  Set debug mask
  */
void FbDriveView::toolsSetDebugMask(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if (dlg && action && action->text().compare(QString("Ok"), Qt::CaseInsensitive) == 0) {
        QString dbgMaskText = dlg->value().toString();
        if (dbgMaskText.length() > 2 && dbgMaskText[0]=='0' && dbgMaskText[1]=='x') {
            bool ok;
            quint32 dbgMask = dbgMaskText.toUInt(&ok, 16);
            if (dbgMask != 0 || ok) {
                mEngineWrapper->toolsSetDebugMask(dbgMask);
                Notifications::showConfirmationNote(QString("Changed"));
            } else {
                Notifications::showErrorNote(QString("Cannot convert value"));
            }
        } else {
            Notifications::showErrorNote(QString("Not in hex format"));
        }
    }
}

void FbDriveView::toolsShowOpenFilesHere()
{

}

// ---------------------------------------------------------------------------
// main menu items
// ---------------------------------------------------------------------------

/**
  Show about note
  */
void FbDriveView::about()
{
    Notifications::showAboutNote();
}

// ---------------------------------------------------------------------------
// End of operations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

/**
  Item is selected from list when selection mode is activated from menu
  */
void FbDriveView::selectionChanged(const QItemSelection &/*selected*/, const QItemSelection &/*deselected*/)
{
    //QItemSelectionModel *selectionModel = mListView->selectionModel();
}

/**
  An item is clicked from navigation item list. Navigation item list contains
  drive-, folder- or file items. Opens selected drive, folder or file popup menu
  */
void FbDriveView::activated(const QModelIndex& index)
{
    if (mFbDriveModel) {
//        if (mEngineWrapper->isDriveListViewActive()) {
            mEngineWrapper->moveDownToDirectory(index);
            //emit aboutToShowFileView(QString(mEngineWrapper->currentPath()));
            emit aboutToShowFileView();
            //refreshList();
//        }
    }
}

// ---------------------------------------------------------------------------

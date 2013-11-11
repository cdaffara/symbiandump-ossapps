/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

static const char *NMUI_MESSAGE_LIST_VIEW_XML = ":/docml/nmmessagelistview.docml";
static const char *NMUI_MESSAGE_LIST_VIEW = "NmMessageListView";
static const char *NMUI_MESSAGE_LIST_TREE_LIST = "MessageTreeList";
static const char *NMUI_MESSAGE_LIST_NO_MESSAGES = "MessageListNoMessages";
static const char *NMUI_MESSAGE_LIST_FOLDER_LABEL = "labelGroupBox";

#include "nmuiheaders.h"

/*!
	\class NmMessageListView
	\brief Message list view
*/

/*!
    Constructor
*/
NmMessageListView::NmMessageListView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    NmMailboxListModel &mailboxListModel,
    NmMessageListModel *messageListModel,
    HbDocumentLoader *documentLoader,
    QGraphicsItem *parent)
: NmBaseView(startParam, application, parent),
mApplication(application),
mMessageListWidget(NULL),
mUiEngine(uiEngine),
mMailboxListModel(mailboxListModel),
mMessageListModel(messageListModel),
mDocumentLoader(documentLoader),
mItemContextMenu(NULL),
mLongPressedItem(NULL),
mNoMessagesLabel(NULL),
mFolderLabel(NULL),
mViewReady(false),
mCurrentFolderType(NmFolderInbox),
mSettingsLaunched(false),
mPreviousModelCount(0),
mIsFirstSyncInMessageList(true)
{
    NM_FUNCTION;
    qApp->installEventFilter(this);  
    loadViewLayout();
    createToolBar();
    initTreeView();
    setMailboxName();
    setFolderName();
}

/*!
    Destructor
*/
NmMessageListView::~NmMessageListView()
{
    NM_FUNCTION;

    delete mDocumentLoader;
    mWidgetList.clear();
    mOptionsMenulist.clear();
    if (mItemContextMenu){
        mItemContextMenu->clearActions();
    }
    delete mItemContextMenu;
}

/*!
    View layout loading from XML
*/
void NmMessageListView::loadViewLayout()
{
    NM_FUNCTION;

    // Use document loader to load the view
    bool ok(false);
    setObjectName(QString(NMUI_MESSAGE_LIST_VIEW));
    QObjectList objectList;
    objectList.append(this);
    // Pass the view to documentloader. Document loader uses this view
    // when docml is parsed, instead of creating new view.
    if (mDocumentLoader) {
        mDocumentLoader->setObjectTree(objectList);
        mWidgetList = mDocumentLoader->load(NMUI_MESSAGE_LIST_VIEW_XML, &ok);
    }

    if (ok) {
        // Get message list widget
        mMessageListWidget = qobject_cast<HbTreeView *>
            (mDocumentLoader->findWidget(NMUI_MESSAGE_LIST_TREE_LIST));

        if (mMessageListWidget) {
            NM_COMMENT("nmailui: list object loaded");

            // Set item prototype.
            mMessageListWidget->setItemPrototype(new NmMessageListViewItem());

            // Set the list widget properties.
            mMessageListWidget->setItemRecycling(true);
            mMessageListWidget->contentWidget()->setProperty("indentation", 0);
            mMessageListWidget->setScrollDirections(Qt::Vertical);
            mMessageListWidget->setClampingStyle(HbScrollArea::BounceBackClamping);
            mMessageListWidget->setFrictionEnabled(true);
            mMessageListWidget->setItemPixmapCacheEnabled(true);
            mMessageListWidget->setEnabledAnimations(HbAbstractItemView::All);
        }
        else {
            NM_ERROR(1,"nmailui: list object loading failed");
        }

        mNoMessagesLabel = qobject_cast<HbLabel *>
            (mDocumentLoader->findWidget(NMUI_MESSAGE_LIST_NO_MESSAGES));
        if (mNoMessagesLabel) {
            mNoMessagesLabel->hide();
        }
        else{
            NM_ERROR(1,"nmailui: (no messages) object loading failed");
        }

        mFolderLabel = qobject_cast<HbGroupBox *>(mDocumentLoader->findWidget(NMUI_MESSAGE_LIST_FOLDER_LABEL));

        // Connect options menu about to show to create options menu function
        QObject::connect(menu(), SIGNAL(aboutToShow()),
                         this, SLOT(createOptionsMenu()));
        QObject::connect(&mUiEngine, SIGNAL(syncStateEvent(NmSyncState, const NmId &)),
                         this, SLOT(handleSyncStateEvent(NmSyncState, const NmId &)));

        // Menu needs one dummy item so that aboutToShow signal is emitted.
        NmAction *dummy = new NmAction(0);
        menu()->addAction(dummy);
        // Set sync icon if needed
        updateSyncIcon();
    }
    else {
        NM_ERROR(1,"nmailui: resource loading failed");
    }
}

/*!
    Lazy loading when view layout has been loaded
*/
void NmMessageListView::viewReady()
{
    NM_FUNCTION;

    if (!mViewReady){
        // Refresh list
        QMetaObject::invokeMethod(this, "refreshList", Qt::QueuedConnection);
        mViewReady=true;
    }
    mSettingsLaunched = false;
}

/*!
    Getter for currently displayed folder type
*/
NmFolderType NmMessageListView::folderType()
{
    NM_FUNCTION;

    return mCurrentFolderType;
}

/*!
    okToExitView. Message list view determines whether it is
    ok to exit view and calls mapplication popview.
*/
void NmMessageListView::okToExitView()
{
    NM_FUNCTION;

    // Close view if current folder is inbox
    if (mCurrentFolderType==NmFolderInbox){
        mApplication.popView();
    }
    // Switch to inbox
    else{
        mSelectedMailboxId=mStartParam->mailboxId();
        mSelectedFolderId=mUiEngine.standardFolderId(mSelectedMailboxId,NmFolderInbox);
        QMetaObject::invokeMethod(this, "folderSelected", Qt::QueuedConnection);
    }
}

/*!
    initTreeView
*/
void NmMessageListView::initTreeView()
{
    NM_FUNCTION;

    // Get mailbox widget pointer and set parameters
    if (mMessageListWidget){
        QObject::connect(mMessageListWidget,
                SIGNAL(activated(const QModelIndex &)), this,
                SLOT(itemActivated(const QModelIndex &)));
        QObject::connect(mMessageListWidget,
                SIGNAL(longPressed(HbAbstractViewItem*, QPointF)), this,
                SLOT(showItemContextMenu(HbAbstractViewItem*, QPointF)));
        mMessageListWidget->setFocus();
    }
}


/*!
    Reload view contents with new start parameters
    Typically when view is already open and external view activation occurs
    for this same view. Startparam ownership is transferred to this view
*/
void NmMessageListView::reloadViewContents(NmUiStartParam* startParam)
{
    NM_FUNCTION;

    // Check start parameter validity, message view cannot
    // be updated if given parameter is zero.
    if (startParam&&startParam->viewId()==NmUiViewMessageList &&
        startParam->mailboxId() != 0 ) {
        // Delete existing start parameter data
        delete mStartParam;
        mStartParam = NULL;
        // Store new start parameter data
        mStartParam = startParam;
        // Disconnect signals from previous model
        QObject::disconnect(mMessageListModel, SIGNAL(rowsInserted(const QModelIndex&,int,int)),
                this, SLOT(itemsAdded(const QModelIndex&,int,int)));
        QObject::disconnect(mMessageListModel, SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
                this, SLOT(itemsRemoved()));
        QObject::disconnect(mMessageListModel, SIGNAL(setNewParam(NmUiStartParam*)),
                this, SLOT(reloadViewContents(NmUiStartParam*)));
        // Update model pointer and refresh mailbox with new model
        mMessageListModel = &mUiEngine.messageListModel(startParam->mailboxId(), startParam->folderId());
        
        refreshList();
        
        // Refresh the mailbox name
        setMailboxName();
        
        // Store active folder type
        mCurrentFolderType=NmFolderInbox;
        if (startParam->folderId()!=0){
            mCurrentFolderType = mUiEngine.folderTypeById(startParam->mailboxId(),startParam->folderId());        
        }

        // Update folder name
        setFolderName();
        
        // Set sync icon if needed
        updateSyncIcon();
        
        // Reconstruct the tool bar. This needed because, for example, the
        // search button needs the new mailbox ID.
        createToolBar();
    }
    else {
        NM_ERROR(1,"nmailui: invalid message list start parameter");
        // Unused start parameter needs to be deleted
        delete startParam;
    }
}


/*!
    Return view id
*/
NmUiViewId NmMessageListView::nmailViewId() const
{
    NM_FUNCTION;

    return NmUiViewMessageList;
}


/*!
    Refresh the message list.
*/
void NmMessageListView::refreshList()
{
    NM_FUNCTION;

    if (mMessageListModel) {
        // In each refresh, e.g. in folder change the UI signals lower layer
        // about the folder that has been opened.
        if (mStartParam) {
            NmId mailboxId = mStartParam->mailboxId();
            mUiEngine.updateActiveFolder(mailboxId, mStartParam->folderId());

            NmFolderType folderType(NmFolderInbox);
            if (mStartParam->folderId()!= 0){
                folderType = mUiEngine.folderTypeById(mStartParam->mailboxId(),
                             mStartParam->folderId());
            }

            // If the new folder is an inbox, first automatic sync should be shown
            if (folderType == NmFolderInbox) {
                mIsFirstSyncInMessageList = true;
            }
 
            // Set item model to message list widget
            if (mMessageListWidget) {
                mMessageListWidget->setModel(static_cast<QStandardItemModel*>(mMessageListModel));
    
                QObject::connect(mMessageListModel, SIGNAL(rowsInserted(const QModelIndex&,int,int)),
                    this, SLOT(itemsAdded(const QModelIndex&,int,int)),Qt::UniqueConnection);
                QObject::connect(mMessageListModel, SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
                    this, SLOT(itemsRemoved()),Qt::UniqueConnection);
                QObject::connect(mMessageListModel, SIGNAL(setNewParam(NmUiStartParam*)),
                    this, SLOT(reloadViewContents(NmUiStartParam*)),Qt::UniqueConnection);
    
                mPreviousModelCount = mMessageListModel->rowCount();
    
                if (mPreviousModelCount == 0) {
                    showNoMessagesText();
                }
                else {
                    hideNoMessagesText();
                }
            }
    
            // Notify the mail agent. 
            NmUiEventsNotifier::notifyViewStateChanged(NmUiEventsNotifier::NmViewShownEvent,
                                                       NmUiViewMessageList,
                                                       mailboxId);
        }
    }
}


/*!
    Sync state event handling
*/
void NmMessageListView::handleSyncStateEvent(NmSyncState syncState, const NmId &mailboxId)
{
    NM_FUNCTION;
    if (mMessageListModel && mailboxId == mMessageListModel->currentMailboxId()) {
        if (syncState == Synchronizing) {
             // before first sync inbox id might be zero
            if (mStartParam->folderId() == 0) {
                // after sync inbox id should be updated to correct value
                NmId folderId = mUiEngine.standardFolderId(
                    mStartParam->mailboxId(),
                    NmFolderInbox);
                mStartParam->setFolderId(folderId);
            }
            // Show sync icon only for the first automatic sync after opening message list.
            // Sync icon for manual sync is shown in NmUiEngine::refreshMailbox, not here.
            if (mIsFirstSyncInMessageList) {
                // Show the indicator only if the application is in the foreground
                if (mApplication.isForeground()) {
                    mUiEngine.enableSyncIndicator(true);
                }
                mIsFirstSyncInMessageList = false;
            }
        }
    }
}

/*!
    folder selection handling within current mailbox
*/
void NmMessageListView::folderSelected()
{
    NM_FUNCTION;
    
    NM_TIMESTAMP("NmMessageListView::folderSelected start ***");

    // Reload view contents with new startparams if mailbox or folder
    // id is different than current values.
    if (mStartParam && (mStartParam->mailboxId()!=mSelectedMailboxId||
                        mStartParam->folderId()!=mSelectedFolderId)){
        // Create start params
        NmUiStartParam* startParam = new NmUiStartParam(NmUiViewMessageList,
                                                        mSelectedMailboxId,
                                                        mSelectedFolderId);
        // Store active folder type
        mCurrentFolderType=NmFolderInbox;
        if (startParam->folderId()!=0){
            mCurrentFolderType = mUiEngine.folderTypeById(startParam->mailboxId(),startParam->folderId());        
        }
        // Reload view, ownership of the startparams is passed and old startparams
        // are deleted within reloadViewContents function
        reloadViewContents(startParam);
        // Set folder text to status bar
        setFolderName();
    }
    NM_TIMESTAMP("NmMessageListView::folderSelected end ***");      
}


/*!
    Long keypress handling
*/
void NmMessageListView::showItemContextMenu(HbAbstractViewItem *listViewItem, const QPointF &coords)
{
    NM_FUNCTION;

    if (listViewItem) {
        // Recreate item context menu each time it is called
        if (mItemContextMenu){
            mItemContextMenu->clearActions();
            delete mItemContextMenu;
            mItemContextMenu=NULL;
        }
        mItemContextMenu = new HbMenu();
        // Store long press item for later use with response
        mLongPressedItem = mMessageListModel->data(
                listViewItem->modelIndex(), Qt::DisplayRole).value<NmMessageListModelItem*>();
        if (mItemContextMenu && mLongPressedItem && mLongPressedItem->itemType() ==
            NmMessageListModelItem::NmMessageItemMessage) {

            // Clear previous items from context menu
            mItemContextMenu->clearActions();
            NmUiExtensionManager &extMngr = mApplication.extManager();
            QList<NmAction*> list;
            // Fetch items from extension based on item
            NmMessageEnvelope *envelope = mLongPressedItem->envelopePtr();
            if (envelope){
                NmActionRequest request(this, NmActionContextMenu, NmActionContextViewMessageList,
                        NmActionContextDataMessage, mStartParam->mailboxId(), mStartParam->folderId(),
                        envelope->messageId(),QVariant::fromValue(envelope));
                extMngr.getActions(request, list);
                for (int i=0;i<list.count();i++) {
                    mItemContextMenu->addAction(list[i]);
                }
			    mItemContextMenu->setPreferredPos(coords);
                mItemContextMenu->open();
            }
        }
    }
}

/*!
    Item activation handling. Expand/collapse for title dividers
    opening for mail items
*/
void NmMessageListView::itemActivated(const QModelIndex &index)
{
    NM_FUNCTION;

    mActivatedIndex = index;
    QMetaObject::invokeMethod(this, "handleSelection", Qt::QueuedConnection);
}

/*!
    Item activation handling. Expand/collapse for title dividers
    opening for mail items
*/
void NmMessageListView::handleSelection()
{
    NM_FUNCTION;

    // Do expand/collapse for title divider items
    NmMessageListModelItem* modelItem = mMessageListModel->data(
            mActivatedIndex, Qt::DisplayRole).value<NmMessageListModelItem*>();
    if (modelItem && modelItem->itemType()==
        NmMessageListModelItem::NmMessageItemTitleDivider) {
        if (!mMessageListWidget->isExpanded(mActivatedIndex)) {
            mMessageListWidget->setExpanded(mActivatedIndex, true);
            modelItem->setExpanded(true);
        }
        else {
            mMessageListWidget->setExpanded(mActivatedIndex, false);
            modelItem->setExpanded(false);
        }
    }
    if (modelItem && modelItem->itemType() == NmMessageListModelItem::NmMessageItemMessage
        && !mSettingsLaunched)
    {
        NmFolderType folderType = mUiEngine.folderTypeById(mStartParam->mailboxId(),
                                  mStartParam->folderId());
        if (folderType==NmFolderDrafts){
            NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                mStartParam->mailboxId(), mStartParam->folderId(),
                modelItem->envelope().messageId(),NmUiEditorFromDrafts);
            mApplication.enterNmUiView(startParam);
        }
        else if (folderType!=NmFolderOutbox){
            NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
                mStartParam->mailboxId(), mStartParam->folderId(),
                modelItem->envelope().messageId());
            mApplication.enterNmUiView(startParam);
        }
    }
}



/*!
    createOptionsMenu. Functions asks menu commands from extension
    to be added to options menu.
*/
void NmMessageListView::createOptionsMenu()
{
    NM_FUNCTION;

    menu()->clearActions();
    NmActionRequest request(this, NmActionOptionsMenu, NmActionContextViewMessageList,
    		NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );
    NmUiExtensionManager &extMngr = mApplication.extManager();
    mOptionsMenulist.clear();
    extMngr.getActions(request, mOptionsMenulist);
    for (int i=0;i<mOptionsMenulist.count();i++) {
        menu()->addAction(mOptionsMenulist[i]);
    }
}

/*!
    handleActionCommand. From NmMenuObserver, extension manager calls this
    call to handle menu command in the UI.
*/
void NmMessageListView::handleActionCommand(NmActionResponse &actionResponse)
{
    NM_FUNCTION;

    // Handle context menu commands here
    if (actionResponse.menuType() == NmActionOptionsMenu) {
        switch (actionResponse.responseCommand()) {
            case NmActionResponseCommandNewMail: {
                // Check that given start response has mailbox and folder id's
                if (actionResponse.mailboxId()!=0){
                    NM_TIMESTAMP("New Mail chose from menu.");
                    NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                            actionResponse.mailboxId(), mStartParam->folderId());
                    // startParam ownerhips transfers
                    mApplication.enterNmUiView(startParam);
                }
                break;
            }
            case NmActionResponseCommandUpdateMailboxName: {
                setMailboxName();
                break;
            }
            case NmActionResponseCommandMailboxDeleted: {
                mApplication.prepareForPopView();
				break;
            }
            case NmActionResponseCommandSwitchFolder: {
                mSelectedFolderId=actionResponse.folderId();
                mSelectedMailboxId=actionResponse.mailboxId();
                QMetaObject::invokeMethod(this, "folderSelected", Qt::QueuedConnection);
                break;
            }
            case NmActionResponseCommandSettings: {
                mSettingsLaunched = true;
                break;
            }
            default: {
                break;
            }
        }
    }

    // Handle context menu commands here
    else if (actionResponse.menuType()==NmActionContextMenu){
        switch (actionResponse.responseCommand()){
           case NmActionResponseCommandOpen:{
               if (mLongPressedItem){
                   NmFolderType folderType = mUiEngine.folderTypeById(mStartParam->mailboxId(),
                                                mStartParam->folderId());
                   if (folderType==NmFolderDrafts){
                       NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                           mStartParam->mailboxId(), mStartParam->folderId(),
                           mLongPressedItem->envelope().messageId(),NmUiEditorFromDrafts);
                       mApplication.enterNmUiView(startParam);
                   }
                   else if (folderType!=NmFolderOutbox){
                       NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
                           mStartParam->mailboxId(), mStartParam->folderId(),
                           mLongPressedItem->envelope().messageId());
                       mApplication.enterNmUiView(startParam);
                   }
                   mLongPressedItem=NULL;
                 }
               break;           
           }
           default: {
               break;           
           }
        }
    }

    // Handle toolbar commands here
    else if ( actionResponse.menuType() == NmActionToolbar ) {
        if ( actionResponse.responseCommand() == NmActionResponseCommandNewMail ) {
            // Check that given start response has mailbox and folder id's
            if (actionResponse.mailboxId()!=0){
                NM_TIMESTAMP("New Mail chose from toolbar.");
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                        actionResponse.mailboxId(), mStartParam->folderId());
                // startParam ownerhips transfers
                mApplication.enterNmUiView(startParam);
            }
        }
        if (actionResponse.responseCommand() == NmActionResponseCommandSearch) {
            // Check that the given start response has mailbox and folder IDs.
            if (actionResponse.mailboxId() != 0) {
                NmUiStartParam *startParam =
                    new NmUiStartParam(NmUiViewMessageSearchList,
                                       actionResponse.mailboxId(),
                                       mStartParam->folderId());

                // startParam ownership transfers.
                mApplication.enterNmUiView(startParam);
            }
        }
    }
}


/*!
    Set mailbox name to title
*/
void NmMessageListView::setMailboxName()
{
    NM_FUNCTION;

    if (mStartParam){
        NmMailboxMetaData *meta = mUiEngine.mailboxById(mStartParam->mailboxId());
        if (meta){
            setTitle(meta->name());
        }
    }
}

/*!
    createToolBar. Function asks menu commands from extension
    to be added to toolbar owned by the HbView. Also toolbar
    specific icons are added in this function.
*/
void NmMessageListView::createToolBar()
{
    NM_FUNCTION;

    HbToolBar *tb = toolBar();
    if (!tb) {
        return;
    }
    tb->clearActions();
    NmActionRequest request(this, NmActionToolbar, NmActionContextViewMessageList,
            NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );
    NmUiExtensionManager &extMngr = mApplication.extManager();
    QList<NmAction *> list;
    extMngr.getActions(request, list);
    for (int i = 0; i < list.count(); i++) {
        tb->addAction(list[i]);
    }
}

/*!
    setFolderName. Function sets folder name to status bar
*/
void NmMessageListView::setFolderName()
{
    NM_FUNCTION;

    if (mStartParam && mFolderLabel){
        switch (mCurrentFolderType) {
        case NmFolderOutbox:
            {
            mFolderLabel->setHeading(hbTrId("txt_mail_subhead_outbox"));
            }
            break;
        case NmFolderDrafts:
            {
            mFolderLabel->setHeading(hbTrId("txt_mail_subhead_drafts"));
            }
            break;
        case NmFolderSent:
            {
            mFolderLabel->setHeading(hbTrId("txt_mail_subhead_sent_items"));
            }
            break;
        case NmFolderDeleted:
            {
            mFolderLabel->setHeading(hbTrId("txt_mail_subhead_deleted_items"));
            }
            break;
        case NmFolderInbox:
        default:
            {
            mFolderLabel->setHeading(hbTrId("txt_mail_subhead_inbox"));
            }
            break;
        }
    }
}


/*!
    Handles the addition of a new item. Makes sure the message list widget is
    visible and keeps the scroll position on the top of the list.

    \param parent Not used.
    \param start
    \param end Not used.
*/
void NmMessageListView::itemsAdded(const QModelIndex &parent, int start, int end)
{
    NM_FUNCTION;

    Q_UNUSED(parent);
    Q_UNUSED(end);

    // Hide no messages if previous model count has been zero
    // and new items have been added to the list
    if (mPreviousModelCount==0) {
        hideNoMessagesText();
    }

    // Make sure the top of the list is kept visible by scrolling back to the
    // top if necessary.
    if (start == 0 && mMessageListWidget) {
        QList<HbAbstractViewItem*> items = mMessageListWidget->visibleItems();

        if (items.count()) {
            QModelIndex index = items.at(0)->modelIndex();

            while (index.row() > 0) {
                QModelIndex previous =
                    mMessageListWidget->modelIterator()->previousIndex(index);

                if (previous.isValid()) {
                    mMessageListWidget->scrollTo(previous);
                }

                index = previous;
            }
        }
    }
    // Store model count
    if (mMessageListModel){
        mPreviousModelCount=mMessageListModel->rowCount();
    }
}


/*!
    Observe items removed
*/
void NmMessageListView::itemsRemoved()
{
    NM_FUNCTION;
    // Store model count
    if (mMessageListModel){
        mPreviousModelCount=mMessageListModel->rowCount();
    }
    if (mPreviousModelCount == 0){
        showNoMessagesText();
    }
}

/*!
    Show "(no messages)" text at the middle of the screen.
*/
void NmMessageListView::showNoMessagesText()
{
    NM_FUNCTION;

    if (mNoMessagesLabel&&mMessageListWidget) {
        mMessageListWidget->hide();
        mNoMessagesLabel->show();
    }
}

/*!
    Hide "(no messages)" text at the middle of the screen.
*/
void NmMessageListView::hideNoMessagesText()
{
    NM_FUNCTION;

    if (mNoMessagesLabel&&mMessageListWidget) {
        mNoMessagesLabel->hide();
        mMessageListWidget->show();
    }
}

/*!
    Updates sync icon based on sync status from the plugin.
*/
void NmMessageListView::updateSyncIcon()
{
    if (mStartParam) {
        NmSyncState syncState = mUiEngine.syncState(mStartParam->mailboxId());
        if (syncState==Synchronizing) {
            mUiEngine.enableSyncIndicator(true);
        } 
        else {
            mUiEngine.enableSyncIndicator(false);
        }
    }
}

/*!
    Handle application background event. 
    In this case the menu has to be closed.
*/
bool NmMessageListView::eventFilter(QObject *obj,  QEvent *event)
{
    if (obj == qApp)
    {
        // Close menu if application is switched to background, e.g. deactivated
        if (event && event->type() == QEvent::ApplicationDeactivate)
        {
            // Close options menu if it active
            HbMenu *optionsMenu = menu();
            if (optionsMenu && optionsMenu->isVisible()) {
                // Close any submenus that might be open
                for (int i=0 ; i<mOptionsMenulist.count() ; i++) {
                    NmAction *menuItem = mOptionsMenulist[i];
                    if (menuItem && menuItem->menu()) {
                        menuItem->menu()->close();
                    }
                }
                // Close options menu
                optionsMenu->close();
            }
            else if (mItemContextMenu && mItemContextMenu->isVisible()) {
                // Close and delete item contextmenu if open
                mItemContextMenu->clearActions();
                delete mItemContextMenu;
                mItemContextMenu=NULL;
            }
        }
    }
    return mApplication.eventFilter(obj, event);
}




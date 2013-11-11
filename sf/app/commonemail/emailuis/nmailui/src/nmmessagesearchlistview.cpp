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

#include "nmuiheaders.h"

static const char *NMUI_MESSAGE_SEARCH_VIEW_XML = ":/docml/nmmessagesearchlistview.docml";
static const char *NMUI_MESSAGE_SEARCH_VIEW = "NmMessageSearchView";
static const char *NMUI_MESSAGE_SEARCH_MESSAGE_TREE_LIST= "MessageTreeList";
static const char *NMUI_MESSAGE_SEARCH_PANEL = "SearchPanel";
static const char *NMUI_MESSAGE_SEARCH_NO_MESSAGES = "MessageNoMessages";

static const QString NmSearchListViewProgressButton("progressbutton");
static const QString NmSearchListViewLineEdit("lineedit");

static const QString NmSearchListViewSpinnerAnimation("qtg_anim_mono_loading");
static const QString NmSearchListViewSpinnerImage("qtg_anim_mono_loading_1");

static const QString NmSearchListViewPortrait("portrait");
static const QString NmSearchListViewLandscape("landscape");


/*!
    \class NmMessageSearchListView
    \brief The view for searching messages.
*/


/*!
    Class constructor.
*/
NmMessageSearchListView::NmMessageSearchListView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    NmMessageListModel &msgListModel,
    HbDocumentLoader *documentLoader,
    QGraphicsItem *parent)
: NmBaseView(startParam, application, parent),
  mApplication(application),
  mUiEngine(uiEngine),
  mMsgListModel(msgListModel),
  mDocumentLoader(documentLoader),
  mItemContextMenu(NULL),
  mMessageList(NULL),
  mNoMessagesLabel(NULL),
  mSearchPanel(NULL),
  mLongPressedItem(NULL),
  mProgressButton(NULL),
  mLineEdit(NULL),
  mViewReady(false),
  mSearchInProgress(false),
  mSpinnerIcon(NULL),
  mAnimationAddedToManger(false),
  mSelectTextAfterOrientationChange(false),
  mVkbHost(NULL)
{
    NM_FUNCTION;
    
    loadViewLayout();
    initTreeView();

    // Add spinner animation axml to animation manager.
    HbIconAnimationManager *animationManager = HbIconAnimationManager::global();
    if (animationManager) {
        mAnimationAddedToManger =
            animationManager->addDefinitionFile(NmSearchListViewSpinnerAnimation);
    }

    HbMainWindow *mainWindow = mApplication.mainWindow();
    connect(mainWindow, SIGNAL(aboutToChangeOrientation()),
            this, SLOT(orientationAboutToChange()));
    connect(mainWindow, SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(orientationChanged()));

    mVkbHost = new HbShrinkingVkbHost(this);
    connect(mVkbHost, SIGNAL(keypadOpened()), this, SLOT(vkbOpened()));
    connect(mVkbHost, SIGNAL(keypadClosed()), this, SLOT(vkbClosed()));
}


/*!
    Class destructor.
*/
NmMessageSearchListView::~NmMessageSearchListView()
{
    NM_FUNCTION;
    delete mSpinnerIcon;
    delete mDocumentLoader;

    mWidgetList.clear();

    if (mItemContextMenu){
        mItemContextMenu->clearActions();
    }

    delete mItemContextMenu;
    delete mVkbHost;
}


/*!
    From NmBaseView.

    Returns the view ID.

    \return The view ID.
*/
NmUiViewId NmMessageSearchListView::nmailViewId() const
{
    NM_FUNCTION;
    
    return NmUiViewMessageSearchList;
}


/*!
    From NmBaseView.

    Makes necessary activities before view can exit.
*/
void NmMessageSearchListView::aboutToExitView()
{
    NM_FUNCTION;
    if (mSearchInProgress) {
       // Search is in progress - do cancel.
       mUiEngine.cancelSearch(mStartParam->mailboxId());
       mSearchInProgress = false;
    }
}

/*!
    From NmBaseView.

    Does the lazy loading after the view layout has been loaded.
*/
void NmMessageSearchListView::viewReady()
{
    NM_FUNCTION;

    // Load orientation specific sections.
    loadOrientationSection();
    
    if (!mViewReady){

        // Set the mailbox name to the title pane.
        setViewTitle();

        // Refresh the list.
        QMetaObject::invokeMethod(this, "refreshList", Qt::QueuedConnection);

        // Undim search field.
        setSearchInputMode(NmNormalMode);

        // Open VKB
        QMetaObject::invokeMethod(this, "sendSoftwareInputPanelRequest", Qt::QueuedConnection);

        mViewReady = true;
    }
}


/*!
    From NmMenuObserver.

    Handles action commands which are usually given by the user by selecting
    menu items.

    \param actionResponse The command details.
*/
void NmMessageSearchListView::handleActionCommand(NmActionResponse &actionResponse)
{
    NM_FUNCTION;
    
    // Handle options menu commands here.
    if (actionResponse.menuType() == NmActionOptionsMenu) {
        switch (actionResponse.responseCommand()) {
            case NmActionResponseCommandUpdateMailboxName: {
                setViewTitle();
                break;
            }
            case NmActionResponseCommandMailboxDeleted: {
                mApplication.prepareForPopView();
                break;
            }
            default: {
                break;
            }
        }
    }
    // Handle context menu commands here.
    if (actionResponse.menuType() == NmActionContextMenu) {
        if (mLongPressedItem){

            // Hide VKB
            vkbClosed();
            sendSoftwareInputPanelRequest(QEvent::CloseSoftwareInputPanel);

            NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
                mStartParam->mailboxId(), mLongPressedItem->envelope().folderId(),
                mLongPressedItem->envelope().messageId());

            mApplication.enterNmUiView(startParam);
            mLongPressedItem = NULL;
       }
    }
}


/*!
    From NmBaseView.

    Loads the view layout from the XML file.
*/
void NmMessageSearchListView::loadViewLayout()
{
    NM_FUNCTION;
    
    // Use the document loader to load the view layout.
    bool ok(false);
    setObjectName(QString(NMUI_MESSAGE_SEARCH_VIEW));
    QObjectList objectList;
    objectList.append(this);

    // Pass the view to the document loader. Instead of creating a new view, the
    // document loader uses this view when the docml file is parsed.
    if (mDocumentLoader) {
        mDocumentLoader->setObjectTree(objectList);
        mWidgetList = mDocumentLoader->load(NMUI_MESSAGE_SEARCH_VIEW_XML, &ok);
    }

    if (ok) {
        // Load the search panel
        mSearchPanel = qobject_cast<HbSearchPanel *>(
            mDocumentLoader->findWidget(NMUI_MESSAGE_SEARCH_PANEL));

        if (mSearchPanel) {
            mSearchPanel->setProgressive(false);
            mSearchPanel->setCancelEnabled(true);

            foreach(QGraphicsItem *obj, mSearchPanel->childItems()) {
                QGraphicsWidget *const widget = static_cast<QGraphicsWidget *>(obj);
                if (widget) {
                    QString objectName(widget->objectName());
                    if (objectName == NmSearchListViewProgressButton) {
                        mProgressButton = qobject_cast<HbPushButton *>(widget);
                        mProgressButton->setIcon(HbIcon("qtg_mono_search"));
                        mProgressButton->setEnabled(false);
                    }
                    else if(objectName == NmSearchListViewLineEdit) {
                        mLineEdit = qobject_cast<HbLineEdit *>(widget);
                        mLineEdit->setInputMethodHints(Qt::ImhNoPredictiveText);
                        connect(mLineEdit, SIGNAL(textChanged(const QString &)),
                                this, SLOT(textChanged(const QString &)));
                    }
                }
            }

            connect(mSearchPanel, SIGNAL(criteriaChanged(const QString &)),
                    this, SLOT(toggleSearch()));

            connect(mSearchPanel, SIGNAL(exitClicked()),
                    this, SLOT(exitClicked()), Qt::QueuedConnection);

        }

        // Get the message list widget.
        mMessageList = qobject_cast<HbTreeView *>(
            mDocumentLoader->findWidget(NMUI_MESSAGE_SEARCH_MESSAGE_TREE_LIST));

        if (mMessageList) {
            NM_COMMENT("NmMessageSearchListView: message list widget loaded");

            // Set the item prototype.
            mMessageList->setItemPrototype(new NmMessageListViewItem());

            // Set the list widget properties.
            mMessageList->setItemRecycling(true);
            mMessageList->contentWidget()->setProperty("indentation", 0);
            mMessageList->setScrollDirections(Qt::Vertical);
            mMessageList->setClampingStyle(HbScrollArea::BounceBackClamping);
            mMessageList->setFrictionEnabled(true);
            mMessageList->setItemPixmapCacheEnabled(true);
            
            // We want the search results to appear one by one.
            mMessageList->setEnabledAnimations(HbAbstractItemView::Appear &
                HbAbstractItemView::Expand);
        }

        // Load the no messages label.
        mNoMessagesLabel = qobject_cast<HbLabel *>(
            mDocumentLoader->findWidget(NMUI_MESSAGE_SEARCH_NO_MESSAGES));

        if (mNoMessagesLabel) {
            NMLOG("NmMessageSearchListView: No messages label loaded.");
            mNoMessagesLabel->setMaximumHeight(0);
        }
    }
    else {
        NM_ERROR(1, "NmMessageSearchListView: failed to load widgets from XML");
    }
}


/*!
    Initializes the tree view.
*/
void NmMessageSearchListView::initTreeView()
{
    NM_FUNCTION;
    
    // Get the mailbox widget pointer and set the parameters.
    if (mMessageList) {
        connect(mMessageList, SIGNAL(activated(const QModelIndex &)),
                this, SLOT(itemActivated(const QModelIndex &)));

        connect(mMessageList, SIGNAL(longPressed(HbAbstractViewItem*, QPointF)),
                this, SLOT(showItemContextMenu(HbAbstractViewItem*, QPointF)));

        mMessageList->setFocus();
        mItemContextMenu = new HbMenu();
    }

    // Clear the previous content if any.
    mMsgListModel.clear();
}


/*!
    Sets the title according to the name of the current mailbox.
*/
void NmMessageSearchListView::setViewTitle()
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
    Toggles the visiblity between the message list widget and the "no messages"
    label.

    \param visible If true, will display the "no messages" label. If false,
                   will display the message list widget.
*/
void NmMessageSearchListView::noMessagesLabelVisibility(bool visible)
{
    NM_FUNCTION;
    
    if (visible) {
        // Hide the message list widget and display the "no messages" label.
        if (mMessageList) {
            mMessageList->setMaximumHeight(0);
            mMessageList->hide();
        }

        if (mNoMessagesLabel && !mNoMessagesLabel->isVisible()) {
            mNoMessagesLabel->setMaximumHeight(QWIDGETSIZE_MAX);
            mNoMessagesLabel->show();
        }
    }
    else {
        // Hide the "no messages" label and display the message list widget.
        if (mNoMessagesLabel && mNoMessagesLabel->isVisible()) {
            mNoMessagesLabel->setMaximumHeight(0);
            mNoMessagesLabel->hide();
        }

        if (mMessageList) {
            mMessageList->setMaximumHeight(QWIDGETSIZE_MAX);
            mMessageList->show();
        }
    }
}


/*!
    Shows no messages label if no matching messages were found.
*/
void NmMessageSearchListView::updateResultView()
{
    NM_FUNCTION;

    if (mMsgListModel.rowCount() < 1) {
        // Display the "no messages".
        noMessagesLabelVisibility(true);
    }
}


/*!
    Sets the mode for the search input.

    \param mode The mode to set.
*/
void NmMessageSearchListView::setSearchInputMode(NmSearchInputMode mode)
{
    NM_FUNCTION;

    if (!mLineEdit) {
        // No line edit widget!
        return;
    }

    if (mode == NmNormalMode) {
        mLineEdit->setEnabled(true);
        mLineEdit->setFocus();
    }
    else {
        mLineEdit->setEnabled(false);
        mLineEdit->deselect();
    }
}

/*!
    From NmBaseView.

    Reloads the view contents with new start parameters. This method is
    typically used when the view is already open and an external view activation
    occurs for this same view.
*/
void NmMessageSearchListView::reloadViewContents(NmUiStartParam *startParam)
{
    NM_FUNCTION;
    
    if (startParam && startParam->viewId() == NmUiViewMessageSearchList) {
        // Delete the existing start parameter data.
        delete mStartParam;
        mStartParam = NULL;

        // Store the new start parameter data.
        mStartParam = startParam;

        // Update the model with new parameters.
        mUiEngine.messageListModelForSearch(startParam->mailboxId());
        refreshList();

        // Refresh the mailbox name.
        setViewTitle();
    }
    else {
        // Invalid start parameter data! Unused start parameter needs to be
        // deleted.
        NM_ERROR(1, "NmMessageSearchListView: invalid message list start parameter");
        delete startParam;
    }
}

/*!
    Displays the item context menu. This method gets called if an item on the
    list is long pressed.
*/
void NmMessageSearchListView::showItemContextMenu(
    HbAbstractViewItem *listViewItem, const QPointF &coords)
{
    NM_FUNCTION;
    
    // Stop search.
    if (mSearchInProgress) {
        toggleSearch();
    }

    // Store long press item for later use with response.
    mLongPressedItem = 
        mMsgListModel.data(listViewItem->modelIndex(),
                           Qt::DisplayRole).value<NmMessageListModelItem*>();

    if (mItemContextMenu && mLongPressedItem &&
        mLongPressedItem->itemType() == NmMessageListModelItem::NmMessageItemMessage) {
        // Clear the previous menu actions.
        mItemContextMenu->clearActions();
        NmUiExtensionManager &extMngr = mApplication.extManager();
        QList<NmAction*> list;

        // Fetch the menu actions based on the selected item.
        NmMessageEnvelope *envelope = mLongPressedItem->envelopePtr();

        if (envelope){
            NmActionRequest request(this, NmActionContextMenu,
                NmActionContextViewMessageSearchList, NmActionContextDataMessage,
                mStartParam->mailboxId(), envelope->folderId(),
                envelope->messageId(), QVariant::fromValue(envelope));

            extMngr.getActions(request, list);
        }

        for (int i(0); i < list.count(); ++i) {
            mItemContextMenu->addAction(list[i]);
        }

        mItemContextMenu->setPreferredPos(coords);
        mItemContextMenu->open();
    }
}


/*!
    Stores the given index and forwards the call to handleSelection(). This
    method gets called when an item on the list is selected.  

    \param index The index of the activated item.
*/
void NmMessageSearchListView::itemActivated(const QModelIndex &index)
{
    NM_FUNCTION;
    
    mActivatedIndex = index;
    QMetaObject::invokeMethod(this, "handleSelection", Qt::QueuedConnection);
}


/*!
    If the selected item is a message, will open the message.
*/
void NmMessageSearchListView::handleSelection()
{
    NM_FUNCTION;
    
    // Do expand/collapse for title divider items
    NmMessageListModelItem* modelItem = mMsgListModel.data(
        mActivatedIndex, Qt::DisplayRole).value<NmMessageListModelItem*>();

    if (modelItem &&
        modelItem->itemType() == NmMessageListModelItem::NmMessageItemMessage)
    {
        if (mSearchInProgress) {
            // Stop the search.
            toggleSearch();
        }

        // Open the message.
        NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
            mStartParam->mailboxId(), modelItem->envelope().folderId(),
            modelItem->envelope().messageId());

        // Hide VKB
        vkbClosed();
        sendSoftwareInputPanelRequest(QEvent::CloseSoftwareInputPanel);

        mApplication.enterNmUiView(startParam);
    }
}


/*!
    Displays the message list widtet if not visible and scrolls to the
    appropriate point on the list.
*/
void NmMessageSearchListView::itemsAdded(const QModelIndex &parent, int start, int end)
{
    NM_FUNCTION;
    
    Q_UNUSED(parent);
    Q_UNUSED(end);

    // The search is an asynchronous operation. If a user stops the search, it
    // might take a short while before the search is actually stopped and during
    // this time it is possible that messages matching the search are added.
    // Therefore, update is needed if items are added after the
    // search has been stopped by the user.
    if (!mSearchInProgress) {
        updateResultView();
    }

    if (!start && mMessageList) {
        QList<HbAbstractViewItem*> items = mMessageList->visibleItems();

        if (items.count()) {
            QModelIndex index = items.at(0)->modelIndex();

            while (index.row() > 0) {
                QModelIndex previous =
                    mMessageList->modelIterator()->previousIndex(index);

                if (previous.isValid()) {
                    mMessageList->scrollTo(previous);
                }

                index = previous;
            }
        }
    }
}


/*!
    This method gets called when an item is removed from the list. If the
    search has completed (or stopped), the search results needs to be updated.
*/
void NmMessageSearchListView::itemsRemoved()
{
    NM_FUNCTION;
    
    if (!mSearchInProgress) {
        updateResultView();
    }
}


/*!
    Refreshes the search list.
*/
void NmMessageSearchListView::refreshList()
{
    NM_FUNCTION;
    
    if (mMessageList) {
        // Set the model.
        mMessageList->setModel(static_cast<QStandardItemModel*>(&mMsgListModel));

        // Connect the signals.
        connect(&mMsgListModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                this, SLOT(itemsAdded(const QModelIndex&, int, int)), Qt::UniqueConnection);

        connect(&mMsgListModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
                this, SLOT(itemsRemoved()), Qt::UniqueConnection);

        connect(&mMsgListModel, SIGNAL(setNewParam(NmUiStartParam*)),
                this, SLOT(reloadViewContents(NmUiStartParam*)), Qt::UniqueConnection);
    }
}


/*!
    Starts and stops the search according to the current status.

    Starts search: Uses the input given by the user as a search string and
    starts an asynchronous search. Any previous search results are removed from
    the search list.

    Stops search: shows no messages label if no matching messages were found.
*/
void NmMessageSearchListView::toggleSearch()
{
    NM_FUNCTION;
    
    if (mSearchInProgress) {
        // Search is in progress - do cancel.
        mUiEngine.cancelSearch(mStartParam->mailboxId());
        handleSearchComplete();
    }
    else {
        // Do start the search.
        mSearchInProgress = true;
    
        // Clear previous results if any.
        mMsgListModel.clear();

        connect(&mUiEngine, SIGNAL(searchComplete()),
                this, SLOT(handleSearchComplete()), Qt::UniqueConnection);
    
        // Get the search input and start the search.
        QStringList searchStrings;

        if (mSearchPanel) {
            searchStrings.append(mSearchPanel->criteria());
        }

        // Hide the virtual keyboard
        sendSoftwareInputPanelRequest(QEvent::CloseSoftwareInputPanel);

        // Hide the "no messages" label if visible and dim the search input.
        noMessagesLabelVisibility(false);
        setSearchInputMode(NmDimmedMode);

        // Change "Go" button icon to spinner.
        if (mProgressButton) {
            // Create icon if not created yet.
            if (!mSpinnerIcon) {
                // Select one picture if animation loading has failed.
                QString iconName = NmSearchListViewSpinnerImage;
                if (mAnimationAddedToManger) {
                    iconName = NmSearchListViewSpinnerAnimation;
                }
                mSpinnerIcon = new HbIcon(iconName);
                mSpinnerIcon->setFlags(HbIcon::Colorized);
            }
            mProgressButton->setIcon(*mSpinnerIcon);
        }

        // Start search
        mUiEngine.search(mStartParam->mailboxId(), searchStrings);
    }
}


/*!
    If the search matched any messages, displays the search result count in the
    info label. If no messages were found, the method displays the "no messages"
    label. In either case, the search panel is updated.
*/
void NmMessageSearchListView::handleSearchComplete()
{
    NM_FUNCTION;
    
    mSearchInProgress = false;
    
    // Change search button icon to magnifying glass.
    if (mProgressButton) {
        mProgressButton->setIcon(HbIcon("qtg_mono_search"));
    }

    // Update result view. Shows no message label if no matching messages were found.
    updateResultView();

    // Undim the search input.
    setSearchInputMode(NmNormalMode);

    // In no matching messages were found. Show no messages label and open VKB.
    if (mMsgListModel.rowCount() < 1) {
        noMessagesLabelVisibility(true);

        sendSoftwareInputPanelRequest(QEvent::RequestSoftwareInputPanel);

        // Select text
        mLineEdit->selectAll();
    }
}

/*!
    By default search button is disabled but when some text is
    written to search field, the search button is enabled. When search field is
    erased, button is disabled.

    \param text chaged text.
*/
void NmMessageSearchListView::textChanged(const QString &text)
{
    NM_FUNCTION;

    if (!text.isEmpty()) {
        if (!mProgressButton->isEnabled()) {
            mProgressButton->setEnabled(true);
        }
    }
    else {
        if (mProgressButton->isEnabled()) {
            mProgressButton->setEnabled(false);
        }
    }
}

/*!
    When exit button is clicked, trigger back navigation action
    which closes the search view.
*/
void NmMessageSearchListView::exitClicked()
{
    NM_FUNCTION;

    navigationAction()->activate(QAction::Trigger);
}

/*!
    Called before orientation chages. Checks if search criteria text
    needs to be selected after the orientation has changed.
*/
void NmMessageSearchListView::orientationAboutToChange()
{
    NM_FUNCTION;

    HbVkbHostBridge *instance = HbVkbHostBridge::instance();
    if (instance &&
        instance->keypadStatus() == HbVkbHost::HbVkbStatusOpened &&
        mLineEdit &&
        mLineEdit->hasSelectedText()) {
        mSelectTextAfterOrientationChange = true;
    }
}

/*!
    Called when orientation has changed. Selects search criteria text if needed.
    \sa  NmMessageSearchListView::orientationAboutToChange()
*/
void NmMessageSearchListView::orientationChanged()
{
    NM_FUNCTION;

    loadOrientationSection();

    if (mSelectTextAfterOrientationChange) {
        mLineEdit->selectAll();
        mSelectTextAfterOrientationChange = false;
    }

}

/*!
    Sends event to software input panel (VKB).

    Used event types:
    QEvent::RequestSoftwareInputPanel(opens VKB)
    QEvent::CloseSoftwareInputPanel(closes VKB)
*/
void NmMessageSearchListView::sendSoftwareInputPanelRequest(QEvent::Type eventType)
{
    NM_FUNCTION;

    QInputContext *ic = qApp->inputContext();
    if (ic) {
        QScopedPointer<QEvent> event(new QEvent(eventType));
        ic->filterEvent(event.data());
    }
}

/*!
    Called when virtual keyboard is about to open.
*/
void NmMessageSearchListView::vkbOpened()
{
    NM_FUNCTION;

    setContentFullScreen(true);
    hideItems(Hb::ToolBarItem);
}

/*!
    Called when virtual keyboard is about to close.
*/
void NmMessageSearchListView::vkbClosed()
{
    NM_FUNCTION;

    setContentFullScreen(false);
    showItems(Hb::ToolBarItem);
}

/*!
    Loads orientation specific sections from docml.
*/
void NmMessageSearchListView::loadOrientationSection()
{
    QString sectionName(NmSearchListViewPortrait);
    if (mApplication.mainWindow()->orientation() == Qt::Horizontal) {
        sectionName = NmSearchListViewLandscape;
    }

    mDocumentLoader->load(NMUI_MESSAGE_SEARCH_VIEW_XML, sectionName);
}

// End of file.

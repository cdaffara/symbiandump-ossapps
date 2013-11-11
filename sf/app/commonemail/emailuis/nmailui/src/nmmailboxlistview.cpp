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

// Layout file and view
static const char *NMUI_MAILBOX_LIST_VIEW_XML = ":/docml/nmmailboxlistview.docml";
static const char *NMUI_MAILBOX_LIST_VIEW = "NmMailboxListView";
static const char *NMUI_MAILBOX_LIST_WIDGET = "MailboxListWidget";

#include "nmuiheaders.h"

/*!
	\class NmMailboxListView
	\brief Mailbox list view
*/

/*!
    Constructor
*/
NmMailboxListView::NmMailboxListView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    NmMailboxListModel &mailboxListModel,
    HbDocumentLoader *documentLoader,
    QGraphicsItem *parent)
: NmBaseView(startParam, application, parent),
mApplication(application),
mMailboxListWidget(NULL),
mUiEngine(uiEngine),
mListModel(mailboxListModel),
mItemContextMenu(NULL),
mDocumentLoader(documentLoader),
mViewReady(false)
{
    NM_FUNCTION;
    
    // Load view layout
    loadViewLayout();
    
    // Set title
    setTitle(hbTrId("txt_mail_title_mail"));
}

/*!
    Destructor
*/
NmMailboxListView::~NmMailboxListView()
{
    NM_FUNCTION;
    
    delete mDocumentLoader;
    mWidgetList.clear();
    if (mItemContextMenu){
        mItemContextMenu->clearActions();
    }
    delete mItemContextMenu;
}

/*!
    View layout loading from XML
*/
void NmMailboxListView::loadViewLayout()
{
    NM_FUNCTION;
    

    // Use document loader to load the view
     bool ok(false);
     setObjectName(QString(NMUI_MAILBOX_LIST_VIEW));
     QObjectList objectList;
     objectList.append(this);
     // Pass the view to documentloader. Document loader uses this view
     // when docml is parsed, instead of creating new view.
     if (mDocumentLoader) {
         mDocumentLoader->setObjectTree(objectList);
         mWidgetList = mDocumentLoader->load(NMUI_MAILBOX_LIST_VIEW_XML, &ok);
     }

    if (ok) {
        // Create item context menu
        mItemContextMenu = new HbMenu();
        // Get mailbox widget pointer and set parameters
        mMailboxListWidget = qobject_cast<HbListView *>
            (mDocumentLoader->findWidget(NMUI_MAILBOX_LIST_WIDGET));
        if (mMailboxListWidget) {
            NM_COMMENT("nmailui: mailboxlistview: list object loaded");
            // Set item prototype.
            mMailboxListWidget->setItemPrototype(new NmMailboxListViewItem(this));
            mMailboxListWidget->setItemRecycling(true);
            QObject::connect(mMailboxListWidget,
                    SIGNAL(activated(const QModelIndex &)),
                this, SLOT(itemActivated(const QModelIndex &)));
            mMailboxListWidget->setClampingStyle(HbScrollArea::BounceBackClamping);
            mMailboxListWidget->setFrictionEnabled(true);
        }
        else {
            NM_ERROR(1,"nmailui: mailboxlistview: list object loading failed");
        }
    }
    else {
         NM_ERROR(1,"nmailui: mailboxlistview: resource loading failed");
    }
}

/*!
    Lazy loading when view layout has been loaded
*/
void NmMailboxListView::viewReady()
{
    NM_FUNCTION;
    
    if (!mViewReady){
        // Set title
        setTitle(hbTrId("txt_mail_title_mail"));
        // Refresh list
        QMetaObject::invokeMethod(this, "refreshList", Qt::QueuedConnection); 
        mViewReady=true;
    }
}

/*!
    Reload view contents with new start parameters
    Typically when view is already open and external view activation occurs
    for this same view
*/
void NmMailboxListView::reloadViewContents(NmUiStartParam* startParam)
{
    NM_FUNCTION;
    
    // Check start parameter validity.
    if (startParam&&startParam->viewId()==NmUiViewMailboxList) {
        // Delete existing start parameter data
        delete mStartParam;
        mStartParam=NULL;
        // Store new start parameter data
        mStartParam=startParam;
        // Refresh model
        refreshList();
    }
    else {
        NM_ERROR(1,"nmailui: mailboxlistview: invalid start parameter");
        // Unused start parameter needs to be deleted
        delete startParam;
        startParam = NULL;
    }
}

/*!
    View id
*/
NmUiViewId NmMailboxListView::nmailViewId() const
{
    NM_FUNCTION;
    
    return NmUiViewMailboxList;
}

/*!
    Refresh list
*/
void NmMailboxListView::refreshList()
{
    NM_FUNCTION;
    
	if (mMailboxListWidget) {
        mMailboxListWidget->setModel(&mListModel);
	}
}

/*!
    Item activated slot
*/
void NmMailboxListView::itemActivated(const QModelIndex &index)
{
    NM_FUNCTION;
    
    mActivatedIndex = index;
    QMetaObject::invokeMethod(this, "openSelectedMailBox", Qt::QueuedConnection);
}


/*!
    Open selected mailbox
*/
void NmMailboxListView::openSelectedMailBox()
{
    NM_FUNCTION;
    
    // Get mailbox meta data
    NmMailboxMetaData *mailbox =
        mListModel.data(mActivatedIndex, Qt::DisplayRole).value<NmMailboxMetaData*>();
    if (mailbox) {
        // Get standard folder inbox id
        NmId inboxId = mUiEngine.standardFolderId(mailbox->id(), NmFolderInbox);
        // Create start params and launch message list view
        NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageList,mailbox->id(),inboxId);
        mApplication.enterNmUiView(startParam);
    }
}



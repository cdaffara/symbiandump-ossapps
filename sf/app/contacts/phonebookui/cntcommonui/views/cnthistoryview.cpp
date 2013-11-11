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

#include "cnthistoryview.h"

#include <hblistview.h>
#include <hbgroupbox.h>
#include <hbdocumentloader.h>
#include <hbmenu.h>
#include <hbview.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <xqappmgr.h>
#include <xqservicerequest.h>
#include <cnthistorymodel.h>
#include <hbparameterlengthlimiter.h>
#include <hbmainwindow.h>
#include <hbframebackground.h>
#include <hbabstractviewitem.h>
#include <QTimer>
#include <qtcontacts.h>

#include "cnthistoryviewitem.h"
#include "cntglobal.h"
#include "cntdebug.h"

const char *CNT_HISTORYVIEW_XML = ":/xml/contacts_history.docml";

/*!
Constructor, initialize member variables.
*/
CntHistoryView::CntHistoryView() : 
    mHistoryListView(NULL),
    mHistoryModel(NULL),
    mDocumentLoader(NULL),
    mViewMgr(NULL),
    mContact(NULL),
    mRequest(NULL)
{
    bool ok = false;
    
    docLoader()->load(CNT_HISTORYVIEW_XML, &ok);
    
    if (ok)
    {
        mView = static_cast<HbView*>(docLoader()->findWidget(QString("view")));
        
        // Create a back key action an set it as the default navigation
        // action once the back key is pressed
        mBackKey = new HbAction(Hb::BackNaviAction, mView);
        mView->setNavigationAction(mBackKey);        
        connect(mBackKey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
        
        HbMenu* viewMenu = static_cast<HbMenu*>(docLoader()->findObject("viewMenu") );
        viewMenu->setParent(mView);
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_history.docml");
    }
}

/*!
Destructor
*/
CntHistoryView::~CntHistoryView()
{
    mView->deleteLater();
    
    if (mDocumentLoader) {
        delete mDocumentLoader;
        mDocumentLoader = NULL;
    }
    if (mHistoryModel) {
        delete mHistoryModel;
        mHistoryModel = NULL;
    }
    if (mContact) {
        delete mContact;
        mContact = NULL;
    }
    
    delete mRequest;
    mRequest = NULL;
}

/*!
 * Deactivate the view
 */
void CntHistoryView::deactivate()
{
    QContactManager* cm = &mEngine->contactManager(SYMBIAN_BACKEND);
    disconnect(cm, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
            this, SLOT(contactDeletedFromOtherSource(const QList<QContactLocalId>&)));
}

/**
 * Activate the view
 */
void CntHistoryView::activate( const CntViewParameters aArgs )
{
    mArgs = aArgs;
    mViewMgr = &mEngine->viewManager();
    
    QContactManager* cm = &mEngine->contactManager(SYMBIAN_BACKEND);
    mContact = new QContact(mArgs.value(ESelectedContact).value<QContact>());
    
    // Set history view heading
    HbGroupBox* groupBox = static_cast<HbGroupBox*>(docLoader()->findWidget(QString("groupBox")));
    QString name = mContact->displayLabel();
    if (name.isEmpty())
    {
        name = hbTrId("txt_phob_list_unnamed");
    }
    groupBox->setHeading(hbTrId("txt_phob_subtitle_history_with_1").arg(name));
    
    //construct listview
    mHistoryListView = static_cast<HbListView*>(docLoader()->findWidget(QString("listView")));
    mHistoryListView->setLayoutName("history");
    CntHistoryViewItem *item = new CntHistoryViewItem;
    item->setSecondaryTextRowCount(1, 3);
    item->setGraphicsSize(HbListViewItem::SmallIcon);
    mHistoryListView->setItemPrototype(item); //ownership is taken
    
    // Connect listview items to respective slots
    connect(mHistoryListView, SIGNAL(activated(const QModelIndex &)),
                      this,  SLOT(itemActivated(const QModelIndex &)));
    connect( mHistoryListView, SIGNAL(longPressed(HbAbstractViewItem*,const QPointF&)), 
            this, SLOT(onLongPressed(HbAbstractViewItem*,const QPointF&)) );
         
    
    
    mHistoryModel = new CntHistoryModel(mContact->localId(), cm);
    mHistoryListView->setModel(mHistoryModel); //ownership is not taken
    
    //start listening to the events amount changing in the model
    connect(mHistoryModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(updateScrollingPosition()));
    connect(mHistoryModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(showClearHistoryMenu()));
    connect(mHistoryModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        this, SLOT(updateScrollingPosition()));
    connect(mHistoryModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
        this, SLOT(showClearHistoryMenu()));
    connect(mHistoryModel, SIGNAL(layoutChanged()),
        this, SLOT(updateScrollingPosition()));
    connect(mHistoryModel, SIGNAL(modelReset()),
        this, SLOT(updateScrollingPosition()));
    connect(mHistoryModel, SIGNAL(modelReset()),
        this, SLOT(showClearHistoryMenu()));
    
    // Connect the menu options to respective slots
    mClearHistory = static_cast<HbAction*>(docLoader()->findObject("cnt:clearhistory"));
    mClearHistory->setParent(mView);
    connect(mClearHistory, SIGNAL(triggered()), this, SLOT(clearHistory()));
    showClearHistoryMenu();
    
    connect(cm, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), 
        this, SLOT(contactDeletedFromOtherSource(const QList<QContactLocalId>&)));
}

/*!
Called after new items are added to or removed from comm history view
*/
void CntHistoryView::updateScrollingPosition()
{
    int rowCnt = mHistoryModel->rowCount();
    
    // Scroll to the last item in the list
    mHistoryListView->scrollTo(mHistoryModel->index(rowCnt - 1, 0), 
        HbAbstractItemView::PositionAtBottom);
}

/*
Query the user for clearing communications history
*/
void CntHistoryView::clearHistory()
{
    // Ask the use if they want to clear the history
    QString name = mContact->displayLabel();
    if (name.isEmpty())
    {
        name = hbTrId("txt_phob_list_unnamed");
    }
    
    HbMessageBox::question(HbParameterLengthLimiter(hbTrId("txt_phob_info_clear_communications_history_with_1")).arg(name), this, 
            SLOT(handleClearHistory(int)), HbMessageBox::Delete | HbMessageBox::Cancel);
}

/*
Handle the selected action for clearing history
*/
void CntHistoryView::handleClearHistory(int action)
{
    if (action == HbMessageBox::Delete)
    {
        mHistoryModel->clearHistory();
    }
}

void CntHistoryView::onLongPressed(HbAbstractViewItem *item, const QPointF &coords)
{
    Q_UNUSED(coords);
    QModelIndex index = item->modelIndex();
    if (index.isValid())
    {
        itemActivated(index);
    }
}

/*!
Once list item is pressed on the list view this slot handles the 
emitted signal
 */
void CntHistoryView::itemActivated(const QModelIndex &index)
{
    int flags = index.data(CntFlagsRole).toInt();
    
    bool createRequest(false);
    
    QString interface;
    QString operation;
    QVariantList args;
    
    // If the list item is a call log a call is made to that item
    if ( flags & CntCallLog ) {
        // Make a call
        QVariant number = index.data(CntPhoneNumberRole);
        
        if ( number.isValid() ) {        
            interface = "com.nokia.symbian.ICallDial";
            operation = "dial(QString)";
            args << number;
            createRequest = true;
            
            // TODO: Using XQApplicationManager is not working with calls
            // The factory method cannot create a request. Find out why
            createRequest = false;
            QString service("com.nokia.symbian.ICallDial");
            QString type("dial(QString,int)");
            XQServiceRequest snd(interface, operation, false);
            snd << number.toString() << mContact->localId();
            snd.send();
        }
        
    } else if ( flags & CntMessage ) {
        // Open conversation view
        QVariant id = index.data(CntConverstaionIdRole);
        
        if ( id.isValid() ) {
            interface = "com.nokia.symbian.IMessageView";
            operation = "view(int)";
            args << id;
            createRequest = true;
        }
    }
    
    if ( createRequest ) {
        XQApplicationManager appMng;
    
        delete mRequest;
        mRequest = NULL;
        mRequest = appMng.create(interface, operation, true); // embedded
        
        if ( mRequest ) {
            mRequest->setArguments(args); 
            mRequest->send();
        }
    }
}

/*!
Go back to previous view
*/
void CntHistoryView::showPreviousView()
{
    QVariant var;
    var.setValue(*mContact);
    mArgs.insert(ESelectedContact, var);
    mViewMgr->back( mArgs );
}

/*!
Go back to the root view
*/
void CntHistoryView::showRootView()
{   
    mViewMgr->back( mArgs, true );
}


void CntHistoryView::contactDeletedFromOtherSource(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    if ( contactIds.contains(mContact->localId()) )
    {
        // Do not switch to the previous view immediately. List views are
        // not updated properly if this is not done in the event loop
        QTimer::singleShot(0, this, SLOT(showRootView()));
    }
    CNT_EXIT
}

/*!
Show or hide the clear history menu
*/
void CntHistoryView::showClearHistoryMenu()
{
    if (mHistoryModel->rowCount() > 0) {
        mClearHistory->setEnabled(true);
    } else {
        mClearHistory->setEnabled(false);
    }
}

/*!
 * Document loader
 */
HbDocumentLoader* CntHistoryView::docLoader()
{
    if (!mDocumentLoader) {
        mDocumentLoader = new HbDocumentLoader();
    }
    return mDocumentLoader;
}

// end of file

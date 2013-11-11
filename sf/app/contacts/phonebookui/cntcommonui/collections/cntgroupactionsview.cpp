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

#include "cntgroupactionsview.h"
#include "cntactionlauncher.h"
#include "cntglobal.h"

#include <cntactionpopup.h>

#include <hblistview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbview.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbicon.h>
#include <hbgroupbox.h>
#include <cntlistmodel.h>
#include <hbframebackground.h>
#include <hbnumbergrouping.h>
#include <QKeyEvent> 
#include <qtcontacts.h>
#include <hbmainwindow.h>
#include "cntactionlauncher.h"
#include <QEvent>
#include <QStandardItemModel>

const char *CNT_GROUPACTIONSVIEW_XML = ":/xml/contacts_groupactions.docml";

CntGroupActionsView::CntGroupActionsView() :
mGroupContact(NULL),
mModel(NULL),
mViewManager(NULL),
mListView(NULL),
mPopupCount(0),
mIsExecutingAction(false)
{
    bool ok = false;
    mDocumentLoader.load(CNT_GROUPACTIONSVIEW_XML, &ok);
  
    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_groupactions.docml");
    }
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    // menu actions
    mEditGrpDetailAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:editgroupdetail"));
    connect(mEditGrpDetailAction, SIGNAL(triggered()), this, SLOT(editGroup()));
}

/*!
Destructor
*/
CntGroupActionsView::~CntGroupActionsView()
{
    mView->deleteLater();
    
    delete mEditGrpDetailAction; 
    mEditGrpDetailAction = NULL;
    
    delete mGroupContact;
    mGroupContact = NULL;
    
    delete mModel;
    mModel = NULL;
}


void CntGroupActionsView::editGroup()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EViewId, groupEditorView);
    QVariant var;
    var.setValue(*mGroupContact);
    viewParameters.insert(ESelectedGroupContact, var);
    mViewManager->changeView(viewParameters);

}


/*!
Activates a previous view
*/
void CntGroupActionsView::showPreviousView()
{
    CntViewParameters viewParameters;
    QVariant var;
    var.setValue(*mGroupContact);
    viewParameters.insert(ESelectedGroupContact, var);
    mViewManager->back(viewParameters);
}

void CntGroupActionsView::deactivate()
{
    
}

/*
Activates a default view and setup name label texts
*/
void CntGroupActionsView::activate( const CntViewParameters aArgs )
{
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);   
    
    mGroupContact = new QContact(aArgs.value(ESelectedGroupContact).value<QContact>());
    mViewManager = &mEngine->viewManager();

    QContactName contactName = mGroupContact->detail( QContactName::DefinitionName );
    QString groupName = contactName.value( QContactName::FieldCustomLabel );        
    if (groupName.isEmpty())
    {
        groupName = hbTrId("txt_phob_list_unnamed");
    }
    
    //group box
    HbGroupBox* groupBox = static_cast<HbGroupBox *>(mDocumentLoader.findWidget(QString("groupBox")));
    groupBox->setHeading(groupName);
    
    // create list & model
    mListView = static_cast<HbListView*> (mDocumentLoader.findWidget("listView"));
    mListView->setUniformItemSizes(true);
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    
    mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::LargeIcon);
    mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
    
    
    mModel = new QStandardItemModel();
    
    QContactPhoneNumber confCallNumber = mGroupContact->detail<QContactPhoneNumber>();
    if(!confCallNumber.number().isEmpty())
    {
        /*
         * Internationalization support, activate the following code 
         * when support available from Orbit
         */
        //populatelist(hbTrId("txt_phob_dblist_conference_call"), HbIcon("qtg_large_call_group"),HbNumberGrouping::formatPhoneNumber(confCallNumber.number()));
        populatelist(hbTrId("txt_phob_dblist_conference_call"), HbIcon("qtg_large_call_group"),confCallNumber.number(),"call");
    }
      
    populatelist(hbTrId("txt_phob_dblist_send_message"),HbIcon("qtg_large_message_group"),hbTrId("txt_phob_dblist_send_message_val_members"),"message");
    populatelist(hbTrId("txt_phob_dblist_email"),HbIcon("qtg_large_email_group"),hbTrId("txt_phob_dblist_send_message_val_members"),"email");
    
    mListView->setModel(mModel);
    mListView->setSelectionMode(HbAbstractItemView::NoSelection);
    
    connect(mListView, SIGNAL(activated(const QModelIndex&)),
                this, SLOT(listItemSelected(const QModelIndex&)));
    
    connect( mListView, SIGNAL(longPressed(HbAbstractViewItem*,const QPointF&)), this, SLOT(onLongPressed(HbAbstractViewItem*,const QPointF&)) );
    
    HbMainWindow* window = mView->mainWindow();
    if (window)
    {
        window->installEventFilter(this);
    }
}

void CntGroupActionsView::populatelist(QString primaryText,HbIcon icon,QString secondaryText,QString action)
{
    QList<QStandardItem*> items;
    QStandardItem *labelItem = new QStandardItem();
    
    QStringList textList;
   
    textList << primaryText << secondaryText;
    
    labelItem->setData(textList, Qt::DisplayRole);
    labelItem->setData(icon, Qt::DecorationRole);
    labelItem->setData(action, Qt::UserRole+1);
    
    items << labelItem ;
    mModel->appendRow(items);
}

void CntGroupActionsView::onLongPressed(HbAbstractViewItem *item, const QPointF &coords)
{
    Q_UNUSED(coords);
    QModelIndex index = item->modelIndex();
    listItemSelected(index);
}

void CntGroupActionsView::listItemSelected(const QModelIndex &index)
{
    if (index.isValid()) {
        //reset flags
        mPopupCount=0;
        mEmailActionParams.clear();
        mMessageActionParams.clear();
        
        QString action = mModel->item(index.row())->data(Qt::UserRole+1).toString();
        QContactManager& mgr = mEngine->contactManager( SYMBIAN_BACKEND );
        
        //conference call
        if (action.compare("call", Qt::CaseInsensitive) == 0 ) {
            CntActionLauncher* other = new CntActionLauncher( mgr, action);
            connect(other, SIGNAL(actionExecuted(CntActionLauncher*)), this, SLOT(actionExecuted(CntActionLauncher*)));
            other->execute(*mGroupContact, QContactDetail());
        }
        //group email, message
        else {
            QContactRelationshipFilter relationshipFilter;
            relationshipFilter.setRelationshipType(QContactRelationship::HasMember);
            relationshipFilter.setRelatedContactRole(QContactRelationship::First);
            relationshipFilter.setRelatedContactId(mGroupContact->id());   
            QList<QContactLocalId> groupMembers = mgr.contactIds(relationshipFilter);
            
            for (int i = 0;i<groupMembers.count();i++) {
                QContact contact = mgr.contact(groupMembers.at(i));
                QContactDetail preferredDetail = contact.preferredDetail(action);
                //use preferred detail if exits
                if (!preferredDetail.isEmpty()) {
                    if(action.compare("message", Qt::CaseInsensitive) == 0) {
                        QContactPhoneNumber phoneNumber = contact.detail<QContactPhoneNumber>();
                        mMessageActionParams.insert(phoneNumber.number(),QVariant(contact.displayLabel()));
                    }
                    else {
                        QContactEmailAddress email = contact.detail<QContactEmailAddress>();
                        mEmailActionParams.append(email.emailAddress());
                    }
                }
                else {
                    CntActionPopup *actionPopup = new CntActionPopup(&contact);
                    if(actionPopup->showActionPopup(action)) {
                        //increment actionpopup counter
                        mPopupCount++;
                        
                        connect( actionPopup, SIGNAL(executeContactAction(QContact&, QContactDetail, QString)), this, 
                                                    SLOT(executeAction(QContact&, QContactDetail, QString)));
                        connect(actionPopup, SIGNAL(actionPopupCancelPressed()), this, SLOT(actionCancelled()));
                    }
                    else {
                        delete actionPopup;
                    }
                }
            }
            //no popup dialog, execute action
            if (mPopupCount==0) {
                QVariantMap map;
                QVariant params;
                if (action.compare("message", Qt::CaseInsensitive) == 0) {
                    params.setValue(mMessageActionParams);
                }
                else if (action.compare("email", Qt::CaseInsensitive) == 0) {
                    params.setValue(mEmailActionParams);
                }
                map.insert(action,params);
                CntActionLauncher* other = new CntActionLauncher(mgr, action);
                connect(other, SIGNAL(actionExecuted(CntActionLauncher*)), this, SLOT(actionExecuted(CntActionLauncher*)));
                other->execute(*mGroupContact, QContactDetail(), map);
            }
        }
    }
}

void CntGroupActionsView::executeAction(QContact& contact, QContactDetail detail, QString action)
{
    Q_UNUSED(contact);
    
    if (action.compare("message", Qt::CaseInsensitive) == 0)
    {
        QContactPhoneNumber phoneNumber = static_cast<QContactPhoneNumber>(detail);
        mMessageActionParams.insert(phoneNumber.number(),QVariant(contact.displayLabel()));
    }
    else if (action.compare("email", Qt::CaseInsensitive) == 0)
    {
        QContactEmailAddress email = static_cast<QContactEmailAddress>(detail);
        mEmailActionParams.append(email.emailAddress());
    }
    
    if (contact.preferredDetail(action).isEmpty())
    {
        contact.setPreferredDetail(action, detail);
       //return value will be ignored because we cannot do anything if it fails.
       mEngine->contactManager(SYMBIAN_BACKEND).saveContact(&contact);
    }
    
    //actionpopup executed, decrement counter
    mPopupCount--;
    if (mPopupCount==0)
    {
        QVariantMap map;
        QVariant params;
        if (action.compare("message", Qt::CaseInsensitive) == 0)
        {
            params.setValue(mMessageActionParams);
        }
        else if (action.compare("email", Qt::CaseInsensitive) == 0)
        {
            params.setValue(mEmailActionParams);
        }
        map.insert(action,params);

        CntActionLauncher* other = new CntActionLauncher( mEngine->contactManager(SYMBIAN_BACKEND), action);
        connect(other, SIGNAL(actionExecuted(CntActionLauncher*)), this, SLOT(actionExecuted(CntActionLauncher*)));
        other->execute(*mGroupContact, QContactDetail(), map);
    }
}

bool CntGroupActionsView::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && obj == mView->mainWindow())
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Yes)
        {
            return sendKeyPressed();
        }
    }
    return false;
}

bool CntGroupActionsView::sendKeyPressed()
{   
    QList<QContactPhoneNumber> numberDetails = mGroupContact->details<QContactPhoneNumber>();
    bool keyConsumed = false;
        
    // check if conference call number is present
    if (!numberDetails.isEmpty()) 
    { 
        executeConferenceCallAction( *mGroupContact, mGroupContact->details<QContactPhoneNumber>().first(), "call");
        
        keyConsumed = true;
    }
    
    return keyConsumed;
}

void CntGroupActionsView::executeConferenceCallAction(QContact& aContact, const QContactDetail& aDetail, const QString& aAction)
{
    if (mIsExecutingAction)
    {
        return;
    }
    else
    {
        mIsExecutingAction = true;
    }
    
    CntActionLauncher* other = new CntActionLauncher( mEngine->contactManager(SYMBIAN_BACKEND), aAction);
    connect(other, SIGNAL(actionExecuted(CntActionLauncher*)), this, SLOT(actionExecuted(CntActionLauncher*)));
    other->execute(aContact, aDetail);  
}

void CntGroupActionsView::actionCancelled()
{
    //actionpopup cancelled, decrement counter
    mPopupCount--;
    
    if (mPopupCount==0)
    {
        QVariantMap map;
        QVariant params;
        QString action;
        if (mMessageActionParams.count())
        {
            params.setValue(mMessageActionParams);
            action = "message";
        }
        else if (mEmailActionParams.count())
        {
            params.setValue(mEmailActionParams);
            action = "email";
        }
        else
        {
            return;
        }
        map.insert(action,params);

        CntActionLauncher* other = new CntActionLauncher( mEngine->contactManager(SYMBIAN_BACKEND), action);
        connect(other, SIGNAL(actionExecuted(CntActionLauncher*)), this, SLOT(actionExecuted(CntActionLauncher*)));
        other->execute(*mGroupContact, QContactDetail(), map);
    }
}


void CntGroupActionsView::actionExecuted(CntActionLauncher* aAction)
{
    //cleanup
    aAction->deleteLater();
    mIsExecutingAction = false;
}


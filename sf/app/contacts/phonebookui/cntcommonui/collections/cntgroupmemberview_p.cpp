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

#include "cntgroupmemberview_p.h"
#include "cntfetchcontactpopup.h"
#include "cntcontactcardheadingitem.h"
#include "cntglobal.h"
#include "cntimagelabel.h"
#include "cntimageutility.h"
#include "cntthumbnailmanager.h"
#include <hbnotificationdialog.h>
#include <hbmessagebox.h>
#include <hbmenu.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbframebackground.h>
#include <hbindexfeedback.h>
#include <hbscrollbar.h>
#include <hbview.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbparameterlengthlimiter.h>
#include <cntlistmodel.h>
#include <hbmainwindow.h>
#include <xqservicerequest.h>

#include "cntdocumentloader.h"

const char *CNT_GROUPMEMBERVIEW_XML = ":/xml/contacts_groupmembers.docml";

/*!
\class CntGroupMemberViewPrivate
\brief

This is the group members view class that shows list of contacts for a user group. View contains a listview that shows actual contacts that
have been added to a particular group. There is also toolbar and menu for navigating between different views. Instance of this class is
created by our viewmanager but view itself is owned by the mainwindow which will also delete it in the end.

*/

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)

*/
CntGroupMemberViewPrivate::CntGroupMemberViewPrivate() :
    mGroupContact(NULL),
    mViewManager(NULL),
    mHeadingItem(NULL),
    mModel(NULL),
    mImageLabel(NULL), 
    mListView(NULL),
    mAvatar(NULL)
{
    mDocument = new CntDocumentLoader;
    
    bool ok;
    mDocument->load( CNT_GROUPMEMBERVIEW_XML, &ok );
    if ( !ok ){
        qFatal( "Unable to load %S", CNT_GROUPMEMBERVIEW_XML );
    }

    mView = static_cast<HbView*>( mDocument->findWidget("view") );
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    mImageLabel = static_cast<CntImageLabel*>(mDocument->findWidget("imageLabel"));
    connect( mImageLabel, SIGNAL(iconClicked()), this, SLOT(openImageEditor()) );
    connect( mImageLabel, SIGNAL(iconLongPressed(const QPointF&)), this, SLOT(drawImageMenu(const QPointF&)) );
    
    mListView = static_cast<HbListView*>( mDocument->findWidget("listView") );
    connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
        SLOT(showContextMenu(HbAbstractViewItem*,QPointF)));
    connect(mListView, SIGNAL(activated (const QModelIndex&)), this,
        SLOT(showContactView(const QModelIndex&)));
    
    mHeadingItem = static_cast<CntContactCardHeadingItem*>( mDocument->findWidget("groupHeading") );
    connect(mHeadingItem, SIGNAL(passShortPressed(const QPointF&)), this, SLOT(openImageEditor()) );
    connect(mHeadingItem, SIGNAL(passLongPressed(const QPointF&)), this, SLOT(drawImageMenu(const QPointF&)));

    // menu actions
    mEditGroupAction = static_cast<HbAction*>( mDocument->findObject("cnt:editgroupdetails"));
    connect(mEditGroupAction, SIGNAL(triggered()), this, SLOT(editGroup()));
    
    // toolbar actions
    mManageAction = static_cast<HbAction*>( mDocument->findObject("cnt:managemembers"));
    connect(mManageAction, SIGNAL(triggered()), this, SLOT(manageMembers()));
    mDeleteAction = static_cast<HbAction*>( mDocument->findObject("cnt:deletegroup"));
    connect(mDeleteAction, SIGNAL(triggered()), this, SLOT(deleteGroup()));
    mShowActionsAction = static_cast<HbAction*>( mDocument->findObject("cnt:groupactions"));
    connect(mShowActionsAction, SIGNAL(triggered()), this, SLOT(openGroupActions()));
    
    //Always disable by default. 
    //Only enable, when atleast one contact contains a CommunicationAddress defined. 
    mShowActionsAction->setDisabled(true);
}

/*!
Destructor
*/
CntGroupMemberViewPrivate::~CntGroupMemberViewPrivate()
{
    mView->deleteLater();
    
    delete mGroupContact;
    mGroupContact = NULL;
    
    delete mModel;
    mModel = NULL;
    
    delete mAvatar;
    mAvatar = NULL;
}

void CntGroupMemberViewPrivate::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Vertical) 
    {
        // reading "portrait" section
        mDocument->load( CNT_GROUPMEMBERVIEW_XML, "portrait" );
    } 
    else 
    {
        // reading "landscape" section
        mDocument->load( CNT_GROUPMEMBERVIEW_XML, "landscape" );
    }
}

void CntGroupMemberViewPrivate::activate( const CntViewParameters aArgs )
{
    mArgs = aArgs;
    mViewManager = &mEngine->viewManager();
    mThumbnailManager = &mEngine->thumbnailManager();

    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void*, int, int)),
            this, SLOT(thumbnailReady(QPixmap, void*, int, int)));
        
    if (mView->navigationAction() != mSoftkey)
        {
        mView->setNavigationAction(mSoftkey);   
        }
        
    QVariant contact = mArgs.value( ESelectedGroupContact );
    mGroupContact = new QContact( contact.value<QContact>() );
    
    HbMainWindow* window = mView->mainWindow();
    if ( window )
    {
        connect(window, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(setOrientation(Qt::Orientation)));
        setOrientation(window->orientation());
    }
    
    mHeadingItem->setGroupDetails(mGroupContact);

    // avatar
    QList<QContactAvatar> details = mGroupContact->details<QContactAvatar>();
    for (int i = 0;i < details.count();i++)
    {
        if (details.at(i).imageUrl().isValid())
            {
            mAvatar = new QContactAvatar(details.at(i));
            mThumbnailManager->getThumbnail(ThumbnailManager::ThumbnailLarge, mAvatar->imageUrl().toString());
            break;
            }
    }
    
    // create list & model
    mListView->setFrictionEnabled(true);
    mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    mListView->verticalScrollBar()->setInteractive(true);
    mListView->setUniformItemSizes(true);
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    HbListViewItem* prototype = mListView->listItemPrototype();
    prototype->setDefaultFrame(frame);
    
    HbIndexFeedback *indexFeedback = new HbIndexFeedback(mView);
    indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    indexFeedback->setItemView(mListView);
    
    mListView->setLayoutName("groupmemberlist");

    // if no contacts are present, then disable the Manage Members toolbar 
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    filter.setValue(QLatin1String(QContactType::TypeContact));
   
    QList<QContactLocalId> contactIds = getContactManager()->contactIds(filter);   
    if (contactIds.isEmpty())
    {
       mManageAction->setEnabled(false); 
    }      
    
    createModel();
    
    if (mArgs.value(ESelectedAction).toString() == CNT_SAVE_ACTION)
    {
        QString name = getContactManager()->synthesizedContactDisplayLabel(*mGroupContact);
        HbNotificationDialog::launchDialog(HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_new_group_1_created").arg(name)));
    }
    decideActionButtonContext();
}

void CntGroupMemberViewPrivate::deactivate()
{
}

void CntGroupMemberViewPrivate::showPreviousView()
{
    Q_Q(CntGroupMemberView);
    
    emit q->backPressed();
    
    //save the contact if avatar has been changed.
    QContact contact = getContactManager()->contact(mGroupContact->localId());
    if ( contact != *mGroupContact )
    {
        getContactManager()->saveContact(mGroupContact);
    }
    mViewManager->back(mArgs);
}

void CntGroupMemberViewPrivate::openGroupActions()
{
    QVariant var;
    var.setValue(*mGroupContact);
    
    mArgs.insert(EViewId, groupActionsView);
    mArgs.insert(ESelectedGroupContact, var);
    mViewManager->changeView(mArgs);
}

void CntGroupMemberViewPrivate::manageMembers()
{
    QContactRelationshipFilter membersFilter;
    membersFilter.setRelationshipType(QContactRelationship::HasMember);
    membersFilter.setRelatedContactRole(QContactRelationship::First);
    membersFilter.setRelatedContactId(mGroupContact->id());   
    
    mOriginalGroupMembers = getContactManager()->contactIds(membersFilter);
    
    QContactName contactName = mGroupContact->detail( QContactName::DefinitionName );
    QString groupName = contactName.value( QContactName::FieldCustomLabel );
    if (groupName.isEmpty())
    {
        groupName = hbTrId("txt_phob_list_unnamed");
    }
    
    CntFetchContactPopup* popup = CntFetchContactPopup::createMultiSelectionPopup(
            HbParameterLengthLimiter(hbTrId("txt_phob_title_members_of_1_group")).arg(groupName),
            hbTrId("txt_common_button_save"),
            mEngine->contactManager(SYMBIAN_BACKEND));
    connect( popup, SIGNAL(fetchReady(QSet<QContactLocalId>)),this, SLOT(handleManageMembers(QSet<QContactLocalId>)) );
    popup->setSelectedContacts( mOriginalGroupMembers.toSet() );
    popup->showPopup();
}

void CntGroupMemberViewPrivate::handleManageMembers( QSet<QContactLocalId> aIds )
{
    QList<QContactRelationship> removedMemberships;
    QList<QContactRelationship> addedMemberships;

    QSet<QContactLocalId> removedMembers = mOriginalGroupMembers.toSet() - aIds;
    setRelationship(removedMembers, removedMemberships);

    QSet<QContactLocalId> addedMembers = aIds - mOriginalGroupMembers.toSet();
    setRelationship(addedMembers, addedMemberships);
    
    if (!addedMemberships.isEmpty()) 
    {
        getContactManager()->saveRelationships(&addedMemberships, NULL);
    }
    
    if (!removedMemberships.isEmpty()) 
    {
        getContactManager()->removeRelationships(removedMemberships, NULL);
    }
    
    decideActionButtonContext();
}

void CntGroupMemberViewPrivate::createModel()
{
    QContactRelationshipFilter rFilter;
    rFilter.setRelationshipType(QContactRelationship::HasMember);
    rFilter.setRelatedContactRole(QContactRelationship::First);
    rFilter.setRelatedContactId(mGroupContact->id());
    
    mModel = new CntListModel(getContactManager(), rFilter, false);
    mListView->setModel(mModel);
}

void CntGroupMemberViewPrivate::editGroup()
{
    mArgs.insert(EViewId, groupEditorView);
    QVariant var;
    var.setValue(*mGroupContact);
    mArgs.insert(ESelectedGroupContact, var);
    mViewManager->changeView(mArgs);
}

void CntGroupMemberViewPrivate::deleteGroup()
{
    QContactName contactName = mGroupContact->detail( QContactName::DefinitionName );
    QString groupName = contactName.value( QContactName::FieldCustomLabel );
    if (groupName.isNull())
    {
        groupName = hbTrId("txt_phob_list_unnamed");
    }
    
    HbLabel *headingLabel = new HbLabel();
    headingLabel->setPlainText(HbParameterLengthLimiter(hbTrId("txt_phob_dialog_delete_1_group")).arg(groupName));
    
    HbMessageBox::question(hbTrId("txt_phob_dialog_only_group_will_be_removed_contac"), this, SLOT(handleDeleteGroup(int)),
            HbMessageBox::Delete | HbMessageBox::Cancel, headingLabel);
}

void CntGroupMemberViewPrivate::handleDeleteGroup(int action)
{
    if (action == HbMessageBox::Delete)
    {
        getContactManager()->removeContact(mGroupContact->localId());
        showPreviousView();
    }
}

/*!
Called when a list item is longpressed
*/
void CntGroupMemberViewPrivate::showContextMenu(HbAbstractViewItem *aItem, const QPointF &aCoords)
{
    QVariant data( aItem->modelIndex().row() );
   
    QModelIndex index = aItem->modelIndex();

    HbMenu *menu = new HbMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setPreferredPos( aCoords );
    
    HbAction *removeFromGroupAction = 0;
    HbAction *openContactAction = 0;
    HbAction *editContactAction = 0;
    HbAction *sendToHsAction = 0;

    openContactAction = menu->addAction(hbTrId("txt_common_menu_open"));
    editContactAction = menu->addAction(hbTrId("txt_common_menu_edit"));
    removeFromGroupAction = menu->addAction(hbTrId("txt_phob_menu_remove_from_group"));
    sendToHsAction = menu->addAction(hbTrId("txt_phob_menu_send_to_homescreen"));
    
    openContactAction->setData( data );
    editContactAction->setData( data );
    removeFromGroupAction->setData( data );
    sendToHsAction->setData( data );

    menu->open(this, SLOT(handleMenu(HbAction*)));
}

void CntGroupMemberViewPrivate::handleMenu(HbAction* action)
{
    int row = action->data().toInt();
    HbMenu *menuItem = static_cast<HbMenu*>(sender());
    QModelIndex index = mModel->index(row);
    
    if ( action == menuItem->actions().first() )
        {
        showContactView(index);
        }
    else if (action == menuItem->actions().at(1))
        {
        editContact(index);
        }
    else if (action == menuItem->actions().at(2))
        {
        removeFromGroup(index);
        }
    else if (action == menuItem->actions().at(3))
        {
        sendToHs(index);
        }
}

/*!
Called after user clicked on the listview.
*/
void CntGroupMemberViewPrivate::sendToHs(const QModelIndex &index)
{
    QVariantHash preferences;
    preferences["contactId"] = mModel->contact(index).id().localId();
    
    XQServiceRequest snd("com.nokia.symbian.IHomeScreenClient",
                         "addWidget(QString,QVariantHash)",
                         false);
    snd << QString("contactwidgethsplugin");
    snd << preferences;
    snd.send();
}

/*!
Called after user clicked on the listview.
*/
void CntGroupMemberViewPrivate::showContactView(const QModelIndex &index)
{
    mArgs.insert(EViewId, contactCardView);
    QVariant var;
    var.setValue(mModel->contact(index));
    mArgs.insert(ESelectedContact, var);
    QVariant varGroup;
    varGroup.setValue(*mGroupContact);
    mArgs.insert(ESelectedGroupContact, varGroup);
    mViewManager->changeView(mArgs);
        
}

void CntGroupMemberViewPrivate::removeFromGroup(const QModelIndex &index)
{
    // get contact id using index
    QContact selectedContact = mModel->contact(index);
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(mGroupContact->id());
    relationship.setSecond(selectedContact.id());
    getContactManager()->removeRelationship(relationship);
    decideActionButtonContext();
}

void CntGroupMemberViewPrivate::editContact(const QModelIndex &index)
{

    mArgs.insert(EViewId, editView);
    QVariant var;
    var.setValue(mModel->contact(index));
    mArgs.insert(ESelectedContact, var);
    mViewManager->changeView(mArgs);
}

void CntGroupMemberViewPrivate::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(data);
    Q_UNUSED(id);
    Q_UNUSED(error);
    if (!error)
    {
        HbIcon icon(pixmap);
        mHeadingItem->setIcon(icon);
        
        mImageLabel->clear();
        mImageLabel->setIcon(pixmap);
    }
}

void CntGroupMemberViewPrivate::openImageEditor()
{
    QVariant var;
    var.setValue(*mGroupContact);
       
    // only group contact is assigned since we want to only
    // change the groups image
    mArgs.insert(ESelectedGroupContact, var);
    
    mArgs.insert(EViewId, imageEditorView );
        
    mViewManager->changeView( mArgs );
}

QContactManager* CntGroupMemberViewPrivate::getContactManager()
{
    return &mEngine->contactManager(SYMBIAN_BACKEND);
}

void CntGroupMemberViewPrivate::setRelationship(QSet<QContactLocalId>        &aLocalIds,
                                         QList<QContactRelationship>  &aRelationshipList)
{
    foreach (QContactLocalId localId, aLocalIds) {
		QContactId id;
		id.setLocalId(localId);
        QContactRelationship membership;
        membership.setRelationshipType(QContactRelationship::HasMember);
        membership.setFirst(mGroupContact->id());
        membership.setSecond(id);
        aRelationshipList.append(membership);
    }
}

/*!
Draw the image specific content menu
*/
void CntGroupMemberViewPrivate::drawImageMenu(const QPointF &aCoords)
{
    HbMenu *menu = new HbMenu();
    HbAction *changeImageAction = menu->addAction(hbTrId("txt_phob_menu_change_picture"), this, SLOT(openImageEditor()));
    if (mAvatar && !mAvatar->imageUrl().isEmpty())
    {
        menu->addAction(hbTrId("txt_phob_menu_remove_image"), this, SLOT(removeImage()));
    }
    menu->setPreferredPos(aCoords);
    menu->open();
}


void CntGroupMemberViewPrivate::removeImage()
{
    if (mAvatar) 
    {
        if (!mAvatar->imageUrl().isEmpty())
        {
            bool success = mGroupContact->removeDetail(mAvatar);
            // Check if image removable.
            CntImageUtility imageUtility;
            if(imageUtility.isImageRemovable(mAvatar->imageUrl().toString()))
            {
                imageUtility.removeImage(mAvatar->imageUrl().toString());
            }
            mAvatar->setImageUrl(QUrl());
            mImageLabel->clear();
            mImageLabel->setAvatarIcon(HbIcon("qtg_large_add_group_picture"));
            mHeadingItem->setIcon(HbIcon("qtg_large_add_group_picture"));
            getContactManager()->saveContact(mGroupContact);
       }
    }
}

void CntGroupMemberViewPrivate::decideActionButtonContext()
{
    bool isActionDefined( false );
    
    QContactPhoneNumber confCallNumber = mGroupContact->detail<QContactPhoneNumber>();
    //first check whether Conf Num is defined for a group.    
    if( !confCallNumber.number().isEmpty() )
    {
        isActionDefined = true;
    }
    else
    {
        //Since no conf number has been defined.
        //lets try to find if any group member has a communication field set or not
        
        int count = mModel->rowCount();
        
        for ( int i = 0; i<count; i++ )
        {
            QModelIndex modelIndex = mModel->index(i,0);
            QContact groupMember = mModel->contact(modelIndex);
            
            QContactDetail phoneDetail = groupMember.detail( QContactPhoneNumber::DefinitionName );
            QContactDetail emailDetail = groupMember.detail( QContactEmailAddress::DefinitionName );        
            
            if ( !phoneDetail.isEmpty() || !emailDetail.isEmpty() )
            {
                isActionDefined = true;
                break;
            }
        }
    }
 
    if ( isActionDefined )
    {
        //Enable when atleast 1 of the contacts in the current group have a 
        //communication address Defined or When a Group has Conf number defined.
        mShowActionsAction->setDisabled(false);    
    }
    else
    {
        //Disable when none of the contacts in the current group have any 
        //communication address Defined.         
        mShowActionsAction->setDisabled(true);
    }
}



// end of file

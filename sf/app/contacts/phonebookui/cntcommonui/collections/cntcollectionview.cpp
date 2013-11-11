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

#include "cntcollectionview.h"
#include "cntfetchcontactpopup.h"
#include "cntgroupdeletepopup.h"
#include "cntcollectionlistmodel.h"
#include "cntextensionmanager.h"
#include "cntglobal.h"
#include "cntfavourite.h"
#include "cntdetailconst.h"
#include "cntdebug.h"
#include <cntuiextensionfactory.h>
#include <cntuigroupsupplier.h>
#include <cntuisocialextension.h>
#include <hblabel.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <hbdevicenotificationdialog.h>
#include <hbinputdialog.h>
#include <hbdialog.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbframebackground.h>
#include <hbgroupbox.h>
#include <hbmessagebox.h>
#include <hbparameterlengthlimiter.h>
#include <hbeffect.h>
#include <hbtoolbar.h>

#include <QActionGroup>
#include <QApplication>
#include <QList>

const char *CNT_COLLECTIONVIEW_XML = ":/xml/contacts_collections.docml";

/*!

*/
CntCollectionView::CntCollectionView() :
    mView(NULL),
    mSoftkey(NULL),
    mViewManager(NULL),
    mModel(NULL),
    mListView(NULL),
    mNamesAction(NULL),
    mFindAction(NULL),
    mExtensionAction(NULL),
    mNewGroupAction(NULL),
    mDeleteGroupsAction(NULL),
    mHandledContact(NULL),
    mActionGroup(NULL)
{
    bool ok = false;
    mDocumentLoader.load(CNT_COLLECTIONVIEW_XML, &ok);

    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_collections.docml");
    }
    
    // disable toolbar effects
    HbEffect::disable(mView->toolBar());
    
    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    // menu actions
    mNewGroupAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:newgroup"));
    connect(mNewGroupAction, SIGNAL(triggered()), this, SLOT(newGroup()));
    mDeleteGroupsAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:deletegroups"));
    connect(mDeleteGroupsAction, SIGNAL(triggered()), this, SLOT(deleteGroups()));
    
    // toolbar actions
    
    mNamesAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:names"));
    connect(mNamesAction, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    HbAction* groups = static_cast<HbAction*> (mDocumentLoader.findObject("cnt:groups"));
    
    mActionGroup = new QActionGroup(this);
    groups->setActionGroup(mActionGroup);
    mNamesAction->setActionGroup(mActionGroup);
    groups->setChecked(true);
    
    mFindAction = static_cast<HbAction*>(mDocumentLoader.findObject("cnt:find"));
    connect(mFindAction, SIGNAL(triggered()), this, SLOT(showNamesViewWithFinder()));
    mExtensionAction = static_cast<HbAction*> (mDocumentLoader.findObject("cnt:activity"));
    connect(mExtensionAction, SIGNAL(triggered()), this, SLOT(handleExtensionAction()));
    
    connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(notifyNewGroup()));
}

/*!

*/
CntCollectionView::~CntCollectionView()
{
    mView->deleteLater();
    
    delete mHandledContact;
    mHandledContact = NULL;
}

/*!
Called when activating the view
*/
void CntCollectionView::activate( const CntViewParameters aArgs )
{
    CNT_ENTRY
    
    Q_UNUSED(aArgs)
    
    if (mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);   
    
    mViewManager = &mEngine->viewManager();
    mExtensionManager = &mEngine->extensionManager();
    
    // disable delete group(s) button if only favorites group is present
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QLatin1String(QContactType::TypeGroup));
    QList<QContactLocalId> groupContactIds = getContactManager()->contactIds(groupFilter);
    if (groupContactIds.count() < 2)
    {
        mDeleteGroupsAction->setEnabled(false);
    }
    
    // set up the list
    mListView = static_cast<HbListView*>(mDocumentLoader.findWidget(QString("listView")));
    mListView->setUniformItemSizes(true);
    mListView->setLayoutName("collectionviewlist");
    
    connect(mListView, SIGNAL(activated(const QModelIndex&)), this,
        SLOT(openGroup(const QModelIndex&)));
    connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
        SLOT(showContextMenu(HbAbstractViewItem*,QPointF)));
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    
    mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::LargeIcon);
    mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
    mModel = new CntCollectionListModel( mEngine, this);
    connect(mModel, SIGNAL(groupCountChanged()), this, SLOT(groupCountChanged()));
    mListView->setModel(mModel);
    
    CntExtensionManager& mgr = mEngine->extensionManager();
    if (mgr.pluginCount())
    {
        for (int i = 0; i < mgr.pluginCount(); i++)
        {
            CntUiSocialExtension* socialExtension = mgr.pluginAt(i)->socialExtension();
            if (socialExtension)
            {
                mExtensionAction->setEnabled(true);
                break;
            }
            else
            {
                mExtensionAction->setEnabled(false);
            }
        }
    }
    else
    {
        mExtensionAction->setEnabled(false);
    }

    
    CNT_EXIT
}

void CntCollectionView::deactivate()
{

}

/*!
Handle view switching request from an extension group item 
*/
void CntCollectionView::openView(CntViewParameters& viewParams)
{
    mViewManager->changeView(viewParams);
}

/*!
Go back to previous view
*/
void CntCollectionView::showPreviousView()
{
    CntViewParameters viewParameters;
    mViewManager->back(viewParameters);
}

/*!
Go back to previous view
*/
void CntCollectionView::showNamesViewWithFinder()
{
    CntViewParameters viewParameters;
    viewParameters.insert(EExtraAction, CNT_FIND_ACTION);
    mViewManager->back(viewParameters);
}

/*!
Called after user clicked on the listview.
*/
void CntCollectionView::openGroup(const QModelIndex &index)
{
    if (mModel->isExtensionGroup(index))
    {
        CntViewParameters params = mModel->extensionGroupActivated(index.row());
        if (params.count())
        {
            mViewManager->changeView(params);
        }
    }
    else
    {
        int id = index.data(Qt::UserRole).toInt();
        int favoriteGrpId = CntFavourite::favouriteGroupId(getContactManager());

        if (id == favoriteGrpId )
        {
            QContact favoriteGroup = getContactManager()->contact(favoriteGrpId);
            QContactRelationshipFilter rFilter;
            rFilter.setRelationshipType(QContactRelationship::HasMember);
            rFilter.setRelatedContactRole(QContactRelationship::First);
            rFilter.setRelatedContactId(favoriteGroup.id());
            // group members and their count
            QList<QContactLocalId> groupMemberIds = getContactManager()->contactIds(rFilter);

            if (groupMemberIds.isEmpty())
            {
                CntViewParameters viewParameters;
                viewParameters.insert(EViewId, collectionFavoritesView);
                QVariant var;
                var.setValue(favoriteGroup);
                viewParameters.insert(ESelectedGroupContact, var);
                mViewManager->changeView(viewParameters);
            }
            else
            {
                CntViewParameters viewParameters;
                viewParameters.insert(EViewId, favoritesMemberView);
                QVariant var;
                var.setValue(favoriteGroup);
                viewParameters.insert(ESelectedGroupContact, var);
                mViewManager->changeView(viewParameters);
            }
        }
        else
        {
            QContact groupContact = getContactManager()->contact(id);

            CntViewParameters viewParameters;
            viewParameters.insert(EViewId, groupMemberView);
            QVariant var;
            var.setValue(groupContact);
            viewParameters.insert(ESelectedGroupContact, var);
            mViewManager->changeView(viewParameters);
        }
    }
}

void CntCollectionView::showContextMenu(HbAbstractViewItem *item, const QPointF &coords)
{
    if (mModel->isExtensionGroup(item->modelIndex()))
    {
        mModel->extensionGroupLongPressed(item->modelIndex().row(), coords, this);
    }
    else
    {
        int id = item->modelIndex().data(Qt::UserRole).toInt();
        QVariant data( id );

        int favoriteGrpId = CntFavourite::favouriteGroupId(getContactManager());
        
        HbMenu *menu = new HbMenu();
        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->setPreferredPos( coords );
        
        HbAction* openAction = menu->addAction(hbTrId("txt_common_menu_open"));
        openAction->setData( data );

        if (id != favoriteGrpId)
        {
            HbAction* deleteAction = menu->addAction(hbTrId("txt_phob_menu_delete_group"));
            deleteAction->setData( data );
        }
        menu->open(this, SLOT(handleMenu(HbAction*)));

    }
}

void CntCollectionView::handleMenu(HbAction* action)
{
    HbMenu *menuItem = static_cast<HbMenu*>(sender());
    
    if ( action == menuItem->actions().first() )
    {
        int id = action->data().toInt();
        QModelIndex index = mModel->indexOfGroup(id);
        openGroup(index);
    }
    else if (action == menuItem->actions().at(1))
    {
        int id = action->data().toInt();
        QContact groupContact = getContactManager()->contact(id);
        deleteGroup(groupContact);
    }
}

void CntCollectionView::handleExtensionAction()
{
    CNT_ENTRY
    CntExtensionManager& mgr = mEngine->extensionManager();
    for (int i = 0; i < mgr.pluginCount(); i++)
    {
        CntUiSocialExtension* socialExtension = mgr.pluginAt(i)->socialExtension();
        if (socialExtension)
        {
            CntViewParameters params;
            socialExtension->handleToolbarAction(params);
            if (params.count())
            {
                mViewManager->changeView(params);
                break;
            }
        }
    }
    
    CNT_EXIT
}

void CntCollectionView::newGroup()
{
    HbInputDialog *popup = new HbInputDialog();
    popup->setAttribute(Qt::WA_DeleteOnClose, true);
    
    HbLineEdit *lineEdit = popup->lineEdit();
    lineEdit->setInputMethodHints(Qt::ImhNoPredictiveText);
    lineEdit->setMaxLength( CNT_GROUPNAME_MAXLENGTH );
    
    popup->setPromptText(hbTrId("txt_phob_title_new_group_name"));
    popup->clearActions();
    HbAction* primaryAction = new HbAction(hbTrId("txt_phob_button_create"));
    popup->addAction(primaryAction);
    
    HbAction* secondaryAction = new HbAction(hbTrId("txt_common_button_cancel"));
    popup->addAction(secondaryAction);
           
    popup->setInputMode(HbInputDialog::TextInput);

    popup->open(this, SLOT(handleNewGroup(HbAction*)));
}

void CntCollectionView::handleNewGroup(HbAction* action)
{
    HbInputDialog *popup = static_cast<HbInputDialog*>(sender());
    
    if (popup && action == popup->actions().first())
    {
        QString text = popup->value().toString();
        
        mHandledContact = new QContact();
        mHandledContact->setType(QContactType::TypeGroup);
        
        QContactName groupName;
        groupName.setCustomLabel(text);
        
        mHandledContact->saveDetail(&groupName);
        getContactManager()->saveContact(mHandledContact);
        
        QString groupNameCreated = mHandledContact->displayLabel();
        if (groupNameCreated.isEmpty())
        {
            groupNameCreated = hbTrId("txt_phob_list_unnamed");
        }
        
        // if no contacts are present, then dont call the fetch popup
       QContactDetailFilter gFilter;
       gFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
       gFilter.setValue(QLatin1String(QContactType::TypeContact));
      
       QList<QContactLocalId> contactIds = getContactManager()->contactIds(gFilter);   
       if (contactIds.isEmpty())
       {
           notifyNewGroup();
       }      
       else
       {
            CntFetchContactPopup* popup = CntFetchContactPopup::createMultiSelectionPopup(
                    HbParameterLengthLimiter(hbTrId("txt_phob_title_members_of_1_group")).arg(groupNameCreated),
                    hbTrId("txt_common_button_save"),
                    mEngine->contactManager(SYMBIAN_BACKEND));
            connect( popup, SIGNAL(fetchReady(QSet<QContactLocalId>)), this, SLOT(handleNewGroupMembers(QSet<QContactLocalId>)) );
            connect( popup, SIGNAL(fetchCancelled()), this, SLOT(handleCancelGroupMembers()) );
            
            QSet<QContactLocalId> ids;
            popup->setSelectedContacts(ids);
            popup->showPopup();
       }
    }
}

void CntCollectionView::handleNewGroupMembers( QSet<QContactLocalId> aIds )
{
    mSelectedContactsSet = aIds;

    if ( aIds.size() > 0 )
    {
        saveNewGroup(mHandledContact);
        
        CntViewParameters viewParameters;
        viewParameters.insert(EViewId, groupMemberView);
        
        QVariant var;
        var.setValue(*mHandledContact);
        viewParameters.insert(ESelectedGroupContact, var);
        mViewManager->changeView(viewParameters);
    }

    notifyNewGroup();
}

void CntCollectionView::handleCancelGroupMembers()
{
    mSelectedContactsSet.clear();
    
    notifyNewGroup();
}

void CntCollectionView::notifyNewGroup()
{
    if (mHandledContact != NULL)
    {
        QString groupNameCreated = mHandledContact->displayLabel();
        if (groupNameCreated.isEmpty())
        {
            groupNameCreated = hbTrId("txt_phob_list_unnamed");
        }
        HbDeviceNotificationDialog::notification(QString(), 
                HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_new_group_1_created")).arg(groupNameCreated));

        delete mHandledContact;
        mHandledContact = NULL;
    }
}

void CntCollectionView::refreshDataModel()
{
    mListView->setModel(0);
    
    delete mModel;
    mModel = NULL;
    
    mModel = new CntCollectionListModel(mEngine, this);
    connect(mModel, SIGNAL(groupCountChanged()), this, SLOT(groupCountChanged()));
    mListView->setModel(mModel);
}

void CntCollectionView::deleteGroup(QContact group)
{
    mHandledContact = new QContact(group);
    QString name = mHandledContact->displayLabel();
    if (name.isEmpty())
    {
        name = hbTrId("txt_phob_list_unnamed");
    }

    HbLabel *headingLabel = new HbLabel();
    headingLabel->setPlainText(HbParameterLengthLimiter(hbTrId("txt_phob_dialog_delete_1_group")).arg(name));
          
    HbMessageBox::question(hbTrId("txt_phob_dialog_only_group_will_be_removed_contac")
            , this, SLOT(handleDeleteGroup(int)), HbMessageBox::Delete | HbMessageBox::Cancel,
                headingLabel);
}

void CntCollectionView::handleDeleteGroup(int action)
{
    if (action == HbMessageBox::Delete)
    {
        getContactManager()->removeContact(mHandledContact->localId());
    }
    
    delete mHandledContact;
    mHandledContact = NULL;
}

void CntCollectionView::deleteGroups()
{
    // save the group here
    CntGroupDeletePopup *groupDeletePopup = new CntGroupDeletePopup( mEngine );
    
    groupDeletePopup->populateListOfGroup();
    groupDeletePopup->open(this, SLOT(handleDeleteGroups(HbAction*)));

}

void CntCollectionView::handleDeleteGroups(HbAction* action)
{
    CntGroupDeletePopup *groupDeletePopup = static_cast<CntGroupDeletePopup*>(sender());
    
    if (groupDeletePopup && action == groupDeletePopup->actions().first())
    {
        groupDeletePopup->deleteGroup();
    }
}

void CntCollectionView::groupCountChanged()
{
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QLatin1String(QContactType::TypeGroup));
    QList<QContactLocalId> groupContactIds = getContactManager()->contactIds(groupFilter);
    if (groupContactIds.count() < 2)
    {
        mDeleteGroupsAction->setEnabled(false);
    }
    else
    {
        mDeleteGroupsAction->setEnabled(true);
    }
}

QContactManager* CntCollectionView::getContactManager()
{
    if (!mEngine) {
        return NULL;
    }
    return &mEngine->contactManager( SYMBIAN_BACKEND );
}

void CntCollectionView::saveNewGroup(QContact* aGroup)
{
    if (!aGroup) {
        return;
    }
    
    // Save the relationship from the selection model of the member selection list
    QList<QContactRelationship> relationships;
    foreach (QContactLocalId localId, mSelectedContactsSet)
    {
        QContactRelationship relationship;
        QContactId id;
        id.setLocalId(localId);
        relationship.setRelationshipType(QContactRelationship::HasMember);
        relationship.setFirst(aGroup->id());
        relationship.setSecond(id);
        relationships.append(relationship);
    }

    // Save relationships
    if (!relationships.isEmpty()) {
        getContactManager()->saveRelationships(&relationships, NULL);
    }
}

// EOF

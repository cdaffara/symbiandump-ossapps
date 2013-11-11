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

#include "cntnamesview_p.h"
#include "cntactionlauncher.h"
#include "cntfetchcontactpopup.h"
#include "cntextensionmanager.h"
#include "cntglobal.h"
#include "cntdebug.h"
#include "cntapplication.h"
#include "cntfavourite.h"
#include <cntabstractengine.h>

#include <cntuiextensionfactory.h>
#include <cntuisocialextension.h>

#include <hbabstractviewitem.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbdocumentloader.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hblineedit.h>
#include <hbindexfeedback.h>
#include <hbscrollbar.h>
#include <hbsearchpanel.h>
#include <hbtoolbar.h>
#include <hbframebackground.h>
#include <hbshrinkingvkbhost.h>
#include <hbmessagebox.h>
#include <hbparameterlengthlimiter.h>

#include <QInputContext>

const char *CNT_CONTACTLIST_XML = ":/xml/contacts_namelist.docml";
static const int CNT_MIN_ROW_COUNT = 2;
bool CntNamesViewPrivate::mIsFirstTimeUse = true;

CntNamesViewPrivate::CntNamesViewPrivate() :
    QObject(),
    mViewManager(NULL),
    mListModel(NULL),
    mListView(NULL),
	mEmptyList(NULL),
    mSearchPanel(NULL),
    mLoader(NULL),
    mVirtualKeyboard(NULL),
    mNamesAction(NULL),
    mMenuBuilder(NULL),
    mHandledContactId(0),
    mActionGroup(NULL),
	mMenu(NULL),
	mFilterChanged(false)
{
    CNT_ENTRY
    
    bool ok;
    document()->load( CNT_CONTACTLIST_XML, &ok);
    if (!ok) {
        qFatal("Unable to read %S", CNT_CONTACTLIST_XML);
    }
    
    document()->load( CNT_CONTACTLIST_XML, "no_find");
    
    mView = static_cast<HbView*> (document()->findWidget("view"));
    
    mVirtualKeyboard = new HbShrinkingVkbHost(mView);

    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    
    mNewContact = static_cast<HbAction*> (document()->findObject("cnt:newcontact"));
    mNewContact->setParent(mView);
    connect(mNewContact, SIGNAL(triggered()), this, SLOT(createNewContact()));

    mMultipleDeleter = static_cast<HbAction*> (document()->findObject("cnt:delete"));
    mMultipleDeleter->setParent(mView);
    connect(mMultipleDeleter, SIGNAL(triggered()), this, SLOT(deleteMultipleContacts()));

    HbAction* findContacts = static_cast<HbAction*> (document()->findObject("cnt:find"));
    findContacts->setParent(mView);
    connect(findContacts, SIGNAL(triggered()), this, SLOT(showFinder()));

    HbAction* groups = static_cast<HbAction*> (document()->findObject("cnt:groups"));
    groups->setParent(mView);
    connect(groups, SIGNAL(triggered()), this, SLOT(showCollectionView()));

    mNamesAction = static_cast<HbAction*> (document()->findObject("cnt:names"));
    mNamesAction->setParent(mView);

    mImportSim = static_cast<HbAction*> (document()->findObject("cnt:importsim"));
    mImportSim->setParent(mView);
    connect(mImportSim, SIGNAL(triggered()), this, SLOT(importSim()));
    
    mActionGroup = new QActionGroup(this);
    groups->setActionGroup(mActionGroup);
    mNamesAction->setActionGroup(mActionGroup);
    
    HbAction* extension = static_cast<HbAction*> (document()->findObject("cnt:activity"));
    extension->setParent(mView);
    connect(extension, SIGNAL(triggered()), this, SLOT(handleExtensionAction()));
       
    HbAction* settings = static_cast<HbAction*>(document()->findObject("cnt:settings") );
    settings->setParent(mView);
    connect( settings, SIGNAL(triggered()), this, SLOT(showSettings()) );

    HbMenu* viewMenu = static_cast<HbMenu*>(document()->findObject("viewMenu") );
    viewMenu->setParent(mView);

    connect(list(), SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
           SLOT(showContextMenu(HbAbstractViewItem*,QPointF)));
    connect(list(), SIGNAL(activated (const QModelIndex&)), this,
           SLOT(showContactView(const QModelIndex&)));

    mEmptyList = static_cast<HbLabel*> (document()->findWidget("emptyLabel"));
    mSearchPanel = static_cast<HbSearchPanel*> (document()->findWidget("searchPanel"));
    connect(mSearchPanel, SIGNAL(exitClicked()), this, SLOT(hideFinder()));
    connect(mSearchPanel, SIGNAL(criteriaChanged(QString)), this, SLOT(setFilter(QString)));
    
    HbMainWindow* win = mView->mainWindow();
    CntApplication* cntApp = static_cast<CntApplication*>(qApp);
    connect(win, SIGNAL(viewReady()), cntApp, SIGNAL(applicationReady()));
    
#ifdef __WINS__
    mView->menu()->addAction("Change Orientation", this, SLOT(switchOrientation()) );
#endif
    CNT_EXIT
}

void CntNamesViewPrivate::handleImportContacts( HbAction *aAction )
{
    CNT_ENTRY
    
    HbDialog *popup = static_cast<HbDialog*>(sender());
    
    if (popup && aAction == popup->actions().first())
    {
        CntViewParameters args;        
        args.insert(EViewId, importsView);
        mViewManager->changeView(args);
    }
          
    CNT_EXIT
}

void CntNamesViewPrivate::switchOrientation()
{
    HbMainWindow* win = mView->mainWindow();
    Qt::Orientation orientation = win->orientation();
    
    win->setOrientation( orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal );
}

CntNamesViewPrivate::~CntNamesViewPrivate()
{
    CNT_ENTRY
    
    delete mListModel;
    mListModel = NULL;

    delete mLoader;
    mLoader = NULL;

    delete mVirtualKeyboard;
    mVirtualKeyboard = NULL;
    
    delete mMenuBuilder;
    mMenuBuilder = NULL;
    
    delete mView;
    mView = NULL;
    
    if (mMenu) 
    {
        delete mMenu;
    }

    CNT_EXIT
}

void CntNamesViewPrivate::activate(const CntViewParameters aArgs)
{
    CNT_ENTRY
    
    mViewManager = &mEngine->viewManager();
    if (mView->mainWindow() != NULL && !(mView->visibleItems() & Hb::AllItems))
    {
        hideFinder();
    }
    
    if (!mListModel) {
        QContactDetailFilter filter;
        filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        QString typeContact = QContactType::TypeContact;
        filter.setValue(typeContact);
        
        mListModel = new CntListModel( &mEngine->contactManager(SYMBIAN_BACKEND), filter);
        list()->setModel(mListModel);
    }
    
    mNamesAction->setChecked(true);

    mMenuBuilder = new CntActionMenuBuilder( mListModel->myCardId() );
    connect( mMenuBuilder, SIGNAL(deleteContact(QContact&)), this, SLOT(deleteContact(QContact&)) );
    connect( mMenuBuilder, SIGNAL(editContact(QContact&)), this, SLOT(showContactEditorView(QContact&)) );
    connect( mMenuBuilder, SIGNAL(openContact(QContact&)), this, SLOT(showContactView(QContact&)) );
    connect( mMenuBuilder, SIGNAL(performContactAction( QContact& , QContactDetail, QString)), this, SLOT(executeAction(QContact& , QContactDetail, QString)));
    
    if ( mView->navigationAction() != mSoftkey)
    {
        mView->setNavigationAction(mSoftkey);
    }

    changeDeleteActionStatus();
    
    QContactManager* contactManager = &mEngine->contactManager( SYMBIAN_BACKEND );
    // make connections unique, that is, duplicate connections are not connected again
    connect(contactManager, SIGNAL(contactsAdded(const QList<QContactLocalId>&)),
            this, SLOT(handleContactAddition(const QList<QContactLocalId>&)), Qt::UniqueConnection);
    connect(contactManager, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
            this, SLOT(handleContactRemoval(const QList<QContactLocalId>&)), Qt::UniqueConnection);
    connect(contactManager, SIGNAL(selfContactIdChanged(const QContactLocalId&, const QContactLocalId&)), 
            this, SLOT(handleSelfContactIdChange(const QContactLocalId&, const QContactLocalId&)), Qt::UniqueConnection);
    
    if (aArgs.value(ESelectedAction) == CNT_CREATE_ACTION || aArgs.value(ESelectedAction) == CNT_EDIT_ACTION)
    {
        setScrollPosition(aArgs.value(ESelectedContact).value<QContact>().localId());
    }
   
    if ( aArgs.value( EExtraAction ).toString() == CNT_FIND_ACTION  )
    {
        showFinder();
    }
    
    if (mIsFirstTimeUse)
    {
        // Disable FTU flag
        mIsFirstTimeUse = false;
        
        // Show import contacts popup only if there no local contacts
        // Check row count from list model and exclude dummy mycard
        if (mListModel->rowCount()==1 && mListModel->myCardId()<= 0)
        {
            // Default filter to fetch all contacts and groups
            QContactFilter filter;
            QList<QContactLocalId> contactIds = contactManager->contactIds(filter);
            
            // Check if favourite group contact exists
            // Favourite group contact is created by default in phonebook
            int favouriteId = CntFavourite::favouriteGroupId(contactManager);
            if ( (contactIds.isEmpty() )
                 || (contactIds.count() == 1 && contactIds.at(0) == favouriteId) )
            {
                HbMessageBox *note = new HbMessageBox();
                note->setDismissPolicy(HbDialog::NoDismiss);
                note->setTimeout(HbPopup::NoTimeout);
                note->setBackgroundFaded(true);
                note->setAttribute(Qt::WA_DeleteOnClose, true);
                note->setStandardButtons(HbMessageBox::NoButton);
                note->addAction(new HbAction(hbTrId("txt_phob_button_import"), note));
                note->addAction(new HbAction(hbTrId("txt_common_button_cancel"), note));
                    
                HbLabel *headingLabel = new HbLabel( note );
                headingLabel->setPlainText(hbTrId("txt_phob_title_import_contacts")); 
                note->setHeadingWidget(headingLabel);
                
                note->setIcon(HbIcon("qtg_large_sim"));
                note->setText(hbTrId("txt_phob_info_your_phonebook_is_empty_do_you_wish"));
                
                note->open(this, SLOT(handleImportContacts(HbAction*)));
            }
        }
    }
    
    CntExtensionManager& mgr = mEngine->extensionManager();
    if (mgr.pluginCount())
    {
        for (int i = 0; i < mgr.pluginCount(); i++)
        {
            CntUiSocialExtension* socialExtension = mgr.pluginAt(i)->socialExtension();
            if (socialExtension)
            {
                static_cast<HbAction*> (document()->findObject("cnt:activity"))->setEnabled(true);
                break;
            }
            else
            {
                static_cast<HbAction*> (document()->findObject("cnt:activity"))->setEnabled(false);
            }
        }
    }
    else
    {
        static_cast<HbAction*> (document()->findObject("cnt:activity"))->setEnabled(false);
    }

    CNT_EXIT
}

void CntNamesViewPrivate::deactivate()
{
    CNT_ENTRY
    HbMainWindow* win = mView->mainWindow();
    if ( win != NULL)
    {
        CntApplication* cntApp = static_cast<CntApplication*>(qApp);
        disconnect(win, SIGNAL(viewReady()), cntApp, SIGNAL(applicationReady()));
    }
    delete mMenuBuilder;
    mMenuBuilder = NULL;
    
    CNT_EXIT
}

void CntNamesViewPrivate::changeDeleteActionStatus()
{
    CNT_ENTRY
    
    bool multipleContactsFound = mListModel->rowCount() >= CNT_MIN_ROW_COUNT;
    mMultipleDeleter->setEnabled(multipleContactsFound);
    
    CNT_EXIT
}

void CntNamesViewPrivate::focusLineEdit()
{
    CNT_ENTRY
    
    HbLineEdit *editor = static_cast<HbLineEdit*>(mSearchPanel->primitive("lineedit"));
    if (editor)
    {
        editor->setObjectName("focusLineEdit");
        editor->setInputMethodHints(Qt::ImhNoPredictiveText);
        editor->setText("");
        editor->setFocus();
        
        // This opens the VKB
        QInputContext *ic = qApp->inputContext();
        if (ic)
        {
            QEvent *event = new QEvent(QEvent::RequestSoftwareInputPanel);
            ic->filterEvent(event);
            delete event;
        }
    }

    CNT_EXIT
}

void CntNamesViewPrivate::setFilter(const QString &filterString)
{
    CNT_ENTRY

    QContactDetailFilter filter;

    if (filterString.isEmpty()) {
        filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        QString typeContact = QContactType::TypeContact;
        filter.setValue(typeContact);
    } else {
        QStringList searchList = filterString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        filter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName,
            QContactDisplayLabel::FieldLabel);
        filter.setMatchFlags(QContactFilter::MatchStartsWith);
        filter.setValue(searchList);
    }

    mListModel->setFilter(filter);
    mFilterChanged = true;

    if (mListModel->rowCount() == 0) {
        document()->load( CNT_CONTACTLIST_XML, "find_empty" );
    } else {
        document()->load( CNT_CONTACTLIST_XML, "find_list" );
    }
    
    CNT_EXIT
}

void CntNamesViewPrivate::showFinder()
{
    CNT_ENTRY
    
    mListModel->showMyCard(false);
    
    HbListViewItem* proto = mListView->listItemPrototype();
    proto->setTextFormat( Qt::RichText );
    
    mView->hideItems(Hb::AllItems);
    
    focusLineEdit();
    
    CNT_EXIT
}

void CntNamesViewPrivate::hideFinder()
{
    CNT_ENTRY
    
    document()->load( CNT_CONTACTLIST_XML, "no_find" );
    mView->showItems(Hb::AllItems);
    
    HbListViewItem* proto = mListView->listItemPrototype();
    proto->setTextFormat( Qt::PlainText );
        
    mListModel->showMyCard(true);

    if (mFilterChanged)
    {
        QContactDetailFilter filter;
        filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        QString typeContact = QContactType::TypeContact;
        filter.setValue(typeContact);
        mListModel->setFilter(filter);
        mFilterChanged = false;
    }
    
    changeDeleteActionStatus();

    CNT_EXIT    
}

bool CntNamesViewPrivate::isFinderVisible()
{
    CNT_ENTRY
    
    bool isVisible = false;
    if ( mSearchPanel )
    {
        isVisible = mSearchPanel->isVisible();
    }
    
    CNT_EXIT
    return isVisible;
}

void CntNamesViewPrivate::showPreviousView()
{
    CNT_ENTRY
    
    if ( !isFinderVisible() )
    {
        mViewManager->back( CntViewParameters() );
    }
    else
    {
        hideFinder();
    }
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleExtensionAction()
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

void CntNamesViewPrivate::createNewContact()
{
    CNT_ENTRY
    
    QContact newContact;
    newContact.setType( QContactType::TypeContact );
    
    showContactEditorView(newContact);
    
    CNT_EXIT
}

void CntNamesViewPrivate::deleteContact(QContact& aContact)
{
    CNT_ENTRY
    
    QContactManager& manager = mEngine->contactManager( SYMBIAN_BACKEND );
    QString name = manager.synthesizedContactDisplayLabel(aContact);
    if (name.isEmpty())
    {
        name = hbTrId("txt_phob_list_unnamed");
    }
    
    mHandledContactId = aContact.localId();
    
    HbMessageBox::question(HbParameterLengthLimiter(hbTrId("txt_phob_info_delete_1")).arg(name), this, SLOT(handleDeleteContact(int)),
            HbMessageBox::Delete | HbMessageBox::Cancel);
    
    CNT_EXIT
}

void CntNamesViewPrivate::deleteMultipleContacts()
{
    CNT_ENTRY
    CntFetchContactPopup* popup = CntFetchContactPopup::createMultiSelectionPopup(
            hbTrId("txt_phob_title_delete_contacts"),
            hbTrId("txt_common_button_delete"),
            mEngine->contactManager(SYMBIAN_BACKEND));
    connect(popup, SIGNAL(fetchReady(QSet<QContactLocalId>)), 
            this, SLOT(handleDeleteMultipleContacts(QSet<QContactLocalId>)) );
    QSet<QContactLocalId> ids;
    popup->setSelectedContacts(ids);
    popup->showPopup();
    CNT_EXIT
}

void CntNamesViewPrivate::handleDeleteMultipleContacts( QSet<QContactLocalId> aIds )
{
    CNT_ENTRY
    
    mEngine->contactManager( SYMBIAN_BACKEND ).removeContacts(aIds.toList(), NULL);
    
    CNT_EXIT
}

void CntNamesViewPrivate::showContactView( const QModelIndex& aIndex )
{
    CNT_ENTRY

    QContact c = mListModel->contact(aIndex);
    showContactView( c );
    
    CNT_EXIT
}

void CntNamesViewPrivate::showContactView( QContact& aContact )
{
    CNT_ENTRY
    
    CntViewParameters args;
    args.insert(EViewId, contactCardView);
    if (aContact.localId() == mListModel->myCardId() && aContact.details().count() <= 4)
    {
        args.insert(EViewId, myCardView);
    }

    QVariant contact;
    contact.setValue(aContact);
    args.insert(ESelectedContact, contact);
    mViewManager->changeView(args);
    
    CNT_EXIT
}

void CntNamesViewPrivate::showContextMenu(HbAbstractViewItem* aItem, QPointF aPoint)
{
    CNT_ENTRY
    
    QContact contact = mListModel->contact(aItem->modelIndex());
    
   // In case of an empty MyCard, do not show any ContextMenu
    if (!(contact.localId() == mListModel->myCardId() && contact.details().count() <= 4))
    {
        if (mMenu) 
        {
            delete mMenu;
            mMenu = NULL;
        }
        mMenu = mMenuBuilder->actionMenu( contact, mListModel->myCardId() );
        mMenu->setPreferredPos( aPoint );
        mMenu->open();
    }
    else
    {
        showContactView(contact);
    }
    
    CNT_EXIT
}

void CntNamesViewPrivate::executeAction( QContact& aContact, QContactDetail aDetail, QString aAction )
{
    CNT_ENTRY
    
    CntActionLauncher* other = new CntActionLauncher( mEngine->contactManager(SYMBIAN_BACKEND), aAction );
    connect(other, SIGNAL(actionExecuted(CntActionLauncher*)), this, SLOT(actionExecuted(CntActionLauncher*)));
    other->execute(aContact, aDetail);
    
    CNT_EXIT
}

void CntNamesViewPrivate::actionExecuted(CntActionLauncher* aAction)
{
    CNT_ENTRY

    aAction->deleteLater();

    CNT_EXIT
}

void CntNamesViewPrivate::handleDeleteContact( int aAction )
{
    CNT_ENTRY
    
    if (aAction == HbMessageBox::Delete)
    {
        mEngine->contactManager( SYMBIAN_BACKEND ).removeContact(mHandledContactId);
    }
    
    mHandledContactId = 0;
    
    CNT_EXIT
}

void CntNamesViewPrivate::showContactEditorView(QContact& aContact)
{
    CNT_ENTRY
    
    CntViewParameters args;
    args.insert(EViewId, editView);
    
    QVariant contact;
    contact.setValue(aContact);
    args.insert(ESelectedContact, contact);

    mViewManager->changeView(args);
    
    CNT_EXIT
}

void CntNamesViewPrivate::showCollectionView()
{
    CNT_ENTRY
    
    CntViewParameters args;
    args.insert(EViewId, collectionView);
    mViewManager->changeView(args);
    
    CNT_EXIT
}

void CntNamesViewPrivate::importSim()
{
    CNT_ENTRY
    
    CntViewParameters args;        
    args.insert(EViewId, importsView);
    mViewManager->changeView(args);
    
    CNT_EXIT
}

void CntNamesViewPrivate::showSettings()
{
    CNT_ENTRY
    
    CntViewParameters args;
    args.insert( EViewId, settingsView );
    mViewManager->changeView( args );
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleContactAddition(const QList<QContactLocalId>& aAddedList)
{
    CNT_ENTRY

    Q_UNUSED(aAddedList);
    changeDeleteActionStatus();
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleContactRemoval(const QList<QContactLocalId>& aRemovedList)
{
    CNT_ENTRY
  
    Q_UNUSED(aRemovedList);
    changeDeleteActionStatus();
    
    if (isFinderVisible() && mListModel->rowCount() == 0)
    {
        document()->load( CNT_CONTACTLIST_XML, "find_empty" );
    }
    
    CNT_EXIT
}

void CntNamesViewPrivate::handleSelfContactIdChange(const QContactLocalId& aOldId, const QContactLocalId& aNewId)
{
    CNT_ENTRY
    
    Q_UNUSED(aOldId);
    Q_UNUSED(aNewId);
    changeDeleteActionStatus();
    
    list()->scrollTo(mListModel->index(0));
    
    CNT_EXIT
}

void CntNamesViewPrivate::setScrollPosition(int focusedContact)
{
    CNT_ENTRY
    
    // Scroll to the focused contact
    if ( focusedContact > 0 )
    {
        list()->scrollTo(mListModel->indexOfContactId(focusedContact), HbAbstractItemView::EnsureVisible);
    }
    
    CNT_EXIT
}

//// lazy accessors
HbListView* CntNamesViewPrivate::list()
{
    CNT_ENTRY

    if (!mListView) {
        mListView = static_cast<HbListView*> (mLoader->findWidget("listView"));

        mListView->setFrictionEnabled(true);
        mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
        mListView->listItemPrototype()->setGraphicsSize(HbListViewItem::Thumbnail);
        mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
        mListView->verticalScrollBar()->setInteractive(true);
        
        HbFrameBackground frame;
        frame.setFrameGraphicsName("qtg_fr_list_normal");
        frame.setFrameType(HbFrameDrawer::NinePieces);
        
        HbListViewItem* prototype = mListView->listItemPrototype();
        prototype->setDefaultFrame( frame );
        
        mListView->setUniformItemSizes(true);
        mListView->setItemRecycling(true);

        HbIndexFeedback *indexFeedback = new HbIndexFeedback(mView);
        indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
        indexFeedback->setItemView(mListView);
    }
    
    CNT_EXIT
    return mListView;
}

HbDocumentLoader* CntNamesViewPrivate::document()
{
    CNT_ENTRY
    
    if (!mLoader) {
        mLoader = new HbDocumentLoader();
    }
    
    CNT_EXIT
    return mLoader;
}


// End of File

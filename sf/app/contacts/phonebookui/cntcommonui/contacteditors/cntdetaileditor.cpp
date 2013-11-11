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
#include "cntdetaileditor.h"
#include "cnteditorfactory.h"
#include "cntgroupeditormodel.h"
#include "cntsavemanager.h"
#include "cntglobal.h"

#include <cntviewparams.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbdocumentloader.h>
#include <hbaction.h>
#include <hblineedit.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>
#include <cntdebug.h>
#include <hbscrollarea.h>
#include <hbscrollbar.h>
#include <hbdevicenotificationdialog.h>
#include <hbparameterlengthlimiter.h>
#include <hbshrinkingvkbhost.h>

#include <QApplication>
#include <QTimer>

const char *CNT_DETAILEDITOR_XML = ":/xml/contacts_detail_editor.docml";

CntDetailEditor::CntDetailEditor( int aId ) :
    QObject(),
    mDataFormModel(NULL),
    mHeader(NULL),   
    mId(aId),
    mView(NULL),
    mLoader(NULL),   
    mViewManager(NULL),
    mEditorFactory(NULL),
    mCancel(NULL),
    mSaveManager(NULL),
    mVirtualKeyboard(NULL)
{
    bool ok;
    document()->load(CNT_DETAILEDITOR_XML, &ok);
    if (!ok) {
        qFatal("Unable to read %S", CNT_DETAILEDITOR_XML);
    }
    else {
        mView = static_cast<HbView*> (document()->findWidget(QString("view")));
        mDataForm = static_cast<HbDataForm*> (document()->findWidget(QString("dataForm")));
    }
 
    mEditorFactory = new CntEditorFactory();
    
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect( mSoftkey, SIGNAL(triggered()), this, SLOT(saveChanges()) );
    
    // closing the application from task swapper or end key will cause the contact to be saved
    connect( qApp, SIGNAL(aboutToQuit()), this, SLOT(saveContact()));
}

CntDetailEditor::~CntDetailEditor()
{
    mView->deleteLater();
    
    delete mDataForm;
    mDataForm = NULL;
    delete mDataFormModel;
    mDataFormModel = NULL;
    delete mHeader;
    mHeader = NULL;
    delete mLoader;
    mLoader = NULL;
    delete mEditorFactory;
    mEditorFactory = NULL;
    delete mSaveManager;
    mSaveManager = NULL;
    delete mVirtualKeyboard;
    mVirtualKeyboard = NULL;
}

void CntDetailEditor::setViewId( int aId )
{
    mId = aId;
}

void CntDetailEditor::setInsertAction( const QString aInsert )
{
    HbAction* insert = new HbAction( aInsert, mView );
    mView->menu()->insertAction(mCancel, insert);
    connect( insert, SIGNAL(triggered()), this, SLOT(insertField()) );
}

void CntDetailEditor::activate( const CntViewParameters aArgs )
{
    mViewManager = &mEngine->viewManager();
    mArgs = aArgs; //don't loose the params while swiching between editview and editorviews.
    
    mCancel = static_cast<HbAction*>(document()->findObject("cnt:discardchanges"));
    mCancel->setParent(mView);
    mView->menu()->addAction(mCancel);
    connect( mCancel, SIGNAL(triggered()), this, SLOT(discardChanges()) );
    mCancel->setDisabled(true);
    
    if ( mView->navigationAction() != mSoftkey) {
        mView->setNavigationAction(mSoftkey);
    }
    
    QContact selectedContact;
    if ( mId == groupEditorView )
    {
        selectedContact = aArgs.value(ESelectedGroupContact).value<QContact>();
        connect( mDataForm, SIGNAL(itemShown(const QModelIndex&)), this, SLOT(handleItemShown(const QModelIndex&)) );
    }
    else
    {
        selectedContact = aArgs.value(ESelectedContact).value<QContact>();
        connect( mDataForm, SIGNAL(itemShown(const QModelIndex&)), this, SLOT(handleItemShown(const QModelIndex&)) );
    }
    
    QContactManager& cm = mEngine->contactManager(SYMBIAN_BACKEND);
    connect(&cm, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), 
        this, SLOT(contactDeletedFromOtherSource(const QList<QContactLocalId>&)));
    
    mEditorFactory->setupEditorView(*this, selectedContact);
    
    QString myCard = mArgs.value( EMyCard ).toString();
    QContactLocalId localId = selectedContact.localId();
    QContactLocalId selfContactId = cm.selfContactId();
    bool isMyCard = ( localId == selfContactId && localId != 0 ) || !myCard.isEmpty();
    
    if (isMyCard)
    {
        mSaveManager = new CntSaveManager(CntSaveManager::EMyCard);
    }
    else if ( mId == groupEditorView )
    {
        mSaveManager = new CntSaveManager(CntSaveManager::EGroup);
    }
    else
    {
        mSaveManager = new CntSaveManager();
    }
    
    mDataForm->setItemRecycling(true);

    // add new field if required
    if ( aArgs.value(ESelectedAction).toString() == CNT_ADD_ACTION )
    {
        mDataFormModel->insertDetailField();
    }

    connect(mDataFormModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(enableDiscardChanges(/*const QModelIndex &, const QModelIndex &*/)));
    
    mDataForm->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded); 
    mDataForm->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    mDataForm->verticalScrollBar()->setInteractive(true);
    
    mVirtualKeyboard = new HbShrinkingVkbHost(mView);
        
    connect(mVirtualKeyboard, SIGNAL(keypadOpened()), this, SLOT(handleKeypadOpen()));
    connect(mVirtualKeyboard, SIGNAL(keypadClosed()), this, SLOT(handleKeypadClosed()));
}

void CntDetailEditor::deactivate()
{
    QContactManager& mgr = mEngine->contactManager(SYMBIAN_BACKEND);
    if( mId == groupEditorView) {
        mgr.saveContact( mDataFormModel->contact() );
    }
    
    disconnect(&mgr, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
            this, SLOT(contactDeletedFromOtherSource(const QList<QContactLocalId>&)));
}
    
bool CntDetailEditor::isDefault() const
{
    return false;
}

HbView* CntDetailEditor::view() const
{
    return mView;   
}

HbDocumentLoader* CntDetailEditor::document()
{
    if (!mLoader) {
        mLoader = new HbDocumentLoader();
    }
    
    return mLoader;    
}

void CntDetailEditor::insertField()
{
    mDataFormModel->insertDetailField();
    mDataForm->scrollTo(mDataFormModel->index(mDataFormModel->rowCount() - 1, 0), HbAbstractItemView::PositionAtBottom);
}

void CntDetailEditor::handleItemShown(const QModelIndex& aIndex )
{
    HbDataFormModelItem* modelItem = mDataFormModel->itemFromIndex( aIndex );
    
    if ( modelItem->type() == HbDataFormModelItem::TextItem ) 
    {
        HbDataFormViewItem* viewItem = static_cast<HbDataFormViewItem*>(mDataForm->itemByIndex( aIndex ));
        HbLineEdit* edit = static_cast<HbLineEdit*>( viewItem->dataItemContentWidget() );
        QString objName;
        
        if ( mId == groupEditorView )
        {
            CntGroupEditorModel* groupModel = static_cast<CntGroupEditorModel*>( mDataFormModel );    
            if ( groupModel->isConferenceNumber(aIndex) )
            {
                edit->setInputMethodHints( Qt::ImhDialableCharactersOnly );
                objName = "Conference number line edit %1";
            }
            else
            {
                edit->setInputMethodHints( Qt::ImhNoPredictiveText );
                objName = "Group name line edit %1";
            }
        }
        else
        {
            edit->setInputMethodHints( Qt::ImhNoPredictiveText );
            
            if (modelItem->contentWidgetData( "preferDigits" ).toBool())
            {
                edit->setInputMethodHints( Qt::ImhPreferNumbers | Qt::ImhNoPredictiveText );
            }
            
            objName = mDataFormModel->detail().definitionName() + " line edit %1";
        }
        
        // Naming UI components for automation testability
        edit->setObjectName(objName.arg(aIndex.row()));
    }
}

void CntDetailEditor::discardChanges()
{
    QContact selected( *mDataFormModel->contact() );
    QVariant var;
    var.setValue(selected);
    mArgs.insert(ESelectedContact, var);
    mViewManager->back( mArgs );
}

/*!
    Saves all changes made to details (type of details depends on
    in which editor we are) of a contact. Doesn't save the contact.
*/
void CntDetailEditor::saveChanges()
{
    mDataFormModel->saveContactDetails();
    
    QContact selected( *mDataFormModel->contact() );
    QVariant var;
    var.setValue(selected);
    if ( mId == groupEditorView )
    {
       mArgs.insert(ESelectedGroupContact, var);
    }
    else
    {
        mArgs.insert(ESelectedContact, var);
        
        QContactDetail selectedDetail( mDataFormModel->detail() );
        QVariant var2;
        var2.setValue( selectedDetail );
        mArgs.insert( ESelectedDetail, var2 );
    }
    
    mViewManager->back( mArgs );
}

/*!
    First saves all changes made to details (type of details depends on
    in which editor we are) of a contact and then saves the contact. This
    is only called when phonebook is being closed either from task swapper
    or by end key.
*/
void CntDetailEditor::saveContact()
{
    mDataFormModel->saveContactDetails();
    QContactManager& mgr = mEngine->contactManager( SYMBIAN_BACKEND );
    QString name = mgr.synthesizedContactDisplayLabel(*mDataFormModel->contact());
    
    if (name.isEmpty())
    {
        name = hbTrId("txt_phob_list_unnamed");
    }
    
    CntSaveManager::CntSaveResult result = mSaveManager->saveContact(mDataFormModel->contact(), &mgr);
    
    if (mId != groupEditorView)
    {
        switch (result)
        {
        case CntSaveManager::ESaved:
            HbDeviceNotificationDialog::notification(QString(),HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_contact_1_saved")).arg(name));
            break;
        case CntSaveManager::EUpdated:
            HbDeviceNotificationDialog::notification(QString(),HbParameterLengthLimiter(hbTrId("txt_phob_dpophead_contacts_1_updated")).arg(name));
            break;
        case CntSaveManager::EFailed:
            HbDeviceNotificationDialog::notification(QString(),hbTrId("SAVING FAILED!"));
            break;
        case CntSaveManager::EDeleted:
        case CntSaveManager::ENothingDone:
        default:
            break;
        }
    }
}

void CntDetailEditor::setHeader(QString aHeader)
{
    if (!mHeader) {
        mHeader = static_cast<HbGroupBox*> (document()->findWidget(QString("headerBox")));
    }
    mHeader->setHeading(aHeader);
}

void CntDetailEditor::setDetails(CntDetailEditorModel* aModel, HbAbstractViewItem* aPrototype)
{
    //Append custom view item prototype
    if ( aPrototype )
    {
        QList <HbAbstractViewItem*> protos = mDataForm->itemPrototypes();
        protos.append( aPrototype );
        mDataForm->setItemPrototypes( protos );
    }

    mDataFormModel = aModel;
    mDataForm->setModel( mDataFormModel );

}

int CntDetailEditor::viewId() const
{
    return mId;
}

/*!
Go back to the root view
*/
void CntDetailEditor::showRootView()
{   
    mViewManager->back( mArgs, true );
}


void CntDetailEditor::contactDeletedFromOtherSource(const QList<QContactLocalId>& contactIds)
{
    CNT_ENTRY
    
    QContact normalContact = mArgs.value(ESelectedContact).value<QContact>();
    QContact groupContact = mArgs.value(ESelectedGroupContact).value<QContact>();
    if ( contactIds.contains(normalContact.localId()) || contactIds.contains(groupContact.localId()) )
    {
        // Do not switch to the previous view immediately. List views are
        // not updated properly if this is not done in the event loop
        QTimer::singleShot(0, this, SLOT(showRootView()));
    }
    CNT_EXIT
}

void CntDetailEditor::enableDiscardChanges()
{
    mCancel->setDisabled(false);
}


void CntDetailEditor::handleKeypadOpen()
{
    CNT_ENTRY
    
  // enable full screen
    mView->setContentFullScreen(true);
    CNT_EXIT
}

void CntDetailEditor::handleKeypadClosed()
{
    CNT_ENTRY
    // disable full screen
    mView->setContentFullScreen(false);
    CNT_EXIT
}

// End of File

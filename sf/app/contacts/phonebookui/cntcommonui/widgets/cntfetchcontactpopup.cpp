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
#include "cntfetchcontactpopup.h"
#include "cntdocumentloader.h"
#include "cntfetchmarkall.h"
#include "cntdebug.h"

#include <cntlistmodel.h>
#include <hblistview.h>
#include <hbstyleloader.h>
#include <hbindexfeedback.h>
#include <hblistviewitem.h>
#include <hbscrollbar.h>
#include <hbmainwindow.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbsearchpanel.h>
#include <hblineedit.h>
#include <hbaction.h>
#include <hbstaticvkbhost.h>
#include <hbstyleloader.h>
#include <hbstyle.h>

#include <QGraphicsLinearLayout>
#include <QCoreApplication>
#include <QtAlgorithms>

const char *CNT_FETCHLIST_XML = ":/xml/contacts_fetchdialog.docml";

CntFetchContactPopup::CntFetchContactPopup( QContactManager& aMgr ) : 
mManager(aMgr),
mPopup( NULL ),
mListView( NULL ),
mEmptyView( NULL ),
mHeading( NULL ),
mPrimaryAction( NULL ),
mSearch( NULL ),
mModel( NULL ),
mMarkAll( NULL ),
mDoc( NULL )
{
    HbStyleLoader::registerFilePath(":/temp/fetchdialog.css");
            
    mDoc = new CntDocumentLoader();
        
    bool ok;
    mDoc->load( CNT_FETCHLIST_XML, &ok );
    
    if ( !ok )
    {
        qFatal("Unable to read %S", CNT_FETCHLIST_XML );
    }
    mDoc->load( CNT_FETCHLIST_XML, "size_section");
    
    mPopup = static_cast<HbDialog*>( mDoc->findWidget( "dialog" ) );
    mSearch = static_cast<HbSearchPanel*>( mDoc->findWidget( "searchPanel" ) );
    mMarkAll = static_cast<CntFetchMarkAll*>( mDoc->findWidget("markAll") );
    mListView = static_cast<HbListView*>( mDoc->findWidget("listView") );
    mEmptyView = static_cast<HbLabel*>( mDoc->findWidget("emptyLabel" ));
    mEmptyView->setSizePolicy(mListView->sizePolicy());
    mHeading = static_cast<HbLabel*>( mDoc->findWidget("heading") );
   
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(closePopup()));
    connect(mPopup, SIGNAL(aboutToClose()), this, SLOT(disconnectSignalsAndDelete()));
    connect(mSearch, SIGNAL(criteriaChanged(QString)), this, SLOT(setFilter(QString)));
    connect(mMarkAll, SIGNAL(markAll(int)), this, SLOT(selectAll(int)) );
    
    HbLineEdit *editor = static_cast<HbLineEdit*>( mSearch->primitive("lineedit"));
    editor->setInputMethodHints(Qt::ImhNoPredictiveText);
    mSearch->setCancelEnabled( false );
    
    mVirtualKeyboard = new HbStaticVkbHost(editor);
    connect(mVirtualKeyboard, SIGNAL(keypadOpened()), this, SLOT(handleKeypadOpen()));
    connect(mVirtualKeyboard, SIGNAL(keypadClosed()), this, SLOT(handleKeypadClosed()));
    
    mMarkAll->setZValue( 2 );
}

CntFetchContactPopup::~CntFetchContactPopup()
{
    HbStyleLoader::unregisterFilePath(":/temp/fetchdialog.css");
    delete mDoc;
}

CntFetchContactPopup* CntFetchContactPopup::createMultiSelectionPopup( QString aTitle, QString aAction, QContactManager& aManager )
{
    CntFetchContactPopup* popup = new CntFetchContactPopup( aManager );
    popup->constructPopupDialog( aTitle, aAction, HbAbstractItemView::MultiSelection );
    return popup;
}

CntFetchContactPopup* CntFetchContactPopup::createSingleSelectionPopup( QString aTitle, QContactManager& aManager )
{
    CntFetchContactPopup* popup = new CntFetchContactPopup( aManager );
    popup->constructPopupDialog( aTitle, "", HbAbstractItemView::NoSelection );
    
    return popup;
}

void CntFetchContactPopup::setSelectedContacts( QSet<QContactLocalId> aIds )
{
    CNT_ENTRY
    if ( mListView->selectionMode() == HbAbstractItemView::MultiSelection )
    {
        mIdList.clear();
        foreach ( QContactLocalId id, aIds ) 
        {
            mIdList.append( id );
            QModelIndex contactIndex = mModel->indexOfContactId(id);
            mSelectionModel->select( contactIndex, QItemSelectionModel::Select );
        }
    }
    CNT_EXIT
}

void CntFetchContactPopup::showPopup()
{
    CNT_ENTRY
    if ( mListView->selectionMode() == HbAbstractItemView::MultiSelection )
    {
        mIds.clear();
        mIds = mIdList;
        qSort(mIds.begin(), mIds.end());
    }
    
    if ( mPrimaryAction )
    {
        mPrimaryAction->setEnabled(false);
    }
    
    
    HbMainWindow* window = mPopup->mainWindow();
    if ( window )
    {
        connect(window, SIGNAL(orientationChanged(Qt::Orientation)), 
                this, SLOT(orientationChanged(Qt::Orientation)) );
    }
    
    mPopup->open( this, SLOT(dialogDismissed(HbAction*)) );
    CNT_EXIT
}

void CntFetchContactPopup::handleKeypadOpen()
{
    CNT_ENTRY
    
    HbListViewItem* prototype = mListView->listItemPrototype();
    prototype->setTextFormat( Qt::RichText );
    
    qreal margin;
    HbStyle style;
    style.parameter("hb-param-margin-gene-popup", margin);

    if (mPopup->mainWindow()->orientation() == Qt::Horizontal)
        margin /= 2;
    
    qreal height = mPopup->size().height() - 
            mVirtualKeyboard->keyboardArea().height() - 
            mSearch->size().height() +
            margin;
    
    // in single selection we don't have the "mark all" option
    if ( mMarkAll->isVisible() )
    {
        height = height - mMarkAll->size().height();
    }

    mEmptyView->setMaximumHeight( height );
    mListView->setMaximumHeight( height );
    CNT_EXIT
}

void CntFetchContactPopup::handleKeypadClosed()
{
    CNT_ENTRY
    
    if (mSearch->criteria().isEmpty())
    {
        HbListViewItem* prototype = mListView->listItemPrototype();
        prototype->setTextFormat( Qt::PlainText );
    }
    
    mListView->setMaximumHeight( -1 );
    mEmptyView->setMaximumHeight( -1 );
    CNT_EXIT
}

void CntFetchContactPopup::contactSelected( const QModelIndex& aIndex )
{
    CNT_ENTRY
    if ( aIndex.isValid() )
    {
        QContact contact = mModel->contact( aIndex );
        mIdList.append( contact.localId() );
        
        emit fetchReady( mIdList.toSet() );
    }
    
    disconnect(mListView, SIGNAL(activated(const QModelIndex&)), 
                    this, SLOT(contactSelected(const QModelIndex&)) );
    mPopup->close();
    CNT_EXIT
}

void CntFetchContactPopup::contactsSelected(
        const QItemSelection& aSelected, 
        const QItemSelection& aDeselected )
{
    CNT_ENTRY
    
    // remove all deselected items
    foreach ( QModelIndex index, aDeselected.indexes() )
    {
        QContactLocalId id = mModel->contactId( index );
        if ( mIdList.contains(id) )
        {
            mIdList.removeAll( id );
        }
    }
    
    // add all selected items
    foreach ( QModelIndex index, aSelected.indexes() )
    {
        QContactLocalId id = mModel->contactId( index );
        if ( !mIdList.contains(id) )
        {
            mIdList.append( id );
        }
    }
    
    qSort(mIdList.begin(), mIdList.end()); 
    if (mIdList == mIds)
    {
        mPrimaryAction->setEnabled(false);
    }
    else
    {
        mPrimaryAction->setEnabled(true);
    }
    
    mMarkAll->setSelectedContactCount( mIdList.size() );
    CNT_EXIT
}

void CntFetchContactPopup::selectAll( int aState )
{
    CNT_ENTRY
    if ( mListView->selectionMode() == HbAbstractItemView::MultiSelection )
    {
        switch ( aState )
        {
        case Qt::Checked: 
            mListView->selectAll();
            mMarkAll->setSelectedContactCount( mIdList.count() );
            break;
            
        case Qt::Unchecked:
            mListView->clearSelection();
            mMarkAll->setSelectedContactCount( 0 );
            break;
        default:
            break;
        }
    }
    CNT_EXIT
}

void CntFetchContactPopup::dialogDismissed( HbAction* aAction )
{
    CNT_ENTRY
   
    if ( aAction != mPrimaryAction )
    {
        emit fetchCancelled();
    }
    else if ( aAction ) 
    {
        disconnect(mSelectionModel, SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                        this, SLOT(contactsSelected(const QItemSelection&, const QItemSelection&)) );
        
        QModelIndexList indexList = mSelectionModel->selectedIndexes();
        foreach ( QModelIndex index, indexList )
        {
            mIdList.append(mModel->contactId(index));
        }
        emit fetchReady( mIdList.toSet() );
    }
    else
    {
        // ignore.
    }
    CNT_EXIT
}

void CntFetchContactPopup::setFilter( const QString& aFilter )
{
    CNT_ENTRY
    QContactDetailFilter detailfilter;
    detailfilter.setMatchFlags( QContactFilter::MatchStartsWith );
    detailfilter.setValue( aFilter.split(QRegExp("\\s+"), QString::SkipEmptyParts) );
    detailfilter.setDetailDefinitionName(
            QContactDisplayLabel::DefinitionName,
            QContactDisplayLabel::FieldLabel);

    int oldRowCount = mModel->rowCount();
    mModel->setFilter(detailfilter);
    int newRowCount = mModel->rowCount();
    
    // minor optimization, only reload layout when needed
    if ((oldRowCount == 0 && newRowCount > 0) || (oldRowCount > 0 && newRowCount == 0))
    {
        HbMainWindow* window = mPopup->mainWindow();
        if ( window )
        {
            loadLayout( window->orientation() );
        }
    }
    
    setSelectedContacts( mIdList.toSet() );
    CNT_EXIT
}

void CntFetchContactPopup::constructPopupDialog( QString aTitle, QString aAction, HbAbstractItemView::SelectionMode aMode )
{
    CNT_ENTRY
    mIdList.clear();
    mTitle = aTitle;
    
    mPopup->setAttribute( Qt::WA_DeleteOnClose, true );
    
    QContactDetailFilter contactsFilter;
    contactsFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    contactsFilter.setValue(QString(QLatin1String(QContactType::TypeContact)));
    
    mModel = new CntListModel( &mManager, contactsFilter, false, mPopup);
    mModel->showMyCard(false);
    mSelectionModel = new QItemSelectionModel( mModel );
    
    mListView->setFrictionEnabled(true);
    mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    mListView->verticalScrollBar()->setInteractive(true);

    HbIndexFeedback* indexFeedback = new HbIndexFeedback(mPopup);
    indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    indexFeedback->setItemView(mListView);
         
    HbListViewItem *prototype = mListView->listItemPrototype();
    prototype->setGraphicsSize(HbListViewItem::Thumbnail);
    prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
        
    mListView->setSelectionMode( aMode );
    mListView->setModel( mModel );
    mListView->setSelectionModel( mSelectionModel );   

    
    if ( !aAction.isEmpty() )
    {
        mPrimaryAction = new HbAction( aAction, mPopup );
        mPopup->addAction( mPrimaryAction );
    }
        
    HbAction* secondaryAction = new HbAction(hbTrId("txt_common_button_cancel"), mPopup);
    mPopup->addAction( secondaryAction );
        
    if ( aMode != HbListView::MultiSelection )
    {
        connect(mListView, SIGNAL(activated(const QModelIndex&)), 
                this, SLOT(contactSelected(const QModelIndex&)) );
        mMarkAll->setVisible( false );
    }
    else
    {
        mMarkAll->setMaxContactCount( mModel->rowCount() );
        mMarkAll->setSelectedContactCount( 0 );
        
        connect(mSelectionModel, SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                this, SLOT(contactsSelected(const QItemSelection&, const QItemSelection&)) );
            
    }
    
    HbMainWindow* window = mPopup->mainWindow();
    if ( window )
    {
        loadLayout( window->orientation() );
    }
        
    CNT_EXIT
}

void CntFetchContactPopup::loadLayout( Qt::Orientation aOrientation )
{
    CNT_ENTRY
    
    bool multi = mListView->selectionMode() == HbAbstractItemView::MultiSelection;
    if ( mModel->rowCount() > 0 )
    {
        if ( aOrientation == Qt::Horizontal )
        {
            mPopup->setHeadingWidget( NULL );
            mDoc->load( CNT_FETCHLIST_XML, "find_list_landscape");
        }
        else
        {
            if (mPopup->headingWidget() == NULL)
            {
                mHeading = new HbLabel( mTitle );
                mPopup->setHeadingWidget( mHeading );
            }
            else
            {
                mHeading->setPlainText( mTitle );
            }
            
            mDoc->load( CNT_FETCHLIST_XML, multi ? "find_list" : "find_list_single");
        }
    }
    else
    {
        if ( aOrientation == Qt::Horizontal )
        {
            mPopup->setHeadingWidget( NULL );
            mDoc->load( CNT_FETCHLIST_XML, "find_empty_landscape" );
        }
        else
        {
            if (mPopup->headingWidget() == NULL)
            {
                mHeading = new HbLabel( mTitle );
                mPopup->setHeadingWidget( mHeading );
            }
            else
            {
                mHeading->setPlainText( mTitle );
            }

            mDoc->load( CNT_FETCHLIST_XML, multi ? "find_empty" : "find_empty_single" );
        }
     }
    
    CNT_EXIT
}

void CntFetchContactPopup::orientationChanged( Qt::Orientation aOrientation )
{
    CNT_ENTRY
    
    mDoc->load( CNT_FETCHLIST_XML, "size_section");
    
    loadLayout(aOrientation);
    
    CNT_EXIT
}

void CntFetchContactPopup::closePopup()
{
    CNT_ENTRY
    
    mPopup->close();
    
    disconnectSignalsAndDelete();
    
    CNT_EXIT
}

void CntFetchContactPopup::disconnectSignalsAndDelete()
{
    CNT_ENTRY
    disconnect(mVirtualKeyboard, SIGNAL(keypadOpened()), this, SLOT(handleKeypadOpen()));
    disconnect(mVirtualKeyboard, SIGNAL(keypadClosed()), this, SLOT(handleKeypadClosed()));
           
    deleteLater();
    CNT_EXIT
}
// End of File

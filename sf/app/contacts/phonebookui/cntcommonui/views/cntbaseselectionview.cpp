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

#include "cntbaseselectionview.h"

#include <hbview.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbindexfeedback.h>
#include <hbscrollbar.h>
#include <hbdocumentloader.h>
#include <hbaction.h>
#include "cntlistmodel.h"

const char *CNT_SELECTION_LISTVIEW_UI_XML = ":/xml/contacts_list.docml";

CntBaseSelectionView::CntBaseSelectionView() : 
QObject(),
mDocument(NULL)
{
    mDocument = new HbDocumentLoader();
    
    bool ok;
    mDocument->load( CNT_SELECTION_LISTVIEW_UI_XML, &ok );
    if ( !ok )
    {
        qFatal( "Unable to load %S", CNT_SELECTION_LISTVIEW_UI_XML );
    }
    mView = static_cast<HbView*>( mDocument->findWidget("view") );
    mListView = static_cast<HbListView*>( mDocument->findWidget("listView") );

    mListView->setFrictionEnabled(true);
    mListView->setScrollingStyle(HbScrollArea::PanWithFollowOn);
    mListView->verticalScrollBar()->setInteractive(true);
    mListView->setUniformItemSizes(true);

    HbListViewItem* prototype = mListView->listItemPrototype();
    
    HbIndexFeedback *indexFeedback = new HbIndexFeedback(mView);
    indexFeedback->setIndexFeedbackPolicy(HbIndexFeedback::IndexFeedbackSingleCharacter);
    indexFeedback->setItemView(mListView);
    
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect( mSoftkey, SIGNAL(triggered()), this, SIGNAL(viewClosed()) );
}

CntBaseSelectionView::~CntBaseSelectionView()
{
    delete mDocument;
}

void CntBaseSelectionView::activate( const CntViewParameters aArgs )
{
    mMgr = &mEngine->viewManager();
    
    if ( mView->navigationAction() != mSoftkey)
        mView->setNavigationAction(mSoftkey);
    
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    QString typeContact = QContactType::TypeContact;
    filter.setValue(typeContact);

    mListModel = new CntListModel( &mEngine->contactManager(SYMBIAN_BACKEND), filter);
    mListModel->showMyCard( false );
    
    mListView->setModel( mListModel );
    
    if ( aArgs.contains(ESelectionMode) ) {
        mListView->setSelectionMode( static_cast<HbAbstractItemView::SelectionMode>(aArgs.value(ESelectionMode).toInt()) );
    }
    
    emit viewOpened( mMgr, aArgs );
}

void CntBaseSelectionView::deactivate()
{
}

bool CntBaseSelectionView::isDefault() const
{
    return false;
}

HbView* CntBaseSelectionView::view() const
{
    return mView;
}

// EOF

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

#include "cntservicecontactselectionview.h"

#include <cntservicescontact.h>
#include <QCoreApplication>
#include <QModelIndex>
#include <hblistview.h>
#include <cntlistmodel.h>

CntServiceContactSelectionView::CntServiceContactSelectionView( CntAbstractServiceProvider& aServiceProvider ):
CntBaseSelectionView(),
mProvider( aServiceProvider )
{
    connect(mListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(onListViewActivated(const QModelIndex&)));
    connect( this, SIGNAL(viewClosed()), this, SLOT(aboutToCloseView()) );
    connect( this, SIGNAL(viewOpened(CntAbstractViewManager*, const CntViewParameters)), this, SLOT(aboutToOpenView(CntAbstractViewManager*, const CntViewParameters)) );
}

CntServiceContactSelectionView::~CntServiceContactSelectionView()
{

}

// An item in the selection list has been clicked
void CntServiceContactSelectionView::onListViewActivated(const QModelIndex &aIndex)
{
    QContact contact = mListModel->contact(aIndex);
    contact.saveDetail(&mDetail);
    
    CntViewParameters params;
    params.insert(EViewId, serviceEditView);
    QVariant var;
    var.setValue(contact);
    params.insert(ESelectedContact, var);
    mMgr->changeView(params);
}

void CntServiceContactSelectionView::aboutToCloseView()
{
    QVariant variant;
    variant.setValue(KCntServicesReturnValueContactNotModified);
    mProvider.CompleteServiceAndCloseApp(variant);
    CntViewParameters args;
    mMgr->back( args );
}

void CntServiceContactSelectionView::aboutToOpenView(CntAbstractViewManager* aMgr, const CntViewParameters viewParameters)
{
    mMgr = aMgr;
    
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    QString typeContact = QContactType::TypeContact;
    filter.setValue(typeContact);
    mListModel->setFilter(filter);
    // hide my card if it's not set
    if (mListModel->myCardId() == 0)
    {
        mListModel->showMyCard(false);
    }
    mDetail = viewParameters.value(ESelectedDetail).value<QContactDetail>();
}

// EOF

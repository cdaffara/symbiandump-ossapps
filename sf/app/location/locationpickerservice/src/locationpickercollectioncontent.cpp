/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: LocationPickerCollectionContent implementation
*
*/

#include <QStandardItemModel>

#include "locationpickerproxymodel.h"
#include "locationpickercollectioncontent.h"
#include "locationpickerdatamanager.h"
#include "locationpickertypes.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------
// LocationPickerCollectionContent::LocationPickerCollectionContent()
// -----------------------------------------------------------------
LocationPickerCollectionContent::LocationPickerCollectionContent( quint32 aCollectionId )
    :mProxyModel(NULL),
    mModel(NULL),
    mDataManager(NULL),
	mLocationFound(false)
{
    // Create a standard model for the view list
    mModel = new QStandardItemModel( this );
    // create data manager to manage data in the model
    mDataManager = LocationPickerDataManager::getInstance();
    if( mDataManager->populateModel( *mModel, ELocationPickerCollectionContent, aCollectionId ) )
    {
        // Create the proxy model.
        mProxyModel = new LocationPickerProxyModel();
        mProxyModel->setSourceModel(mModel);
        mProxyModel->setDynamicSortFilter(TRUE);
        mProxyModel->setSortRole(Qt::DisplayRole);
        mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
        // sort
        mProxyModel->sort(0, Qt::AscendingOrder);
        mLocationFound = true;
    }
    else
    {
        // no locations to display.
        QStandardItem *modelItem = new QStandardItem();
        modelItem->setData(QVariant(hbTrId("txt_lint_list_no_location_entries_present")), Qt::DisplayRole);
        mModel->appendRow( modelItem );
        mLocationFound = false;
     }
}

// ----------------------------------------------------------------
// LocationPickerCollectionContent::~LocationPickerCollectionContent
// -----------------------------------------------------------------
LocationPickerCollectionContent::~LocationPickerCollectionContent()
{
    delete mProxyModel;
    delete mModel;
}

// ----------------------------------------------------------------
// LocationPickerCollectionContent::getProxyModel
// -----------------------------------------------------------------
LocationPickerProxyModel* LocationPickerCollectionContent::getProxyModel()
{
    return mProxyModel;
}

// ----------------------------------------------------------------------------
// LocationPickerCollectionContent::getData()
// ----------------------------------------------------------------------------

void LocationPickerCollectionContent::getData( QModelIndex aIndex, quint32& aValue )
{
    QStandardItem* item = mModel->item( aIndex.row(), aIndex.column() );
    QVariant var = item->data( Qt::UserRole );
    aValue = var.toUInt();
}

// ----------------------------------------------------------------------------
// LocationPickerCollectionContent::locationFound()
// ----------------------------------------------------------------------------
bool LocationPickerCollectionContent::locationFound()
{
    return mLocationFound;
}

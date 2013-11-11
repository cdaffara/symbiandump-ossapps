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
* Description: LocationPickerCollectionListContent implementation
*
*/

#include <QStandardItemModel>

#include "locationpickercollectionlistcontent.h"
#include "locationpickerdatamanager.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------
// LocationPickerCollectionListContent::LocationPickerCollectionListContent()
// ----------------------------------------------------------------------
LocationPickerCollectionListContent::LocationPickerCollectionListContent()
    :mModel(NULL),
    mDataManager(NULL)
{
    // Create a standard model for the view list
    mModel = new QStandardItemModel( this );
    if(mModel)
    {
        // create data manager to manage data in the model
        mDataManager = LocationPickerDataManager::getInstance();
        bool populated = mDataManager->populateModel( *mModel, ELocationPickerCollectionListContent);
        if(!populated)
        {
        qFatal("Error creating collection");
        }
    }
}

// ----------------------------------------------------------------------
// LocationPickerCollectionListContent::~LocationPickerCollectionListContent()
// ----------------------------------------------------------------------
LocationPickerCollectionListContent::~LocationPickerCollectionListContent()
{
    delete mModel;
}

// ----------------------------------------------------------------
// LocationPickerCollectionListContent::getStandardModel
// -----------------------------------------------------------------
QStandardItemModel* LocationPickerCollectionListContent::getStandardModel()
{
    return mModel;
}

// ----------------------------------------------------------------------------
// LocationPickerCollectionListContent::getData()
// ----------------------------------------------------------------------------

void LocationPickerCollectionListContent::getData( QModelIndex aIndex, quint32& aValue )
{
    QStandardItem* item = mModel->item( aIndex.row(), aIndex.column() );
    QVariant var = item->data( Qt::UserRole );
    aValue = var.toUInt();
}

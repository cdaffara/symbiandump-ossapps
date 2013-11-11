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
* Description: LocationPickerContent implementation
*
*/

#include <QStandardItemModel>

#include "locationpickercontent.h"
#include "locationpickerproxymodel.h"
#include "locationpickertypes.h"
#include "locationpickerdatamanager.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// LocationPickerContent::LocationPickerContent()
// -----------------------------------------------------------------------------
LocationPickerContent::LocationPickerContent()
	:mDataManager(NULL),
	mStandardModel(NULL)
{
    // create data manager to manage data in the model
    mDataManager = LocationPickerDataManager::getInstance();

}

// -----------------------------------------------------------------------------
// LocationPickerContent::populateModel()
// -----------------------------------------------------------------------------
bool LocationPickerContent::populateModel()
{
    bool locationsFound;
    // Create a standard model for the list view
    mStandardModel = new QStandardItemModel( this );
    if( mDataManager->populateModel( *mStandardModel, ELocationPickerContent ) )
    {
        locationsFound = true;
    }
    else
    {
        createNoEntryDisplay(mStandardModel);
        locationsFound = false;
    }
    return locationsFound;
}

// -----------------------------------------------------------------------------
// LocationPickerContent::LocationPickerContent()
// -----------------------------------------------------------------------------
LocationPickerContent::~LocationPickerContent()
{
    delete mStandardModel;
}


// -----------------------------------------------------------------------------
// LocationPickerContent::getStandardModel()
// -----------------------------------------------------------------------------
QStandardItemModel* LocationPickerContent::getStandardModel()
{
    return mStandardModel;
}

// ----------------------------------------------------------------------------
// LocationPickerContent::createNoEntryDisplay()
// ----------------------------------------------------------------------------

void LocationPickerContent::createNoEntryDisplay( QStandardItemModel *aModel )
{
    // no locations to display.
    QStandardItem *modelItem = new QStandardItem();
    modelItem->setData(QVariant(hbTrId("txt_lint_list_no_location_entries_present")), Qt::DisplayRole);
    aModel->appendRow( modelItem );
}

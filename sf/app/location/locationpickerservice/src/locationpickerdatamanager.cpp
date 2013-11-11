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
* Description: LocationPickerDataManager implementation
*
*/

#include "locationpickerdatamanager.h"
#include "locationpickerdatamanager_p.h"


// ----------------------------------------------------------------------------
// LocationPickerDataManager::getInstance()
// ----------------------------------------------------------------------------
LocationPickerDataManager* LocationPickerDataManager::getInstance()
{
    static LocationPickerDataManager singletonInstance;
    return &singletonInstance;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManager::LocationPickerDataManager()
// ----------------------------------------------------------------------------

LocationPickerDataManager::LocationPickerDataManager() :
    d_ptr( new LocationPickerDataManagerPrivate() )
{
}


// ----------------------------------------------------------------------------
// LocationPickerDataManager::~LocationPickerDataManager()
// ----------------------------------------------------------------------------
LocationPickerDataManager::~LocationPickerDataManager()
{
    delete d_ptr;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManager::populateModel()
// ----------------------------------------------------------------------------
bool LocationPickerDataManager::populateModel( QStandardItemModel &aModel, TViewType aViewType, 
        quint32 aCollectionId )
{   
    Q_D( LocationPickerDataManager);
    return( d->populateModel( aModel, aViewType, aCollectionId) );
}

// ----------------------------------------------------------------------------
// LocationPickerDataManager::getLocationItem()
// ----------------------------------------------------------------------------

void LocationPickerDataManager::getLocationItem( quint32 aLmId, QLocationPickerItem& aItem )
{
    Q_D( LocationPickerDataManager);
    return( d->getLocationItem( aLmId, aItem ) );
}

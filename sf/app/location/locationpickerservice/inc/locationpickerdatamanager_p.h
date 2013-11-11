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
* Description: LocationPickerDataManager private declaration
*
*/

#ifndef LOCATIONPICKERDATAMANAGER_P_H
#define LOCATIONPICKERDATAMANAGER_P_H

#include <QStandardItemModel>
#include "locationpickertypes.h"
#include "qlocationpickeritem.h"

class LocationDataLookupDb;
class QLookupItem;

/**  Class used for managing the data of model
 *
 */
class LocationPickerDataManagerPrivate
{
public:
    // constructor
    LocationPickerDataManagerPrivate();
    
    // destructor
    ~LocationPickerDataManagerPrivate();
    
    // populates the model with data
    bool populateModel( QStandardItemModel &aModel, TViewType aViewType, 
            quint32 aCollectionId = 0 );
    
    // gets the location item
    void getLocationItem( quint32 aLmId, QLocationPickerItem &aItem );

private:
    // populates landmarks
    bool populateLandmarks( QList<QLookupItem> &aItemArray );
    // populates collections
    void populateCollections( QList<int>& aCount );

private:
    QStandardItemModel *mModel;
    TViewType mViewType;
    LocationDataLookupDb *mDb; 
    quint32 mCategoryId;
};

#endif // LOCATIONPICKERDATAMANAGER_P_H

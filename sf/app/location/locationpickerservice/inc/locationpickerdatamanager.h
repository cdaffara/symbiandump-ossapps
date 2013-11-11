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
* Description: LocationPickerDataManager declaration
*
*/

#ifndef LOCATIONPICKERDATAMANAGER_H
#define LOCATIONPICKERDATAMANAGER_H

#include <QStandardItemModel>
#include "locationpickertypes.h"
#include "qlocationpickeritem.h"

class LocationPickerDataManagerPrivate;

/**  Class used for managing the data of model
 *
 */
class LocationPickerDataManager
{
public:
    static LocationPickerDataManager* getInstance();
private:
    // constructors
    LocationPickerDataManager();

    // destructor
    ~LocationPickerDataManager();
public:
    // populates the model with data
    bool populateModel( QStandardItemModel &aModel, TViewType aViewType, 
            quint32 aCollectionId = 0);

    // gets the location item
    void getLocationItem( quint32 aLmId, QLocationPickerItem &aItem );

private:

    LocationPickerDataManagerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, LocationPickerDataManager)
};

#endif // LOCATIONPICKERDATAMANAGER_H

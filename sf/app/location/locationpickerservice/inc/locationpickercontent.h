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
* Description: LocationPickerContent declaration
*
*/

#ifndef LOCATIONPICKERCONTENT_H
#define LOCATIONPICKERCONTENT_H


#include "locationpickerproxymodel.h"

//forward declaration
class QStandardItemModel;
class LocationPickerDataManager;

/**  
 *Class to create model for Location Picker
 */
class LocationPickerContent : public QObject
{
    Q_OBJECT
public:

    // contructor
    LocationPickerContent();

    // destructor
    ~LocationPickerContent();
    
    //get standard Grid Model
    QStandardItemModel* getStandardModel();
    
    void createNoEntryDisplay( QStandardItemModel *aModel );
    
    bool populateModel();

private:
    Qt::Orientations mOrientation;
    LocationPickerDataManager *mDataManager;
    QStandardItemModel *mStandardModel;
    
};


#endif // LOCATIONPICKERCONTENT_H

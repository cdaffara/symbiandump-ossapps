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
* Description: LocationPickerCollectionListContent declaration
*
*/

#ifndef LOCATIONPICKERCOLLECTIONLISTCONTENT_H
#define LOCATIONPICKERCOLLECTIONLISTCONTENT_H

class QStandardItemModel;
class LocationPickerDataManager;

/**  
 * Class for handling Collections content of location picker
 */
class LocationPickerCollectionListContent : public QObject
{
public:

    // constructor
    LocationPickerCollectionListContent();

    // destructor
    ~LocationPickerCollectionListContent();
    //get standard model
    QStandardItemModel* getStandardModel();
    // gets the data pointed to by index and copies to the aValue
    void getData( QModelIndex aIndex, quint32& aValue );

private:
    //standard Item Model
    QStandardItemModel *mModel;
    //datamanager
    LocationPickerDataManager *mDataManager;
};

#endif // LOCATIONPICKERCOLLECTIONLISTCONTENT_H

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
* Description: HgWidgetDataModel implementation
*
*/

#ifndef HGWIDGETDATAMODEL_H
#define HGWIDGETDATAMODEL_H

#include <QAbstractListModel>
#include <HbIcon>

class LocationPickerProxyModel;

/*!
 Constructs the data model
 */
class HgWidgetDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    //ImageType to be set
    enum TImageType
    {
        ETypeQIcon,
        ETypeHbIcon,
        ETypeQImage
    };
public:
    //constructor
    explicit HgWidgetDataModel( LocationPickerProxyModel *aProxyModel, QObject *aParent=0);
    //destructor
    virtual ~HgWidgetDataModel();
    //count of number of items in model
    int rowCount(const QModelIndex &aParent=QModelIndex()) const;
    //get the data from QStandardModel for HgWidgetModel 
    QVariant data(const QModelIndex &aIndex, int aRole=Qt::DisplayRole) const;
    //Set Image Type
    void setImageDataType(TImageType type);
    //reset the model with new data
    void resetModel( LocationPickerProxyModel *aProxyModel );

#ifdef LOCPICKER_UNIT_TEST
public:
#else    
private:
#endif
	//Image Type
    TImageType                       mImageType;
    //default Image type
    HbIcon                          mDefaultImage;
    //low resolution image
    bool                            mUseLowResImages;
    //proxymodel
    LocationPickerProxyModel        *mProxyModel;
};

#endif // HgWidgetDataModel_H


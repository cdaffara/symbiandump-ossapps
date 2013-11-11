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
* Description: LocationPickerDataManager private implementation
*
*/

#include <HbIcon>
#include <locationdatalookupdb.h>
#include <QFile>
#include "locationpickerdatamanager_p.h"


// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::LocationPickerDataManagerPrivate()
// ----------------------------------------------------------------------------

LocationPickerDataManagerPrivate::LocationPickerDataManagerPrivate() :
        mModel( NULL ),
        mViewType( ELocationPickerContent ),
        mDb( NULL )
{
    mDb = new LocationDataLookupDb();
    mDb->open();
}


// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::~LocationPickerDataManagerPrivate()
// ----------------------------------------------------------------------------
LocationPickerDataManagerPrivate::~LocationPickerDataManagerPrivate()
{
    // delete the member variables;
    if( mDb )
    {
        mDb->close();
        delete mDb;
        mDb = NULL;
    }
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateModel()
// ----------------------------------------------------------------------------
bool LocationPickerDataManagerPrivate::populateModel(  QStandardItemModel &aModel, 
        TViewType aViewType, quint32 aCollectionId )
{
    mModel = &aModel;
    mViewType = aViewType;

    if( !mDb )
    {
        // no items in the landmark database, so return false.
        return false;
    }

    switch( mViewType )
    {
        case ELocationPickerContent:
        case ELocationPickerSearchView:
             {
                 QList<QLookupItem> itemArray;
                 mDb->getEntries( itemArray );
                 return populateLandmarks( itemArray );
             }
             
        case ELocationPickerCollectionListContent:
             {
                 QList<int> aCount;
                 mDb->getCount(aCount, aCollectionId);
                 populateCollections(aCount);
                 return true;
             }
        case ELocationPickerCollectionContent:
             {
                 QList<QLookupItem> itemArray;
                 mDb->getEntries( itemArray, aCollectionId );             
                 if( itemArray.count() == 0 )
                     return false;
                 
                 return populateLandmarks( itemArray );
             }
    }
    return true;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateLandmarks()
// ----------------------------------------------------------------------------

bool LocationPickerDataManagerPrivate::populateLandmarks( QList<QLookupItem> &aItemArray )
{    
    mModel->clear();

    if( !aItemArray.count() )
    {
        return false;
    }
    QString lmAddressLine1;
    QString lmAddressLine2;
    QString lmAddressLine3;
    QString contextAddress;
    QString contextAddressCountry;
    
    for( int i = 0; i < aItemArray.count(); i++ )
    {
        contextAddress.clear();
        contextAddressCountry.clear();
        lmAddressLine1.clear();
        lmAddressLine2.clear();
        lmAddressLine3.clear();
    
        if( !aItemArray[i].mIsDuplicate )
        {
            lmAddressLine1 = aItemArray[i].mName;
            
            bool addressEmtpy = true; // used to check if address line 2 is empty
            if( !aItemArray[i].mStreet.isEmpty() )
            {
                if( lmAddressLine1.isEmpty() )
                {
                    lmAddressLine2 = aItemArray[i].mStreet;
                }
                else
                {
                    lmAddressLine3 = aItemArray[i].mStreet;
                    addressEmtpy = EFalse;
            	}
            }
            if( !aItemArray[i].mCity.isEmpty() )
            {
                if( lmAddressLine2.isEmpty() )
                {
                    lmAddressLine2 = aItemArray[i].mCity;
                }
                else
                {
                    if( !addressEmtpy )
                    {
                        lmAddressLine3 = lmAddressLine3 + KSeparator;
                        lmAddressLine3 = lmAddressLine3 + KSpace;
                        lmAddressLine3 = lmAddressLine3 + aItemArray[i].mCity;
                    }
                    else
                    {
                        lmAddressLine3 = aItemArray[i].mCity;
                        addressEmtpy = EFalse;
                    }
                }
            }
            if( !aItemArray[i].mState.isEmpty() )
            {
                if( lmAddressLine2.isEmpty() )
                {
                    lmAddressLine2 = aItemArray[i].mState;
                }
                else
                {
                    if( !addressEmtpy )
                    {
                        lmAddressLine3 = lmAddressLine3 + KSeparator;
                        lmAddressLine3 = lmAddressLine3 + KSpace;
                        lmAddressLine3 = lmAddressLine3 + aItemArray[i].mState;
                    }
                    else
                    {
                        lmAddressLine3 = aItemArray[i].mState;
                        addressEmtpy = EFalse;
                    }
                }
            }
            contextAddress = lmAddressLine3;
            if( !aItemArray[i].mCountry.isEmpty() )
            {
                if( !addressEmtpy )
                {
                    lmAddressLine3 = lmAddressLine3 + KSeparator;
                    lmAddressLine3 = lmAddressLine3 + KSpace;
                    lmAddressLine3 = lmAddressLine3 + aItemArray[i].mCountry;
                }
                else
                {
                    lmAddressLine3 = aItemArray[i].mCountry;
                    addressEmtpy = EFalse;
                }
                contextAddressCountry = aItemArray[i].mCountry;
            }
            // set icons based on contact address type
            QVariantList icons;
            HbIcon adressTypeIcon;
            HbIcon potraitIcon;
            QString adressType;
            bool adressIconPresent = false;
            if( aItemArray[i].mSourceType == ESourceContactsHome )
            {
                adressTypeIcon = HbIcon(KContactHomeIcon);
                adressType = KContactHomeIcon;
                adressIconPresent = true;
            }
            else if( aItemArray[i].mSourceType == ESourceContactsWork )
            {
                adressTypeIcon = HbIcon(KContactWorkIcon);
                adressType = KContactWorkIcon;
                adressIconPresent = true;
            }
            
            if( aItemArray[i].mSourceType == ESourceCalendar )
            {
                potraitIcon = HbIcon( KCalendarImage );
            }
            else if( aItemArray[i].mSourceType == ESourceContactsWork ||  aItemArray[i].mSourceType 
                    == ESourceContactsHome || aItemArray[i].mSourceType == ESourceContactsPref )
            {
                potraitIcon = HbIcon( KContactsImage );
            }    
            else
            {
                potraitIcon = HbIcon( KDummyImage );
            }
        
            // create a list item and set to model
            QStringList addressData;
            QString landscapeIconPath;

            if( QFile::exists( aItemArray[i].mMapTilePath ) )
            { 
                //draw maptile Icon            
                landscapeIconPath = QString( aItemArray[i].mMapTilePath );
            }
            else
            {
                //draw dummy icon
                landscapeIconPath = QString("");
            }
            addressData.clear();
            icons<<potraitIcon;
            if(adressIconPresent)
            {
                icons<<adressTypeIcon;
            }
            QStringList contextList;
            contextList<<lmAddressLine1<<contextAddress<<contextAddressCountry;
            QStandardItem *modelItem = new QStandardItem();
            addressData << lmAddressLine1 << lmAddressLine2<<lmAddressLine3;
            modelItem->setData(QVariant(addressData), Qt::DisplayRole);
            modelItem->setData( icons, Qt::DecorationRole );
            modelItem->setData( aItemArray[i].mId, Qt::UserRole );
            modelItem->setData(landscapeIconPath,Qt::UserRole+1);
            modelItem->setData(adressType,Qt::UserRole+2);
            modelItem->setData(QVariant(contextList), Qt::UserRole+3);
            
            mModel->appendRow( modelItem );
         }
   }
    
    return true;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateCollections()
// ----------------------------------------------------------------------------

void LocationPickerDataManagerPrivate::populateCollections(QList<int>& aCount)
{
    // add contact collection
    QStandardItem *modelItemContact = new QStandardItem();
    int conNum = aCount.value(0);
    QString contactCollectionNum;
    contactCollectionNum.setNum(conNum);
    contactCollectionNum.append(" items");
    QString contactCollectionName( hbTrId("txt_lint_list_contact_addresses") );
 

    QStringList contact = (QStringList()<<contactCollectionName<<contactCollectionNum);
    modelItemContact->setData( QVariant( contact ), Qt::DisplayRole );
      
    modelItemContact->setData( HbIcon ( KCollectionsContacts ), Qt::DecorationRole );
    modelItemContact->setData( ESourceLandmarksContactsCat, Qt::UserRole );
    mModel->appendRow( modelItemContact );
   
    //txt_lint_list_calender_addresses
    int calNum = aCount.value(1);
    QString calendarCollectionNum;
    calendarCollectionNum.setNum(calNum);
    calendarCollectionNum.append(" items");
    QString calendarCollectionName( hbTrId("txt_lint_list_calendar_locations") );
    QStringList calender = (QStringList()<<calendarCollectionName<<calendarCollectionNum);
    
    QStandardItem *modelItemCalendar = new QStandardItem();
    modelItemCalendar->setData( QVariant( calender ), Qt::DisplayRole );
    modelItemCalendar->setData( HbIcon ( KCollectionsCalendar ), Qt::DecorationRole );
    modelItemCalendar->setData( ESourceLandmarksCalendarCat, Qt::UserRole );
    mModel->appendRow( modelItemCalendar );
     
    //txt_lint_list_places_addresses
    int placNum = aCount.value(2);
    QString placesCollectionNum;
    placesCollectionNum.setNum(placNum);
    placesCollectionNum.append(" items");
    QString placesCollectionName( hbTrId("txt_lint_list_places") );
    QStringList places = (QStringList()<<placesCollectionName<<placesCollectionNum);

    QStandardItem *modelItemPlaces = new QStandardItem();
    modelItemPlaces->setData( QVariant( places ), Qt::DisplayRole );
    modelItemPlaces->setData( HbIcon (KCollectionsPlaces), Qt::DecorationRole );
    modelItemPlaces->setData( ESourceLandmarks, Qt::UserRole );
    mModel->appendRow( modelItemPlaces );
}


// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::getLocationItem()
// ----------------------------------------------------------------------------

void LocationPickerDataManagerPrivate::getLocationItem( quint32 aId, QLocationPickerItem& aItem )
{
    QLookupItem item;
    item.mId = aId;
    if( mDb->findEntryById( item ) )
    {
        aItem.mName = item.mName;
        aItem.mStreet = item.mStreet;
        aItem.mPostalCode = item.mPostalCode;
        aItem.mCity = item.mCity;
        aItem.mState = item.mState;
        aItem.mCountry = item.mCountry;
        aItem.mLatitude = item.mLatitude;
        aItem.mLongitude = item.mLongitude;
        aItem.mIsValid = true;
    }
    else
    {
        aItem.mIsValid = false;
    }
    
}


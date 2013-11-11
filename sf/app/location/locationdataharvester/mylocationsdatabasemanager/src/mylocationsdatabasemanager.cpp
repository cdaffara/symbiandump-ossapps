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
* Description: Database manager implementation 
*
*/

#include <EPos_CPosLandmarkCategory.h>
#include <EPos_CPosLmTextCriteria.h>
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_CPosLmDatabaseManager.h>
#include <EPos_CPosLmNearestCriteria.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmCategoryManager.h>

#include <lbsposition.h>
#include <barsread.h>
#include <barsc.h>
#include <locationservicedefines.h>
#include "mylocationsdatabasemanager.h"
#include "mylocationlogger.h"
#include "mylocationsdefines.h"

#include <locationdatalookupdb.h>
#include <QString>
#include <QList>
// separator
_LIT( KSeparator, ",");
// space
_LIT( KSpace, " ");

// QString separator
const QString KQStringSeparator = ",";
// QString space
const QString KQStringSpace = " ";

// Used to set nearest landmarks search distance criteria
const TUint32 KSearchCriteriaDistance = 100; 

// Maximum string length of landmark address.
const TUint32 KMaxAddressLength = 255; 

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::ConstructL()
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C void CMyLocationsDatabaseManager::ConstructL()
{
    __TRACE_CALLSTACK;//Open and intialize Landmark DB
    iLandmarkDb = CPosLandmarkDatabase::OpenL();
    ExecuteAndDeleteLD(iLandmarkDb->InitializeL());
 
    iLocationAppLookupDb = new LocationDataLookupDb();
    if( !iLocationAppLookupDb->open() )
    {
        User::Leave( KErrUnknown );
    }
    
    // Create category manager for landmarks
    iLandmarksCatManager = CPosLmCategoryManager::NewL(*iLandmarkDb);

    // open file session
    User::LeaveIfError(iFsSession.Connect());

    // Add contacts and calendar  categories
    iLmContactsCatId = AddMylocationsCategoryL(KContactsCategory);
    iLmCalendarCatId = AddMylocationsCategoryL( KCalendarCategory );

}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CMyLocationsDatabaseManager()
// Default constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C  CMyLocationsDatabaseManager::CMyLocationsDatabaseManager() : iLandmarkDb( NULL ),
                iLmContactsCatId( 0 ), //iLandmarksLookupDb( NULL ), 
                iLocationAppLookupDb( NULL ),
                iLandmarksCatManager( NULL )
{
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::_CMyLocationsDatabaseManager()
// Destructor.
// -----------------------------------------------------------------------------
//
CMyLocationsDatabaseManager::~CMyLocationsDatabaseManager()
{
    __TRACE_CALLSTACK;// delete member variables.

    if (iLocationAppLookupDb)
    {
        iLocationAppLookupDb->close();
        delete iLocationAppLookupDb;
    }

    delete iLandmarksCatManager;

    delete iLandmarkDb;

    // close the file session
    iFsSession.Close();

}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::AddMylocationsCategoryL()
// Adds the category to the mylocations and landmarks database..
// -----------------------------------------------------------------------------
//
TUint32 CMyLocationsDatabaseManager::AddMylocationsCategoryL( const TDesC&  aCategoryName )
{
    __TRACE_CALLSTACK;//Open the resource file
    
    TPosLmItemId catId = 0;
    
    //create category
    CPosLandmarkCategory *category = CPosLandmarkCategory::NewL();
    CleanupStack::PushL(category);
    category->SetCategoryNameL( aCategoryName );
    
    // Add category to landmarks database
    TRAPD ( error, ( catId = iLandmarksCatManager->AddCategoryL( *category ) ) );
    if (error == KErrNone || error == KErrAlreadyExists)
    {
        catId = iLandmarksCatManager->GetCategoryL( aCategoryName );
    }

    CleanupStack::PopAndDestroy(category);

    return catId;
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::UpdateDatabaseL()
// Updates the location into the landmark database and lookup table. Based on 
// the entry source type and the entry change type the updation can be 
// addition/modification/deletion.
// -----------------------------------------------------------------------------
//
EXPORT_C void CMyLocationsDatabaseManager::UpdateDatabaseL(CPosLandmark* aLandmark,
        const TUint32 aUid, const TUint32 aSourceType, const TUint32 aChangeType)
{
    __TRACE_CALLSTACK;//open the lookup database
    switch (aChangeType)
    {
    // if the entry is added
    case EEntryAdded:
    {
        // Handle this entry in the lookup table and update landmarks db.
        HandleEntryAdditionL(aLandmark, aUid, aSourceType);
        break;
    }
        // if the entry is modified
    case EEntryModified:
    {
        // Handle this entry in the lookup table and update landmarks db.
        HandleEntryModificationL(aLandmark, aUid, aSourceType);
        break;
    }
        // if the entry is deleted
    case EEntryDeleted:
    {
        // Handle this entry in the lookup table and update landmarks db.
        HandleEntryDeletionL(aUid, aSourceType);
        break;
    }
    }

}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CheckIfDuplicateExistsL()
// Checks if this landmark is already present in database. If present returns the landmark id, else 0
// -----------------------------------------------------------------------------
//
TPosLmItemId CMyLocationsDatabaseManager::CheckIfDuplicateExistsL(
                                    const CPosLandmark* aLandmark)
{
    __TRACE_CALLSTACK;// Stores the found duplicate landmark's id. 
    TPosLmItemId retId = 0;

    // Initially filter only the landmarks which are nearer to the current landmark.
    // Then we can make a duplicate check on each of the found landmarks.

    // create a search object.
    CPosLandmarkSearch* search = CPosLandmarkSearch::NewL(
            *iLandmarkDb);
    CleanupStack::PushL(search);

    TBuf<KMaxAddressLength> lmAddress1;
    GetLandmarkFullAddress( lmAddress1, aLandmark );
    QString str1 = QString( (QChar*)lmAddress1.Ptr(), lmAddress1.Length());

    // create nearest search criteria object
    TLocality position( TCoordinate( 0, 0), 0 );
    aLandmark->GetPosition( position );
    CPosLmNearestCriteria* nearestCriteria = 
                CPosLmNearestCriteria::NewLC( 
                        TCoordinate( position.Latitude(), position.Longitude() ) );
    nearestCriteria->SetMaxDistance( KSearchCriteriaDistance );
    
    // Start the search and execute it at once.
    ExecuteAndDeleteLD( search->StartLandmarkSearchL( *nearestCriteria ) );
    CleanupStack::PopAndDestroy( nearestCriteria );

    // Retrieve an iterator to access the matching landmarks.
    CPosLmItemIterator* iter = search->MatchIteratorL();
    CleanupStack::PushL(iter);

    // Iterate the search matches.
    TPosLmItemId lmId;

    while( ( lmId = iter->NextL() ) != KPosLmNullItemId )
    {
        CPosLandmark* lm = iLandmarkDb->ReadLandmarkLC( lmId );
        TBuf<KMaxAddressLength> lmAddress2;
        GetLandmarkFullAddress( lmAddress2, lm );
        QString str2 = QString( (QChar*)lmAddress2.Ptr(), lmAddress2.Length());
        CleanupStack::PopAndDestroy( lm );
        
        if( str1 == str2 )
        {
            retId = lmId;
            break;
        }
    }

    CleanupStack::PopAndDestroy(iter);
    CleanupStack::PopAndDestroy(search);

    return retId;
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CompareLandmarks()
// Compares two landmarks. Only the text fields, landmark name, street, city, state country and 
// postal code are compared.
// -----------------------------------------------------------------------------
//
TBool CMyLocationsDatabaseManager::CompareLandmarks(
        const CPosLandmark* aLandmark1, const CPosLandmark* aLandmark2 )
{
    __TRACE_CALLSTACK;

    TBuf<KMaxAddressLength> lmAddress1;
    GetLandmarkFullAddress( lmAddress1, aLandmark1 );
    QString str1 = QString( (QChar*)lmAddress1.Ptr(), lmAddress1.Length());

    TBuf<KMaxAddressLength> lmAddress2;
    GetLandmarkFullAddress( lmAddress2, aLandmark2 );
    QString str2 = QString( (QChar*)lmAddress2.Ptr(), lmAddress2.Length());

    if( str1 == str2 )
        return ETrue;
    else
        return EFalse;
    
    
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::HandleEntryAdditionL()
// Handles the entry addition in lookup table and landmarks db
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::HandleEntryAdditionL(CPosLandmark* aLandmark,
        const TUint32 aUid, const TUint32 aSourceType)
{
    __TRACE_CALLSTACK;
    // Create a lookup item
    QLookupItem lookupItem;
    lookupItem.mSourceUid = aUid;
    lookupItem.mSourceType = aSourceType;
    lookupItem.mDestId = 0;
    lookupItem.mIconType = QLookupItem::EIconTypeDefault;
    lookupItem.mIsDuplicate = 0;
    lookupItem.mIconPath = "";
    lookupItem.mMapTilePath = "";
    //fill address into lookup item.
    FillLookupItemAddressDetails( aLandmark, lookupItem );

    if ( aSourceType == ESourceLandmarks )
    {
        // Logic: check if the entry is already present in lookupdb. 
        // If present, it means the landmark corresponds to a contact/calendar. So ignore it.
        // If not present, it means the landmark is created directly into the landmarks db. So add
        // it in lookupdb as well.
        
        // check if the entry is already present in lookup db.
        QList<QLookupItem> itemArray;
        iLocationAppLookupDb->findEntriesByLandmarkId( aUid, itemArray );
        if( itemArray.count() )
        {
            return;
        }
        else
        {
            lookupItem.mDestId = aUid;
            iLocationAppLookupDb->createEntry( lookupItem );
            return;
        }
    }

    TPosLmItemId catId;
    if( aSourceType == ESourceCalendar )
    {
        // category id to calendar
        catId = iLmCalendarCatId;
    }
    else 
    {
        // remove landmark name, which is basically contact's name.
        aLandmark->SetLandmarkNameL( KNullDesC );
        // category id to contacts
        catId = iLmContactsCatId;
    }
    // check if this landmark is already present in database
    TPosLmItemId dupLmId = CheckIfDuplicateExistsL( aLandmark );
    if ( dupLmId )
    {
        // landmark already present in db. get the details
        CPosLandmark* dupLandmark = iLandmarkDb->ReadLandmarkLC(dupLmId);
        if( dupLandmark )
        {
            // add category.
            dupLandmark->AddCategoryL( catId );
            // update the landmark object in the db
            iLandmarkDb->UpdateLandmarkL( *dupLandmark );
            CleanupStack::PopAndDestroy( dupLandmark );
        }

        // point the lookup item's landmark uid to the existing landmark.
        lookupItem.mDestId = dupLmId;
        if( aSourceType == ESourceCalendar )
        {
            // set duplicate flag to true. only if it is calendar entry.
            // for contacts duplicate doesnot hold good as the location name is the contact name.
        
            // set duplicate only if there are calendar entries already pointing to this landmark. 
            if( IsDuplicateEntry( dupLmId ) )
            {
                lookupItem.mIsDuplicate = 1;
            }
        }
    }
    else // it is a new entry, so add into the database
    {
        // add category.
        aLandmark->AddCategoryL( catId );
        // add the landmark into the db. 
        // point the lookup item's landmark uid to the newly created landmark in the db.
        lookupItem.mDestId = iLandmarkDb->AddLandmarkL( *aLandmark );
    }

    // create the entry in the lookup table.
    iLocationAppLookupDb->createEntry( lookupItem );
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::HandleEntryModificationL()
// Handles the entry modification in the lookup table and landmarks db.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::HandleEntryModificationL(
        CPosLandmark* aLandmark, const TUint32 aUid, const TUint32 aSourceType )
{
    __TRACE_CALLSTACK;
    if ( aSourceType == ESourceLandmarks )
    {
        HandleLandmarkModificationL( aLandmark, aUid );
        return;
    }

    QLookupItem lookupItem;
    lookupItem.mSourceUid = aUid;
    lookupItem.mSourceType = aSourceType;
    lookupItem.mIconType = QLookupItem::EIconTypeDefault;

    // Behavior: If an entry is modified, 
    // If this entry is not present in lookup table. add the entry and update the landmarks db.
    // If this entry is already present in lookup table, check if the location info is modified or not.
    // If the location info is modified, delete the landmark from db and add the new landmark
    // into the db. 
    // Before deletion make sure that the landmark is not being refered by other lookup entries.

    // find the entry in the lookup table.
    if ( iLocationAppLookupDb->findEntryBySourceIdAndType( lookupItem ) )
    {
        //fill address into lookup item.
        FillLookupItemAddressDetails( aLandmark, lookupItem );
        
        QString locationName = lookupItem.mName;
    
        TPosLmItemId catId;
        
        if( aSourceType == ESourceCalendar )
        {
            catId = iLmCalendarCatId;
        }
        else
        {
            // remove landmark name, which is basically contact's name.
            aLandmark->SetLandmarkNameL( KNullDesC );
            
            // category id to contacts
            catId = iLmContactsCatId;
        }

        
        // check if the location info is modified by comparing the new landmark with the existing landmark
        CPosLandmark* existingLandmark = NULL;
        TRAPD( error, ( existingLandmark = 
               CheckAndReadLandmarkL( iLandmarkDb, lookupItem.mDestId ) ) );
        CleanupStack::PushL( existingLandmark );
        if ( error == KErrNotFound )
        {
            // Landmarks item deleted. So delete corresponding lookup entries.
            QList<QLookupItem> itemArray;
            iLocationAppLookupDb->findEntriesByLandmarkId( lookupItem.mDestId, itemArray );
            for ( int i = 0; i < itemArray.count(); i++)
            {
                iLocationAppLookupDb->deleteEntryBySourceIdAndType( itemArray[i] );
            }

            // Add the entry into the lookup table and update landmarks db.
            HandleEntryAdditionL( aLandmark, aUid, aSourceType );
            
            CleanupStack::PopAndDestroy( existingLandmark );
            return;
        }

        if ( !CompareLandmarks( existingLandmark, aLandmark ) )
        {
            // landmarks are not same, means location information is modified.

            // Check if the new landmark is already in db.
            TPosLmItemId dupLmId = CheckIfDuplicateExistsL( aLandmark );
            if ( dupLmId )
            {
                // landmark already present in db. get the details
                CPosLandmark* dupLandmark = iLandmarkDb->ReadLandmarkLC( dupLmId );
                if ( dupLandmark )
                {
                    // add category.
                    dupLandmark->AddCategoryL( catId );

                    // update the landmark object in the db
                    iLandmarkDb->UpdateLandmarkL( *dupLandmark );
                }
                CleanupStack::PopAndDestroy( dupLandmark );

                // update the lookup item to refer to the newly created landmark.
                lookupItem.mDestId = dupLmId;
                if( aSourceType == ESourceCalendar )
                {
                    // for contacts duplicate doesnot hold good as the location name is the contact name.
                    if( !lookupItem.mIsDuplicate )
                    {
                        // if current lookup item duplicate property is 0, then remove next corresponding
                        // calendar lookup entry duplicate property.
                        // this is required because the current entry will be pointing to a new landmark.
                        UnsetDuplicateNextCalEntry( aUid, existingLandmark->LandmarkId() );
                    } 

                    // set duplicate only if there are calendar entries already pointing to this landmark. 
                    if( IsDuplicateEntry( dupLmId ) )
                    {
                        lookupItem.mIsDuplicate = 1;
                    }

                }
                
                iLocationAppLookupDb->updateEntryBySourceIdAndType( lookupItem );
            }
            else
            {
                // landmark not already present in db.
                // Create a new entry in the db
                aLandmark->AddCategoryL( catId );
                lookupItem.mDestId = iLandmarkDb->AddLandmarkL( *aLandmark );
                if( aSourceType == ESourceCalendar )
                {
                    // for contacts duplicate doesnot hold good as the location name is the contact name.
                    if( !lookupItem.mIsDuplicate )
                    {
                        // if current lookup item duplicate property is 0, then remove next corresponding
                        // calendar lookup entry duplicate property.
                        // this is required because the current entry will be pointing to a new landmark.
                        UnsetDuplicateNextCalEntry( aUid, existingLandmark->LandmarkId() );
                    } 
                }
                
                lookupItem.mIsDuplicate = 0;
                // update the lookup table
                iLocationAppLookupDb->updateEntryBySourceIdAndType( lookupItem );
            }
        }
        else
        {
            // landmarks are same, means location not modified. So return.
            if( aSourceType == ESourceContactsPref
                                    || aSourceType == ESourceContactsWork
                                    || aSourceType == ESourceContactsHome
                                    )
            {
                // in case of contacts, there is a chance that contact name is modified. 
                // so update the lookup database entry with that name.
                lookupItem.mName = locationName;
                iLocationAppLookupDb->updateEntryBySourceIdAndType( lookupItem );
            }

            CleanupStack::PopAndDestroy( existingLandmark );
            return;
        }

        // delete the existing landmark only if it not being refered by other lookup entries.

        // Check if any other entries are refering this landmark.
        QList<QLookupItem> itemArray;
        iLocationAppLookupDb->findEntriesByLandmarkId(
                existingLandmark->LandmarkId(), itemArray );

        if ( itemArray.count() )
        {
            // There are other lookup entries refering this landmark. So do not delete the landmark

            // If none of these lookup item's source type is current source type, disassociate 'catId' category
            // from this landmark.
            TInt i = 0;
            while ( i < itemArray.count() )
            {
                if( aSourceType == ESourceCalendar )
                {
                    if ( itemArray[i].mSourceType == aSourceType )
                    {
                        // a lookup item exists which is from calendar, so 'catId' is still valid.
                        break;
                    }
                 }
                else 
                {
                    // a lookup item exists which is from contacts, so 'catId' is still valid.
                    break;
                }
                i++;
            }
            if ( i == itemArray.count() )
            {
                // no lookup items from current source type exists refering this landmark.
                // so disassociate 'catId' from this landmark

                existingLandmark->RemoveCategory( catId );
                iLandmarkDb->UpdateLandmarkL( *existingLandmark );
            }
        }
        else
        {
            // no other lookup entry is refering this landmark. 
            // delete the landmark.
            iLandmarkDb->RemoveLandmarkL( existingLandmark->LandmarkId() );
        }
        CleanupStack::PopAndDestroy( existingLandmark );

    }
    else // entry not present in lookup table
    {
        // Add the entry into the lookup table and update landmarks db.
        HandleEntryAdditionL( aLandmark, aUid, aSourceType );
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::HandleEntryDeletionL()
// Handles the entry deletion in lookup table and landmarks db.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::HandleEntryDeletionL(const TUint32 aUid,
                                                const TUint32 aSourceType)
{
    __TRACE_CALLSTACK;
    QLookupItem lookupItem;
    lookupItem.mSourceUid = aUid;
    lookupItem.mSourceType = aSourceType;

    // Behavior: if an entry is deleted, delete the corresponding entries from 
    // both lookup table and iLandmarkDb.  
    // Before deleting the entry from iLandmarkDb, make sure that this entry is not being refered by
    // other entries of the lookup table. If it is being refered by other entries in lookup table, then
    // do not delete the landmark.
 
    if ( !iLocationAppLookupDb->findEntryBySourceIdAndType( lookupItem ) )
    {
        if( aSourceType == ESourceLandmarks )
        {
            lookupItem.mDestId = aUid;
        }
        else
        {
            return;
        }
    }
    
    // Find the corresponding landmark uid
    

    // delete the lookup entry.
    iLocationAppLookupDb->deleteEntryBySourceIdAndType( lookupItem );

    // Check if any other entries are refering this landmark.
    QList<QLookupItem> itemArray;
    iLocationAppLookupDb->findEntriesByLandmarkId( lookupItem.mDestId, itemArray );

    if ( itemArray.count() )
    {
    
        if( aSourceType == ESourceLandmarks )
        {
            CPosLandmark* lm = NULL;
        
            for( int i = 0; i < itemArray.count(); i++ )
            {
                if( itemArray[i].mSourceType == ESourceCalendar )
                {
                    // add landmark entry since a calendar item is present with this location.
                   if( !lm )
                   {
                       lm = CreateLandmarkItemLC( itemArray[i] );
                   }
                   lm->AddCategoryL( iLmCalendarCatId );
                }
                else
                {
                   // add landmark entry since a contact item is present with this location.
                   if( !lm )
                   {
                       QString tempStr = itemArray[i].mName;
                       itemArray[i].mName = "";
                       lm = CreateLandmarkItemLC( itemArray[i] );
                       itemArray[i].mName = tempStr;
                   }
                   lm->AddCategoryL( iLmCalendarCatId );
                }    
            }

            lookupItem.mDestId = iLandmarkDb->AddLandmarkL( *lm );
            CleanupStack::PopAndDestroy( lm );

            bool dupUnset = false;
            for( int i=0; i<itemArray.count(); i++ )
            {
                itemArray[i].mDestId = lookupItem.mDestId;
                if( itemArray[i].mSourceType == ESourceCalendar && dupUnset == false )
                {
                    dupUnset = true;
                    itemArray[i].mIsDuplicate = 0;
                }
                iLocationAppLookupDb->updateEntryById( itemArray[i] );
            }   
            
            return;
        }

        // There are other lookup entries refering this landmark. So do not delete the landmark

        // If none of these lookup item's source type is current source type, disassociate current source category
        // from this landmark.
        TInt i = 0;
        while ( i < itemArray.count() )
        {
            if( aSourceType == ESourceCalendar )
            {
                if( itemArray[i].mSourceType == aSourceType )
                {
                    if( lookupItem.mIsDuplicate == 0 )
                    {
                        itemArray[i].mIsDuplicate = 0;
                        iLocationAppLookupDb->updateEntryById( itemArray[i] );
                    }
                    // a lookup item exists which is from calendar, so 'iLmCalendarCatId' is still valid.
                    break;
                }
        
            }
            else if ( itemArray[i].mSourceType == ESourceContactsPref
                    || itemArray[i].mSourceType == ESourceContactsWork
                    || itemArray[i].mSourceType == ESourceContactsHome)
            {
                // a lookup item exists which is from contacts, so 'iLmContactsCatId' is still valid.
                break;
            }
            i++;
        }
        if ( i == itemArray.count() )
        {
            // no lookup items from current source type exists refering this landmark.
            // so disassociate current source category from this landmark

            CPosLandmark* landmark = iLandmarkDb->ReadLandmarkLC( lookupItem.mDestId );
            if( aSourceType == ESourceCalendar )
            {
                landmark->RemoveCategory( iLmCalendarCatId );
            }
            else
            {
                landmark->RemoveCategory( iLmContactsCatId );
            }
            
            iLandmarkDb->UpdateLandmarkL( *landmark );
            CleanupStack::PopAndDestroy( landmark );
        }
    }
    else
    {
        // no other lookup entry is refering this landmark. 
        // delete the landmark.
        if ( aSourceType != ESourceLandmarks )
        {
            iLandmarkDb->RemoveLandmarkL( lookupItem.mDestId );
        }
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::HandleLandmarkModificationL()
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::HandleLandmarkModificationL(
        CPosLandmark* aLandmark, const TUint32 aUid )
{
    __TRACE_CALLSTACK;
    // logic: if a landmark is modified, 
    // first update the corresponding landmark lookup entry if present, else create a new entry.
    // Check for any contact/calendar entries refering this landmark entry,
    // if exists, create a new landmark entry with that location details and update all those 
    // lookup entry's destid with the newly created landmark id.
    
    QLookupItem lookupItem;
    lookupItem.mSourceUid = aUid;
    lookupItem.mSourceType = ESourceLandmarks;
    lookupItem.mIconType = QLookupItem::EIconTypeDefault;

    bool found = iLocationAppLookupDb->findEntryBySourceIdAndType( lookupItem );
    //fill address into lookup item.
    FillLookupItemAddressDetails( aLandmark, lookupItem );
    lookupItem.mDestId = aUid;
    lookupItem.mIsDuplicate = 0;
    lookupItem.mIconType = QLookupItem::EIconTypeDefault;
    lookupItem.mIconPath = "";
    lookupItem.mMapTilePath = "";

    // update entry in lookup table.
    if ( found )
    {
        iLocationAppLookupDb->updateEntryById( lookupItem );
    }
    else
    {
        iLocationAppLookupDb->createEntry( lookupItem );
    }
    
    QList<QLookupItem> itemArray;
    iLocationAppLookupDb->findEntriesByLandmarkId( lookupItem.mDestId, itemArray );
    
    if( itemArray.count() == 1 )
    {
        //only one entry ie the entry corresponding to landmark db is present.
        return;
    }
    
    CPosLandmark* lm = NULL;
    
    for( int i = 0; i < itemArray.count(); i++ )
    {
        if( itemArray[i].mSourceType != ESourceLandmarks )
        {
            if( itemArray[i].mSourceType == ESourceCalendar )
            {
                // add landmark entry since a calendar item is present with this location.
               if( !lm )
               {
                   lm = CreateLandmarkItemLC( itemArray[i] );
               }
               lm->AddCategoryL( iLmCalendarCatId );
            }
            else
            {
               // add landmark entry since a calendar item is present with this location.
               if( !lm )
               {
                   QString tempStr = itemArray[i].mName;
                   itemArray[i].mName = "";
                   lm = CreateLandmarkItemLC( itemArray[i] );
                   itemArray[i].mName = tempStr;
               }
               lm->AddCategoryL( iLmCalendarCatId );
            }    
        }
    }
    
    // add the entry to landmarks db
    quint32 newDestId = iLandmarkDb->AddLandmarkL( *lm );
    CleanupStack::PopAndDestroy( lm );

    bool calDuplicateUnset = false;
    // update all the lookup entries with new landmark id
    for( int i = 0; i < itemArray.count(); i++ )
    {
        if( itemArray[i].mSourceType != ESourceLandmarks )
        {
            itemArray[i].mDestId = newDestId;
            
            if( itemArray[i].mSourceType == ESourceCalendar )
            {
                if( !calDuplicateUnset )
                {
                    itemArray[i].mIsDuplicate = 0;
                    calDuplicateUnset = true;
                }
                else
                {
                    itemArray[i].mIsDuplicate = 1;
                }
            }
            iLocationAppLookupDb->updateEntryById( itemArray[i] );
        }
    }
}
// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::GetLandmarkFullAddress()
// Gets the comma separated full address of the given landmark.
// -----------------------------------------------------------------------------
//
EXPORT_C void CMyLocationsDatabaseManager::GetLandmarkFullAddress(
        TBuf<KMaxAddressLength>& aLandmarkAddress,
        const CPosLandmark* aLandmark)
{
    __TRACE_CALLSTACK;
    TPtrC tempStr;
    TInt retStatus;
    TBool addressEmtpy = ETrue;
    aLandmarkAddress.Copy( KNullDesC );
    retStatus = aLandmark->GetPositionField(EPositionFieldStreet, tempStr);
    if (retStatus == KErrNone && tempStr.Length())
    {
        {
            aLandmarkAddress.Copy(tempStr);
            addressEmtpy = EFalse;
        }
    }

    retStatus = aLandmark->GetPositionField(EPositionFieldCity, tempStr);
    if (retStatus == KErrNone && tempStr.Length())
    {
        if (!addressEmtpy)
        {
            aLandmarkAddress.Append(KSeparator);
            aLandmarkAddress.Append(KSpace);
            aLandmarkAddress.Append(tempStr);
        }
        else
        {
            aLandmarkAddress.Copy(tempStr);
            addressEmtpy = EFalse;
        }
    }

    retStatus = aLandmark->GetPositionField(EPositionFieldState, tempStr);
    if (retStatus == KErrNone && tempStr.Length())
    {
        if (!addressEmtpy)
        {
            aLandmarkAddress.Append(KSeparator);
            aLandmarkAddress.Append(KSpace);
            aLandmarkAddress.Append(tempStr);
        }
        else
        {
            aLandmarkAddress.Copy(tempStr);
            addressEmtpy = EFalse;
        }
    }

    retStatus = aLandmark->GetPositionField(EPositionFieldCountry, tempStr);
    if (retStatus == KErrNone && tempStr.Length())
    {
        if (!addressEmtpy)
        {
            aLandmarkAddress.Append(KSeparator);
            aLandmarkAddress.Append(KSpace);
            aLandmarkAddress.Append(tempStr);
        }
        else
        {
            aLandmarkAddress.Copy(tempStr);
            addressEmtpy = EFalse;
        }
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CheckAndReadLandmarkL()
// Checks if given landmark id is found in the database and returns the read landmark.
// -----------------------------------------------------------------------------
//
CPosLandmark* CMyLocationsDatabaseManager::CheckAndReadLandmarkL(
        CPosLandmarkDatabase* aDb, const TUint32 aLmId)
{
    __TRACE_CALLSTACK;
    CPosLandmark* lm = aDb->ReadLandmarkLC(aLmId);
    CleanupStack::Pop(lm);    
    return lm;
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::FillLookupItemAddressDetails()
// Creates a new category in Mylocations Db and adds a corresponding entry in 
// mylocations lookup table.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::FillLookupItemAddressDetails( CPosLandmark* aLandmark, QLookupItem& aLookupItem )
{
    __TRACE_CALLSTACK;// Read the category.

    // fill geo-coordinates
    TLocality position;
    aLandmark->GetPosition( position );
    aLookupItem.mLatitude = position.Latitude();
    aLookupItem.mLongitude = position.Longitude();

    TPtrC tempStr;
    TInt retStatus;

    // Copy landmark name in address 1
    retStatus = aLandmark->GetLandmarkName( tempStr );
    aLookupItem.mName = "";
    if( retStatus == KErrNone && tempStr.Length() > 0 )
    {
        aLookupItem.mName = QString( (QChar*)tempStr.Ptr(), tempStr.Length() );
    }

    // get street
    aLookupItem.mStreet = "";
    retStatus = aLandmark->GetPositionField( EPositionFieldStreet, tempStr );
    if( retStatus == KErrNone && tempStr.Length() )
    {
        aLookupItem.mStreet = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
    }

    // get postal code
    aLookupItem.mPostalCode = "";
    retStatus = aLandmark->GetPositionField( EPositionFieldPostalCode, tempStr );
    if( retStatus == KErrNone && tempStr.Length() )
    {
        aLookupItem.mPostalCode = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
    }

    // get city
    aLookupItem.mCity = "";
    retStatus = aLandmark->GetPositionField( EPositionFieldCity, tempStr );
    if( retStatus == KErrNone && tempStr.Length() )
    {
        aLookupItem.mCity = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
    }

    // get State
    aLookupItem.mState = "";
    retStatus = aLandmark->GetPositionField( EPositionFieldState, tempStr );
    if( retStatus == KErrNone && tempStr.Length() )
    {
        aLookupItem.mState = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
    }

    // get country
    aLookupItem.mCountry = "";
    retStatus = aLandmark->GetPositionField( EPositionFieldCountry, tempStr );
    if( retStatus == KErrNone && tempStr.Length() )
    {
        aLookupItem.mCountry = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::UnsetDuplicateNextCalEntry()
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::UnsetDuplicateNextCalEntry( quint32 aUid, quint32 aLandmarkId )
{
    __TRACE_CALLSTACK;
    // get next duplicate item
    QList<QLookupItem> itemArray;
    iLocationAppLookupDb->findEntriesByLandmarkId( aLandmarkId, itemArray );
    for ( int i = 0; i < itemArray.count(); i++)
    {
        if( itemArray[i].mSourceType == ESourceCalendar && itemArray[i].mSourceUid != aUid )
        {
            itemArray[i].mIsDuplicate = 0;
            iLocationAppLookupDb->updateEntryById( itemArray[i] );
            break;
        }
    }

}
// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::IsDuplicateEntry()
// -----------------------------------------------------------------------------
//
bool CMyLocationsDatabaseManager::IsDuplicateEntry( quint32 aLandmarkId )
{
    __TRACE_CALLSTACK;
    // get next duplicate item
    QList<QLookupItem> itemArray;
    iLocationAppLookupDb->findEntriesByLandmarkId( aLandmarkId, itemArray );
    for ( int i = 0; i < itemArray.count(); i++)
    {
        if( itemArray[i].mSourceType == ESourceCalendar ||
            itemArray[i].mSourceType == ESourceLandmarks )
        {
            return true;
        }
    }
    
    return false;
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CreateLandmarkItemLC()
// -----------------------------------------------------------------------------
//
CPosLandmark* CMyLocationsDatabaseManager::CreateLandmarkItemLC( const QLookupItem &aLookupItem )
{
    __TRACE_CALLSTACK;//return value
    CPosLandmark *landmark = NULL;
    TLocality loc( TCoordinate( aLookupItem.mLatitude, aLookupItem.mLongitude ), 0 );

    landmark = CPosLandmark::NewL();
    CleanupStack::PushL( landmark );

    // Fill the location details into the landmark object
    landmark->SetPositionL( loc );

    // Set the landmark name as contact name
    TBuf<KBufSize> text( aLookupItem.mName.utf16() );
    TRAP_IGNORE( landmark->SetLandmarkNameL( text ) );

    text.Copy( aLookupItem.mStreet.utf16() );
    landmark->SetPositionFieldL( EPositionFieldStreet, text );
    
    // Set the City
    text.Copy( aLookupItem.mCity.utf16() );
    landmark->SetPositionFieldL( EPositionFieldCity, text );

    // Set the state/region
    text.Copy( aLookupItem.mState.utf16() );
    landmark->SetPositionFieldL( EPositionFieldState, text );

    // Set the Postal code
    text.Copy( aLookupItem.mPostalCode.utf16() );
    landmark->SetPositionFieldL( EPositionFieldPostalCode, text );

    // Set the country
    text.Copy( aLookupItem.mCountry.utf16() );
    landmark->SetPositionFieldL( EPositionFieldCountry, text );

    return landmark;
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::UpdateMapTilePath()
// -----------------------------------------------------------------------------
//
EXPORT_C void CMyLocationsDatabaseManager::UpdateMapTilePath( TUint32 aSourceId, TUint32 aSourceType, 
                                            TFileName& aFilePath )
{
    __TRACE_CALLSTACK;
    QString filePath = QString( (QChar*)aFilePath.Ptr(), aFilePath.Length() );
    iLocationAppLookupDb->updateMaptileBySourceIdAndType( aSourceId, aSourceType, filePath );
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::UpdateEntryName()
// -----------------------------------------------------------------------------
//
EXPORT_C void CMyLocationsDatabaseManager::UpdateEntryName( TUint32 aSourceId, TUidSourceType aSourceType, 
                                            const TDesC& aName )
{
    __TRACE_CALLSTACK;
    QString name = QString( (QChar*)aName.Ptr(), aName.Length() );
    iLocationAppLookupDb->updateEntryNameByIdAndType( aSourceId, aSourceType, name );
	
}


// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CheckIfAddressChanged()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMyLocationsDatabaseManager::CheckIfAddressChanged(const CPosLandmark& aLandmarks,
        const TUint32 aId, const TUidSourceType aAddressType)
{
    __TRACE_CALLSTACK;
    QString target = iLocationAppLookupDb->getAddressDetails(aId, aAddressType);
    TBuf<KMaxAddressLength> lmAddress;
    GetLandmarkFullAddress(lmAddress, &aLandmarks);
    QString source = QString((QChar*) lmAddress.Ptr(), lmAddress.Length());
    if (source == target)
    {
        return EFalse;
    }
    return ETrue;

}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CheckIfAddressChanged()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMyLocationsDatabaseManager::CheckIfAddressChanged(const TDesC& aAddress,
        const TUint32 aId, const TUidSourceType aAddressType)
{   
    __TRACE_CALLSTACK;
    TBool compareStatus = ETrue;
    QString target=iLocationAppLookupDb->getAddressDetails( aId , aAddressType );
    QString source = QString( (QChar*)aAddress.Ptr(), aAddress.Length());
    if( source == target )
    {
        compareStatus= EFalse;
    }
    else
    {
        iLocationAppLookupDb->updateCalendarLocationById(aId,source);   
    }
    return compareStatus;
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::UpdateCalendarLocationById()
// -----------------------------------------------------------------------------
//
EXPORT_C void CMyLocationsDatabaseManager::UpdateCalendarLocationById(const TUint32 aUid, const TUint32 aChangeType,const TDesC& aLocation)
{
    
    
    switch (aChangeType)
       {
           case EEntryAdded:
           case EEntryModified:
               
           {
               QString location = QString( (QChar*)aLocation.Ptr(), aLocation.Length());
               iLocationAppLookupDb->updateCalendarLocationById(aUid,location);
               break;
           }          
           case EEntryDeleted:
           {
               iLocationAppLookupDb->deleteCalendarEntry(aUid);
               break;
           }
       }       
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::GetAllCalendarEntry()
// -----------------------------------------------------------------------------
//
EXPORT_C void CMyLocationsDatabaseManager::GetAllCalendarEntry(QList<QCalendarLocationDetails>& aCalEntryArray)
{
    iLocationAppLookupDb->getAllCalendarEntry(aCalEntryArray);
}

// End of file

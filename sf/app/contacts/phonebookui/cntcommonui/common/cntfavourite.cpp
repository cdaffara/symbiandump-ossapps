/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/
#include "cntfavourite.h"
#include <cntglobal.h>
#include <QDebug>

int CntFavourite::mCachedSymbianFavouriteId = 0;

QContactLocalId CntFavourite::createFavouriteGroup(QContactManager* aManager)
{
   QContactLocalId favouriteId = CntFavourite::favouriteGroupId(aManager);
    
   if (favouriteId == 0) {
       QContact favouriteGroup;
       favouriteGroup.setType( QContactType::TypeGroup );

       QContactName favouriteGroupName;
       favouriteGroupName.setCustomLabel( QLatin1String(FavouriteGroupName) );

       favouriteGroup.saveDetail(&favouriteGroupName);
       if (aManager->saveContact(&favouriteGroup)) {
           favouriteId = favouriteGroup.localId();
           if (aManager->managerUri() == SYMBIAN_BACKEND) {
               mCachedSymbianFavouriteId = favouriteId;
           }
       }
   }

   return favouriteId;
}

QContactLocalId CntFavourite::favouriteGroupId(QContactManager* aManager)
{
    QContactLocalId favouriteId(0);
   
    if (mCachedSymbianFavouriteId != 0 && aManager->managerUri() == SYMBIAN_BACKEND) {
        favouriteId = mCachedSymbianFavouriteId;
    } else {
        QContactDetailFilter groupFilter;
        groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
        groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));
    
        QList<QContactLocalId> groupContactIds = aManager->contactIds( groupFilter );
            
        for (int i = 0; i < groupContactIds.count(); i++) {
            QContactFetchHint noRelationshipsFetchHint;
            noRelationshipsFetchHint.setOptimizationHints(QContactFetchHint::NoRelationships);
            QContact contact = aManager->contact(groupContactIds.at(i), noRelationshipsFetchHint);
            QContactName contactName = contact.detail<QContactName>();
            QString groupName = contactName.customLabel();
            if (groupName.compare(QLatin1String(FavouriteGroupName)) == 0) {
                favouriteId = groupContactIds.at(i);
                if (aManager->managerUri() == SYMBIAN_BACKEND) {
                    mCachedSymbianFavouriteId = favouriteId;
                }
                break;
            }
        }
    }
    
    return favouriteId;
}

bool CntFavourite::isMemberOfFavouriteGroup(QContactManager* contactManager, QContact* contact)
{
    QContactId favouriteId;
    favouriteId.setManagerUri(contactManager->managerUri());
    favouriteId.setLocalId(CntFavourite::favouriteGroupId(contactManager));

    QContactRelationship favouriteMembership;
    favouriteMembership.setFirst(favouriteId);
    favouriteMembership.setSecond(contact->id());
    favouriteMembership.setRelationshipType(QContactRelationship::HasMember);

    return contact->relationships().contains(favouriteMembership);
}

void CntFavourite::addContactToFavouriteGroup( QContactManager* aManager, QContactId& aId)
{
    QContactLocalId favouriteId = CntFavourite::favouriteGroupId( aManager );
    if ( favouriteId == 0 )
    {
        favouriteId = CntFavourite::createFavouriteGroup( aManager );
    }
    
    QContact favoriteGroup = aManager->contact( favouriteId );
        
    // new contact added to the favorite group
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst( favoriteGroup.id() );
    relationship.setSecond( aId );
    
    // save relationship
    aManager->saveRelationship( &relationship );
}

void CntFavourite::removeContactFromFavouriteGroup(QContactManager* aManager, QContactId& aId)
{
    QContactLocalId favouriteId = CntFavourite::favouriteGroupId(aManager);
    if (favouriteId != 0) {
        QContact favoriteGroup = aManager->contact(favouriteId);
        QContactRelationship relationship;
        relationship.setRelationshipType(QContactRelationship::HasMember);
        relationship.setFirst(favoriteGroup.id());
        relationship.setSecond(aId);
        aManager->removeRelationship(relationship);
    }
}

// End of File

/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_PRESENCE_CACHE_BUDDYINFO_PRIVATE_QT_H
#define C_PRESENCE_CACHE_BUDDYINFO_PRIVATE_QT_H

// INCLUDES
#include <QString>
#include <QList>
#include <prcpresencebuddyinfo_qt.h>
class CPresenceCacheBuddyInfo;
class MPresenceBuddyInfo2;


// CLASS DECLARATION

class PresenceCacheBuddyInfoQtPrivate
    {
        
public:

    PresenceCacheBuddyInfoQtPrivate(MPresenceBuddyInfo2* symbianBuddyInfo);
    ~PresenceCacheBuddyInfoQtPrivate() ;
public:
       MPresenceBuddyInfo2* getSymbianBuddyInstance();
public:    
        
        void setIdentity( const QString& buddyId ) ;        
        QString BuddyId( ) const ;        
        void setAvailability(PrcPresenceBuddyInfoQt::AvailabilityValues availability, const QString& availabilityText ) ;
        PrcPresenceBuddyInfoQt::AvailabilityValues Availability( ) ;         
        QString availabilityText( ) ;                   
        void setAvatar( const QString& avatar ) ;
        QString avatar( ) ;                     
        void setStatusMessage( 
        const QString& statusMessage ) ; 
        QString statusMessage( ) ;     
        void setAnyField(const QString& key,const QString& value ) ;
        QString getAnyField( const QString& aKey ) ; 
        void getFieldKeys(QList<QString>& qtKeyList ) ;
        void removeField(const QString& aKey ) ;      
        bool EqualsIdentity( const PrcPresenceBuddyInfoQt& aOtherInstance ) const ;
        
        

private :
        /*
         * MPresenceBuddyInfo2 Symbian instance 
         * own
         */
        MPresenceBuddyInfo2* iSymbianInstance;
    };

#endif // C_PRESENCE_CACHE_BUDDYINFO_PRIVATE_QT_H

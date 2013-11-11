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


#ifndef C_PRESENCE_CACHE_BUDDYINFO_QT_H
#define C_PRESENCE_CACHE_BUDDYINFO_QT_H

// INCLUDES
#include <QString>
#include <QList>
#include <prcpresencebuddyinfo_qt.h>
class CPresenceCacheBuddyInfo;
class MPresenceBuddyInfo2;
class PresenceCacheBuddyInfoQtPrivate;

/* CLASS DECLARATION
 * This class implements the interface PrcPresenceBuddyInfoQt
 */ 

class PresenceCacheBuddyInfoQt : public PrcPresenceBuddyInfoQt
    {
        
public:

    PresenceCacheBuddyInfoQt(MPresenceBuddyInfo2* symbianBuddyInfo);


    IMPORT_C static PrcPresenceBuddyInfoQt* createInstance(MPresenceBuddyInfo2& symbianBuddyInfo );    
    ~PresenceCacheBuddyInfoQt();

public:
        //From PrcPresenceBuddyInfoQt
        void setIdentity( const QString& buddyId ) ;        
        QString buddyId( ) const ;        
        void setAvailability(AvailabilityValues availability, const QString& availabilityText ) ;
        PrcPresenceBuddyInfoQt::AvailabilityValues availability( ) const ;         
        QString availabilityText( ) const;                   
        void setAvatar( const QString& avatar ) ;
        QString avatar( ) const;                     
        void setStatusMessage( 
        const QString& statusMessage ) ; 
        QString statusMessage( ) const ;     
        void setAnyField(const QString& key,const QString& value ) ;
        QString getAnyField( const QString& aKey )  const; 
        void getFieldKeys(QList<QString>& qtKeyList ) const;
        void removeField(const QString& aKey ) ;      
        bool equalsIdentity( const PrcPresenceBuddyInfoQt& aOtherInstance ) const ;
       
        MPresenceBuddyInfo2* getSymbianBuddyInstance();
private:
        /*
         * Private implementation
         * own
         */
        PresenceCacheBuddyInfoQtPrivate* d;
    };

#endif // C_PRESENCE_CACHE_BUDDYINFO_QT_H

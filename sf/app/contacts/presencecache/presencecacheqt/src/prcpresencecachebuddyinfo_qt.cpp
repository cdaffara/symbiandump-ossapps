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
//INCLUDES
#include <prcpresencebuddyinfo_qt.h>

#include "prcpresencecachebuddyinfo_qt.h"
#include "prcpresencecachebuddyinfoprivate_qt.h"
#include "mpresencebuddyinfo2.h"



        

// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::PresenceCacheBuddyInfoQt()
// -----------------------------------------------------------------------------
PresenceCacheBuddyInfoQt::PresenceCacheBuddyInfoQt(MPresenceBuddyInfo2* symbianBuddyInfo): 
                    d (new PresenceCacheBuddyInfoQtPrivate(symbianBuddyInfo))
                      
    {
    
    
    }
PresenceCacheBuddyInfoQt::~PresenceCacheBuddyInfoQt()
{
    delete d;
    d = NULL;
}
// ---------------------------------------------------------------------------
// PrcPresenceBuddyInfoQt::createInstance()
// ---------------------------------------------------------------------------
//
EXPORT_C PrcPresenceBuddyInfoQt* PrcPresenceBuddyInfoQt::createInstance( )
    {
    return new PresenceCacheBuddyInfoQt(NULL);
    }
    


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::setIdentity()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQt::setIdentity( const QString& buddyId )
{
    d->setIdentity(buddyId);
}


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::BuddyId()
// ---------------------------------------------------------------------------
QString PresenceCacheBuddyInfoQt::buddyId( ) const
{
    return d->BuddyId();
} 


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::setAvailability()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQt::setAvailability(PrcPresenceBuddyInfoQt::AvailabilityValues availability,const QString& availabilityText )
{
    d->setAvailability(availability,availabilityText);
}


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::Availability()
// ---------------------------------------------------------------------------
PrcPresenceBuddyInfoQt::AvailabilityValues PresenceCacheBuddyInfoQt::availability( ) const 
    {
    
    return d->Availability();
    }         

// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::availabilityText()
// ---------------------------------------------------------------------------

QString PresenceCacheBuddyInfoQt::availabilityText( ) const
{
    return d->availabilityText();
}      
 

 // ---------------------------------------------------------------------------
 // PresenceCacheBuddyInfoQt::setAvatar()
 // ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQt::setAvatar( const QString& avatar ) 

{
    d->setAvatar(avatar);
}


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::avatar()
// ---------------------------------------------------------------------------
QString PresenceCacheBuddyInfoQt::avatar( ) const
{
    return d->avatar();   
}    


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::setStatusMessage()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQt::setStatusMessage(const QString& statusMessage ) 
{
    d->setStatusMessage(statusMessage);
} 

// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::statusMessage()
// ---------------------------------------------------------------------------
QString PresenceCacheBuddyInfoQt::statusMessage( ) const
{
    return d->statusMessage();
}  


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::setAnyField()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQt::setAnyField(const QString& key,const QString& value ) 
{   
   d->setAnyField(key,value);
}


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::getAnyField()
// ---------------------------------------------------------------------------
QString PresenceCacheBuddyInfoQt::getAnyField(const QString& aKey ) const
{
    return d->getAnyField(aKey);
} 


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::getFieldKeys()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQt::getFieldKeys(QList<QString>& keyList ) const
{
   d->getFieldKeys(keyList);
}

// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::getFieldKeys()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQt::removeField(const QString& key ) 
{
    d->removeField(key);
}      

// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::EqualsIdentity()
// ---------------------------------------------------------------------------
bool PresenceCacheBuddyInfoQt::equalsIdentity( const PrcPresenceBuddyInfoQt& otherInstance ) const 
{
     return d->EqualsIdentity(otherInstance);
}
// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQt::getSymbianBuddyInstance()
// ---------------------------------------------------------------------------
MPresenceBuddyInfo2* PresenceCacheBuddyInfoQt::getSymbianBuddyInstance()
{
    return d->getSymbianBuddyInstance();
}


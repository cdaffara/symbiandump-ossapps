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
* Description:  Implementation for presence cache QT client.
*
*/
//Includes
#include <e32std.h>
#include <s32buf.h>
#include <QString>

#include "prcpresencereader_qt.h"
#include "prcpresencereaderprivate_qt.h"

// ---------------------------------------------------------------------------
// PresenceReader::createReader()
// ---------------------------------------------------------------------------
PrcPresenceReader* PrcPresenceReader::createReader()
{
    return  new PrcPresenceReader();
} 


// ---------------------------------------------------------------------------
// PrcPresenceReader::~PrcPresenceReader()
// ---------------------------------------------------------------------------
PrcPresenceReader::~PrcPresenceReader()
{
   
  delete d;

}
    
// ---------------------------------------------------------------------------
// PrcPresenceReader::PrcPresenceReader()
// ---------------------------------------------------------------------------
//
PrcPresenceReader::PrcPresenceReader():
        d( new PrcPresenceReaderPrivate())
{
   QObject::connect( d, SIGNAL( signalPresenceRead(bool , QList<PrcPresenceBuddyInfoQt*> ) ),
                   this, SIGNAL( signalPresenceRead(bool , QList<PrcPresenceBuddyInfoQt*> )) );
    QObject::connect( d, SIGNAL( signalPresenceNotification(bool , PrcPresenceBuddyInfoQt* ) ),
                   this, SIGNAL( signalPresenceNotification(bool , PrcPresenceBuddyInfoQt* )));

  
}

// ---------------------------------------------------------------------------
// PrcPresenceReader::presenceInfo()
// ---------------------------------------------------------------------------

PrcPresenceBuddyInfoQt* PrcPresenceReader::presenceInfo(const QString& identity)
{
    
    return d->presenceInfo(identity);
      
}
               
// ---------------------------------------------------------------------------
// PrcPresenceReader::buddyCountInAllServices()
// ---------------------------------------------------------------------------
int PrcPresenceReader::buddyCountInAllServices()
{
    return  d->buddyCountInAllServices();
}
 
// ---------------------------------------------------------------------------
// PrcPresenceReader::buddyCountInService()
// ---------------------------------------------------------------------------
int  PrcPresenceReader::buddyCountInService(const QString& serviceName) 
{   
    return d->buddyCountInService(serviceName);
}
   
// ---------------------------------------------------------------------------
// PrcPresenceReader::servicesCount()
// ---------------------------------------------------------------------------
int PrcPresenceReader::servicesCount()
{
    return  d->servicesCount();
}
       
// ---------------------------------------------------------------------------
// PrcPresenceReader::cancelRead()
// ---------------------------------------------------------------------------
bool PrcPresenceReader::cancelRead() 
{
   return d->cancelRead();
}
// ---------------------------------------------------------------------------
// PrcPresenceReader::allBuddiesPresenceInService()
// ---------------------------------------------------------------------------                    
bool PrcPresenceReader::allBuddiesPresenceInService(const QString& serviceName) 
{
    return d->allBuddiesPresenceInService(serviceName);
}
   

// ---------------------------------------------------------------------------
// presenceCacheClientQt::subscribePresenceBuddyChange()
// ---------------------------------------------------------------------------
bool  PrcPresenceReader::subscribePresenceBuddyChange( const QString& identity)
{
   return d->subscribePresenceBuddyChange(identity);

}

// ---------------------------------------------------------------------------
// presenceCacheClientQt::unSubscribePresenceBuddyChange()
// ---------------------------------------------------------------------------
void PrcPresenceReader::unSubscribePresenceBuddyChange(const QString& identity)
{
    d->unSubscribePresenceBuddyChange(identity);
}




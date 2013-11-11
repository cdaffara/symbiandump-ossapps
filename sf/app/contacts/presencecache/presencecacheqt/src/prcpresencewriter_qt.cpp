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
// Includes
#include <QDebug>
#include <QString>

#include <prcpresencewriter_qt.h>
#include "prcpresencewriterprivate_qt.h"

// ---------------------------------------------------------------------------
// PrcPresenceWriter::createWriter()
// ---------------------------------------------------------------------------
PrcPresenceWriter* PrcPresenceWriter::createWriter()
{
    return new PrcPresenceWriter();
    
} 
// ---------------------------------------------------------------------------
// PrcPresenceWriter::~PrcPresenceWriter()
// ---------------------------------------------------------------------------
PrcPresenceWriter::~PrcPresenceWriter()
{
   
    delete d;
}
    
// ---------------------------------------------------------------------------
// PrcPresenceWriter::PrcPresenceWriter()
// ---------------------------------------------------------------------------
//
PrcPresenceWriter::PrcPresenceWriter():d(new PrcPresenceWriterPrivate() )
{
    QObject::connect( d, SIGNAL(  signalPresenceWrite(bool ) ),
                   this, SIGNAL(  signalPresenceWrite(bool)));
}


   
// ---------------------------------------------------------------------------
// PrcPresenceWriter::writePresence()
// ---------------------------------------------------------------------------
bool PrcPresenceWriter::writePresence(const PrcPresenceBuddyInfoQt& aPresenceBuddyInfo) 
{
    return d->writePresence(&aPresenceBuddyInfo);

}
     
// ---------------------------------------------------------------------------
// PrcPresenceWriter::newBuddyPresenceInfo()
// ---------------------------------------------------------------------------
PrcPresenceBuddyInfoQt* PrcPresenceWriter::newBuddyPresenceInfo() 
{
    
    return d->newBuddyPresenceInfo();

}

// ---------------------------------------------------------------------------
// PrcPresenceWriter::deleteService()
// ---------------------------------------------------------------------------
bool PrcPresenceWriter::deleteService(const QString& serviceName)
{
    return d->deleteService(serviceName);
}
// ---------------------------------------------------------------------------
// PrcPresenceWriter::deletePresence()
// ---------------------------------------------------------------------------        
bool PrcPresenceWriter::deletePresence(const QString& identity) 
{
    return d->deletePresence(identity);
}
        
// ---------------------------------------------------------------------------
// PrcPresenceWriter::cancelWrite()
// ---------------------------------------------------------------------------
bool PrcPresenceWriter::cancelWrite()
{
    return d->cancelWrite();
}

// ---------------------------------------------------------------------------
// PrcPresenceWriter::writeMultiplePresence()
// ---------------------------------------------------------------------------
bool  PrcPresenceWriter::writeMultiplePresence(
    const QList<PrcPresenceBuddyInfoQt*> & presenceBuddyInfos)
 {
    return d->writeMultiplePresence(presenceBuddyInfos);
 }
 


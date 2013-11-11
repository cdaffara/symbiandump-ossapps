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

#include <e32std.h>
#include <s32buf.h>
 #include <QString>

#include "prcpresencewriterprivate_qt.h"
#include <prcpresencebuddyinfo_qt.h>
#include "prcpresencecachebuddyinfo_qt.h"



// ---------------------------------------------------------------------------
// PrcPresenceWriterPrivate::~PrcPresenceWriterPrivate()
// ---------------------------------------------------------------------------
PrcPresenceWriterPrivate::~PrcPresenceWriterPrivate()
{
    delete iSymbianWriter;  
     
}
    
// ---------------------------------------------------------------------------
// PrcPresenceWriterPrivate::PrcPresenceWriterPrivate()
// ---------------------------------------------------------------------------
//
PrcPresenceWriterPrivate::PrcPresenceWriterPrivate()
{
    
    iSymbianWriter= NULL;
    TRAP_IGNORE(
            iSymbianWriter = MPresenceCacheWriter2::CreateWriterL();
               );
}


   
// ---------------------------------------------------------------------------
// PrcPresenceWriterPrivate::writePresence()
// ---------------------------------------------------------------------------
bool PrcPresenceWriterPrivate::writePresence(const PrcPresenceBuddyInfoQt* aPresenceBuddyInfo) 
{
    PresenceCacheBuddyInfoQt* bInfo = static_cast<PresenceCacheBuddyInfoQt*>(const_cast<PrcPresenceBuddyInfoQt*>(aPresenceBuddyInfo));
    MPresenceBuddyInfo2* t = bInfo->getSymbianBuddyInstance();
    TRAPD(err,iSymbianWriter->WritePresenceL(t));
    return ( KErrNone == err );
 
    
}
     
// ---------------------------------------------------------------------------
// PrcPresenceWriterPrivate::newBuddyPresenceInfo()
// ---------------------------------------------------------------------------
PrcPresenceBuddyInfoQt* PrcPresenceWriterPrivate::newBuddyPresenceInfo() 
{
    
    MPresenceBuddyInfo2* newbuddy = NULL; 
    TRAP_IGNORE(
            newbuddy = iSymbianWriter->NewBuddyPresenceInfoLC();
            CleanupStack::Pop();
               ); 
    return new PresenceCacheBuddyInfoQt(newbuddy);
      
 

}

// ---------------------------------------------------------------------------
// PrcPresenceWriterPrivate::deleteService()
// ---------------------------------------------------------------------------
bool PrcPresenceWriterPrivate::deleteService(const QString& serviceName)
{

    TPtrC sn(static_cast<const TUint16*>(serviceName.utf16()),serviceName.length());
    TInt ret = iSymbianWriter->DeleteService(sn);    
    return (KErrNone == ret);
}
// ---------------------------------------------------------------------------
// PrcPresenceWriterPrivate::deletePresence()
// ---------------------------------------------------------------------------        
bool PrcPresenceWriterPrivate::deletePresence(const QString& identity) 
{
    TPtrC id(static_cast<const TUint16*>(identity.utf16()),identity.length());
    TRAPD(err, iSymbianWriter->DeletePresenceL(id));
    return ( KErrNone == err );
}
        
// ---------------------------------------------------------------------------
// PrcPresenceWriterPrivate::cancelWrite()
// ---------------------------------------------------------------------------
bool PrcPresenceWriterPrivate::cancelWrite()
{
    int ret = iSymbianWriter->CancelWrite();
    return (KErrNone == ret );
}

// ---------------------------------------------------------------------------
// PrcPresenceWriterPrivate::writeMultiplePresence()
// ---------------------------------------------------------------------------
bool  PrcPresenceWriterPrivate::writeMultiplePresence(
    const QList<PrcPresenceBuddyInfoQt*> & presenceBuddyInfos)
 {
    RPointerArray<MPresenceBuddyInfo2> buddyInfosArray;
    for(int i= 0 ; i < presenceBuddyInfos.count();i++)
       {
       PresenceCacheBuddyInfoQt* bInfo = static_cast<PresenceCacheBuddyInfoQt*>(const_cast<PrcPresenceBuddyInfoQt*>(presenceBuddyInfos[i]));
       MPresenceBuddyInfo2* t = bInfo->getSymbianBuddyInstance();
       buddyInfosArray.Append(t);
       }
    TRAPD(err,iSymbianWriter->WriteMultiplePresenceL(buddyInfosArray,this));
    return ( KErrNone == err );
   
   
 }
 
// ---------------------------------------------------------------------------
// PrcPresenceWriterPrivate::HandlePresenceWriteL()
// ---------------------------------------------------------------------------
void PrcPresenceWriterPrivate::HandlePresenceWriteL(int aErrorCode)
{
    bool ret = (KErrNone == aErrorCode);
    emit signalPresenceWrite(ret);
}


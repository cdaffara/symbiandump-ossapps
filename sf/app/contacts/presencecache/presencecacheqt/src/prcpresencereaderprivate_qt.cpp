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

#include "prcpresencereaderprivate_qt.h"
#include <prcpresencebuddyinfo_qt.h>
#include "prcpresencecachebuddyinfo_qt.h"
#include "presencecachereader2.h"


PrcPresenceReaderPrivate::~PrcPresenceReaderPrivate()
{
    delete iSymbianReader;

}
    

PrcPresenceReaderPrivate::PrcPresenceReaderPrivate()
{

    TRAP_IGNORE (
                iSymbianReader = MPresenceCacheReader2::CreateReaderL()
                );
    iSymbianReader->SetObserverForSubscribedNotifications(this);

}


PrcPresenceBuddyInfoQt* PrcPresenceReaderPrivate::presenceInfo(const QString& identity)
{
    TPtrC ident (static_cast<const TUint16*>(identity.utf16()),identity.length());
    MPresenceBuddyInfo2* symBuddyInfo = NULL;
    TRAP_IGNORE(
                symBuddyInfo = iSymbianReader->PresenceInfoLC(ident);
                if(symBuddyInfo)
                    {
                    //If temp != NULL, then it is on clean up stack 
                    CleanupStack::Pop();
                    }
                );            
    if(symBuddyInfo)
        {
        return new PresenceCacheBuddyInfoQt(symBuddyInfo);
        }
    return NULL;
}
               

int PrcPresenceReaderPrivate::buddyCountInAllServices()
{
    return  iSymbianReader->BuddyCountInAllServices();
}
 

int  PrcPresenceReaderPrivate::buddyCountInService(const QString& serviceName) 
{
    TPtrC serv(static_cast<const TUint16*>(serviceName.utf16()),serviceName.length());
    return iSymbianReader->BuddyCountInService(serv);
     
}
   

int PrcPresenceReaderPrivate::servicesCount()
{
    return  iSymbianReader->ServicesCount();
}
       

bool PrcPresenceReaderPrivate::cancelRead() 
{
    int ret = -1;
    ret =  iSymbianReader->CancelRead();
    return (KErrNone == ret);
}

bool PrcPresenceReaderPrivate::allBuddiesPresenceInService(const QString& serviceName) 
{
    TPtrC sn(static_cast<const TUint16*>(serviceName.utf16()),serviceName.length());
    int ret =  iSymbianReader->AllBuddiesPresenceInService(sn,this);
    return (KErrNone == ret );
}

 
bool  PrcPresenceReaderPrivate::subscribePresenceBuddyChange( const QString& identity)
{
    iSymbianReader->SetObserverForSubscribedNotifications(this);
    TPtrC id(static_cast<const TUint16*>(identity.utf16()),identity.length());
    TRAPD(err,iSymbianReader->SubscribePresenceBuddyChangeL(id));
    return ( err == KErrNone );

}

void PrcPresenceReaderPrivate::unSubscribePresenceBuddyChange(const QString& identity)
{
    TPtrC id(static_cast<const TUint16*>(identity.utf16()),identity.length());
    TRAP_IGNORE(iSymbianReader->UnSubscribePresenceBuddyChangeL(id));
    
}

void PrcPresenceReaderPrivate::HandlePresenceReadL(TInt aErrorCode,RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfoList) 
{
    //convert RPointerArray to QList
    QList<PrcPresenceBuddyInfoQt*> bInfo;
    for(int i=0; i<aPresenceBuddyInfoList.Count();i++ )
        {
        //convert MPresenceBuddyInfo2 to PrcPresenceBuddyInfoQt
        PresenceCacheBuddyInfoQt* symBuddyInfo = new PresenceCacheBuddyInfoQt(aPresenceBuddyInfoList[i]);
        bInfo.append(symBuddyInfo);
        }
    //Emit the signal
    bool ret = (KErrNone == aErrorCode);
    emit signalPresenceRead(ret,bInfo);
}


void PrcPresenceReaderPrivate::HandlePresenceNotificationL(TInt aErrorCode, MPresenceBuddyInfo2* aPresenceBuddyInfo) 
{
    bool ret = (KErrNone == aErrorCode);
    PresenceCacheBuddyInfoQt* bInfo = new PresenceCacheBuddyInfoQt (aPresenceBuddyInfo);
    emit signalPresenceNotification(ret, bInfo); 
}


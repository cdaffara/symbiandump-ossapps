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


#include <bamdesca.h>
#include <s32strm.h>
#include <utf.h>

#include "prcpresencecachebuddyinfoprivate_qt.h"
#include "mpresencebuddyinfo2.h"



// ---------------------------------------------------------------------------
// PrcPresenceBuddyInfoQt::PresenceCacheBuddyInfoQtPrivate()
// -----------------------------------------------------------------------------
PresenceCacheBuddyInfoQtPrivate::PresenceCacheBuddyInfoQtPrivate(MPresenceBuddyInfo2* symbianBuddyInfo)
                      
    {
    if(symbianBuddyInfo == NULL)
        {
        TRAP_IGNORE(
                symbianBuddyInfo = MPresenceBuddyInfo2::NewL();
               );    
        }
    iSymbianInstance = symbianBuddyInfo;
    }


PresenceCacheBuddyInfoQtPrivate::~PresenceCacheBuddyInfoQtPrivate() 
{
    delete iSymbianInstance;
    iSymbianInstance  = NULL;
    
}
// ---------------------------------------------------------------------------
// PrcPresenceBuddyInfoQt::setIdentity()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQtPrivate::setIdentity( const QString& buddyId )
{
    TPtrC bId(static_cast<const TUint16*>(buddyId.utf16()),buddyId.length());
    TRAP_IGNORE(
                iSymbianInstance->SetIdentityL(bId);
               );
}


// ---------------------------------------------------------------------------
// PrcPresenceBuddyInfoQt::BuddyId()
// ---------------------------------------------------------------------------
QString PresenceCacheBuddyInfoQtPrivate::BuddyId( ) const
{
    TPtrC16 ptr = iSymbianInstance->BuddyId();
    return  QString::fromUtf16(ptr.Ptr(),ptr.Length());
 } 


// ---------------------------------------------------------------------------
// PrcPresenceBuddyInfoQt::setAvailability()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQtPrivate::setAvailability(PrcPresenceBuddyInfoQt::AvailabilityValues availability,const QString& availabilityText )
{
    TPtrC atext(static_cast<const TUint16*>(availabilityText.utf16()),availabilityText.length());
    MPresenceBuddyInfo2::TAvailabilityValues av = (MPresenceBuddyInfo2::TAvailabilityValues)availability; 
    TRAP_IGNORE(
             iSymbianInstance->SetAvailabilityL(av,atext);
               );
}


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::Availability()
// ---------------------------------------------------------------------------
PrcPresenceBuddyInfoQt::AvailabilityValues PresenceCacheBuddyInfoQtPrivate::Availability( ) 
    {
    return (PrcPresenceBuddyInfoQt::AvailabilityValues) iSymbianInstance->Availability();
    }         

// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::availabilityText()
// ---------------------------------------------------------------------------

QString PresenceCacheBuddyInfoQtPrivate::availabilityText( )
{
    TPtrC aTextPtr = iSymbianInstance->AvailabilityText();
    return QString::fromUtf16(aTextPtr.Ptr(),aTextPtr.Length());
    
}      
 

 // ---------------------------------------------------------------------------
 // PresenceCacheBuddyInfoQtPrivate::setAvatar()
 // ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQtPrivate::setAvatar( const QString& avatar ) 

{
    //Convert to 8 bit and set avatar
    TPtrC8 av( reinterpret_cast<const TUint8*> (avatar.toUtf8().constData()),avatar.toUtf8().size());
    TRAP_IGNORE(
              iSymbianInstance->SetAvatarL(av);
               );
}


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::avatar()
// ---------------------------------------------------------------------------
QString PresenceCacheBuddyInfoQtPrivate::avatar( ) 
{
    TPtrC8 av = iSymbianInstance->Avatar();
    QString ret =  QString::fromLocal8Bit(reinterpret_cast<const char*> (av.Ptr()),av.Length());
    return ret;
}    


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::setStatusMessage()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQtPrivate::setStatusMessage(const QString& statusMessage ) 
{
    TPtrC sm(static_cast<const TUint16*>(statusMessage.utf16()),statusMessage.length()); 

    TRAP_IGNORE(
             iSymbianInstance->SetStatusMessageL(sm);
               );
} 
QString PresenceCacheBuddyInfoQtPrivate::statusMessage( ) 
{
    TPtrC st = iSymbianInstance->StatusMessage();
    return QString::fromUtf16(st.Ptr(),st.Length());
    
}  


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::setAnyField()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQtPrivate::setAnyField(const QString& key,const QString& value ) 
{
    
    TPtrC k(static_cast<const TUint16*>(key.utf16()),key.length());    
    TPtrC16 textPtr(reinterpret_cast<const TUint16*>(value.utf16()));
    HBufC8 *pHeap8 = HBufC8::NewLC(textPtr.Length());
    pHeap8->Des().Copy(textPtr);
    TRAP_IGNORE(
    iSymbianInstance->SetAnyFieldL(k,pHeap8->Des());    
                );
    CleanupStack::PopAndDestroy();
}


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::getAnyField()
// ---------------------------------------------------------------------------
QString PresenceCacheBuddyInfoQtPrivate::getAnyField(const QString& aKey )
{
    TPtrC k(static_cast<const TUint16*>(aKey.utf16()),aKey.length());
    TPtrC8 f = iSymbianInstance->GetAnyField(k);

    QString ret =  QString::fromLocal8Bit(reinterpret_cast<const char*> (f.Ptr()),f.Length());
    return ret;
} 


// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::getFieldKeys()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQtPrivate::getFieldKeys(QList<QString>& qtKeyList )
{
    //Check what should be the size of the array
    CDesCArrayFlat* itemArray = new ( ELeave ) CDesCArrayFlat( 10); 
    TRAP_IGNORE(
    iSymbianInstance->GetFieldKeysL(*itemArray);
                );
    int count = itemArray->Count();
    for (int i = 0; i < count; i++) 
        {
        QString st( QString::fromUtf16(itemArray->MdcaPoint(i).Ptr(),itemArray->MdcaPoint(i).Length()));
        qtKeyList.append(st);
        }
}

// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::getFieldKeys()
// ---------------------------------------------------------------------------
void PresenceCacheBuddyInfoQtPrivate::removeField(const QString& aKey ) 
{
    TPtrC k(static_cast<const TUint16*>(aKey.utf16()),aKey.length());
    iSymbianInstance->RemoveField(k);
        
}      

// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::EqualsIdentity()
// ---------------------------------------------------------------------------
bool PresenceCacheBuddyInfoQtPrivate::EqualsIdentity( const PrcPresenceBuddyInfoQt& otherInstance ) const 
{

     QString otherBuddyId = otherInstance.buddyId();
     return !(this->BuddyId().compare(otherBuddyId));
     
}




// ---------------------------------------------------------------------------
// PresenceCacheBuddyInfoQtPrivate::getSymbianBuddyInstance()
// ---------------------------------------------------------------------------
MPresenceBuddyInfo2* PresenceCacheBuddyInfoQtPrivate::getSymbianBuddyInstance()
{
    return iSymbianInstance;
}

   



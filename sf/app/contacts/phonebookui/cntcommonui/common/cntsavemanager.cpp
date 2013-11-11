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

#include "cntsavemanager.h"
#include "cntdebug.h"

/*!
    Contact saving helper class. Used mainly in edit view for contact saving,
    but should also be used in other (special) cases a contact needs to be saved.
    For example "forcing" phonebook to shut down (end key, from task swapper etc..)
    in detail editors.
*/
CntSaveManager::CntSaveManager(CntContactType type, QObject* parent) :
    QObject(parent),
    mContactType(type)
{
    CNT_ENTRY
    
    CNT_EXIT
}

/*!
    Destructor
*/
CntSaveManager::~CntSaveManager()
{
    CNT_ENTRY
    
    CNT_EXIT
}

/*!
    Saves the given QContact to the given QContactManager. Also takes care of checking
    if the contact is MyCard or a group and behaves different accordingly.

    \param aContact the contact to be saved, ownership not taken
    \param aManager the QContactManager which should be used for saving the contact, ownership not taken
    \return CntSaveResult enum to describe what was done to the contact (saved, updated etc...)
*/
CntSaveManager::CntSaveResult CntSaveManager::saveContact(QContact* aContact, QContactManager* aManager)
{
    CNT_ENTRY
    
    CntSaveResult result = ENothingDone;
    bool isSavedContact = aContact->localId() > 0;
    
    // if the contact is really changed or a new one
    if ( (*aContact) != aManager->contact(aContact->localId()) || !isSavedContact )
    {
        int detailCount = aContact->details().count();
        
        // Don't set preferred details for a group
        if (mContactType != EGroup)
        {
            setPreferredDetails( aContact );
        }
        
        // If its a new contact
        if ( !isSavedContact )
        {
            // if a new contact has 2 or less details, it's still considered empty
            if ( detailCount > 2 )
            {
                bool success = aManager->saveContact( aContact );
                if ( success && mContactType == EMyCard )
                {
                    aManager->setSelfContactId( aContact->localId() );
                }
                
                result = success ? ESaved : EFailed;
            }
        }
        else
        {
            // Contact details has been cleared out, a symbian QContact with 
            // 4 or less details is in reality empty.
            if ( detailCount <= 4 )
            {
                bool success = aManager->removeContact( aContact->localId() );
                
                result = success ? EDeleted : EFailed;
            }
            else
            {
                bool success = aManager->saveContact(aContact);
                
                result = success ? EUpdated : EFailed;     
            }
        }
    }
    
    CNT_EXIT_ARGS(result)
    
    return result;
}

/*!
    Set the preferred call, sms and email details for a contact if possible
    and needed.

    \param aContact the contact being handled
*/
void CntSaveManager::setPreferredDetails( QContact* aContact )
{
    CNT_ENTRY
    
    QList<QContactPhoneNumber> numberList( aContact->details<QContactPhoneNumber>() );
    //set preferred number for call if there is only one phone number
    if ( aContact->preferredDetail("call").isEmpty() && numberList.count() == 1 )
    {
        aContact->setPreferredDetail( "call", numberList.first() );
    }
    //set preferred number for message if there is only one mobile phone number
    if ( aContact->preferredDetail("message").isEmpty() && numberList.count() >= 1 )
    {
        int mobileNumbers = 0;
        int mobileNumberIndex = -1;
        for (int i = 0; i < numberList.count(); i++)
        {
            QStringList subTypes = numberList.at(i).subTypes();
            if (subTypes.count() && subTypes.first() == QContactPhoneNumber::SubTypeMobile)
            {
                mobileNumbers++;
                mobileNumberIndex = i;
            }      
        }
        if ( mobileNumbers == 1 )
        {
            aContact->setPreferredDetail( "message", numberList.at(mobileNumberIndex) );
        }
    }
    QList<QContactEmailAddress> emailList( aContact->details<QContactEmailAddress>() );
    //set preferred number for email if there is only one email address
    if ( aContact->preferredDetail("email").isEmpty() && emailList.count() == 1 )
    {
        aContact->setPreferredDetail( "email", emailList.first() );
    }
    
    CNT_EXIT
}

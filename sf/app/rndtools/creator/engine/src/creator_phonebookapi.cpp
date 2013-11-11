/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "creator_phonebookapi.h"

CCreatorPhonebookAPI::CCreatorPhonebookAPI ()
    {
    mContactMngr = new QContactManager("symbian");
    }

CCreatorPhonebookAPI::~CCreatorPhonebookAPI ()
    {
    if( mContactMngr )
        {
        delete mContactMngr;
        mContactMngr = NULL;
        }
    }

quint32 CCreatorPhonebookAPI::saveContact( const QList<QContactDetail>& list )
    {
    // create a new contact item
    QContact store;
    quint32 id;
    for(int i = 0 ; i < list.count() ; i++ )
    	{
		QContactDetail cntdetail = list.at(i);
		store.saveDetail(&cntdetail);
    	}
    /*foreach( QContactDetail cntdetail, list )
        {
        store.saveDetail( &cntdetail );
        }
    */
    mContactMngr->saveContact( &store );
    id = store.localId();
    return id;
    }

quint32 CCreatorPhonebookAPI::createGroup( const QString& groupName )
	{
	QContact newGroup;
	newGroup.setType(QContactType::TypeGroup);
	QContactName newGroupName;
	newGroupName.setCustomLabel( groupName );
	newGroup.saveDetail(&newGroupName);
	mContactMngr->saveContact(&newGroup);
	return newGroup.localId();
	}

int CCreatorPhonebookAPI::numberOfContacts()
	{
	QList<QContactLocalId> contacts = mContactMngr->contactIds();
	return contacts.count();
	
	}

bool CCreatorPhonebookAPI::IsContactGroupL( const QContact& contact )
	{
	 
	if( contact.type() == QContactType::TypeGroup )
		{
		return true;
		}
	return false;
	}

int CCreatorPhonebookAPI::addContactToGroup( QContactLocalId group, QContactLocalId contact )
	{
	QContact newGroup = mContactMngr->contact( group );
	QContact contactLink = mContactMngr->contact( contact );
	int ret = 0;
	if( contact && IsContactGroupL( contactLink ) == false )
	    {
        QList<QContactRelationship> relationships = contactLink.relationships(QContactRelationship::HasMember);
        if( !relationships.count() && contactLink.type() == QContactType::TypeContact ) //just for contacts that are not in relationship - not in group yet
			{
			QContactRelationship* contactRel = new QContactRelationship();
			contactRel->setRelationshipType(QContactRelationship::HasMember);
			contactRel->setFirst(newGroup.id());
			contactRel->setSecond( contactLink.id() );
			mContactMngr->saveRelationship( contactRel );
			delete contactRel;
			ret++;
			}         
		}
	return ret;
	}
int CCreatorPhonebookAPI::addToGroup(QContactLocalId group, int amount)
	{
	QList<QContactLocalId> contacts = mContactMngr->contactIds();
	int ret = 0;		
	int tmp = 0;
	int cnt = 0;
	
	for( int i=0; cnt < amount && i < contacts.count() ; i++ )
		{
		QContact contact = mContactMngr->contact( mContactMngr->contactIds().at(i) );
		if( contact.type() == QContactType::TypeContact )
			{
			tmp = addContactToGroup(group,mContactMngr->contactIds().at(i)); 
			ret += tmp;
			if(tmp)
				{
				cnt++;
				tmp = 0;
				}
			}
		}
	return ret;
	}


bool CCreatorPhonebookAPI::deleteAllContacts()
    {
    QList<QContactLocalId> all = mContactMngr->contactIds();
    return deleteContacts( all );
    }

bool CCreatorPhonebookAPI::deleteAllContacts( const QString& type )
    {
    QList<QContactLocalId> contactsToDelete;
    QList<QContactLocalId> contacts = mContactMngr->contactIds();
    foreach(QContactLocalId contactId, contacts)
        {
        QContact contact = mContactMngr->contact( contactId );
        if( contact.type() == type )
            {
            contactsToDelete.append( contact.localId() );
            }
        }
    return deleteContacts( contactsToDelete );
    }


bool CCreatorPhonebookAPI::deleteContacts( const QList<QContactLocalId>& list )
    {
    QMap<int, QContactManager::Error> errorMap;
    return mContactMngr->removeContacts( list, &errorMap );
    }

QContact CCreatorPhonebookAPI::contact( const QContactLocalId& contactId )
    {
    return mContactMngr->contact( contactId );
    }

QString CCreatorPhonebookAPI::phoneNumber( const QContactLocalId& contactId )
    {
    QString strNumber;
    QContact contact = mContactMngr->contact( contactId );
    if( !contact.isEmpty() )
        {
        QContactPhoneNumber phoneNumber = static_cast<QContactPhoneNumber>( contact.detail( QContactPhoneNumber::DefinitionName ) );
        strNumber = phoneNumber.number();
        }
    return strNumber;
    }

bool CCreatorPhonebookAPI::contactDetails( const QContactLocalId& contactId, QString& name, QString& phoneNumber, QString& email )
    {
    bool success(false);
    QContact contact = mContactMngr->contact( contactId );
    if( !contact.isEmpty() )
        {
        QContactPhoneNumber contactPhoneNumber = static_cast<QContactPhoneNumber>( contact.detail( QContactPhoneNumber::DefinitionName ) );
        phoneNumber = contactPhoneNumber.number();
        QContactEmailAddress contactEmailAddress = static_cast<QContactEmailAddress>( contact.detail( QContactEmailAddress::DefinitionName ) );
        email = contactEmailAddress.emailAddress();
        QContactDisplayLabel contactDisplayLabel = static_cast<QContactDisplayLabel>( contact.detail( QContactDisplayLabel::DefinitionName ) );
        name = contactDisplayLabel.label();
        success = true;
        }
    return success;
    }
// End of File

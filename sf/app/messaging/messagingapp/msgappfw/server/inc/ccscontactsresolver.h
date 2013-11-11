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
 * Description: QT contact resolver interface
 *
 */
#ifndef CONTACTS_RESOLVER_H
#define CONTACTS_RESOLVER_H

// SYSTEM INCLUDES
#include <QObject>
#include <QStringList>
#include <qtcontactsglobal.h>

//USER INCLUDES

QTM_BEGIN_NAMESPACE
//FORAWRD DELCARATIONS
class QContactManager;
QTM_END_NAMESPACE


QTM_USE_NAMESPACE

// CLASS DECLARATION

/**
 *  CCsContactDetail class holds the details obtained from the phonebook
 *  for a given contact 
 */
class CCsContactDetail
{
public:
    
	/**
	 * Constructor
	 */
    CCsContactDetail():contactId(-1),displayName(QString()){};
    
public:
    int contactId;
	QString displayName;
	QStringList addressList;
};

/**
 *  QT contact resolver interface, Gets ConvergedMessageAddress from 
 *  conversation harvester and resolves the address to an entry in the 
 *  contacts database and returns required phonebook
 *  details to be updated to conversation DB
 */
class CCsContactsResolver
{
public: 

	/**
	 * Constructor
	 */
	explicit CCsContactsResolver(QContactManager* manager);

	/**
	 * Destructor
	 */
	virtual ~CCsContactsResolver();

	/**
	 * resolveContact
	 * Resolve contact from phonebook and fill contact 
	 * details in contactDetail object  
	 * 
	 * @param address ConvergedMessageAddress object 
	 * @param contactDetail ContactDetail object
	 */ 
	bool resolveContact(const QString &address,
	                     CCsContactDetail &contactDetail);

	/**
	 * resolveContactId
	 * Resolve contactId from phonebook and fill contact 
	 * details in contactDetail object  
	 * 
	 * @param id quint32 contact id
	 * @param contactDetail ContactDetail object 
	 */ 
	void resolveContactId(const quint32 &contactId,
	                         CCsContactDetail &contactDetail);	
	
private:
	
    /**
     * mPhonebookManager
     * An instance of the QContactManager
     */
    QContactManager* mPhonebookManager;    

};

#endif // CONTACTS_RESOLVER_H

// EOF


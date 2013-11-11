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


#ifndef CREATOR_PHONEBOOKAPI_H_
#define CREATOR_PHONEBOOKAPI_H_

#include <QString>
#include <qglobal.h>
#include <QTime>
#include <qtcontacts.h>
QTM_USE_NAMESPACE


class CCreatorPhonebookAPI
    {
public:
    CCreatorPhonebookAPI ();
    ~CCreatorPhonebookAPI ();

    quint32 saveContact( const QList<QContactDetail>& list );
    quint32 createGroup( const QString& groupName );
    int numberOfContacts();
    int addContactToGroup( QContactLocalId group, QContactLocalId contact );
    int addToGroup(QContactLocalId group, int amount);
    bool deleteAllContacts();
    bool deleteAllContacts( const QString& type );
    bool deleteContacts( const QList<QContactLocalId>& list );
    QContact contact( const QContactLocalId& contactId );

    QString phoneNumber( const QContactLocalId& contactId );
    bool contactDetails( const QContactLocalId& contactId, QString& name, QString& phoneNumber, QString& email );
    
private:
    bool IsContactGroupL( const QContact& contact );
    
private:
    QContactManager* mContactMngr;

    };

#endif /* CREATOR_PHONEBOOKAPI_H_ */

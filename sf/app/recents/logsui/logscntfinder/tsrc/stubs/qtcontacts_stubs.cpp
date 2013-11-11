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

#include <QContactManager.h>
#include <QContactDetailFilter.h>
#include <QContactPhoneNumber.h>
#include <QSharedDataPointer>
#include <QContactName.h>
#include <QContactDetail.h>
#include <QContactAction.h>

#include <QSharedData>
#include <QContactAvatar.h>

#include "qtcontacts_stubs.h"

ContactQueryResults* ContactQueryResults::mInstance = 0;
QString cntPreferredPhoneNumber = "123456";

void ContactStubHelper::setPreferredNumber(QString number)
{
    cntPreferredPhoneNumber = number;
}

QString ContactStubHelper::preferredNumber()
{
    return cntPreferredPhoneNumber;
}

// -----------------------------------------------------------------------------
// ContactQueryResults::instance()
// -----------------------------------------------------------------------------
//
ContactQueryResults* ContactQueryResults::instance()
{
    if ( !mInstance ) {
        mInstance = new ContactQueryResults();
    }
    return mInstance;
}

// -----------------------------------------------------------------------------
// ContactQueryResults::deleteInstance()
// -----------------------------------------------------------------------------
//
void ContactQueryResults::deleteInstance()
{
    delete mInstance;
    mInstance = 0;
}

// -----------------------------------------------------------------------------
// ContactQueryResults::~ContactQueryResults()
// -----------------------------------------------------------------------------
//
ContactQueryResults::~ContactQueryResults()
{
    reset();
}

// -----------------------------------------------------------------------------
// ContactQueryResults::set()
// -----------------------------------------------------------------------------
//
void ContactQueryResults::set( int count, QString fn, QString ln )
{
    reset();
    for( int i=0;i<count;i++ ) {
        mDB.append( fn + QString::number( i + 1 ) );
        mDB.append( ln + QString::number( i + 1 ) );
    }
}

// -----------------------------------------------------------------------------
// ContactQueryResults::reset()
// -----------------------------------------------------------------------------
//
void ContactQueryResults::reset()
{
    mDB.clear();
    mQuery.clear();
}


// -----------------------------------------------------------------------------
// ContactQueryResults::firstNameAt()
// -----------------------------------------------------------------------------
//
const QString& ContactQueryResults::firstNameAt( int index ) const
{
    return mDB[ index ];
}

// -----------------------------------------------------------------------------
// ContactQueryResults::lastNameAt()
// -----------------------------------------------------------------------------
//
const QString& ContactQueryResults::lastNameAt( int index ) const
{
    return mDB[ index + 1 ];
}

// -----------------------------------------------------------------------------
// ContactQueryResults::contacts()
// -----------------------------------------------------------------------------
//
int ContactQueryResults::contacts() const
{
    return (mDB.length() ? mDB.length() / 2 : 0);
}


QTM_USE_NAMESPACE

class QtMobility::QContactData : public QSharedData
{
public:
    QContactData(): QSharedData()
    {
    }

    ~QContactData() {}
    
    QContactId m_id;
    
};

class QtMobility::QContactDetailPrivate : public QSharedData
{
public:
    QContactDetailPrivate(): QSharedData()
    {
    }

    ~QContactDetailPrivate() {}
};

// ----------------------------------------------------------------------------
// QContactManager
// ----------------------------------------------------------------------------
//
QContactManager::QContactManager(
        const QString& managerName, const QMap<QString, QString>& parameters, 
        QObject* parent)
{
    Q_UNUSED(managerName)
    Q_UNUSED(parameters)
    Q_UNUSED(parent)

}

QContactManager::~QContactManager()
{

}


QList<QContactLocalId> QContactManager::contactIds(
        const QContactFilter& filter, 
        const QList<QContactSortOrder>& sortOrders ) const
{
    Q_UNUSED(filter)
    Q_UNUSED(sortOrders)
        
    QList<QContactLocalId> list;
    ContactQueryResults* resultSet = ContactQueryResults::instance();
    
    for( int i=0;i<resultSet->contacts();i++) {
        list.append( i + 1 );
    }
    resultSet->allResultsAdded();
    
    return list;
}


QContact QContactManager::contact( 
        const QContactLocalId& contactId, 
        const QContactFetchHint& fetchHint) const
{
    Q_UNUSED(fetchHint)
            
    QContact contact;
    QContactId id;
    id.setLocalId(contactId  );

    contact.setId( id );
    return contact;
}


// ----------------------------------------------------------------------------
// QContactDetailFilter
// ----------------------------------------------------------------------------
//
QContactDetailFilter::QContactDetailFilter()
{

}
  
void QContactDetailFilter::setDetailDefinitionName(
        const QString& definition, const QString& fieldName )
{
    Q_UNUSED(definition)
    Q_UNUSED(fieldName)

}


void QContactDetailFilter::setMatchFlags(QContactFilter::MatchFlags flags )
{
    Q_UNUSED(flags)

}
void QContactDetailFilter::setValue(const QVariant& value )
{
    ContactQueryResults* resultSet = ContactQueryResults::instance();
    resultSet->filterValueSet( value.toString() );
    
}

QVariant QContactDetailFilter::value() const
{
    return QVariant();
}


// ----------------------------------------------------------------------------
// QContact
// ----------------------------------------------------------------------------
//
QContact::QContact() : d(new QContactData)
{

}

QContact::~QContact()
{

}


QContact& QContact::operator=(const QContact& other )
{
    Q_UNUSED(other)
    return *this;
}

QContactDetail QContact::detail(const char* definitionId) const
{
    return detail( QString( definitionId ) );
}


QContactDetail QContact::detail(const QString& definitionId) const
{
    ContactQueryResults* results = ContactQueryResults::instance();
    
    if ( definitionId == QContactName::DefinitionName ){
        QContactName name;
        QContactLocalId id = localId();
        name.setValue(QContactName::FieldFirstName, results->firstNameAt( (int) id-1 ) );
        name.setValue(QContactName::FieldLastName, results->lastNameAt( (int) id-1 ) );
        return name;
    }
    if ( definitionId == QContactPhoneNumber::DefinitionName ){
        QContactPhoneNumber number;
        number.setValue(QContactPhoneNumber::FieldNumber, cntDetailPhoneNumber );
        return number;
    }
    if ( definitionId == QContactAvatar::DefinitionName){
        QContactAvatar avatar;
        avatar.setValue(QContactAvatar::FieldImageUrl, "c:\\data\\images\\logstest1.jpg" );
        return avatar;
    }
    QContactDetail detail;
    return detail;
}

QContactDetail QContact::preferredDetail(const QString& actionName) const
{
    Q_UNUSED(actionName)
    QContactPhoneNumber number;
    number.setValue(QContactPhoneNumber::FieldNumber, cntPreferredPhoneNumber );
    return number;
}

QContactDetail QContact::detailWithAction( QContactAction* action ) const
{
    Q_UNUSED(action)
    QContactPhoneNumber number;
    number.setValue(QContactPhoneNumber::FieldNumber, cntPhoneNumberWithActionCall );
    return number;
}

QContactAction* QContactAction::action(const QContactActionDescriptor& descriptor)
{
    Q_UNUSED(descriptor)
    return 0;        
}

// ----------------------------------------------------------------------------
// QContactDetail
// ----------------------------------------------------------------------------
//
QContactDetail::QContactDetail()
{

}

QContactDetail::QContactDetail(const QString& definitionName ) : d(new QContactDetailPrivate)
{
    Q_UNUSED(definitionName)
}

QContactDetail::~QContactDetail()
{

}


QContactDetail& QContactDetail::operator=(const QContactDetail& other )
{
    Q_UNUSED(other)
    return *this;
}

QString QContactDetail::definitionName() const
{
    return QString("");
}

bool QContactDetail::isEmpty() const
{
    return false;
}


QVariant QContactDetail::variantValue(const QString& key) const
{
    QString val = value(key);
    return val;
}




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

#include <qcontactgeolocation.h>
#include <qcontactmanager.h>
#include <qtcontacts.h>
#include <cntdef.h>
#include <agendautil.h>
#include <agendaentry.h>
#include <QString>
#include <QEventLoop>
#include <locationservicedefines.h>
#include "geocodeupdate.h"
#include "mylocationsdefines.h"
#include "mylocationlogger.h"

using namespace QTM_NAMESPACE;


// ----------------------------------------------------------------------------
// GeocodeUpdate::GeocodeUpdate()
// ----------------------------------------------------------------------------
GeocodeUpdate::GeocodeUpdate() :
    mAgendaUtil(NULL), mUtilInstanceCreated(false)
{
    mAgendaUtil = new AgendaUtil();
    connect(mAgendaUtil, SIGNAL(instanceViewCreationCompleted(int)), this,
        SLOT(agendautilInstanceCreated(int)));
}

// ----------------------------------------------------------------------------
// GeocodeUpdate::~GeocodeUpdate()
// ----------------------------------------------------------------------------
GeocodeUpdate::~GeocodeUpdate()
{
    __TRACE_CALLSTACK;    
    delete mAgendaUtil;    
}

// ----------------------------------------------------------------------------
// GeocodeUpdate::createContactdb()
// ----------------------------------------------------------------------------
void GeocodeUpdate::createContactdb()
{
    QContactManager* contactManger = NULL;
    MYLOCLOGSTRING("call to create contactManger object and contactdb as well.");
    contactManger = new QContactManager("symbian");
    delete contactManger;
}

// ----------------------------------------------------------------------------
// CGeocodeUpdate::updateGeocodeToContactDB()
// Geo-cordinate updation to contact db  
// ----------------------------------------------------------------------------
bool GeocodeUpdate::updateGeocodeToContactDB(const quint32 contactId, const int addressType,
    const double latitude, const double longitude)

{
    __TRACE_CALLSTACK;
    QContactManager* contactManger = NULL;
    MYLOCLOGSTRING("call to create contactManger object .");
    contactManger = new QContactManager("symbian");
    MYLOCLOGSTRING("contactManger object created .");


    MYLOCLOGSTRING("contactManger object is not null .");
    QContact contact = contactManger->contact(contactId);
    QContactGeoLocation *location=NULL;
    location=new QContactGeoLocation();

    switch (addressType) {
        case ESourceContactsPref:
        {
            break;
        }
        case ESourceContactsWork:
        {
            location->setContexts(QContactDetail::ContextWork);
            break;
        }
        case ESourceContactsHome:
        {
            location->setContexts(QContactDetail::ContextHome);
            break;
        }
        default:
        {
            delete location;                
            delete contactManger;
            return false;
        }
    }
    location->setLongitude(longitude);
    location->setLatitude(latitude);
    contact.saveDetail(location);
    delete location;
    location=NULL;
    bool ret = false;
    ret = contactManger->saveContact(&contact);
    delete contactManger;
    return ret;
}

// ----------------------------------------------------------------------------
// CGeocodeUpdate::updateGeocodeToCalenderDB()
// Geo-cordinate updation to contact db  
// ----------------------------------------------------------------------------
bool GeocodeUpdate::updateGeocodeToCalenderDB(const ulong& calEntryId, const double& latitude,
    const double& longitude)

{
    __TRACE_CALLSTACK;
    bool ret = false;
    if (mUtilInstanceCreated) {
        AgendaEntry agendaEntry(mAgendaUtil->fetchById(calEntryId));
        MYLOCLOGSTRING("agenda entry created from calender id .");
        AgendaGeoValue geoValue;
        geoValue.setLatLong(latitude, longitude);
        MYLOCLOGSTRING("latitude and longitude set to  AgendaGeoValue object.");
        if( !agendaEntry.isNull() )
        {
            agendaEntry.setGeoValue(geoValue);
            ret = mAgendaUtil->store(agendaEntry);
        }
    }    
    return ret;
}

// ----------------------------------------------------------------------------
// CGeocodeUpdate::isGeocodeNotAvailable()
// check if geo-cordinate are available for this specific entry id.
// return true , for available otherwise false.
// ----------------------------------------------------------------------------
bool GeocodeUpdate::isGeocodeNotAvailable(const ulong& calEntryId)
{
    __TRACE_CALLSTACK;
    bool ret = false;
    if (mUtilInstanceCreated) {
        AgendaEntry agendaEntry(mAgendaUtil->fetchById(calEntryId));
        const AgendaGeoValue &geoValue = agendaEntry.geoValue();
        ret = geoValue.isNull();
    }
    return ret;
}

// ----------------------------------------------------------------------------
// CGeocodeUpdate::agendautilInstanceCreated()
// call back on agendautil instance creation  
// ----------------------------------------------------------------------------
void GeocodeUpdate::agendautilInstanceCreated(int status)
{
    if (AgendaUtil::NoError == status) {
        mUtilInstanceCreated = true;
        emit eventCompleted();
    }
}
//end of line

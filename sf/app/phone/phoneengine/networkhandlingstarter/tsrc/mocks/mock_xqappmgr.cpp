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
#include <QDebug>
#include <smcmockclassincludes.h>
#include <xqappmgr.h>
#include <xqaiwrequest.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// XQApplicationManager::XQApplicationManager
// -----------------------------------------------------------------------------
//
XQApplicationManager::XQApplicationManager(  )
    //:
    //QObject( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::~XQApplicationManager
// -----------------------------------------------------------------------------
//
XQApplicationManager::~XQApplicationManager(  )
    {
    
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::create
// -----------------------------------------------------------------------------
//
XQAiwRequest * XQApplicationManager::create( 
        const QString & interface,
        const QString & operation,
        bool embedded )
    {
    SMC_MOCK_METHOD3( XQAiwRequest *, const QString &, interface, 
        const QString &, operation, 
        bool, embedded )
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::create
// -----------------------------------------------------------------------------
//
XQAiwRequest * XQApplicationManager::create( 
        const XQAiwInterfaceDescriptor & implementation,
        const QString & operation,
        bool embedded )
    {
    SMC_MOCK_METHOD3( XQAiwRequest *, const XQAiwInterfaceDescriptor &, implementation, 
        const QString &, operation, 
        bool, embedded )
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::create
// -----------------------------------------------------------------------------
//
XQAiwRequest * XQApplicationManager::create( 
        const QString & service,
        const QString & interface,
        const QString & operation,
        bool embedded )
    {
    SMC_MOCK_METHOD4( XQAiwRequest *, const QString &, service, 
        const QString &, interface, 
        const QString &, operation, 
        bool, embedded )
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::list
// -----------------------------------------------------------------------------
//
QList <XQAiwInterfaceDescriptor > XQApplicationManager::list( 
        const QString & interface,
        const QString & operation )
    {
   /* SMC_MOCK_METHOD2( QList <XQAiwInterfaceDescriptor >, const QString &, interface, 
        const QString &, operation )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::list
// -----------------------------------------------------------------------------
//
QList <XQAiwInterfaceDescriptor > XQApplicationManager::list( 
        const QString & service,
        const QString & interface,
        const QString & operation )
    {
    /*SMC_MOCK_METHOD3( QList <XQAiwInterfaceDescriptor >, const QString &, service, 
        const QString &, interface, 
        const QString &, operation )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::create
// -----------------------------------------------------------------------------
//
XQAiwRequest * XQApplicationManager::create( 
        const QUrl & url,
        bool embedded )
    {
    /*SMC_MOCK_METHOD2( XQAiwRequest *, const QUrl &, url, 
        bool, embedded )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::create
// -----------------------------------------------------------------------------
//
XQAiwRequest * XQApplicationManager::create( 
        const QUrl & url,
        const XQAiwInterfaceDescriptor & implementation,
        bool embedded )
    {
    /*SMC_MOCK_METHOD3( XQAiwRequest *, const QUrl &, url, 
        const XQAiwInterfaceDescriptor &, implementation, 
        bool, embedded )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::create
// -----------------------------------------------------------------------------
//
XQAiwRequest * XQApplicationManager::create( 
        const QFile & file,
        bool embedded )
    {
    /*SMC_MOCK_METHOD2( XQAiwRequest *, const QFile &, file, 
        bool, embedded )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::create
// -----------------------------------------------------------------------------
//
XQAiwRequest * XQApplicationManager::create( 
        const QFile & file,
        const XQAiwInterfaceDescriptor & implementation,
        bool embedded )
    {
    /*SMC_MOCK_METHOD3( XQAiwRequest *, const QFile &, file, 
        const XQAiwInterfaceDescriptor &, implementation, 
        bool, embedded )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::list
// -----------------------------------------------------------------------------
//
QList <XQAiwInterfaceDescriptor > XQApplicationManager::list( 
        const QUrl & uri )
    {
    //SMC_MOCK_METHOD1( QList <XQAiwInterfaceDescriptor >, const QUrl &, uri )
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::list
// -----------------------------------------------------------------------------
//
QList <XQAiwInterfaceDescriptor > XQApplicationManager::list( 
        const QFile & file )
    {
    //SMC_MOCK_METHOD1( QList <XQAiwInterfaceDescriptor >, const QFile &, file )
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::list
// -----------------------------------------------------------------------------
//
QList <XQAiwInterfaceDescriptor > XQApplicationManager::list( 
        const XQSharableFile & file )
    {
    //SMC_MOCK_METHOD1( QList <XQAiwInterfaceDescriptor >, const XQSharableFile &, file )
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::create
// -----------------------------------------------------------------------------
//
XQAiwRequest * XQApplicationManager::create( 
        const XQSharableFile & file,
        bool embedded )
    {
   /* SMC_MOCK_METHOD2( XQAiwRequest *, const XQSharableFile &, file, 
        bool, embedded )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::create
// -----------------------------------------------------------------------------
//
XQAiwRequest * XQApplicationManager::create( 
        const XQSharableFile & file,
        const XQAiwInterfaceDescriptor & implementation,
        bool embedded )
    {
    /*SMC_MOCK_METHOD3( XQAiwRequest *, const XQSharableFile &, file, 
        const XQAiwInterfaceDescriptor &, implementation, 
        bool, embedded )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::lastError
// -----------------------------------------------------------------------------
//
int XQApplicationManager::lastError(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::isRunning
// -----------------------------------------------------------------------------
//
bool XQApplicationManager::isRunning( 
        const XQAiwInterfaceDescriptor & implementation ) const
    {
   // SMC_MOCK_METHOD1( bool, const XQAiwInterfaceDescriptor &, implementation )
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::getDrmAttributes
// -----------------------------------------------------------------------------
//
bool XQApplicationManager::getDrmAttributes( 
        const QFile & file,
        const QList<int> & attributeNames,
        QVariantList & attributeValues )
    {
  /*  SMC_MOCK_METHOD3( bool, const QFile &, file, 
        const QList<int> &, attributeNames, 
        QVariantList &, attributeValues )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::getDrmAttributes
// -----------------------------------------------------------------------------
//
bool XQApplicationManager::getDrmAttributes( 
        const XQSharableFile & file,
        const QList<int> & attributeNames,
        QVariantList & attributeValues )
    {
   /* SMC_MOCK_METHOD3( bool, const XQSharableFile &, file, 
        const QList<int> &, attributeNames, 
        QVariantList &, attributeValues )*/
    }


// -----------------------------------------------------------------------------
// XQApplicationManager::status
// -----------------------------------------------------------------------------
//
XQApplicationManager::ServiceStatus XQApplicationManager::status( 
        const XQAiwInterfaceDescriptor & implementation )
    {
   // SMC_MOCK_METHOD1( XQApplicationManager::ServiceStatus, const XQAiwInterfaceDescriptor &, implementation )
    }

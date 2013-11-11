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
#include <xqserviceprovider.h>
#include <xqrequestinfo.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// XQServiceProvider::XQServiceProvider
// -----------------------------------------------------------------------------
//
XQServiceProvider::XQServiceProvider( 
        const QString & service,
        QObject * parent )
    :
    QObject(parent)
    {
    Q_UNUSED(service)
    }


// -----------------------------------------------------------------------------
// XQServiceProvider::~XQServiceProvider
// -----------------------------------------------------------------------------
//
XQServiceProvider::~XQServiceProvider(  )
    {
    
    }


// -----------------------------------------------------------------------------
// XQServiceProvider::SetPlugin
// -----------------------------------------------------------------------------
//
void XQServiceProvider::SetPlugin( 
        QObject * impl_plugin )
    {
    SMC_MOCK_METHOD1( void, QObject *, impl_plugin )
    }


// -----------------------------------------------------------------------------
// XQServiceProvider::publishAll
// -----------------------------------------------------------------------------
//
void XQServiceProvider::publishAll(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// XQServiceProvider::setCurrentRequestAsync
// -----------------------------------------------------------------------------
//
int XQServiceProvider::setCurrentRequestAsync(  )
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// XQServiceProvider::completeRequest
// -----------------------------------------------------------------------------
//
bool XQServiceProvider::completeRequest( 
        int index,
        const QVariant & retValue )
    {
    SMC_MOCK_METHOD2( bool, int, index, 
        const QVariant &, retValue )
    }


// -----------------------------------------------------------------------------
// XQServiceProvider::requestInfo
// -----------------------------------------------------------------------------
//
XQRequestInfo XQServiceProvider::requestInfo(  ) const
    {
    SMC_MOCK_METHOD0( XQRequestInfo )
    }



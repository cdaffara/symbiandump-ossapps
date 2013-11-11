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
#include <xqaiwrequest.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// XQAiwRequest::XQAiwRequest
// -----------------------------------------------------------------------------
//
XQAiwRequest::XQAiwRequest( 
        const XQAiwInterfaceDescriptor & descriptor,
        const QString & operation,
        bool embedded )
    //:
    //QObject( /*descriptor, operation, embedded*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::XQAiwRequest
// -----------------------------------------------------------------------------
//
XQAiwRequest::XQAiwRequest( 
        const QUrl & uri,
        const XQAiwInterfaceDescriptor & descriptor,
        const QString & operation )
    //:
    //QObject( /*uri, descriptor, operation*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::XQAiwRequest
// -----------------------------------------------------------------------------
//
XQAiwRequest::XQAiwRequest( 
        const QFile & file,
        const XQAiwInterfaceDescriptor & descriptor,
        const QString & operation )
    //:
    //QObject( /*file, descriptor, operation*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::XQAiwRequest
// -----------------------------------------------------------------------------
//
XQAiwRequest::XQAiwRequest( 
        const XQSharableFile & file,
        const XQAiwInterfaceDescriptor & descriptor,
        const QString & operation )
    //:
    //QObject( /*file, descriptor, operation*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::~XQAiwRequest
// -----------------------------------------------------------------------------
//
XQAiwRequest::~XQAiwRequest(  )
    {
    
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::createAction
// -----------------------------------------------------------------------------
//
QAction * XQAiwRequest::createAction(  )
    {
    SMC_MOCK_METHOD0( QAction * )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::setArguments
// -----------------------------------------------------------------------------
//
void XQAiwRequest::setArguments( 
        const QList<QVariant> & arguments )
    {
    //SMC_MOCK_METHOD1( void, const QList<QVariant> &, arguments )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::lastError
// -----------------------------------------------------------------------------
//
int XQAiwRequest::lastError(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::lastErrorMessage
// -----------------------------------------------------------------------------
//
const QString & XQAiwRequest::lastErrorMessage(  ) const
    {
    SMC_MOCK_METHOD0( const QString & )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::descriptor
// -----------------------------------------------------------------------------
//
const XQAiwInterfaceDescriptor & XQAiwRequest::descriptor(  ) const
    {
    //SMC_MOCK_METHOD0( const XQAiwInterfaceDescriptor & )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::send
// -----------------------------------------------------------------------------
//
bool XQAiwRequest::send(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::send
// -----------------------------------------------------------------------------
//
bool XQAiwRequest::send( 
        QVariant & returnValue )
    {
    //SMC_MOCK_METHOD1( bool, QVariant &, returnValue )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::setEmbedded
// -----------------------------------------------------------------------------
//
void XQAiwRequest::setEmbedded( 
        bool embedded )
    {
    SMC_MOCK_METHOD1( void, bool, embedded )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::isEmbedded
// -----------------------------------------------------------------------------
//
bool XQAiwRequest::isEmbedded(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::setOperation
// -----------------------------------------------------------------------------
//
void XQAiwRequest::setOperation( 
        const QString & operation )
    {
    SMC_MOCK_METHOD1( void, const QString &, operation )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::operation
// -----------------------------------------------------------------------------
//
const QString & XQAiwRequest::operation(  ) const
    {
    SMC_MOCK_METHOD0( const QString & )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::setSynchronous
// -----------------------------------------------------------------------------
//
void XQAiwRequest::setSynchronous( 
        bool synchronous )
    {
    SMC_MOCK_METHOD1( void, bool, synchronous )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::isSynchronous
// -----------------------------------------------------------------------------
//
bool XQAiwRequest::isSynchronous(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::setBackground
// -----------------------------------------------------------------------------
//
void XQAiwRequest::setBackground( 
        bool background )
    {
    SMC_MOCK_METHOD1( void, bool, background )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::isBackground
// -----------------------------------------------------------------------------
//
bool XQAiwRequest::isBackground(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::setInfo
// -----------------------------------------------------------------------------
//
void XQAiwRequest::setInfo( 
        const XQRequestInfo & info )
    {
   // SMC_MOCK_METHOD1( void, const XQRequestInfo &, info )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::info
// -----------------------------------------------------------------------------
//
XQRequestInfo XQAiwRequest::info(  ) const
    {
    //SMC_MOCK_METHOD0( XQRequestInfo )
    }



// -----------------------------------------------------------------------------
// XQAiwRequest::results
// -----------------------------------------------------------------------------
//
const QVariant & XQAiwRequest::results(  ) const
    {
    //SMC_MOCK_METHOD0( const QVariant & )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::sendExecute
// -----------------------------------------------------------------------------
//
bool XQAiwRequest::sendExecute(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQAiwRequest::handleAsyncResponse
// -----------------------------------------------------------------------------
//
void XQAiwRequest::handleAsyncResponse(const QVariant& value)
{
    SMC_MOCK_METHOD1( void, const QVariant &, value )
}


// -----------------------------------------------------------------------------
// XQAiwRequest::sendFromAction
// -----------------------------------------------------------------------------
//
void XQAiwRequest::sendFromAction(bool checked)
{
    SMC_MOCK_METHOD1( void, bool, checked )
}


// -----------------------------------------------------------------------------
// XQAiwRequest::handleAsyncError
// -----------------------------------------------------------------------------
//
void XQAiwRequest::handleAsyncError(int err)
{
    SMC_MOCK_METHOD1( void, int, err )
}

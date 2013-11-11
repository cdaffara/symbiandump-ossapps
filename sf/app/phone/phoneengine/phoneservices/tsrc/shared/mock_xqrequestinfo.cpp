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
#include <QStringList>
#include <QSet>
#include <smcmockclassincludes.h>
#include "xqrequestinfo.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// XQRequestInfo::XQRequestInfo
// -----------------------------------------------------------------------------
//
XQRequestInfo::XQRequestInfo(  )
    {
    
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::~XQRequestInfo
// -----------------------------------------------------------------------------
//
XQRequestInfo::~XQRequestInfo(  )
    {
    
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::isValid
// -----------------------------------------------------------------------------
//
bool XQRequestInfo::isValid(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::setEmbedded
// -----------------------------------------------------------------------------
//
void XQRequestInfo::setEmbedded( 
        bool on )
    {
    SMC_MOCK_METHOD1( void, bool, on )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::isEmbedded
// -----------------------------------------------------------------------------
//
bool XQRequestInfo::isEmbedded(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::setBackground
// -----------------------------------------------------------------------------
//
void XQRequestInfo::setBackground( 
        bool on )
    {
    SMC_MOCK_METHOD1( void, bool, on )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::isBackground
// -----------------------------------------------------------------------------
//
bool XQRequestInfo::isBackground(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::isSynchronous
// -----------------------------------------------------------------------------
//
bool XQRequestInfo::isSynchronous(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::setForeground
// -----------------------------------------------------------------------------
//
void XQRequestInfo::setForeground( 
        bool on )
    {
    SMC_MOCK_METHOD1( void, bool, on )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::isForeground
// -----------------------------------------------------------------------------
//
bool XQRequestInfo::isForeground(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::clientSecureId
// -----------------------------------------------------------------------------
//
quint32 XQRequestInfo::clientSecureId(  ) const
    {
    SMC_MOCK_METHOD0( quint32 )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::clientVendorId
// -----------------------------------------------------------------------------
//
quint32 XQRequestInfo::clientVendorId(  ) const
    {
    SMC_MOCK_METHOD0( quint32 )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::clientCapabilities
// -----------------------------------------------------------------------------
//
QSet <int > XQRequestInfo::clientCapabilities(  ) const
    {
    SMC_MOCK_METHOD0( QSet <int > )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::id
// -----------------------------------------------------------------------------
//
int XQRequestInfo::id(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::setInfo
// -----------------------------------------------------------------------------
//
void XQRequestInfo::setInfo( 
        const QString & key,
        const QVariant & value )
    {
    SMC_MOCK_METHOD2( void, const QString &, key, 
        const QVariant &, value )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::info
// -----------------------------------------------------------------------------
//
QVariant XQRequestInfo::info( 
        const QString & key ) const
    {
    SMC_MOCK_METHOD1( QVariant, const QString &, key )
    }


// -----------------------------------------------------------------------------
// XQRequestInfo::infoKeys
// -----------------------------------------------------------------------------
//
QStringList XQRequestInfo::infoKeys(  ) const
    {
    SMC_MOCK_METHOD0( QStringList )
    }

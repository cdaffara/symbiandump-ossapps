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
#include <smcmockclassincludes.h>
#include "qserviceinterfacedescriptor.h"

QTM_BEGIN_NAMESPACE

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::QServiceInterfaceDescriptor
// -----------------------------------------------------------------------------
//
QServiceInterfaceDescriptor::QServiceInterfaceDescriptor(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::QServiceInterfaceDescriptor
// -----------------------------------------------------------------------------
//
QServiceInterfaceDescriptor::QServiceInterfaceDescriptor( 
        const QServiceInterfaceDescriptor & other )
    {
    
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::~QServiceInterfaceDescriptor
// -----------------------------------------------------------------------------
//
QServiceInterfaceDescriptor::~QServiceInterfaceDescriptor(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::=
// -----------------------------------------------------------------------------
//
QServiceInterfaceDescriptor & QServiceInterfaceDescriptor::operator=(const QServiceInterfaceDescriptor & other )
    {
    SMC_MOCK_METHOD1( QServiceInterfaceDescriptor &, const QServiceInterfaceDescriptor &, other )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::=
// -----------------------------------------------------------------------------
//
bool QServiceInterfaceDescriptor::operator==(const QServiceInterfaceDescriptor& other) const
    {
    SMC_MOCK_METHOD1( bool, const QServiceInterfaceDescriptor &, other )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::serviceName
// -----------------------------------------------------------------------------
//
QString QServiceInterfaceDescriptor::serviceName(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::interfaceName
// -----------------------------------------------------------------------------
//
QString QServiceInterfaceDescriptor::interfaceName(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::majorVersion
// -----------------------------------------------------------------------------
//
int QServiceInterfaceDescriptor::majorVersion(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::minorVersion
// -----------------------------------------------------------------------------
//
int QServiceInterfaceDescriptor::minorVersion(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::isValid
// -----------------------------------------------------------------------------
//
bool QServiceInterfaceDescriptor::isValid(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::scope
// -----------------------------------------------------------------------------
//
QService::Scope QServiceInterfaceDescriptor::scope(  ) const
    {
    SMC_MOCK_METHOD0( QService::Scope )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::attribute
// -----------------------------------------------------------------------------
//
QVariant QServiceInterfaceDescriptor::attribute( 
        QServiceInterfaceDescriptor::Attribute which ) const
    {
    SMC_MOCK_METHOD1( QVariant, QServiceInterfaceDescriptor::Attribute, which )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::customAttribute
// -----------------------------------------------------------------------------
//
QString QServiceInterfaceDescriptor::customAttribute( 
        const QString & which ) const
    {
    SMC_MOCK_METHOD1( QString, const QString &, which )
    }


// -----------------------------------------------------------------------------
// QServiceInterfaceDescriptor::customAttributes
// -----------------------------------------------------------------------------
//
QStringList QServiceInterfaceDescriptor::customAttributes(  ) const
    {
    SMC_MOCK_METHOD0( QStringList )
    }

QTM_END_NAMESPACE

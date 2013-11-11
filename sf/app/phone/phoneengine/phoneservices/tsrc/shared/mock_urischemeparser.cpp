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
#include "urischemeparser.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// UriSchemeParser::UriSchemeParser
// -----------------------------------------------------------------------------
//
UriSchemeParser::UriSchemeParser( 
        QObject * parent )
    :
    QObject(parent)
    {
    
    }


// -----------------------------------------------------------------------------
// UriSchemeParser::~UriSchemeParser
// -----------------------------------------------------------------------------
//
UriSchemeParser::~UriSchemeParser(  )
    {
    
    }


// -----------------------------------------------------------------------------
// UriSchemeParser::parsePhoneDialString
// -----------------------------------------------------------------------------
//
bool UriSchemeParser::parsePhoneDialString(
    const QString &dialString,
    QString &phoneNumber,
    QString &dtmfString) const
{
    SMC_MOCK_METHOD3( bool, 
        const QString &, dialString,
        QString &, phoneNumber,
        QString &, dtmfString)
}

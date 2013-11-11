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
#include "ctiurischemeparser.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CtiUriSchemeParser::CtiUriSchemeParser
// -----------------------------------------------------------------------------
//
CtiUriSchemeParser::CtiUriSchemeParser( 
        QObject * parent )
    :
    UriSchemeParser(parent)
    {
    
    }


// -----------------------------------------------------------------------------
// CtiUriSchemeParser::~CtiUriSchemeParser
// -----------------------------------------------------------------------------
//
CtiUriSchemeParser::~CtiUriSchemeParser(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CtiUriSchemeParser::isSupportedUriScheme
// -----------------------------------------------------------------------------
//
bool CtiUriSchemeParser::isSupportedUriScheme( 
        const QString & uri ) const
    {
    SMC_MOCK_METHOD1( bool, const QString &, uri )
    }


// -----------------------------------------------------------------------------
// CtiUriSchemeParser::parseUri
// -----------------------------------------------------------------------------
//
bool CtiUriSchemeParser::parseUri( 
        const QString & uri,
        QString &phoneNumber,
        QString &dtmfString ) const
    {
    SMC_MOCK_METHOD3( bool, 
        const QString &, uri,
        QString &, phoneNumber,
        QString &, dtmfString)
    }

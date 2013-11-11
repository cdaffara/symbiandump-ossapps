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
#include "telurischemeparser.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TelUriSchemeParser::TelUriSchemeParser
// -----------------------------------------------------------------------------
//
TelUriSchemeParser::TelUriSchemeParser( 
        QObject * parent )
    :
    UriSchemeParser(parent)
    {
    
    }


// -----------------------------------------------------------------------------
// TelUriSchemeParser::~TelUriSchemeParser
// -----------------------------------------------------------------------------
//
TelUriSchemeParser::~TelUriSchemeParser(  )
    {
    
    }


// -----------------------------------------------------------------------------
// TelUriSchemeParser::isSupportedUriScheme
// -----------------------------------------------------------------------------
//
bool TelUriSchemeParser::isSupportedUriScheme( 
        const QString & uri ) const
    {
    SMC_MOCK_METHOD1( bool, const QString &, uri )
    }


// -----------------------------------------------------------------------------
// TelUriSchemeParser::parseUri
// -----------------------------------------------------------------------------
//
bool TelUriSchemeParser::parseUri( 
        const QString & uri,
        QString &phoneNumber,
        QString &dtmfString ) const
    {
    SMC_MOCK_METHOD3( bool, 
        const QString &, uri,
        QString &, phoneNumber,
        QString &, dtmfString)
    }

/*!
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
* Description: Base class for URI Scheme parsers.
*
*/

#include <QRegExp>
#include <pevirtualengine.h>
#include "urischemeparser.h"
#include "qtphonelog.h"

const QString KDtmfPauseChar = "p";
const QString KDtmfWaitChar = "w";

/*!
  UriSchemeParser::UriSchemeParser.
 */
UriSchemeParser::UriSchemeParser(QObject* parent)
    : 
    QObject(parent)
{
    PHONE_DEBUG("UriSchemeParser::UriSchemeParser");
}


/*!
  UriSchemeParser::~UriSchemeParser.
 */
UriSchemeParser::~UriSchemeParser()
{
    PHONE_DEBUG("UriSchemeParser::~UriSchemeParser");
}


/*!
  UriSchemeParser::parsePhoneDialString.
 */
bool UriSchemeParser::parsePhoneDialString(
    const QString &dialString,
    QString &phoneNumber,
    QString &dtmfString) const
{
    PHONE_DEBUG("UriSchemeParser::parsePhoneDialString");
    
    bool parsingResult = false;
    
    int dialStringLength = dialString.length();
    if ((dialStringLength <= 0) || 
        (KPEPhoneNumberMaxLength < dialStringLength)) {
        parsingResult = false;
        phoneNumber.clear();
        dtmfString.clear();
    } else {
        parsingResult = true;
        QRegExp dtmfFindExp(
            QString("[") + KDtmfPauseChar + KDtmfWaitChar + QString("]"));
        int dtmfPostfixIndex = dialString.indexOf(dtmfFindExp);
        phoneNumber = dialString.left(dtmfPostfixIndex);
        if (0 <= dtmfPostfixIndex) {
            dtmfString = dialString.mid(dtmfPostfixIndex);
        }
    }
    
    return parsingResult;
}

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
* Description: Provides services to parse CTI URIs.
*
*/

#include <QRegExp>
#include "ctiurischemeparser.h"
#include "qtphonelog.h"

const QString KCtiUriSchemePrefix = "cti:";
const QString KVisualSeparators("-.() ");
const QString KCtiPauseChar = ",";
const QString KCtiWaitChar = "/";
const QString KDtmfPauseChar = "p";
const QString KDtmfWaitChar = "w";

/*!
  CtiUriSchemeParser::CtiUriSchemeParser.
 */
CtiUriSchemeParser::CtiUriSchemeParser(QObject* parent)
    : 
    UriSchemeParser(parent)
{
    PHONE_DEBUG("CtiUriSchemeParser::CtiUriSchemeParser");
}


/*!
  CtiUriSchemeParser::~CtiUriSchemeParser.
 */
CtiUriSchemeParser::~CtiUriSchemeParser()
{
    PHONE_DEBUG("CtiUriSchemeParser::~CtiUriSchemeParser");
}


/*!
  CtiUriSchemeParser::isSupportedUriScheme.
 */
bool CtiUriSchemeParser::isSupportedUriScheme(const QString &uri) const
{
    PHONE_DEBUG("CtiUriSchemeParser::isSupportedUriScheme");
    
    QRegExp expression(QString("^") + KCtiUriSchemePrefix);
    return uri.contains(expression);
}


/*!
  CtiUriSchemeParser::parseUri.
 */
bool CtiUriSchemeParser::parseUri(
    const QString &uri,
    QString &phoneNumber,
    QString &dtmfString) const
{
    PHONE_DEBUG("CtiUriSchemeParser::parseUri");
    
    QString dialString = "";
    
    if (validateUri(uri)) {
        dialString = uri.mid(KCtiUriSchemePrefix.length());
        
        removeVisualSeparators(dialString);
        convertToneInputToPostDialDtmfString(dialString);
    }
    
    return UriSchemeParser::parsePhoneDialString(
        dialString, phoneNumber, dtmfString);
}


/*!
  CtiUriSchemeParser::validateUri.
  Validity rules:
  1. Uri must start with "cti:"
  2. + -sign may occur only at the beginning of the URI.
  3. Uri cannot contain tone input only or start with tone input.
  4. Uri length must be at least 1. 
  5. Uri must contain only valid characters.
  5.1. Phone number part cannot contain tone input characters.
  6. Uri cannot contain multiple wait characters in succession.
 */
bool CtiUriSchemeParser::validateUri(const QString &uri) const
{
    PHONE_DEBUG("CtiUriSchemeParser::validateUri");
    
    bool isValid = false;
    
    QRegExp expression(
        "^cti:\\+?[\\d-\\.\\(\\) ]{1,}([,/][\\d*#,/-\\.\\(\\) ]*)?$");
    isValid = uri.contains(expression);
    if (isValid) {
        QRegExp validateMultipleWaitExp("//+");
        isValid = !uri.contains(validateMultipleWaitExp);
    }
    
    return isValid;
}


/*!
  CtiUriSchemeParser::removeVisualSeparators.
 */
void CtiUriSchemeParser::removeVisualSeparators(QString &uri) const
{
    PHONE_DEBUG("CtiUriSchemeParser::removeVisualSeparators");
    
    QRegExp visualSeparatorExp("[" + QRegExp::escape(KVisualSeparators) + "]");
    uri.remove(visualSeparatorExp);
}


/*!
  CtiUriSchemeParser::convertToneInputToPostDialDtmfString.
 */
void CtiUriSchemeParser::convertToneInputToPostDialDtmfString(
    QString &uri) const
{
    PHONE_DEBUG("CtiUriSchemeParser::convertToneInputToPostDialDtmfString");
    
    uri.replace(KCtiWaitChar, KDtmfWaitChar);
    uri.replace(KCtiPauseChar, KDtmfPauseChar);
}

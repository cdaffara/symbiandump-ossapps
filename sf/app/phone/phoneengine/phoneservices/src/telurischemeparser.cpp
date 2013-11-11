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
* Description: TEL URI parser.
*
*/

#include <QRegExp>
#include <QStringList>
#include "telurischemeparser.h"
#include "qtphonelog.h"

const QString KTelUriSchemaPrefix = "tel:";
const QString KPostDialTag = "postd=";
const QString KSharpDigit = "%23"; 
const QChar KSplit = ';';
const QChar KSharpDigitHex = '#';
const QChar KPauseDigit = 'p';

const QString KWhitespacesAndVisuals = "[\\s,.\\[\\]\\(\\)\\-]";
const QString KInvalidTelNum = "[^\\d\\+]";
const QString KInvalidPostDial = "[^\\d*#pw]";
const QString KNotJustWaitsAndPauses = "[^pw]";

/*!
  TelUriSchemeParser::TelUriSchemeParser.
 */
TelUriSchemeParser::TelUriSchemeParser(QObject* parent) 
    : 
    UriSchemeParser(parent)
{
    PHONE_DEBUG("TelUriSchemeParser::TelUriSchemeParser");
}


/*!
  TelUriSchemeParser::~TelUriSchemeParser.
 */
TelUriSchemeParser::~TelUriSchemeParser()
{
    PHONE_DEBUG("TelUriSchemeParser::~TelUriSchemeParser");
}


/*!
  TelUriSchemeParser::isSupportedUriScheme.
 */
bool TelUriSchemeParser::isSupportedUriScheme(const QString &uri) const
{
    PHONE_DEBUG("TelUriSchemeParser::isSupportedUriScheme");
    
    QRegExp expression(QString("^") + KTelUriSchemaPrefix);
    return uri.contains(expression);
}


/*!
  TelUriSchemeParser::parseUri.
  Accepted Uri's:
  1. Uri must start with "tel:1*phonedigit"
  2. Only "tel:1*phonedigit" and 
     ";postd=1*(phonedigit / dtmf-digit / pause-character)" tags are recognized
  3. DTMF digits "A" "B" "C" and "D" are not supported
  4. ";postd=" can't contain only pause-characters "p" "w" 
  5. ";postd=" can contain only '*''#''%23''0123456789''p''w' characters
  6. ";postd=" should start 'p' character, if not it's inserted
 */
bool TelUriSchemeParser::parseUri(
    const QString &uri,
    QString &phoneNumber,
    QString &dtmfString) const
{
    PHONE_DEBUG("TelUriSchemeParser::parseUri");
    QString dialString = "";
    QString postNum;
    
    if (uri.startsWith(KTelUriSchemaPrefix,Qt::CaseInsensitive)) {
        dialString = uri;
        dialString.remove(QRegExp(KWhitespacesAndVisuals));
        QStringList list = dialString.split(KSplit);
        dialString = list.at(0).mid(KTelUriSchemaPrefix.length());
        if (dialString.contains(QRegExp(KInvalidTelNum))) {
            dialString.clear();
        } else{
            for(int i = 1; i < list.count(); i++){
                if(list.at(i).startsWith(KPostDialTag,Qt::CaseInsensitive)){
                    postNum = list.at(i).mid(KPostDialTag.length());
                    //found, check sharp digit
                    int ndx = postNum.indexOf(KSharpDigit);
                    while(-1 != ndx){
                        postNum.replace(ndx, KSharpDigit.length(),KSharpDigitHex);
                        ndx = postNum.indexOf(KSharpDigit);
                    }
                    //now check if it's valid
                    if(postNum.contains(QRegExp(KInvalidPostDial))||
                        !postNum.contains(QRegExp(KNotJustWaitsAndPauses))){
                        postNum.clear();
                    }else{
                        //finally check it's starting with 'p'
                        if(KPauseDigit != postNum.at(0)){
                            postNum.insert(0,KPauseDigit);
                        }
                        dialString += postNum;
                    }
                }
            }
        }
    }
    
    PHONE_DEBUG2("TelUriSchemeParser::parseUri: ", dialString);
    return UriSchemeParser::parsePhoneDialString(
        dialString, phoneNumber, dtmfString);
}

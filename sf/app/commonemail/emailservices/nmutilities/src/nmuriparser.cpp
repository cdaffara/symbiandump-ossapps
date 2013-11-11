/*
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
* Description: A utility class for parsing URI strings
*
*/
#include "nmuriparser.h"
#include "emailtrace.h"

#include <QUrl>


/*!
    Keys for parsing data.
*/
static const QString NmUriParserEmailSendSubjectKey = "subject";
static const QString NmUriParserEmailEmailSendToKey = "to";
static const QString NmUriParserEmailSendCcKey = "cc";
static const QString NmUriParserEmailSendBccKey = "bcc";
static const QString NmUriParserEmailSendBodyTextKey = "body";

/*!
    Class constructor.
*/
NmUriParser::NmUriParser()
{
    NM_FUNCTION;
}

/*!
    Class destructor.
*/
NmUriParser::~NmUriParser()
{
    NM_FUNCTION;
}

/*!
    Extracts the data from the given QString into the class members.
    \param data QString containing the data.
    \return True if success, false otherwise.
*/
bool NmUriParser::extractData(const QString &data)
{
    NM_FUNCTION;
    
    bool success(false);
    
    QUrl uri(data);
    
    if (uri.isValid()) {
        
        mSubject = uri.queryItemValue(NmUriParserEmailSendSubjectKey);
        QString to = uri.path();
        QString cc = uri.queryItemValue(NmUriParserEmailSendCcKey);
        QString bcc = uri.queryItemValue(NmUriParserEmailSendBccKey);
        QString bodyText = uri.queryItemValue(NmUriParserEmailSendBodyTextKey);
        if (bodyText.length()) {
            mBodyText = bodyText;
        }
        
        addAddressesToList(to, mToAddresses);
        addAddressesToList(cc, mCcAddresses);
        addAddressesToList(bcc, mBccAddresses);
        
        success = true;
    }
    
    return success;
}

/*!
    Appends the given addresses into the given list.
    \param address The addresses to append.
    \param list The list where the addresses are appended to.
*/
void NmUriParser::addAddressesToList(QString &addresses, QStringList &list)
{
    NM_FUNCTION;
    
    if (!addresses.isEmpty()) {

        QStringList foundAddresses;
                
        // Process multiple addresses.
        if (addresses.contains(",")) {
            QString str;               
            while (addresses.contains(",")) {
                str = addresses.section(",", 0, 0); // Get the occurance.
                addresses.remove(0, (addresses.indexOf(",")+1)); // Remove the occurance.
                if (!str.isEmpty()) { // In case str would be empty on some error data. 
                    foundAddresses.append(str);    
                }
            }
        }
        if (!addresses.isEmpty()) { // In case addresses would be empty on some error data. 
            // Last one or single address.
            foundAddresses.append(addresses);
        }
        // Append the found addresses into the given list.
        list.append(foundAddresses);
    }
}
    
/*!
    Resets the data.
*/
void NmUriParser::resetData()
{
    NM_FUNCTION;
    
    mSubject.clear();
    mToAddresses.clear();
    mCcAddresses.clear();
    mBccAddresses.clear();
    mBodyText.clear();
}

/*!
    Getter for subject
    \return QString subject of parsed uri
*/
QString NmUriParser::subject() const
{
    return mSubject;
}
/*!
    Getter for to-addresses
    \return QStringList list of parsed to-addresses
*/
QStringList NmUriParser::toAddresses() const
{
    return mToAddresses;
}
/*!
    Getter for ccAddresses
    \return QStringList list of parsed cc-addresses
*/
QStringList NmUriParser::ccAddresses() const
{
    return mCcAddresses;
}
/*!
    Getter for bcc-Addresses
    \return QStringList list of parsed bcc-addresses
*/
QStringList NmUriParser::bccAddresses() const
{
    return mBccAddresses;
}
/*!
    Getter for bcc-Addresses
    \return QString parsed body text
*/
QString NmUriParser::bodyText() const
{
    return mBodyText;
}

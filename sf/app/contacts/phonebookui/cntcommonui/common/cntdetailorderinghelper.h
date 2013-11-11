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

#ifndef CNTPHONENUMBERHELPER_H
#define CNTPHONENUMBERHELPER_H

#include <qmobilityglobal.h>
#include <qtcontacts.h>

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntDetailOrderingHelper
{
    
    enum CntPhoneNumberPosition
    { 
        EMobile = 0,
        EMobileHome,
        EMobileWork,
        EPhone,
        EPhoneHome,
        EPhoneWork,
        EFax,
        EFaxHome,
        EFaxWork,
        EPager,
        EAssistant,
        ECar
    };
    
    enum CntOnlineAccountPosition
    {
        EInternetTelephone = 0,
        EInternetTelephoneHome,
        EInternetTelephoneWork,
        ESip
    };
    
    enum CntEmailAccountsPosition
    {
        EEmail = 0,
        EEmailHome,
        EEmailWork
    };
    
    enum CntUrlPosition
    {
        EUrl = 0,
        EUrlHome,
        EUrlWork
    };
    
    enum CntAddressPosition
    {
        EAddress = 0,
        EAddressHome,
        EAddressWork
    };
    
public:
    static QList<QContactPhoneNumber> getOrderedSupportedPhoneNumbers( const QContact& contact );
    static QList<QContactOnlineAccount> getOrderedSupportedOnlineAccounts( const QContact& contact );
    static QList<QContactEmailAddress> getOrderedEmailAccounts( const QContact& contact );
    static QList<QContactUrl> getOrderedUrls( const QContact& contact );
    static QList<QContactAddress> getOrderedAddresses( const QContact& contact );
};

#endif /* CNTPHONENUMBERHELPER_H */

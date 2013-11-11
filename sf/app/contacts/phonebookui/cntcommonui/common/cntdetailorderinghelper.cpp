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

#include "cntdetailorderinghelper.h"

/**
* Return ordered list of supported (by UI implementation) phone numbers
*
* @param QContact&, contact
* @return QList<QContactPhoneNumber>, ordered list of supported phone numbers
*/    
QList<QContactPhoneNumber> CntDetailOrderingHelper::getOrderedSupportedPhoneNumbers( const QContact& contact )
{
    QMap<QPair<QString, QString>, int> orderMap;
    
    QPair<QString, QString> pair = qMakePair(QContactPhoneNumber::SubTypeMobile.operator QString(), QString(""));
    orderMap.insert(pair , EMobile);
    pair = qMakePair(QContactPhoneNumber::SubTypeMobile.operator QString(), QContactPhoneNumber::ContextHome.operator QString());
    orderMap.insert(pair , EMobileHome);
    pair = qMakePair(QContactPhoneNumber::SubTypeMobile.operator QString(), QContactPhoneNumber::ContextWork.operator QString());
    orderMap.insert(pair , EMobileWork);
    
    pair = qMakePair(QContactPhoneNumber::SubTypeLandline.operator QString(), QString(""));
    orderMap.insert(pair , EPhone);
    pair = qMakePair(QContactPhoneNumber::SubTypeLandline.operator QString(), QContactPhoneNumber::ContextHome.operator QString());
    orderMap.insert(pair , EPhoneHome);
    pair = qMakePair(QContactPhoneNumber::SubTypeLandline.operator QString(), QContactPhoneNumber::ContextWork.operator QString());
    orderMap.insert(pair , EPhoneWork);
    
    pair = qMakePair(QContactPhoneNumber::SubTypeFax.operator QString(), QString(""));
    orderMap.insert(pair , EFax);
    pair = qMakePair(QContactPhoneNumber::SubTypeFax.operator QString(), QContactPhoneNumber::ContextHome.operator QString());
    orderMap.insert(pair , EFaxHome);
    pair = qMakePair(QContactPhoneNumber::SubTypeFax.operator QString(), QContactPhoneNumber::ContextWork.operator QString());
    orderMap.insert(pair , EFaxWork);
    
    pair = qMakePair(QContactPhoneNumber::SubTypePager.operator QString(), QString(""));
    orderMap.insert(pair , EPager);
    pair = qMakePair(QContactPhoneNumber::SubTypeAssistant.operator QString(), QString(""));
    orderMap.insert(pair , EAssistant);
    pair = qMakePair(QContactPhoneNumber::SubTypeCar.operator QString(), QString(""));
    orderMap.insert(pair , ECar);
    
    QList<QContactPhoneNumber> completeList = contact.details<QContactPhoneNumber>();
    QList<QContactPhoneNumber> orderedSupportedList;
    
    foreach ( QContactPhoneNumber number, completeList )
    {
        QString context = number.contexts().isEmpty() ? QString() : number.contexts().first();
        QString subtype = number.subTypes().isEmpty() ? number.definitionName() : number.subTypes().first();
        
        QPair<QString, QString> pair = qMakePair(subtype, context);
        
        if ( orderMap.keys().contains(pair) )
        {
            int position = orderMap.value(pair);
            bool added = false;
            
            for (int i = 0; i < orderedSupportedList.count(); i++)
            {
                QString currentContext = orderedSupportedList.at(i).contexts().isEmpty() ? 
                        QString() : orderedSupportedList.at(i).contexts().first();
                
                // this is safe because no details without a subtype can be in the list
                QString currentSubtype = orderedSupportedList.at(i).subTypes().first();
                
                QPair<QString, QString> currentPair = qMakePair(currentSubtype, currentContext);
                
                int currentPosition = orderMap.value(currentPair);
                
                if (currentPosition > position)
                {
                    orderedSupportedList.insert(i, number);
                    added = true;
                    break;
                }
            }
            
            if (!added)
            {
                orderedSupportedList.append(number);
            }
        }
    }
    
    return orderedSupportedList;
}

/**
* Return ordered list of supported (by UI implementation) online accounts
*
* @param QContact&, contact
* @return QList<QContactOnlineAccount>, ordered list of supported online accounts
*/    
QList<QContactOnlineAccount> CntDetailOrderingHelper::getOrderedSupportedOnlineAccounts( const QContact &contact )
{
    QMap<QPair<QString, QString>, int> orderMap;
    
    QPair<QString, QString> pair = qMakePair(QContactOnlineAccount::SubTypeSipVoip.operator QString(), QString(""));
    orderMap.insert(pair , EInternetTelephone);
    pair = qMakePair(QContactOnlineAccount::SubTypeSipVoip.operator QString(), QContactOnlineAccount::ContextHome.operator QString());
    orderMap.insert(pair , EInternetTelephoneHome);
    pair = qMakePair(QContactOnlineAccount::SubTypeSipVoip.operator QString(), QContactOnlineAccount::ContextWork.operator QString());
    orderMap.insert(pair , EInternetTelephoneWork);
    
    pair = qMakePair(QContactOnlineAccount::SubTypeSip.operator QString(), QString(""));
    orderMap.insert(pair , ESip);
    
    QList<QContactOnlineAccount> completeList = contact.details<QContactOnlineAccount>();
    QList<QContactOnlineAccount> orderedSupportedList;
    
    foreach ( QContactOnlineAccount account, completeList )
    {
        QString context = account.contexts().isEmpty() ? QString() : account.contexts().first();
        QString subtype = account.subTypes().isEmpty() ? account.definitionName() : account.subTypes().first();
        
        QPair<QString, QString> pair = qMakePair(subtype, context);
        
        if ( orderMap.keys().contains(pair) )
        {
            int position = orderMap.value(pair);
            bool added = false;
            
            for (int i = 0; i < orderedSupportedList.count(); i++)
            {
                QString currentContext = orderedSupportedList.at(i).contexts().isEmpty() ? 
                        QString() : orderedSupportedList.at(i).contexts().first();
                
                // this is safe because no details without a subtype can be in the list
                QString currentSubtype = orderedSupportedList.at(i).subTypes().first();
                
                QPair<QString, QString> currentPair = qMakePair(currentSubtype, currentContext);
                
                int currentPosition = orderMap.value(currentPair);
                
                if (currentPosition > position)
                {
                    orderedSupportedList.insert(i, account);
                    added = true;
                    break;
                }
            }
            
            if (!added)
            {
                orderedSupportedList.append(account);
            }
        }
    }
    
    return orderedSupportedList;
}

/**
* Return ordered list of email accounts
*
* @param QContact&, contact
* @return QList<QContactEmailAddress>, ordered list of email accounts
*/    
QList<QContactEmailAddress> CntDetailOrderingHelper::getOrderedEmailAccounts( const QContact &contact )
{
    QMap<QString, int> orderMap;
    
    orderMap.insert("" , EEmail);
    orderMap.insert(QContactEmailAddress::ContextHome , EEmailHome);
    orderMap.insert(QContactEmailAddress::ContextWork , EEmailWork);
    
    QList<QContactEmailAddress> completeList = contact.details<QContactEmailAddress>();
    QList<QContactEmailAddress> orderedSupportedList;
    
    foreach ( QContactEmailAddress email, completeList )
    {
        QString context = email.contexts().isEmpty() ? QString() : email.contexts().first();
        
        if ( orderMap.keys().contains(context) )
        {
            int position = orderMap.value(context);
            bool added = false;
            
            for (int i = 0; i < orderedSupportedList.count(); i++)
            {
                QString currentContext = orderedSupportedList.at(i).contexts().isEmpty() ? 
                        QString() : orderedSupportedList.at(i).contexts().first();
                
                int currentPosition = orderMap.value(currentContext);
                
                if (currentPosition > position)
                {
                    orderedSupportedList.insert(i, email);
                    added = true;
                    break;
                }
            }
            
            if (!added)
            {
                orderedSupportedList.append(email);
            }
        }
    }
    
    return orderedSupportedList;
}

/**
* Return ordered list of url details
*
* @param QContact&, contact
* @return QList<QContactUrl>, ordered list of url details
*/   
QList<QContactUrl> CntDetailOrderingHelper::getOrderedUrls( const QContact &contact )
{
    QMap<QString, int> orderMap;
    
    orderMap.insert("" , EUrl);
    orderMap.insert(QContactUrl::ContextHome , EUrlHome);
    orderMap.insert(QContactUrl::ContextWork , EUrlWork);
    
    QList<QContactUrl> completeList = contact.details<QContactUrl>();
    QList<QContactUrl> orderedSupportedList;
    
    foreach ( QContactUrl url, completeList )
    {
        QString context = url.contexts().isEmpty() ? QString() : url.contexts().first();
        
        if ( orderMap.keys().contains(context) )
        {
            int position = orderMap.value(context);
            bool added = false;
            
            for (int i = 0; i < orderedSupportedList.count(); i++)
            {
                QString currentContext = orderedSupportedList.at(i).contexts().isEmpty() ? 
                        QString() : orderedSupportedList.at(i).contexts().first();
                
                int currentPosition = orderMap.value(currentContext);
                
                if (currentPosition > position)
                {
                    orderedSupportedList.insert(i, url);
                    added = true;
                    break;
                }
            }
            
            if (!added)
            {
                orderedSupportedList.append(url);
            }
        }
    }
    
    return orderedSupportedList;
}

/**
* Return ordered list of address details
*
* @param QContact&, contact
* @return QList<QContactAddress>, ordered list of address details
*/   
QList<QContactAddress> CntDetailOrderingHelper::getOrderedAddresses( const QContact& contact )
{
    QMap<QString, int> orderMap;
    
    orderMap.insert("" , EAddress);
    orderMap.insert(QContactAddress::ContextHome , EAddressHome);
    orderMap.insert(QContactAddress::ContextWork , EAddressWork);
    
    QList<QContactAddress> completeList = contact.details<QContactAddress>();
    QList<QContactAddress> orderedSupportedList;
    
    foreach ( QContactAddress address, completeList )
    {
        QString context = address.contexts().isEmpty() ? QString() : address.contexts().first();
        
        if ( orderMap.keys().contains(context) )
        {
            int position = orderMap.value(context);
            bool added = false;
            
            for (int i = 0; i < orderedSupportedList.count(); i++)
            {
                QString currentContext = orderedSupportedList.at(i).contexts().isEmpty() ? 
                        QString() : orderedSupportedList.at(i).contexts().first();
                
                int currentPosition = orderMap.value(currentContext);
                
                if (currentPosition > position)
                {
                    orderedSupportedList.insert(i, address);
                    added = true;
                    break;
                }
            }
            
            if (!added)
            {
                orderedSupportedList.append(address);
            }
        }
    }
    
    return orderedSupportedList;
}

// EOF

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
#ifndef QCONTACTFIELDS_H_
#define QCONTACTFIELDS_H_


//-------------- Fields supported by database ----------------------------------// 
//Fields
const QString fieldFirstName("FIRSTNAME"); //QVariant( QString )
const QString fieldLastName("LASTNAME");
const QString fieldNickName("NICKNAME");
const QString fieldNumber("NUMBER"); 
const QString fieldAddress("ADDRESS"); 
const QString fieldEmail("EMAIL"); 
const QString fieldSip("SIP"); 
const QString fieldPortrait("PORTRAIT"); 
//....

//Field properties
const QString typeMobile("MOBILE"); //NUMBER 
const QString typeFax("FAX"); //NUMBER 
const QString typeHome("HOME"); //NUMBER & EMAIL & ADDRESS
const QString typeWork("WORK"); //NUMBER & EMAIL & ADDRESS
//....

//Field parameters
const QString parameterStreet("STREET"); //ADDRESS
const QString parameterPostCode("POSTCODE"); //ADDRESS


//-------------- Field Collections supported by the database --------------------//
//All fields used for call (e.g. number and sip)
const QString callFields("CALL");

//All fields used for sms
const QString smsFields("SMS");

//All fields used for email (home, business etc.)
const QString emailFields("EMAIL");

#endif // QCONTACTFIELDS_H_


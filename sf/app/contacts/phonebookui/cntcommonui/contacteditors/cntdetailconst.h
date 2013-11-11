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

#ifndef CNTDETAILCONST_H_
#define CNTDETAILCONST_H_

#include <Qt>
#include <QDate>
#include <QString>
const int CNT_NOTE_EDITOR_MIN_ROWCOUNT = 5;
const int CNT_PHONENUMBER_EDITOR_MAXLENGTH = 48;
const int CNT_DTMF_EDITOR_MAXLENGTH = 60;
const int CNT_URL_EDITOR_MAXLENGTH = 1000;
const int CNT_NAMES_EDITOR_MAXLENGTH = 50;
const int CNT_PREFIXSUFFIX_EDITOR_MAXLENGTH = 10;
const int CNT_EMAIL_EDITOR_MAXLENGTH = 150;
const int CNT_NOTE_EDITOR_MAXLENGTH = 1500;
const int CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH = 100;

const int CNT_FIRSTNAME_MAXLENGTH = 50;
const int CNT_LASTNAME_MAXLENGTH = 50;
const int CNT_MIDDLENAME_MAXLENGTH = 50;
const int CNT_NICKNAME_MAXLENGTH = 50;
const int CNT_PREFIX_MAXLENGTH = 10;
const int CNT_SUFFIX_MAXLENGTH = 10;

const int CNT_STREET_MAXLENGTH = 250;
const int CNT_POSTCODE_MAXLENGTH = 20;
const int CNT_POBOX_MAXLENGTH = 20;
const int CNT_LOCALITY_MAXLENGTH = 100;
const int CNT_REGION_MAXLENGTH = 100;
const int CNT_COUNTRY_MAXLENGTH = 150;

const int CNT_ORGANIZATION_MAXLENGTH = 150;
const int CNT_JOBTITLE_MAXLENGTH = 100;
const int CNT_DEPARTMENT_MAXLENGTH = 100;
const int CNT_ASSISTANT_MAXLENGTH = 200;

const int CNT_SPOUSE_MAXLENGTH = 50;
const int CNT_CHILDREN_MAXLENGTH = 250;

const int CNT_GROUPNAME_MAXLENGTH = 50;

const QDate CNT_DATEPICKER_FROM(1900,0,0);
const QDate CNT_DATEPICKER_TO(2050,0,0);

const QString CNT_PREFILLED_URL = "http://";

enum DetailEditorDataRole {
    DetailPhoneNumber = Qt::UserRole + 1,
    DetailOnlineAccount,
    DetailStreetAddress,
    DetailSubType,
    DetailContext,
    DetailMaxLength
};
#endif /* CNTDETAILCONST_H_ */

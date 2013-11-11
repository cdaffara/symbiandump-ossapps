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
* Description: Default info provider plugin for CntListModel. It can provide
*              the phone number and the image url of a contact (text and
*              icon1 field respectively).
*
*/

#ifndef CNTDEFAULTPROVIDER_H
#define CNTDEFAULTPROVIDER_H

#include <cntinfoprovider.h>
#include <qcontact.h>

QTM_USE_NAMESPACE

/*
   The default info provider plugin. It can provide the phone number and the
   image url of a contact (text and icon1 field respectively).
 */
class CntDefaultInfoProvider : public CntInfoProvider
{
    friend class TestCntDefaultInfoProvider;
    Q_OBJECT
    
public:
    QString id() const { return "default"; };
    ContactInfoFields supportedFields() const;
    void requestInfo(const QContact& contact, ContactInfoFields requestedInfo);
    
signals:
    void infoFieldReady(CntInfoProvider* sender, int contactId, ContactInfoField field, const QString& value);
};

#endif

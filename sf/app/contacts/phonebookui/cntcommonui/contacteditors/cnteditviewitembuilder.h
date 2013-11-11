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

#ifndef CNTEDITVIEWITEMBUILDER_H_
#define CNTEDITVIEWITEMBUILDER_H_

#include <QObject>
#include <qmobilityglobal.h>
QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntEditViewDelegateItem;
class CntStringMapper;
class CntEditViewItem;

class CntEditViewItemBuilder : public QObject
{
public:
    CntEditViewItemBuilder();
    ~CntEditViewItemBuilder();
    
public:
    QList<CntEditViewItem*> phoneNumberItems( QContact& aContact ); // includes online account
    QList<CntEditViewItem*> emailAddressItems(QContact& aContact);
    QList<CntEditViewItem*> addressItems(QContact& aContact);
    QList<CntEditViewItem*> urlItems(QContact& aContact);
    QList<CntEditViewItem*> addressDetails(QContact& aContact);
    QList<CntEditViewItem*> companyDetails(QContact& aContact);
    QList<CntEditViewItem*> dateDetails(QContact& aContact);
    QList<CntEditViewItem*> noteDetails(QContact& aContact);
    QList<CntEditViewItem*> familyDetails(QContact& aContact);
    QList<CntEditViewItem*> ringtoneDetails(QContact& aContact);
    //QList<CntEditViewItem*> synchronizationDetails();
    bool isTemplateItem( const QString& aDefinition );
    void removeDetail( QContact& aContact, QContactDetail& aDefinition, QStringList aFields );
private:
    CntStringMapper* mMap;
};
#endif /* CNTEDITVIEWITEMBUILDER_H_ */

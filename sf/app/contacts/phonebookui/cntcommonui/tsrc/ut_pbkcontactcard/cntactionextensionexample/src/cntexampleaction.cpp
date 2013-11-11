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
* Description:
*
*/

#include "cntexampleaction.h"
#include <qcontactonlineaccount.h>
#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>
#include <cntuiactionextension.h>
#include <QIcon>
#include <hbicon.h>

//Action class
MobExampleAction::MobExampleAction(const QString& actionName, const QString& vendorName) : 
    MobBaseAction(actionName, vendorName)
{
}

MobExampleAction::~MobExampleAction()
{
}

QVariantMap MobExampleAction::metaData() const
{
    QVariantMap variantMap;
    const QString& string("Test action 1");
    QVariant localization(string);
    variantMap.insert(KCntUiActionMetaTitleText, localization);
    const QString& string2(QContactOnlineAccount::FieldAccountUri);
    const QVariant detailField(string2);
    variantMap.insert(KCntUiActionMetaValueTextDetail, detailField);
    QString menuString("Do example action");
    QVariant longPressMenu(menuString);
    variantMap.insert(KCntUiActionMetaValueTextLongPress, longPressMenu);
    HbIcon hIcon("qtg_large_voip.svg");
    // Must be QIcon
    QIcon icon(hIcon.qicon());
    QVariant iconVariant(icon);
    variantMap.insert("icon", iconVariant);
    return variantMap;
}

QContactFilter MobExampleAction::contactFilter(const QVariant& value) const
{
    Q_UNUSED(value);
    
    QContactDetailFilter onlineFilter;
    onlineFilter.setDetailDefinitionName(QContactOnlineAccount::DefinitionName, QContactOnlineAccount::FieldSubTypes);
    onlineFilter.setValue(QLatin1String(QContactOnlineAccount::SubTypeImpp));

    return onlineFilter;
}

bool MobExampleAction::isDetailSupported(const QContactDetail &detail, const QContact &contact) const
{
    if(QContactOnlineAccount::DefinitionName == detail.definitionName())
    {
        return true;
    }
    else
    {
        return false;
    }
}

QList<QContactDetail> MobExampleAction::supportedDetails(const QContact& contact) const
{
    
}

QContactAction::State MobExampleAction::state() const
{
    
}

MobExampleAction* MobExampleAction::clone() const
{
     return new MobExampleAction(m_actionName, m_vendorName);
}

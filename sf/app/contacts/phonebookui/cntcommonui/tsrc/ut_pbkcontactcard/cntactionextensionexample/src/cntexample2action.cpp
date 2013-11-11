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

#include "cntexample2action.h"
#include <qcontactonlineaccount.h>
#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>
#include <cntuiactionextension.h>

//Action class
MobExample2Action::MobExample2Action(const QString& actionName, const QString& vendorName) : 
    MobBaseAction(actionName, vendorName)
{
}

MobExample2Action::~MobExample2Action()
{
}

QVariantMap MobExample2Action::metaData() const
{
    QVariantMap variantMap;
    QVariant localization("Test action 2");
    variantMap.insert(KCntUiActionMetaTitleText, localization);
    const QString& string(QContactPhoneNumber::FieldNumber);
    QVariant detailField(string);
    variantMap.insert(KCntUiActionMetaValueTextDetail, detailField);
    QString menuString("Do example action2");
    QVariant longPressMenu(menuString);
    variantMap.insert(KCntUiActionMetaValueTextLongPress, longPressMenu);
    return variantMap;
}

QContactFilter MobExample2Action::contactFilter(const QVariant& value) const
{
    Q_UNUSED(value);
    
    QContactDetailFilter mobileFilter;
    mobileFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    mobileFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));

    return mobileFilter;
}

bool MobExample2Action::isDetailSupported(const QContactDetail &detail, const QContact &contact) const
{
    if(QContactPhoneNumber::DefinitionName == detail.definitionName())
    {
        return true;
    }
    else
    {
        return false;
    }
}

QList<QContactDetail> MobExample2Action::supportedDetails(const QContact& contact) const
{
    
}

QContactAction::State MobExample2Action::state() const
{

}

MobExample2Action* MobExample2Action::clone() const
{
     return new MobExample2Action(m_actionName, m_vendorName);
}

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

#include "cntexampledynamicaction.h"
#include <qcontactonlineaccount.h>
#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>
#include <cntuiactionextension.h>

//Action class
MobExampleDynamicAction::MobExampleDynamicAction(const QString& actionName, const QString& vendorName) : 
    MobBaseAction(actionName, vendorName)
{
}

MobExampleDynamicAction::MobExampleDynamicAction(const QString& actionName, const QString& vendorName, const MobExampleDynamicAction& copy) : 
    MobBaseAction(actionName, vendorName)
{
    mTitleField = copy.mTitleField;
    mValueField = copy.mValueField;
    mTitleFieldDetail = copy.mTitleFieldDetail;
    mValueFieldDetail = copy.mValueFieldDetail;
    mIcon = copy.mIcon;
    mFilter = copy.mFilter;
    mDefinitionName = copy.mDefinitionName;
}

MobExampleDynamicAction::~MobExampleDynamicAction()
{
}

QVariantMap MobExampleDynamicAction::metaData() const
{
    QVariantMap variantMap;
    
    variantMap.insert(KCntUiActionMetaTitleTextDetail, !mTitleField.isEmpty() ? mTitleField : mTitleFieldDetail);
    
    if(!mValueFieldDetail.isEmpty())
        variantMap.insert(KCntUiActionMetaValueTextDetail, mValueFieldDetail);
    else if(!mValueField.isEmpty())
        variantMap.insert(KCntUiActionMetaValueText, mValueField);
    // Must be QIcon
    QIcon icon(mIcon);
    QVariant iconVariant(icon);
    variantMap.insert("icon", iconVariant);
    return variantMap;
}

QContactFilter MobExampleDynamicAction::contactFilter(const QVariant& value) const
{
    return mFilter;
}

bool MobExampleDynamicAction::isDetailSupported(const QContactDetail &detail, const QContact &contact) const
{
    if(mDefinitionName == detail.definitionName())
    {
        return true;
    }
    else
    {
        return false;
    }
}

QList<QContactDetail> MobExampleDynamicAction::supportedDetails(const QContact& contact) const
{
    
}

QContactAction::State MobExampleDynamicAction::state() const
{
    
}

MobExampleDynamicAction* MobExampleDynamicAction::clone() const
{
     return new MobExampleDynamicAction(m_actionName, m_vendorName, *this);
}

void MobExampleDynamicAction::setDefinitionName(const QString& definitionName)
{
    mDefinitionName = definitionName;
}

void MobExampleDynamicAction::setFilter(QContactFilter filter)
{
    mFilter = filter;
}

void MobExampleDynamicAction::setIcon(QIcon icon)
{
    mIcon = icon;
}

void MobExampleDynamicAction::setTitleField(QString titleField)
{
    mTitleField = titleField;
}

void MobExampleDynamicAction::setValueField(QString valueField)
{
    mValueField = valueField;
}

void MobExampleDynamicAction::setTitleFieldDetail(QString titleFieldDetail)
{
    mTitleFieldDetail = titleFieldDetail;
}

void MobExampleDynamicAction::setValueFieldDetail(QString valueFieldDetail)
{
    mValueFieldDetail = valueFieldDetail;
}

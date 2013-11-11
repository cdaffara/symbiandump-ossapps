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

#include "cntexamplenodetailaction.h"
#include <cntuiactionextension.h>

//Action class
MobExampleNoDetailAction::MobExampleNoDetailAction(const QString& actionName, const QString& vendorName) : 
    MobBaseAction(actionName, vendorName)
{
}

MobExampleNoDetailAction::~MobExampleNoDetailAction()
{
}

QVariantMap MobExampleNoDetailAction::metaData() const
{
    QVariantMap variantMap;
    QVariant localization("Test action 3");
    variantMap.insert(KCntUiActionMetaTitleText, localization);
    QVariant detailField("Description");
    variantMap.insert(KCntUiActionMetaValueText, detailField);
    QString menuString("Do action 3");
    QVariant longPressMenu(menuString);
    variantMap.insert(KCntUiActionMetaValueTextLongPress, longPressMenu);
    return variantMap;
}

QContactFilter MobExampleNoDetailAction::contactFilter(const QVariant& value) const
{
    Q_UNUSED(value);
    
    QContactFilter all;

    return all;
}

// If detail is empty plugin knows that this is special action query that
// is targetted to whole contact not to specific detail on contact.
// If you want to create action to my card only, check cntuiactionextension.h for details
bool MobExampleNoDetailAction::isDetailSupported(const QContactDetail &detail, const QContact &contact) const
{
    if(detail.isEmpty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

QList<QContactDetail> MobExampleNoDetailAction::supportedDetails(const QContact& contact) const
{
    
}

QContactAction::State MobExampleNoDetailAction::state() const
{

}

MobExampleNoDetailAction* MobExampleNoDetailAction::clone() const
{
    return new MobExampleNoDetailAction(m_actionName, m_vendorName);
}

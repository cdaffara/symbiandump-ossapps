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

#include "cntbrowseraction.h"

#include <qcontacturl.h>
#include <qcontactfilters.h>
#include <QDesktopServices>

//Action class
CntBrowserAction::CntBrowserAction() : 
    CntAction("url")
{
}

CntBrowserAction::~CntBrowserAction()
{
}

QContactFilter CntBrowserAction::contactFilter(const QVariant& value) const
{
     Q_UNUSED(value);
     
    QContactDetailFilter urlFilter;
    urlFilter.setDetailDefinitionName(QContactUrl::DefinitionName); 
    
    return urlFilter;
}

bool CntBrowserAction::isDetailSupported(const QContactDetail &detail, const QContact &/*contact*/) const
{
    return (detail.definitionName() == QContactUrl::DefinitionName);
}

QList<QContactDetail> CntBrowserAction::supportedDetails(const QContact& contact) const
{
    return contact.details(QContactUrl::DefinitionName);
}

CntBrowserAction* CntBrowserAction::clone() const
{
     return new CntBrowserAction();
}

void CntBrowserAction::performAction()
{
    if (m_detail.definitionName() == QContactUrl::DefinitionName)
    {
        const QContactUrl &contactUrl = static_cast<const QContactUrl &>(m_detail);
        
        QString address = contactUrl.url();
        if (!address.startsWith("http://"))
        {
            address.prepend("http://");
        }
        
        bool success = QDesktopServices::openUrl(QUrl(address));
        QVariant retValue;
        emitResult(success, retValue);
    }
    else
    {
        QVariant retValue;
        emitResult(GeneralError, retValue);
    }
}

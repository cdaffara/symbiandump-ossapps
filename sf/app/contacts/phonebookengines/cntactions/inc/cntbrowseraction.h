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

#ifndef MOBCNTBROWSERACTION_H
#define MOBCNTBROWSERACTION_H

#include "cntaction.h"

class CntBrowserAction : public CntAction
{
    Q_OBJECT

public:
    CntBrowserAction();
    ~CntBrowserAction();
    
    QContactFilter contactFilter(const QVariant& value) const;
    bool isDetailSupported(const QContactDetail &detail, const QContact &contact = QContact()) const;
    QList<QContactDetail> supportedDetails(const QContact& contact) const;
    CntBrowserAction* clone() const;
    void performAction();
};

#endif // MOBCNTBROWSERACTION_H

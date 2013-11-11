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

#include "cntactionpopup.h"
#include "cntactionpopup_p.h"

CntActionPopup::CntActionPopup( QContact* aContact ) :
d_ptr( new CntActionPopupPrivate(aContact) )
{
    Q_D(CntActionPopup);
    d->q_ptr = this;
    connect(d_ptr, SIGNAL(executeContactAction(QContact&, QContactDetail, QString)), this, SIGNAL(executeContactAction(QContact&, QContactDetail, QString)));
    connect(d_ptr, SIGNAL(actionPopupCancelPressed()), this, SIGNAL(actionPopupCancelPressed()));
}

/*!
 Destructor
 */
CntActionPopup::~CntActionPopup()
{
    Q_D(CntActionPopup);
    delete d;
}

bool CntActionPopup::showActionPopup(QString action)
{
    Q_D(CntActionPopup);
    return d->showActionPopup( action );
}

// end of file

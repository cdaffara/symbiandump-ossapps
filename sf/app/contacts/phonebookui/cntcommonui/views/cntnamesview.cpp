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

#include "cntnamesview.h"
#include "cntnamesview_p.h"
#include <hbview.h>

CntNamesView::CntNamesView() : d_ptr( new CntNamesViewPrivate() )
{
    Q_D(CntNamesView);
    d->q_ptr = this;
}

/*!
 Destructor
 */
CntNamesView::~CntNamesView()
{
    Q_D(CntNamesView);
    delete d;
}

void CntNamesView::activate(const CntViewParameters aArgs)
{
    Q_D(CntNamesView);
    d->activate( aArgs );
}

void CntNamesView::deactivate()
{
    Q_D(CntNamesView);
    d->deactivate();
}

HbView* CntNamesView::view() const
{
    Q_D(const CntNamesView);
    return d->mView;
}

bool CntNamesView::isDefault() const 
{ 
    Q_D(const CntNamesView);
    return true; 
}
int CntNamesView::viewId() const 
{
    Q_D(const CntNamesView);
    return namesView;
}

void CntNamesView::setEngine( CntAbstractEngine& aEngine )
{
    Q_D(CntNamesView);
    d->setEngine( aEngine );
}
   

// end of file

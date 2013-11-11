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

#include "cntgroupmemberview.h"
#include "cntgroupmemberview_p.h"
#include "cntabstractviewmanager.h"

/*!
\class CntGroupMemberView
\brief

This is the group members view class that shows list of contacts for a user group. View contains a listview that shows actual contacts that
have been added to a particular group. There is also toolbar and menu for navigating between different views. Instance of this class is
created by our viewmanager but view itself is owned by the mainwindow which will also delete it in the end.

*/

/*!
Constructor, initialize member variables.
\a viewManager is the parent that creates this view. \a parent is a pointer to parent QGraphicsItem (by default this is 0)

*/
CntGroupMemberView::CntGroupMemberView() : d_ptr(new CntGroupMemberViewPrivate())
{
    Q_D( CntGroupMemberView );
    d->q_ptr = this;
}

CntGroupMemberView::~CntGroupMemberView()
{
    Q_D(CntGroupMemberView);
    delete d;
}
    
void CntGroupMemberView::activate( const CntViewParameters aArgs )
{
    Q_D(CntGroupMemberView);
    d->activate(aArgs);
}

void CntGroupMemberView::deactivate()
{
    Q_D(CntGroupMemberView);
    d->deactivate();
}

bool CntGroupMemberView::isDefault() const
{
    return false;
}

HbView* CntGroupMemberView::view() const
{
    Q_D( const CntGroupMemberView );
    return d->mView;
}

int CntGroupMemberView::viewId() const
{
    return groupMemberView;
} 

void CntGroupMemberView::setEngine( CntAbstractEngine& aEngine )
{
    Q_D( CntGroupMemberView );
    d->mEngine = &aEngine;
}
// EOF


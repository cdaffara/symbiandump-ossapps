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
 * Description: Example implementation of contact card extension.
 *
 */

#include "contactcardextension.h"
#include <HbMessageBox>

ContactCardExtension::ContactCardExtension()
{
}

ContactCardExtension::~ContactCardExtension()
{
    delete mDetail;
}

void ContactCardExtension::prepare( QContact& /*aContact*/, bool /*aMyCard*/ )
{
// You should examine the contact here and decide what actions are to be offered.

    mDetail = new QContactDetail();
}

int ContactCardExtension::actionCount() const
{
    return 1;
}

const CntUiContactCardExtAction& ContactCardExtension::actionAt( int /*aIndex*/ ) const
{
    return *this;
}


void ContactCardExtension::handleShortPress( int /*aActionId*/ )
    {
    HbMessageBox::information("Jumping");
    }

//====================================================
//======= from CntUiContactCardExtAction =============


QString ContactCardExtension::firstTextLine() const
    {
    return QString("Jump");
    }

QString ContactCardExtension::secondTextLine() const
    {
    return QString("over the fence");
    }

const QIcon ContactCardExtension::icon() const 
    {
    return QIcon("c:/jump_icon.jpg");
    }

const QContactDetail& ContactCardExtension::detailUsedForChoosingPosition() const
    {
    // This is not a proper example.
    return *mDetail;
    }


// end of file

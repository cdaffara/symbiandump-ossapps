/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hblineedit.h>
#include "dialpad.h"

Dialpad::Dialpad()
{
    mLineEdit = new HbLineEdit();
    setVisible(false);
    mIsCallButtonEnabled = false;
    mIsOpen = false;
}

Dialpad::Dialpad(const HbMainWindow& mainWindow)
{
    Q_UNUSED(mainWindow);
    mLineEdit = new HbLineEdit();
    setVisible(false);
    mIsCallButtonEnabled = false;
    mIsOpen = false;    
}

Dialpad::~Dialpad()
{
    delete mLineEdit;
}

HbLineEdit& Dialpad::editor() const
{
    return *mLineEdit;
}

void Dialpad::setCallButtonEnabled(bool enabled)
{
    mIsCallButtonEnabled = enabled;
}


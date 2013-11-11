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
* Description:  Test title resolver.
*
*/
#include <HbMainWindow>
#include <HbView>

#include "t_hsutils.h"
#include "hsspinnerdialog.h"

#ifndef ONLY_MENU_TESTCASES

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testSpinnerDialog()
{
    HbMainWindow mainWindow;
    
    HsSpinnerDialog *dialog = new HsSpinnerDialog();
    QVERIFY(!dialog->isVisible());
    
    dialog->start();    
    QVERIFY(dialog->isVisible());
    dialog->stop();    
    // dialog deletes itself
}

#endif // ONLY_MENU_TESTCASES

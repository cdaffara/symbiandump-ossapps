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

#ifndef ONLY_MENU_TESTCASES

#include "t_hsutils.h"
#include "hsmessageboxwrapper.h"


void  t_hsUtils::testMessageBoxWrapper()
{
    HsMessageBoxWrapper msgBox;
    msgBox.setHeader("Why?");
    msgBox.setQueryText("Do You really want to do it?");

    emit msgBox.accepted();
    emit msgBox.rejected();
}

#endif // ONLY_MENU_TESTCASES

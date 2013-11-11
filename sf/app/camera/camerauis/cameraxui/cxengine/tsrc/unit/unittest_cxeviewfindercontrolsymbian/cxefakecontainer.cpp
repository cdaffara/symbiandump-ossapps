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

#include "cxutils.h"
#include "cxefakecontainer.h"

/**
* CxeFakeContainer::create()
*/
CxeFakeContainer* CxeFakeContainer::create()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeFakeContainer* me = new CxeFakeContainer();
    me->init();

    CX_DEBUG_EXIT_FUNCTION();
    return me;
}

/**
* CxeFakeContainer::~CxeFakeContainer()
*/
CxeFakeContainer::~CxeFakeContainer()
{
    CX_DEBUG_IN_FUNCTION();
}

/**
* CxeFakeContainer::init()
*/
void CxeFakeContainer::init()
{
    QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION(
        {
        CreateWindowL();
        SetSize(TSize(640,360));
        ActivateL();        
        });
}

/**
* CxeFakeContainer::CxeFakeContainer()
*/
CxeFakeContainer::CxeFakeContainer()
    : CCoeControl()
{
    CX_DEBUG_IN_FUNCTION();
}

// end of file

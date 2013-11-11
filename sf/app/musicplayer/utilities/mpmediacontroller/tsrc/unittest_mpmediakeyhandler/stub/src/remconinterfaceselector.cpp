/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: remconinterfaceselector stub for testing MpMediaKeyHandler
*
*/


#include <e32debug.h>

#include "stub/inc/remconinterfaceselector.h"


int gCRemConInterfaceSelectorCount = 0;
bool gCRemConInterfaceSelectorNewLLeave = false;
bool gCRemConInterfaceSelectorOpenTargetLLeave = false;


//static functions start

int CRemConInterfaceSelector::getCount()
{
    return gCRemConInterfaceSelectorCount;
}

void CRemConInterfaceSelector::setNewLLeave()
{
    gCRemConInterfaceSelectorNewLLeave = true;
}

void CRemConInterfaceSelector::NewLLeaveIfDesiredL()
{
    if (gCRemConInterfaceSelectorNewLLeave)
    {
        RDebug::Print(_L("CRemConInterfaceSelector::NewLLeaveIfDesiredL Leave"));
        gCRemConInterfaceSelectorNewLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void CRemConInterfaceSelector::setOpenTargetLLeave()
{
    gCRemConInterfaceSelectorOpenTargetLLeave = true;
}

void CRemConInterfaceSelector::OpenTargetLLeaveIfDesiredL()
{
    if (gCRemConInterfaceSelectorOpenTargetLLeave)
    {
        RDebug::Print(_L("CRemConInterfaceSelector::NewLLeaveIfDesiredL Leave"));
        gCRemConInterfaceSelectorOpenTargetLLeave = false;
        User::Leave(KErrGeneral);
    }
}

//static functions end

CRemConInterfaceSelector::CRemConInterfaceSelector()
{
    gCRemConInterfaceSelectorCount++;
}

CRemConInterfaceSelector::~CRemConInterfaceSelector()
{
    gCRemConInterfaceSelectorCount--;
}

CRemConInterfaceSelector* CRemConInterfaceSelector::NewL()
{
    RDebug::Print(_L("stub CRemConInterfaceSelector::NewL"));
    CRemConInterfaceSelector::NewLLeaveIfDesiredL();
    CRemConInterfaceSelector* self = new(ELeave) CRemConInterfaceSelector();
    return self;
}

void CRemConInterfaceSelector::OpenTargetL()
{
    CRemConInterfaceSelector::OpenTargetLLeaveIfDesiredL();
}

//end of file

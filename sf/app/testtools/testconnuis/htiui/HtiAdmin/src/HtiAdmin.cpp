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
* Description:  Implementation of HtiAdmin main.
*
*/


#include "HtiAdminApplication.h"
#include <eikstart.h>


LOCAL_C CApaApplication* NewApplication()
    {
    //HTI_LOG_TEXT("NewApplication");
    return new CHtiAdminApplication;
    }

GLDEF_C TInt E32Main()
    {
    //HTI_LOG_TEXT("E32Main");
    return EikStart::RunApplication(NewApplication);
    }

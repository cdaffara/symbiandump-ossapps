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

#ifndef MEMSPYAPP_H
#define MEMSPYAPP_H

// System includes
#include <aknapp.h>

// Constants
const TUid KUidMemSpy = { 0x2002129F };


class CMemSpyApp : public CAknApplication
    {
private: // From CApaApplication
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
    };



#endif

/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateResult class 
*
*/



#ifndef IA_UPDATE_RESULT_H
#define IA_UPDATE_RESULT_H

#include <e32base.h>

#define private public
#include "videoiadupdatewrapper.h"
#undef private

/**
 * CIAUpdateResult gives results of the completed update operation.
 *
 * @see CIAUpdate
 * @see MIAUpdateObserver
 *
 * @since S60 v3.2
 */
class CIAUpdateResult
    {
public:

    CIAUpdateResult(){};

    TInt SuccessCount() const{return 1;};
    };

#endif // IA_UPDATE_RESULT_H


/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Stub implementation of the HAL class for testing purposes
*
*/

#include "hal.h"

TInt HAL::Get(TAttribute /*aAttribute*/, TInt& aValue)
{
    aValue = 1000000;
    return KErrNone;
}

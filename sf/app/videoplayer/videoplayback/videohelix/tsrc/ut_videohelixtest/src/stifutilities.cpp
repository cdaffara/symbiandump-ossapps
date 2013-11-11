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
* Description:  mpx utilities class stub for STIF testing
*
*/

// Version : %version: 2 %

#include "stifutilities.h"

void StifUtilities::CreateStifUtility()
{
    StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

    utility = new (ELeave) StifUtilities;

    Dll::SetTls( utility );
}

// EOF

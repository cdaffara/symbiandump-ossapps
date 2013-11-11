/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data types for call control
*
*/



// -----------------------------------------------------------------------------
// TVtCtlCSSessionInfoV1::TVtCtlSessionInfoV1
// c++ default constructor
// -----------------------------------------------------------------------------
//
inline TVtCtlCSSessionInfoV1::TVtCtlCSSessionInfoV1() 
    : iDuration( 0 ),
      iState( MVtCtlCallControl::EUnknown ),
      iPreviousState( MVtCtlCallControl::EUnknown ),
      iDirection( MVtCtlCallControl::EDirectionUnknown )
    {
    iExtension = EVtCtlCSSessionInfoExtensionId;
    }

// End of File

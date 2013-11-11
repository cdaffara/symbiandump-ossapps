/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Panic codes for MsgMedia
*
*/



#ifndef __MSGMEDIAPANIC_H__
#define __MSGMEDIAPANIC_H__

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

enum TMsgMediaPanic
    {
    EMsgMediaAlreadyActive = 0,
    EMsgMediaNullPointer,
    EMsgMediaUnhandledLinkFile
    };

// ========== FUNCTION PROTOTYPES ==========================

// ---------------------------------------------------------
// Panic
//
// ---------------------------------------------------------
//
GLREF_C void MsgMediaPanic ( TMsgMediaPanic aPanic );

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

#endif   // __MSGMEDIAPANIC_H__

// End of File

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
* Description:  This header file contains macro definitions used by the database
*                subsystem.
*
*/

#ifndef MPXDBCOMMONDEF_H
#define MPXDBCOMMONDEF_H

#include <e32def.h>
#include <e32cmn.h>

// Literals to speed up mcdb by not having to create TPtr's at runtime */
_LIT(KMCSingleQuote, "'");
_LIT(KMCBackSlash, "\\");
_LIT(KMCPercentage, "%");
_LIT(KMCUnderscore, "_");
_LIT(KMCCommaSign, ",");
_LIT(KMCEqualSign, " = ");
_LIT(KMCOpenBracket, "(");
_LIT(KMCCloseBracket, ")");
_LIT(KMCAndKeyword," AND ");
_LIT(KMCOrKeyword, " OR ");

// QT localization TS file
_LIT(KMPXMusicPlayerTsFile, "musicplayer_");
_LIT(KMPXMusicPlayerTsPath, "z:/resource/qt/translations/");

#endif // MPXDBCOMMONDEF_H

//End of File

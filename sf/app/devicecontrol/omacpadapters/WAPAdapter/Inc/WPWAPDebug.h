/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     WPWAPDebug contains debug macros.
*
*/


#ifndef WPWAPDEBUG_H 
#define WPWAPDEBUG_H 

// MACROS

// Define this to enable debug output
//#define WAPDEBUG

// DEBUG can be used for outputting debug
#ifdef WAPDEBUG
#define DEBUG(TEXT) Debug( _L(#TEXT) );
#define DVA(TEXT, ARG1) Debug( _L(#TEXT), (ARG1) );
#define DVA2(TEXT, ARG1, ARG2) Debug( _L(#TEXT), (ARG1), (ARG2) );
#define DVA3(TEXT, ARG1, ARG2, ARG3) Debug( _L(#TEXT), (ARG1), (ARG2), (ARG3) );
#else
#define DEBUG(TEXT)
#define DVA(TEXT, ARG1)
#define DVA2(TEXT, ARG1, ARG2)
#define DVA3(TEXT, ARG1, ARG2, ARG3)
#endif

#if defined(__WINS__) || defined(__EPOC32__)
GLDEF_C void Debug( TRefByValue<const TDesC> aText, ...);
#endif

#endif /* WPWAPDEBUG_H */

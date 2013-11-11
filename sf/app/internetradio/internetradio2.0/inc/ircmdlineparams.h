/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Command line parameter interface for Internet Radio.
*
*/


#ifndef IRCMDLINEPARAMS_H
#define IRCMDLINEPARAMS_H

#include <e32def.h>

/**
 * Command line option for activating certain view at startup.
 * For internal use.
 */
_LIT8( KIRCmdLineOptionView, "-view" );

/**
 * Command line argument for now playing view.
 * For internal use.
 */
_LIT8( KIRCmdLineArgumentNowPlaying, "nowplaying" );

/**
 * Full command line parameter for activating now playing view at startup.
 * External applications should use directly this literal.
 */
_LIT8( KIRCmdLineParamViewNowPlaying, "-view nowplaying" );

#endif //  IRCMDLINEPARAMS_H

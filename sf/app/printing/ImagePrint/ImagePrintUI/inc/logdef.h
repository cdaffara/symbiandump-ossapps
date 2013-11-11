/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LOGDEF_H
#define LOGDEF_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
// logging is always enabled in debug builds
#define ENABLE_LOGGING
#else
// manually enable in release builds
//#define ENABLE_LOGGING
#endif // _DEBUG

//#define LOG_TO_FILE

#endif // LOGDEF_H

//  End of File

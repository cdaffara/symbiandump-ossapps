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
* Description:  The dll header file.
*
*/


#ifndef CLOCKWIDGETDEFINES_H
#define CLOCKWIDGETDEFINES_H

#include <qglobal.h>

#ifdef CLOCKWIDGET_LIB
#   define CLOCKWIDGET_EXPORT Q_DECL_EXPORT
#else
#   define CLOCKWIDGET_EXPORT Q_DECL_IMPORT
#endif

#endif // CLOCKWIDGETDEFINES_H

// End of file  --Don't remove this.

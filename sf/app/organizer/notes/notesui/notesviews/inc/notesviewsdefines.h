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
 * Description: The notes main view common defition document.
 *
 */

#ifndef NOTESVIEWSDEFINES_H
#define NOTESVIEWSDEFINES_H

// System includes
#include <qglobal.h>

#ifdef NOTESVIEWS_LIB
#   define NOTESVIEWS_EXPORT Q_DECL_EXPORT
#else
#   define NOTESVIEWS_EXPORT Q_DECL_IMPORT
#endif

const QString notes = "NotesAllList"; // Activity Id to be stored in Activity Manager DB
#endif // NOTESVIEWSDEFINES_H

// End of file  --Don't remove this.

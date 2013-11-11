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
 * Description: This is the common definition file for the notes model.
 *
 */

#ifndef NOTESMODELDEFINES_H
#define NOTESMODELDEFINES_H

// System includes
#include <qglobal.h>

#ifdef NOTESMODEL_LIB
#	define NOTESMODEL_EXPORT Q_DECL_EXPORT
#else
#	define NOTESMODEL_EXPORT Q_DECL_IMPORT
#endif

#endif // NOTESMODELDEFINES_H

// End of file	--Don't remove this.

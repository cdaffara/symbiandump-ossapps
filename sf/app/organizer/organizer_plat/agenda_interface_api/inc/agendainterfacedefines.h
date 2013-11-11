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
 * Description: AgendaUtil library definition file.
 *
 */

#ifndef AGENDAINTERFACEDEFINES_H
#define AGENDAINTERFACEDEFINES_H

#include <QtCore/qglobal.h>

#if defined(XQCALENDAR_LIBRARY)
#  define XQCALENDAR_EXPORT Q_DECL_EXPORT
#else
#  if defined(XQCALENDAR_NO_LIBRARY)
#    define XQCALENDAR_EXPORT
#  else
#    define XQCALENDAR_EXPORT Q_DECL_IMPORT
#  endif
#endif

#endif // AGENDAINTERFACEDEFINES_H

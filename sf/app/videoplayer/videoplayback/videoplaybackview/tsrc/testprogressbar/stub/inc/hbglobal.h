/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  class for HbGlobal
*
*/

// Version : %version:   1 %



#ifndef HBGLOBAL_H
#define HBGLOBAL_H

#include <QtGlobal>

#ifdef BUILD_HB_CORE
#    define HB_CORE_EXPORT Q_DECL_EXPORT
#    define HB_CORE_PRIVATE_EXPORT Q_DECL_EXPORT
#else
#    define HB_CORE_EXPORT Q_DECL_IMPORT
#    define HB_CORE_PRIVATE_EXPORT Q_DECL_IMPORT
#endif // BUILD_HB_CORE

#ifdef BUILD_HB_WIDGETS
#    define HB_WIDGETS_EXPORT Q_DECL_EXPORT
#    define HB_WIDGETS_PRIVATE_EXPORT Q_DECL_EXPORT
#else
#    define HB_WIDGETS_EXPORT Q_DECL_IMPORT
#    define HB_WIDGETS_PRIVATE_EXPORT Q_DECL_IMPORT
#endif // BUILD_HB_WIDGETS


QString hbTrId( QString string, int n = -1 );


#endif // HBGLOBAL_H

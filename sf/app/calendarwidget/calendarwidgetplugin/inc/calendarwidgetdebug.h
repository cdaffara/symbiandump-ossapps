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
* Description: Home screen calendar widget
*
*/

#ifndef CALENDARWIDGETDEBUG_H_
#define CALENDARWIDGETDEBUG_H_

// System includes
#include <QDebug>

// MACROS
// Writing data into log
// LOGS("My log string");

#ifndef _DEBUG
    // No logging wanted --> define empty macros

    #define LOGS(A)

#else // qDebug logging
    #define LOGS(A) { qDebug() << A; }
#endif

#endif /* CALENDARWIDGETDEBUG_H_ */

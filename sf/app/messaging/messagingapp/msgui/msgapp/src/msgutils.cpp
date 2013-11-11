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
 * Description:Utility class having static helper methods.
 *
 */

#include "msgutils.h"
#include <QDateTime>
#include <QString>

//---------------------------------------------------------------
// MsgUtils::dateTimeToString
// gives string representation of dateTime.
//---------------------------------------------------------------
QString MsgUtils::dateTimeToString(const QDateTime& dateTime,
    const QString& format)
{
    return dateTime.toString(format);
}

// EOF

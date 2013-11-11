/*!
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
* Description:  
*/

#include "hbindicatorstubhelper.h"
#include <qglobal.h>
#include <hbindicator.h>

bool testIndicatorActive = false;

void HbIndicatorStubHelper::reset()
{
    testIndicatorActive = false;
}

bool HbIndicatorStubHelper::indicatorActive()
{
    return testIndicatorActive;
}

bool HbIndicator::activate(const QString &indicatorType, const QVariant &parameter)
{
    Q_UNUSED(indicatorType)
    Q_UNUSED(parameter)
    testIndicatorActive = true;
    return true;
}

bool HbIndicator::deactivate(const QString &indicatorType, const QVariant &parameter)
{
    Q_UNUSED(indicatorType)
    Q_UNUSED(parameter)
    testIndicatorActive = false;
    return true;
}

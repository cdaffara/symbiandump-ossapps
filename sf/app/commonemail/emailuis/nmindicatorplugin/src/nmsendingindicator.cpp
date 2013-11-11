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
 * Description:
 *
 */
#include "nmsendingindicator.h"
#include <QTimer>
#include <nmcommon.h>

const int NmSendingStateDelay = 2000; // delay for 'send in progress' indicator

/*!
    \class NmSendingIndicator
    \brief Handles sending progress indicator
*/

/*!
     Class constructor.
*/
NmSendingIndicator::NmSendingIndicator(const QString &indicatorType)
:HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::ProgressCategory,
        HbIndicatorInterface::NoInteraction)
{
}

/*!
    Class destructor.
*/
NmSendingIndicator::~NmSendingIndicator()
{
}

/*!
    \fn QVariant HbIndicatorInterface::indicatorData(int role) const = 0

    No texts or icons show by this indicator
*/
QVariant NmSendingIndicator::indicatorData(int role) const
{
    if (role==DecorationNameRole) {
		// Must return non-empty to make the indicator visible
		return " ";
	}

    return QVariant();
}

/*!
    Timer callback for hiding 'send in progress' indicator
*/
void NmSendingIndicator::hideSendIndicator()
{
    NM_FUNCTION;
    emit deactivate();
}

/*!
    Handles the requests sent from the client
    \a type is a type of the request.
    \a parameter is extra parameter from the client. Can be invalid, if not given.

    Should return true, when the request is handled.
    Default implementation returns false.

    \sa RequestType, HbIndicator
*/
bool NmSendingIndicator::handleClientRequest(RequestType type,
        const QVariant &parameter)
{
    NM_FUNCTION;
    Q_UNUSED(parameter);

    switch (type) {
        case RequestActivate:
            {
            // Hide the progress state after some delay
            QTimer::singleShot(NmSendingStateDelay, this, SLOT(hideSendIndicator()));
            }
            break;
        default:
            break;
    }
    return false;
}

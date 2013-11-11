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
* Description:  FM Radio home screen widget
*
*/

// User includes
#include "radiohswidget.h"
#include "radiologger.h"

// This is defined in t_radiohswidget.cpp
extern int EMPTY_REPLY;

/*!
    \class RadioHsWidget
    \brief Stub implementation of FM Radio home screen widget for unit
    testing.

    RadioHsWidget implements needed functions for the FM Radio home screen
    widget unit tests.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constrcutor.
 */
RadioHsWidget::RadioHsWidget()
{
    LOG_METHOD;
}

/*!
    Destructor.
 */
RadioHsWidget::~RadioHsWidget()
{
    LOG_METHOD;
}

/*!
    Handles changes in FM Radio information.

    \param informationType Type of changed information.
    \param information Actual information.
 */
void RadioHsWidget::handleRadioInformationChange(const int informationType,
        const QVariant &information)
{
    LOG_METHOD;
    if (informationType != EMPTY_REPLY) {
        ReceivedInfo info;
        info.informationType = informationType;
        info.information = information;
        mReceivedInformation.append(info);
    }
}

/*!
    Handles changes in FM Radio state.

    \param value New state of the radio application.
*/
void RadioHsWidget::handleRadioStateChange(const QVariant &value)
{
    LOG_METHOD;
    Q_UNUSED(value);
}

/*!
    Returns the last received information and removes it from the list.
*/
ReceivedInfo RadioHsWidget::receivedInformation()
{
    LOG_METHOD;
    ReceivedInfo ret;
    if (!mReceivedInformation.isEmpty()) {
        ret = mReceivedInformation.takeLast();
    }
    return ret;
}
/*!
    Returns the count of received information.
*/
int RadioHsWidget::receivedInformationCount()
{
    return mReceivedInformation.count();
}

/*!
    Clears the received info list.
*/
void RadioHsWidget::reset()
{
    mReceivedInformation.clear();
}

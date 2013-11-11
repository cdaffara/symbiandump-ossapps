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
* Description:  FM Radio home screen widget unit tests.
*
*/

#ifndef T_RADIOHSWIDGETSTUB_H
#define T_RADIOHSWIDGETSTUB_H

// INCLUDES
#include <QObject>
#include <QVariant>
#include <QList>

/*!
    \struct ReceivedInfo
    \brief Helper to store received information on unit tests.
*/
struct ReceivedInfo {
    /*! Stores the type of received information. */
    int informationType;
    /*! Stores the actual data of received information. */
    QVariant information;
};

/*!
    \namespace FmRadio
    \brief Wraps enumerators for radio state and service request visibility. 
 */
namespace FmRadio
{
    /*! Enum for radio application states. */
    enum State
    {
        StateUndefined,
        StateNotRunning,
        StateStarting,
        StateRunning,
        StateSeeking,
        StateAntennaNotConnected,
        StateClosing
    };
    
    /*! Enum for controlling the visibility of the radio application. */
    enum VisibiltyAfterRequest
    {
        VisibiltyDoNotChange,
        VisibiltyToForeground,
        VisibiltyToBackground
    };

}

class RadioHsWidget : public QObject
{
    Q_OBJECT

public:

    RadioHsWidget();
    ~RadioHsWidget();

    void handleRadioInformationChange(const int informationType,
        const QVariant &information);

    void handleRadioStateChange(const QVariant &value);
    
    ReceivedInfo receivedInformation();
    int receivedInformationCount();

    void reset();

private: // data
    
    /*! Stores received information. Correctness of
        RadioHsWidgetRadioServiceClient mediated information is checked from here.
    */
    QList<ReceivedInfo> mReceivedInformation;
    
};

#endif // T_RADIOHSWIDGETSTUB_H

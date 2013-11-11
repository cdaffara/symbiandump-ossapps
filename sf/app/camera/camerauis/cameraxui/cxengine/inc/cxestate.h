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
* Description:
*
*/

#ifndef CXESTATE_H
#define CXESTATE_H

#include <QString>

/**
* CxeState represents one unique state in a finite state machine.
* @sa CxeStateMachine
*/
class CxeState
{
public:
    /**
     * Constructor.
     *
     * @param stateId            Integer ID for this state. Note that this must be unique
     *                           and a power of two. This is needed so we can combine
     *                           state IDs using bitwise or.
     * @param stateName          Name for this state (used for debug prints)
     * @param allowedNextStates  Bitmask of allowed next states.
     */
    CxeState(int stateId, const char* stateName, int allowedNextStates);

    virtual ~CxeState();

    /**
     * Get the state ID of the state.
     * @return State ID
     */
    int stateId() const;

    /**
     * Get the allowed state transitions
     * @return A bitmask of state IDs which are considered valid next states after this state.
     */
    int allowedNextStates() const;

    /**
     * Get the name of this state
     * @return Name of this state.
     */
    QString name() const;

private:
    //! State name (for debug prints)
    QString mName;

    //! ID of this state
    int mId;

    //! Bitmask of allowed states
    int mAllowedNextStates;
};

#endif // CXESTATE_H

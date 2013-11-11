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
 * Description:  Declaration of the SEARCH init state.
 *
 */

#ifndef SEARCH_STATES_INITSTATE_H
#define SEARCH_STATES_INITSTATE_H

#include <qstate.h>
#include "search_global.h"

SEARCH_CLASS( SearchStateProviderTest)
class SearchInitState : public QState
    {
Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Owner.
     */
    SearchInitState(QState *aParent = 0);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    virtual ~SearchInitState();

private:

    /**
     * Constructs this state.
     * @since S60 ?S60_version.
     */
    void construct();

protected:

    /**
     * @copydoc QState::onEntry()
     */
    void onEntry(QEvent *event);

    /**
     * @copydoc QState::onExit()
     */
    void onExit(QEvent *event);

private:
SEARCH_FRIEND_CLASS    ( SearchStateProviderTest)

    };

#endif //SEARCH_STATES_INITSTATE_H

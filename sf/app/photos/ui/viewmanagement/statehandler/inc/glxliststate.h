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
* Description:   ?Description
*
*/

#ifndef GLXLISTSTATE_H
#define GLXLISTSTATE_H

#include <glxbasestate.h>

/**
 * class to manage list state
 */
class GlxListState : public GlxState
{
public :
    /**
     * constructor
     */
	GlxListState(GlxState *preState = NULL);

	/**
	 * return current state of list
	 * @return current state
	 */
	int state() const { return (int) mState; }
	/**
	 * set current state
	 * @param internalState substate of list to be set
	 */
    void setState(int internalState) { mState = (ListState) internalState; }
	
    /**
     * event handler
     */
	void eventHandler(qint32 &id);
	/**
	 * set transition parameter
	 * @param dir navigation direction
	 * @param effect effect to be run on view transition
	 * @param viewEffect 
	 */
	void setTranstionParameter(NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect);
private:
    ///list internal state
	ListState mState;   
	
};

#endif /* GLXLISTSTATE_H */

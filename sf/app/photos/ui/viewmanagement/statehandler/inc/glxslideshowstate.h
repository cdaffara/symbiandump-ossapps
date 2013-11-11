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

#ifndef GLXSLIDESHOWSTATE_H
#define GLXSLIDESHOWSTATE_H

#include <glxbasestate.h>

class GlxStateManager;
class GlxOrientationSensorFilter;

/**
 * Class Description
 * This is slide show state corresponding to slide show view.
 */
class GlxSlideShowState : public GlxState
{
public :
    /**
     * constructor
     * @param pointer of state manager.
     * @param pointer of perivious state.
     */
	GlxSlideShowState(GlxStateManager *stateManager, GlxState *preState = NULL);
	
	/**
	 * state()
	 * @return return the substate.
	 */
	int state() const { return (int) mState; }
	
	/**
	 * setState() - set the current substate
	 * @param substate of the this state
	 */
	void setState(int internalState) { mState = (SlideShowState) internalState; }
	
	/**
	 * eventHandler() - A command handler of this state
	 * @param command id 
	 */
	void eventHandler(qint32 &commandId);
	
	/**
	 * albumItemEventHandler() - A command handle for album substate
	 * @param command id 
	 */
	void albumItemEventHandler( qint32 &id );
	
	/**
	 * Destructor
	 */
	~GlxSlideShowState();	
	
private:
    //slide show internal state
    SlideShowState mState;
    GlxStateManager *mStateManager;
    GlxOrientationSensorFilter *mOrientSensorFilter;
};

#endif /* GLXSLIDESHOWSTATE_H */

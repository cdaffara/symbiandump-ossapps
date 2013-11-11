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



#ifndef GLXGRIDSTATE_H
#define GLXGRIDSTATE_H

#include <glxbasestate.h>

class GlxStateManager;

class GlxGridState : public GlxState
{
public :
    /**
     * Constructor 
     * @param pointor of state manager
     * @param - previous state
     */
	GlxGridState( GlxStateManager *stateManager, GlxState *preState = NULL );
	
    /**
     * state() - This funtion return the internal state of a state
     * @return - internal state
     */	
	int state() const { return (int) mState; }
	
    /** 
     * setState() - This funtion use for set the internal state of a state
     * @param - internal state of the state
     */	
	void setState( int internalState ) { mState = (GridState) internalState; }
	
    /**
     * eventHandler() - It is event handler of the grid state
     * @param - command or event id
     */	
	void eventHandler( qint32 &id );
	
    /**
     * setTranstionParameter() - This function set the view transtion effect parameter.
     * These values is use to run the animation during view transition.
     * In the case of forward direction it should call with the next state and in the case of back ward direction
     * it should call with the current state
     * @param View transtion id
     * @param to play the animation on which views
     */
     void setTranstionParameter( NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect );

    /**
     * commandId() - it will return the current runing command id
     * @reutn - command id
     */
     qint32 commandId( ) { return mCommandId; }
	
signals :    

public slots:

protected:
	
private slots:

private:
    /**
     * defaultEventHandler() - It is a common event handler used for all the internal states.
     * @param - command or event id
     */
	void defaultEventHandler ( qint32 &id );

    /**
     * allItemEventHandler() - It is a event handler used for All and fetcher internal states.
     * @param - command or event id
     */
	void allItemEventHandler ( qint32 &id );

    /**
     * albumItemEventHandler() - It is a event handler used for album and fetcher album interanl states.
     * @param - command or event id
     */
	void albumItemEventHandler ( qint32 &id );

private :
	GridState mState; 	            // grid internal state
	GlxStateManager *mStateManager; // state manager
	bool mIsMarkingMode;            // marking mode status
	qint32 mCommandId;              //commandID, save the command before entering into the marking mode for next user interaction
};


#endif /* GLXGRIDSTATE_H_ */

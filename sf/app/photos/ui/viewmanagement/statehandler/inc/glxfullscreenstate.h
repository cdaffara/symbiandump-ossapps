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



#ifndef GLXFULLSCREENSTATE_H
#define GLXFULLSCREENSTATE_H

#include <glxbasestate.h>
class GlxStateManager;

class GlxFullScreenState : public GlxState
{
public :
    GlxFullScreenState( GlxStateManager *stateManager, GlxState *preState = NULL );
    int state() const { return (int) mState; }
/*
 *  This Function set the internal state of full screen state
 */	
    void setState(int internalState) { mState = (FullScreenState) internalState; }
    void eventHandler(qint32 &id);
/*
 *  This function set the transition parameter ( for animation) from full screen view to other view
 */	
    void setTranstionParameter(NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect);
	
signals :    

public slots:

protected:
	
private slots:

private:

private:
    FullScreenState mState;
    GlxStateManager *mStateManager;
	
};


#endif /* GLXFULLSCREENSTATE_H_ */

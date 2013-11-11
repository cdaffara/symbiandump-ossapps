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


#ifndef GLXDETAILSTATE_H
#define GLXDETAILSTATE_H

#include <glxbasestate.h>


class GlxDetailState : public GlxState
{
public :
	GlxDetailState(GlxState *preState = NULL);
	void eventHandler(qint32 &id);
	int state() const { return (int) mState; }
/*
 *  This Function set the internal state of details state
 */ 
    void setState(int internalState) { mState = (DetailState) internalState; }
/*
 *  This function set the transition parameter ( for animation) from full screen view to other view
 */ 
    void setTranstionParameter(NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect);
	
	
signals :    

public slots:

protected:
	
private slots:

private:
//Functions

private:
    DetailState mState;
//Data Member
	
};


#endif /* GLXDETAILSTATE_H*/

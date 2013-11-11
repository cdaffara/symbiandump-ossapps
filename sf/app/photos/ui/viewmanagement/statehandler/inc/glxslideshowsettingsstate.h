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
* Description:   This class handles the state corresponding to the Slide Show settings view
*
*/



#ifndef GLXSLIDESHOWSETTINGSSTATE_H
#define GLXSLIDESHOWSETTINGSSTATE_H

#include <glxbasestate.h>

class GlxStateManager;

class GlxSlideShowSettingsState : public GlxState
{
public :
	
	GlxSlideShowSettingsState(GlxStateManager *stateManager, GlxState *preState = NULL);
	void eventHandler(qint32 &id);	//Overriding the pure virtual function 
	
private:

	GlxStateManager *mStateManager;
};


#endif /* GLXSLIDESHOWSETTINGSSTATE_H_ */

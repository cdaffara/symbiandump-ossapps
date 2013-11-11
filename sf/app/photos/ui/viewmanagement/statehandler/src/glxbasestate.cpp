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



#include <glxbasestate.h>

GlxState::GlxState (qint32 id, GlxState *preState) : mId(id), mPreState(preState)
{

}

bool GlxState::compare (qint32 id) const
{
	return id == mId ;	
}

void GlxState::setTranstionParameter(NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect)
{
    //This Function will set the default value
    Q_UNUSED(dir)
    effect = NO_EFFECT;
    viewEffect = NO_VIEW ;
}

GlxState::~GlxState()
{

}


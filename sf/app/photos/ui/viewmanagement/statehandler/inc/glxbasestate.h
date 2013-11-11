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



#ifndef GLXBASESTATE_H
#define GLXBASESTATE_H

#include <QObject>
#include <glxviewids.h>
#include <glxuistd.h>

typedef enum
{
    NO_DIR,
    FORWARD_DIR,
    BACKWARD_DIR,    
}NavigationDir;

class GlxState: public QObject
{
Q_OBJECT

public:
    /**
     * Constructor
     * id = state id, state is and view id should be same
     * preState = previous state pointer. It value will be null, if it is only one state in the stack
     */
    GlxState( qint32 id, GlxState *preState );
    
    /**
     * Destructor
     */
    virtual ~GlxState();
    
    /**
     * compare() - It wiil compare the states, is it same or different state.
     * @param - Id of the state
     * @return - return the true if the state is same else false
     */
    bool compare( qint32 id ) const;
    
    /**
     * id() - Return the state id of the state
     * @return - return the state id of the state
     */
    qint32 id() const
    {
        return mId;
    }
    
    /**
     * previousState() - Retrun the pointer of the the previous state
     * @return - Retrun the pointer of the the previous state
     */
    GlxState * previousState() const
    {
        return mPreState;
    }
    
    /**
     * eventHandler() - Pure virtual function. It is default event handler of the state
     * @param - command or event id
     */
    virtual void eventHandler( qint32 &id ) = 0;
    
    /** 
     * setState() - This funtion use for set the internal state of a state
     * default implementation
     * @param - internal state of the state
     */
    virtual void setState( int internalState )
    {
        Q_UNUSED( internalState )
    }

    /**
     * commandId() - it will return the current runing command id
     * @reutn - command id
     */
    virtual qint32 commandId()
    {
        return -1;
    }
    
    /**
     * state() - This funtion return the internal state of a state
     * default implementation
     * @return - internal state
     */
    virtual int state() const
    {
        return 0;
    }
    
    /**
     * setTranstionParameter() - This function set the view transtion effect parameter.
     * These values is use for run the animation for vies transtion
     * In the case of forward direction it should call with the next state and in the case of back ward direction
     * it should call with the current state
     * @param View transtion id
     * @param to play the animation on which views
     */
    virtual void setTranstionParameter( NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect );

private:
    const qint32 mId;
    GlxState *mPreState; //contain the pointer of previous state	
};



#endif /* GLXBASESTATE_H */

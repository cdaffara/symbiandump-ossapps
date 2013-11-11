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
#include "cntviewnavigator.h"
#include <cntviewparams.h>
#include <QDebug>

CntViewNavigator::CntViewNavigator( QObject* aParent ) : 
    QObject( aParent ),
    iTop( noView )
{
    
}

CntViewNavigator::~CntViewNavigator()
{
    
}

void CntViewNavigator::next( const int& aId, QFlags<Hb::ViewSwitchFlag> &flags )
{
    if ( !iViewStack.isEmpty() )
    {
        int top = iViewStack.top();
        
        // If any special effects are defined for the current (top) view and associated with the next view
        if ( iEffects.contains( top ) && iEffects.value( top ) == aId ) 
        {
            flags = Hb::ViewSwitchUseNormalAnim | Hb::ViewSwitchUseAltEvent;
        }
        else
        {
            flags = Hb::ViewSwitchUseNormalAnim;
        }
    }
    iViewStack.push( aId );
}

const int& CntViewNavigator::back( QFlags<Hb::ViewSwitchFlag> &flags, bool toRoot )
{
    iTop = noView;
    // Check if exception is set for current view item. Exception
    // means that instead of one step back, we go back until that 
    // execption value is located. So all items that are jumped over,
    // their history will be eared.
    if ( !iViewStack.isEmpty() ) 
    {
        int top = iViewStack.top();
        
        // If we need to go to the latest root view
        if ( toRoot )
        {
            while ( !iViewStack.isEmpty() ) 
            {
                int current = iViewStack.top();
                
                if ( iRoots.contains( current ) )
                {
                    iTop = current;
                    break;
                }
                iViewStack.pop();
            }
        }
        // If any exception defined for the current (top) view
        else if ( iExceptions.contains( top ) ) 
        {
            iTop = iExceptions.value( top );
            // cleanup the history until the exception value is found
            while ( !iViewStack.isEmpty() ) 
            {
                if ( iTop == iViewStack.top() ) 
                {
                    break; // don't pop the exception it self.
                }
                iViewStack.pop();
            }
        }
        // No exceptions defined, go one step back
        else 
        {
            iViewStack.pop();
            if ( !iViewStack.isEmpty() )
            {
                iTop = iViewStack.top();
            }
        }
        
        // If any special effects are defined for the current (top) view and associated with the previous view
        if ( iEffects.contains( top ) && iEffects.value( top ) == iTop ) 
        {
            flags = Hb::ViewSwitchUseBackAnim | Hb::ViewSwitchUseAltEvent;
        }
        else
        {
            flags = Hb::ViewSwitchUseBackAnim;
        }
    } 

    return iTop;
}

void CntViewNavigator::addException( const int& aCurrent, const int& aBack )
{
    iExceptions.insert( aCurrent, aBack );
}

void CntViewNavigator::removeException( const int& aCurrent )
{
    if ( iExceptions.contains(aCurrent) )
    {
        iExceptions.remove( aCurrent );
    }
}

void CntViewNavigator::addEffect( const int& aCurrent, const int& aBack )
{
    iEffects.insert( aCurrent, aBack );
}

void CntViewNavigator::removeEffect( const int& aCurrent )
{
    if ( iEffects.contains(aCurrent) )
    {
        iEffects.remove( aCurrent );
    }
}

void CntViewNavigator::addRoot( const int& aCurrent )
{
    iRoots.append( aCurrent );
}
    
// End of File

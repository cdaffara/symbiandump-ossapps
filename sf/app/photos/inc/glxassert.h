/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Type-safe unique id template
*
*/


#ifndef GLXASSERT_H
#define GLXASSERT_H

#include <glxlog.h>
#include <glxpanic.h> // enable client to directly use GLX - Panic

/**
 * Used to assert the truth of some condition. If the condition is false, logging will be performed
 * and appropriate action taken. Unlike GLX_ASSERT_DEBUG it is defined in both release and debug builds.
 * 
 * Logging is performed using GLX_LOG_WARNING and is subject to GLX_LOG_WARNING being enabled.
 *
 * Code is generated for both debug and release builds.
 *
 * @param c A conditional expression which results in true or false.
 * @param p A function which is called if the conditional expression c is false.
 * @param l logging that is performed before function p is called.
 * 
 * @see GLX_ASSERT_DEBUG
 * @see GLX_LOG_WARNING
 */ 
#define GLX_ASSERT_ALWAYS( c, p, l )    \
    {                                   \
    if ( !( c ) )                       \
        {                               \
        GLX_LOG_WARNING( l );           \
        p;                              \
        }                               \
    }

#ifdef _DEBUG
/**
 * Used to assert the truth of some condition. If the condition is false, logging will be performed
 * and appropriate action taken. Used in the same way as GLX_ASSERT_ALWAYS, except that it is only 
 * defined for debug builds.
 * 
 * Logging is performed using GLX_LOG_WARNING and is subject to GLX_LOG_WARNING being enabled.
 *
 * Code is generated for debug builds only.
 *
 * @param c A conditional expression which results in true or false.
 * @param p A function which is called if the conditional expression c is false.
 * @param l logging that is performed before function p is called.
 * 
 * @see GLX_ASSERT_ALWAYS
 * @see GLX_LOG_WARNING
 */ 
# define GLX_ASSERT_DEBUG( c, p, l )    GLX_ASSERT_ALWAYS( c, p, l )
#else // _DEBUG
# define GLX_ASSERT_DEBUG( c, p, l )
#endif // _DEBUG 

#endif //GLXASSERT_H

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
* Description:   Viewer Panics
*
*/



#ifndef T_GLXPANIC_H
#define T_GLXPANIC_H

#include <e32std.h>

/** Viewer application panic codes */
enum TGlxPanics 
    {
    EGlxPanicNotImplemented = 1,                  // Feature not implemented yet
    EGlxPanicNotInitialised,                      // Object is used without having been initialised
    EGlxPanicAlreadyInitialised,                  // Object has already been initialised
    EGlxPanicIllegalState,                        // The current internal state of the object(s) is not allowed
    EGlxPanicNullPointer,                         // Null pointer
    EGlxPanicNullLayout,                          // Null Layout
    EGlxPanicNullTHuiImage,                       // Null THuiImage in layouts
    EGlxPanicNullCHuiVisual,                      // Null CHuiVisual
    EGlxPanicNullVisualList,                      // Null Visual List
    EGlxPanicNullScreenInfo,                      // Null screen info in layouts
    EGlxPanicWrongLayoutType,                     // Wrong layout type for this amount of parameters
    EGlxPanicNullDescriptor,                      // Provided descriptor pointer was null
    EGlxPanicNullMediaList,                       // Provided media list pointer was null
    EGlxPanicIllegalArgument,                     // Argument is not valid
    EGlxPanicUnhandledError,                      // Error occured, but it is not being handled
    EGlxPanicLogicError,                          // Error in code logic
    EGlxPanicTaskNotAddedToManager,               // The task has not been added to the task manager
    EGlxPanicMemoryLeak,
    EGlxPanicAlreadyAdded,                        // Item has been already added to the array/list
    EGlxPanicDebugUnexpectedError,                // A legal error occured, but so unusual that panic is in order in debug builds
    EGlxPanicInvalidIdentifier,                   // Invalid identifier
    EGlxPanicInvalidPathLevel,                    // An invalid path level
    EGlxPanicMediaRequestedWithEmptyPath,         // MediaL has been called with a path that has a count of 0.
    EGlxPanicCollectionManagerOutstandingRequest, // A request has been made to the collection manager while an outstanding request exists.
    EGlxPanicDatasourceMdeSessionNotOpen,         // An attempt has been made to access the MDE session before it has been opened.
    EGlxPanicDatasourceMdeTaskExecutedTwice,      // An attempt has been made to execute a task a second time.
    EGlxPanicWrongAttributeType,                  // Attempt to read attribute as wrong type (text, C-class, T-class)
    EGlxPanicCastFailed,                          // An attempt to cast an object from one class to another has failed.
    EGlxPanicRequiredItemNotFound,                // An item that is required to be in a media list was not found.
    EGlxPanicEnvironment,                         // Problem with the environment at run time found
    EGlxPanicSmoothedValueIllegalState,           // A TGlxSmoothedValue is misbehaving
    EGlxPanicUnhandledValue,                      // An unexpected value has been encountered
    EGlxPanicUnsupportedCollection,               // The collection is specified is unsupported by the collection manager.
    EGlxPanicWrongMediaList,                      // The media list instance passed to a method is not the expected instance.   
    EGlxPanicNoReservation,                       // Attempt made to add an item to a list without reserving space first
    EGlxPanicIndexOutOfRange,                     // Index is out of range
    EGlxPanicRequiredPropertyNotFound,            // A required property was not found
    EGlxPanicEmptyArray,                          // An array was empty when it was expected that it contained values.
    EGlxPanicCommandHasNoGeneralSessionId,        // A command has been sent to the server that does not have the KMPXCommandGeneralSessionId attribute set
    EGlxPanicUnsupportedQueryType,                // An attempt has been made to execute a query of an unsupported type 
    EGlxPanicQueryLogicError,                     // A logic error occurred executing a query.
    EGlxPanicInvalidCollectionUid,                // An attempt was made to get a container id for a collection that does not have a container id associated with it.
    EGlxPanicUnexpectedQueryResultCount           // The query did not return a single result as expected.
    // add further panics here
    };

inline void Panic(TGlxPanics aReason)
    {
	_LIT(applicationName,"Images & videos");
    User::Panic(applicationName, aReason);
    }

#endif // T_GLXPANIC_H

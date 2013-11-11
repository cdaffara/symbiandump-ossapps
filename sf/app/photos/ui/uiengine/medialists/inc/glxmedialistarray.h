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
* Description:    List of media items
*
*/




#ifndef C_GLXMEDIALISTARRAY_H
#define C_GLXMEDIALISTARRAY_H

#include <glxsingletonstore.h>

class CGlxMediaList;

/**
 * A singleton that owns a pointer array of media lists
 */
NONSHARABLE_CLASS(CGlxMediaListArray) : public CBase
    {
public:
	/**
	 * Returns the CMediaListArray singleton
	 * @return the instance of CMediaListArray
	 */
	static CGlxMediaListArray* InstanceL();
    
    /**
     * Closes the singleton
     */
     void Close();

     /**
      * Returns array of media lists
      */
     RPointerArray<CGlxMediaList>& Array();
          
private:
	/**
	 * Constructor (private so that only
	 * the singleton store can use it)
	 */
	static CGlxMediaListArray* NewL();
	
	/**
	 * Destructor
	 */
	~CGlxMediaListArray();
	
private:
    /**
     * Array of media lists pointers (owned, contents not owned)
     */
	RPointerArray<CGlxMediaList> iArray;
    };

#endif // C_GLXMEDIALISTARRAY_H
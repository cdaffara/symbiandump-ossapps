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
* Description:   Media item id
*
*/



#ifndef T_GLXMEDIAID_H
#define T_GLXMEDIAID_H

#include <e32base.h>

#include "glxid.h"

const TUint KGlxCollectionRootId = 0;
// Media type ID definitions - may need to be moved somewhere else

/**
 *  TGlxIdSpaceIdBase
 *
 *  Don't use this. This is just to make TGlxId type-safe for TGlxIdSpaceId
 *
 *  @lib none
 */
class TGlxIdSpaceIdBase {};


/**
 *  TGlxIdSpaceId
 *
 *  Typesafe Id-space identifier.
 *
 *  @lib none
 */
typedef TGlxId<TGlxIdSpaceIdBase> TGlxIdSpaceId;
	
/**
 * Known Id Spaces
 */
const TUint KGlxIdSpaceIdRoot = 0x200071A6; ///< Root menu id space

/**
* used to open Media Model for albums
*/
const TUint KGlxAlbumsMediaId = 0x200104EC  ;
/**
 *  Don't use this. This is just to make TGlxId concrete
 */
class TGlxIdMediaBase {}; 

/**
 *  TGlxMediaId
 *
 *  Type safe id class for media item id. Provides getters/setters for all bit groupings.
 *
 *  @lib ViewerEngine
 */
class TGlxMediaId : public TGlxId<TGlxIdMediaBase>
    {
public:
    /**
     * Default constructor 
     */
    inline TGlxMediaId() : TGlxId<TGlxIdMediaBase>() { }; 
    /**
     * Constructor 
     */
	inline TGlxMediaId(const TGlxMediaId& aId) : TGlxId<TGlxIdMediaBase>(aId) { };
    /**
     * Constructor 
     */
	explicit inline TGlxMediaId(TUint aId) : TGlxId<TGlxIdMediaBase>(aId) { }; 
    /**
     * Constructor 
     */
	inline TGlxMediaId(const TGlxIdNone& aIdNone) : TGlxId<TGlxIdMediaBase>(aIdNone) { }; 
    };

#endif // T_GLXMEDIAID_H

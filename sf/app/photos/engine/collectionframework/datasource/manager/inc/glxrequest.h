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
* Description:   Represents a  request
*
*/



#ifndef C_GLXREQUEST_H
#define C_GLXREQUEST_H

// INCLUDES
#include <e32cmn.h>
#include <e32base.h>
#include <mpxfilter.h>
		
// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION

/**
 *  CGlxRequest class 
 *
 *  Requests encapsulate the requests with filters that can be sent from
 *  collection plugins to the Data Source Manager.
 *	@ingroup collection_component_design
 *  @internal reviewed 11/07/2007 by Dave Schofield
 */
class CGlxRequest : public CBase 
	{
public:
	IMPORT_C virtual ~CGlxRequest();

	/**
	 * Collection plugin UID.
	 * Gets the UID of the collection plugin that created this request.
	 * @return the UID of the collection plugin that created this request.
	 */
	IMPORT_C TUid CollectionPluginUid() const;
	IMPORT_C CMPXFilter* Filter() const;
	
	inline const TBool DoNotNeedDataSource() const
	{
	return iDoNotNeedDataSource;
	}
	


protected:
	CGlxRequest(const TUid& aCollectionPluginUid, const TBool aDoNotNeedDataSource = EFalse);
	void ConstructL(const CMPXFilter* aFilter);

private:
	const TUid iCollectionPluginUid;
	CMPXFilter* iFilter;
protected:
	const TBool iDoNotNeedDataSource;
	};

#endif // C_GLXREQUEST_H


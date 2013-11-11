/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM Utilities
*
*/


#ifndef __NSMLDMURI_H
#define __NSMLDMURI_H

#include <e32std.h>
#include <e32base.h>
#include <e32def.h>


const TUint8 KNSmlDMUriSeparator = 0x2f; //forward slash

_LIT8(KNSmlDmRootUri, ".");
_LIT8(KNSmlDmUriDotSlash, "./");
_LIT8( KNSmlDmQuestionMark, "?" );

// ===========================================================================
// NSmlDmURI
// ===========================================================================
/**
* NSmlDmURI contains just static methods for parsing URIs
*
*  @since
*/
class NSmlDmURI
	{
	public:
	/**
	* Return the parent URI of given URI
	* @param	aURI		Uri
	* @return 		 		Parent URI
	*/
	static TPtrC8 ParentURI(const TDesC8& aURI);

	/**
	* Return the last URI segment of given URI
	* @param	aURI		Uri
	* @return 		 		Last URI segment
	*/
	static TPtrC8 LastURISeg(const TDesC8& aURI);

	/**
	* Return the URI without ./ at the beginning, if those exist
	* @param	aURI		Uri
	* @return 		 		Uri without ./
	*/
	static TPtrC8 RemoveDotSlash(const TDesC8& aURI);

	/**
	* Return the URI without property info at the end (?prop=...)
	* @param	aURI		Uri
	* @return 		 		Uri without property info
	*/
	static TPtrC8 RemoveProp(const TDesC8& aURI);

	/**
	* Return the URI without last segment
	* @param	aURI		Uri
	* @return 		 		URI without last uri segment
	*/
	static TPtrC8 RemoveLastSeg(const TDesC8& aURI);

	/**
	* Return the aLocationth URI segment.
	* @param	aURI		Uri
	* @param	aLocation	The wanted segment number
	* @return 		 		The aLocationth URI segment
	*/
	static TPtrC8 URISeg(const TDesC8& aURI,TInt aLocation);

	/**
	* Return the number of URI  segments of given URI
	* @param	aURI		Uri
	* @return 		 		Number of segments
	*/
	static TInt NumOfURISegs(const TDesC8& aURI);
	};

#endif // __NSMLDMURI_H

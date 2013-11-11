/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Group vCard object conversion routines
*
*/


#ifndef __CONTACTSGRPCONVERTER_H__
#define __CONTACTSGRPCONVERTER_H__

#include <e32base.h>

#include <cntitem.h>

class CContactIdArray;

/* @brief CContactsGrpConverter Class
 * This class is used to convert Contact Group items to vCard presentation and vice versa.
 */

NONSHARABLE_CLASS( CContactsGrpConverter ): public CBase
	{
public:

	static CContactsGrpConverter* NewL();
	static CContactsGrpConverter* NewLC();
	~CContactsGrpConverter();

	// returns group label
	const TDesC& GroupLabel() const;
	// returns contact items beloging to group
	const CContactIdArray& ItemsContained() const;
	
	// Import group item
	void ImportDbItemL( CContactGroup& aItem );
	
	// Export / Import vCard info from/to this object
	void ExportVCardL( CBufBase& aBuffer );
	void ExportVCardL( RWriteStream& aWriteStream );
	void ImportVCardL( const TDesC8& aBuffer );
	
	// Reset all data
	void ResetL();
	
private:
    
    CContactsGrpConverter();	
	void ConstructL();
	
    void SetContactItemsL( const TDesC& aValue );
        
    void SplitL( const TDesC& aText, const TChar aSeparator, RArray<TPtrC>& aArray );
    void TrimAll( TPtrC& aValue );
private:
    HBufC*           iGroupLabel;
    CContactIdArray* iContactsIdArray;
    
	};

#endif // __CONTACTSGRPCONVERTER_H__

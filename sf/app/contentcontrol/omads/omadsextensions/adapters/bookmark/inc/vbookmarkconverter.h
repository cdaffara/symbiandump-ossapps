/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Bookmark object conversion routines
*
*/


#ifndef __VBOOKMARKCONVERTER_H__
#define __VBOOKMARKCONVERTER_H__

#include <e32base.h>

class CFavouritesItem;
class CFolderItem;

typedef enum
    {
    ENoEncoding = 0,
    EQuotedPrintable
    } TBookmarkEncoding;

class CVBookmarkConverter : public CBase
	{
public:

	static CVBookmarkConverter* NewL();
	static CVBookmarkConverter* NewLC();
	~CVBookmarkConverter();

	// Get
	const TDesC& GetVersion() const;
	const TDesC& GetUrl() const;
	const TDesC& GetTitle() const;
	const TDesC& GetInternetShortcut() const;	
	
	// Set
	void SetVersionL( const TDesC& aVersion );
	void SetVersionL( const TDesC8& aVersion );
	
	void SetUrlL( const TDesC& aUrl );
	void SetUrlL( const TDesC8& aUrl );
	
	void SetTitleL( const TDesC& aTitle );
	void SetTitleL( const TDesC8& aTitle, TBookmarkEncoding aEncoding = ENoEncoding );
		
	void SetInternetShortcutL( const TDesC& aInternetShortcut );
	void SetInternetShortcutL( const TDesC8& aInternetShortcut );
	
	void ExportDbItemL( CFavouritesItem& aItem );		
	void ImportDbItemL( const CFavouritesItem& aItem );
	void ImportDbItemL( const CFolderItem& aItem );


	// Export / Import VBookmark info from/to this object
	void ExportVBookmarkL( TDes8& aBuffer );
	void ExportVBookmarkL( CBufBase& aBuffer );
	
	TInt ImportVBookmarkL( const TDesC8& aBuffer );

private:

	CVBookmarkConverter();	
	void ConstructL();
	
	void WriteMessageLineL( const TDesC8& aMsgLineTag, const TDesC8& aMsgLineData,
	    const TDesC8& aSeparator );
	
	void WriteEnvelopeL();
	
	void ExportL();
	
	void ResetL();
	
	TInt ReadTagAndValue( const TDesC8& aBuffer, TInt& aPosition,
	    TPtrC8& aTag, TPtrC8& aValue, TPtrC8& aProperties );
	    
	TInt ReadEnvelopeL( const TDesC8& aBuffer, TInt& aPosition );
	
	TInt ReadAssignment( const TDesC8& aBuffer, TInt& aPosition,
	    TPtrC8& aTag, TPtrC8& aValue );
	
	void WriteL( const TDesC8 &aData );
	
	void IgnoreSpaces( TPtrC8& aData );
	
	TBookmarkEncoding Encoding( TPtrC8& aProperties );
	
	HBufC8* DecodeQuotedPrintableLC( const TDesC8& aEncodedData );
	
private:

	HBufC*      iVersion;
	HBufC*      iUrl;
	HBufC*      iTitle;
	HBufC*      iInternetShortcut;
	CBufBase*   iBuffer;
	TInt        iWriteBufPosition;
	TInt        iWriteBufSize;
	TDes8*      iDesc;
	};

#endif

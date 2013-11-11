/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef ICPIXNPPLUGIN_H_
#define ICPIXNPPLUGIN_H_

class CCPixNPSearcher; 

/**
 * MCPixNPPlugin
 * Represents the CPix NPPllugin providing the CPix search services for
 * clients in the S60 NPRuntime environment (e.g. JavaScript applications in 
 * S60 Browser). 
 */
class MCPixNPPlugin
    {
public: 
	
	/**
     * The name of the mandatory field that will store the unique ID of
     * a document.
     */
	virtual const TDesC& DocUidFieldName() const = 0; 

    /**
     * The name of the mandatory field that will store the application
     * class of a document.
     */
	virtual const TDesC& AppClassFieldName() const = 0; 

    /**
	  * The name of the optional field that will store the excerpt text
	  * for a document.
	  */
	virtual const TDesC& ExcerptFieldName() const = 0; 

	/**
     * The name of the optional field that will store the application
     * ID of the document.
     */
	virtual const TDesC& MimeTypeFieldName() const = 0; 

	/**
     * Aaggregate of the contents of other fields.
     */
	virtual const TDesC& AggregateFieldName() const = 0; 

	/**
     * The default field to search for (currently aggregate field).
     */
	virtual const TDesC& DefaultFieldName() const = 0; 

	/**
	 * In the documents returned by searcher's suggest() method, 
	 * this field will contain the term's text content.
     */
	virtual const TDesC& TermTextFieldName() const = 0; 

	/**
	 * In the documents returned by suggest() method, this field 
	 * will contain the term's document frequence
     */
	virtual const TDesC& TermDocFreqFieldName() const = 0; 
	
	/*
	 * Analysis constants 
     */
	
	virtual const TDesC& StandardAnalyzerId() const = 0; 
	virtual const TDesC& StandardTokenizerId() const = 0; 
	virtual const TDesC& WhitespaceTokenizerId() const = 0; 
	virtual const TDesC& LetterTokenizerId() const = 0; 
	virtual const TDesC& KeywordTokenizerId() const = 0; 
	virtual const TDesC& StandardFilterId() const = 0; 
	virtual const TDesC& LowercaseFilterId() const = 0; 
	virtual const TDesC& AccentFilterId() const = 0; 
	virtual const TDesC& StopFilterId() const = 0; 
	virtual const TDesC& StemFilterId() const = 0; 
	virtual const TDesC& LengthFilterId() const = 0; 
	virtual const TDesC& EnglishLanguageId() const = 0; 
	virtual const TDesC& FinnishLanguageId() const = 0; 
	virtual const TDesC& HungarianLanguageId() const = 0; 
	virtual const TDesC& RussianLanguageId() const = 0; 

	/**
	 * CreateSearcherL
	 * Returns ownership to NPObject struct. (The searcher object
	 * itself is reference counted). 
	 * @param aDomainSelector The domain selector that will be searched
	 * E.g. "root", "root contacts,root file", "@c:root file"
	 * @param aDefaultSearchField field to be searched.
	 * @return CCPixNPSearcher searcher object
	 */
	virtual CCPixNPSearcher* CreateSearcherL( const TDesC& aDomainSelector, 
											  const TDesC& aDefaultSearchField ) const = 0;

	/**
	 * CreateSearcherL
	 * Returns ownership to NPObject struct. (The searcher object
	 * itself is reference counted). 
	 * @param aDomainSelector The domain selector that will be searched
	 * E.g. "root", "root contacts,root file", "@c:root file"
	 * @return CCPixNPSearcher searcher object
	 */
	virtual CCPixNPSearcher* CreateSearcherL( const TDesC& aDomainSelector ) const = 0;

	/**
	 * DefineVolumeL
	 * Defines a new CPIX qualified base app class and the physical location of it on disk. 
	 * @param aQualifiedBaseAppClass The qualified base application class of the database to be 
	 * defined.
	 * E.g. "@e:root file" when the MMC is inserted
	 * @param aIndexDbPath The physical index database path.
	 */
	virtual void DefineVolumeL( const TDesC& aQualifiedBaseAppClass, const TDesC& aIndexDbPath ) = 0;

	/**
	 * UnDefineVolumeL
	 * Undefines an existing CPIX qualified base app class. 
	 * @param aQualifiedBaseAppClass The qualified base app class to be deleted. 
	 * E.g. "@e:root file" when the MMC is ejected
	 */
	virtual void UnDefineVolumeL( const TDesC& aQualifiedBaseAppClass ) = 0;
	
	
	virtual void OpenContactL(TInt aIndex) = 0;

	virtual void OpenMessagingL(TInt aMesId) = 0;
	
	virtual void OpenCalendarL(TInt aCalId) = 0;
	
	virtual void OpenMapsL( TReal aLatitude, TReal aLongitude ) = 0;
	
	virtual TInt GetVersionNumber() const = 0;
	
	//Added to launch Notes from Widget
	virtual void OpenNoteL( TInt aNoteId ) = 0;
	
	virtual void GetDriveFromMediaIdL(HBufC* aMediaId,TDes& aDrive) = 0;
	
    };

#endif /*ICPIXNPPLUGIN_H_*/

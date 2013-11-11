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
#ifndef CPIXBROWSERPLUGIN_H
#define CPIXBROWSERPLUGIN_H

// INCLUDES
#include "CCPixNPPluginInterface.h" 
#include <RSearchServerSession.h>
#include <AknServerApp.h>

#include <mnprovider.h>    // CMnProvider

class CJSCallback;
class CFbsBitmap;
class CMsvUI;
class CMnMapView;

// CLASS DECLARATION

/**
 * Implements services described in MCPixNPPlugin. Provides
 * the functionality for accessing the CPix services from NP Runtime 
 * environment. In practice is used for creating searchers and 
 * indexers at some latter point. 
 */
class CCPixNPPlugin : public CCPixNPPluginInterface, public MAknServerAppExitObserver
{
public: // Constructors and destructor

	virtual ~CCPixNPPlugin();
	static CCPixNPPlugin* NewL();
	static CCPixNPPlugin* NewLC();
	
	void SetWindowL( NPWindow* aWindow );
   
private:
	
	CCPixNPPlugin();
	void ConstructL();

public: // From ICPixNPPlugin

	virtual const TDesC& DocUidFieldName() const; 

	virtual const TDesC& AppClassFieldName() const; 

	virtual const TDesC& ExcerptFieldName() const; 

	virtual const TDesC& MimeTypeFieldName() const; 

	virtual const TDesC& AggregateFieldName() const; 

	virtual const TDesC& DefaultFieldName() const; 
	
	virtual const TDesC& TermTextFieldName() const; 
	
	virtual const TDesC& TermDocFreqFieldName() const; 
	
	// Analysis related constants
	
	virtual const TDesC& StandardAnalyzerId() const; 

	virtual const TDesC& StandardTokenizerId() const; 
	
	virtual const TDesC& WhitespaceTokenizerId() const;
	
	virtual const TDesC& LetterTokenizerId() const;
	
	virtual const TDesC& KeywordTokenizerId() const; 
	
	virtual const TDesC& StandardFilterId() const; 
	
	virtual const TDesC& LowercaseFilterId() const; 
	
	virtual const TDesC& AccentFilterId() const; 
	
	virtual const TDesC& StopFilterId() const; 
	
	virtual const TDesC& StemFilterId() const; 
	
	virtual const TDesC& LengthFilterId() const; 
	
	virtual const TDesC& EnglishLanguageId() const; 
	
	virtual const TDesC& FinnishLanguageId() const; 
	
	virtual const TDesC& HungarianLanguageId() const; 
	
	virtual const TDesC& RussianLanguageId() const; 

	virtual CCPixNPSearcher* CreateSearcherL( const TDesC& aDomainSelector, const TDesC& aDefaultSearchField ) const;

	virtual CCPixNPSearcher* CreateSearcherL( const TDesC& aDomainSelector ) const;

	virtual void DefineVolumeL( const TDesC& aQualifiedBaseAppClass,
								const TDesC& aIndexDbPath );

	virtual void UnDefineVolumeL( const TDesC& aQualifiedBaseAppClass );
	
	
	virtual void OpenContactL(TInt aContactId);	

	virtual void OpenMessagingL(TInt aMesId);	

	virtual void OpenCalendarL(TInt aCalId);
	
	virtual void OpenMapsL( TReal aLatitude, TReal aLongitude );
	
	//Added to launch Notes from Widget
	virtual void OpenNoteL( const TInt aNoteId );
	
	virtual void GetDriveFromMediaIdL(HBufC* aMediaId,TDes& aDrive);
	
	virtual TInt GetVersionNumber() const;

public: // From MAknServerAppExitObserver
	
    void HandleServerAppExit(TInt aReadon);
	
private: // member variables

	// Search Server Session
	RSearchServerSession iSearchServer;
	
	// Launches Message Viewer
	CMsvUI* iMsvUI;
	
	// Maps content providers
	RPointerArray<CMnProvider> iMnProviders;
	
	// Map view
	CMnMapView* iMapView;
	
	//RFs file session
	RFs     iFs;
};

#endif // WLESSPLUGIN_H


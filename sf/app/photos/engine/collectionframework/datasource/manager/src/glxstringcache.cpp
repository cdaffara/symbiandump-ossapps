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
* Description:   This class caches resource strings
*
*/



#include <glxlog.h>

#include <barsc.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <glxtracer.h>

#include "glxstringcache.h"


// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
CGlxStringCache::CGlxStringItem* CGlxStringCache::CGlxStringItem::NewL(TInt aId, HBufC* aString)
    {
    CGlxStringItem* self = new (ELeave) CGlxStringItem(aId);
    CleanupStack::PushL(self);
    self->ConstructL(aString);
    CleanupStack::Pop(self);
    return self;
    }
    
// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxStringCache::CGlxStringItem::ConstructL(HBufC* aString)
    {
    if (NULL == aString)
        {
        User::Leave(KErrArgument);
        }
    
    iString = (*aString).AllocL();
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxStringCache::CGlxStringItem::CGlxStringItem(TInt aId)
: iId(aId)
    {
    
    }
    
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxStringCache::CGlxStringItem::~CGlxStringItem()
    {
    delete iString;
    }
    
// ----------------------------------------------------------------------------
// GetId
// ----------------------------------------------------------------------------
//

TInt CGlxStringCache::CGlxStringItem::GetId() const
    {
    return iId;
    }

// ----------------------------------------------------------------------------
// GetString
// ----------------------------------------------------------------------------
//

HBufC* CGlxStringCache::CGlxStringItem::GetString()
    {
    return iString;
    }
 
// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
 
EXPORT_C CGlxStringCache* CGlxStringCache::NewL()
	{
	CGlxStringCache* self = new(ELeave) CGlxStringCache;
	return self;
	}
    
// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxStringCache::CGlxStringCache()
    {
    TRACER("CGlxStringCache::CGlxStringCache()");
    iResourceFileName.Zero();
    }
    
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxStringCache::~CGlxStringCache()
    {
    TRACER("CGlxStringCache::~CGlxStringCache()");
    iStringCache.ResetAndDestroy();
    }
    
// ----------------------------------------------------------------------------
// Count
// ----------------------------------------------------------------------------
//
TInt CGlxStringCache::Count()
    {
    TRACER("TInt CGlxStringCache::Count()");
    return iStringCache.Count();
    }

// ----------------------------------------------------------------------------
// Find
// ----------------------------------------------------------------------------
//
HBufC* CGlxStringCache::FindL(TInt aId)
    {
    TRACER("HBufC* CGlxStringCache::FindL(TInt aId)");
    // Create a TIdentityRelation to be used in the Find
    TIdentityRelation<CGlxStringCache::CGlxStringItem> match(&MatchById);
    
    // Create a dummy string item with the correct id to use in the find
    CGlxStringItem* dummyItem = new (ELeave) CGlxStringItem(aId);
    
    // Call Find
    TInt index = iStringCache.Find(dummyItem, match);
    
    // delete the dummy item
    delete dummyItem;
    
    
    if (KErrNotFound != index)
        {
        // The string was found so create a copy and pass to caller
        CGlxStringItem* item = iStringCache[index];
        return item->GetString()->Des().AllocL();
        }
    
    // String was not found    
    return NULL;
    }
    
// ----------------------------------------------------------------------------
// InsertL
// ----------------------------------------------------------------------------
//
void CGlxStringCache::InsertL(TInt aId, HBufC* aString)
    {
    TRACER("void CGlxStringCache::InsertL(TInt aId, HBufC* aString)");
    // Create a TLinearOrder to be used in the InsertInOrder
    TLinearOrder<CGlxStringCache::CGlxStringItem> orderer(&OrderById);
    
    // Create the string item
    CGlxStringItem* item = CGlxStringItem::NewL(aId, aString);
    
    // attempt to insert it
    TInt err = iStringCache.InsertInOrder(item, orderer);
    
    switch (err)
        {
    case KErrNone:
        // Do nothing as item has been inserted into cache
        break;
        
    case KErrAlreadyExists:
        // This is not an error, but we must tidy up
        delete item;
        break;
        
    default:
        // We have an error
        delete item;
        User::Leave(err);
        break;
        }
    }
    
// ----------------------------------------------------------------------------
// Compares two Id's by subtraction
// ----------------------------------------------------------------------------
//
TInt CGlxStringCache::OrderById(const CGlxStringCache::CGlxStringItem& aItem1, const CGlxStringCache::CGlxStringItem& aItem2)
    {
    TRACER("TInt CGlxStringCache::OrderById()");
    return aItem1.GetId() - aItem2.GetId();
    }
    
// ----------------------------------------------------------------------------
// Compares two Id's logical equality
// ----------------------------------------------------------------------------
//
TBool CGlxStringCache::MatchById(const CGlxStringCache::CGlxStringItem& aItem1, const CGlxStringCache::CGlxStringItem& aItem2)
    {
    TRACER("TBool CGlxStringCache::MatchById()");
    return aItem1.GetId() == aItem2.GetId();
    }
    
// ----------------------------------------------------------------------------
// Load the String
// ----------------------------------------------------------------------------
//    
EXPORT_C HBufC* CGlxStringCache::LoadLocalizedStringLC(const TDesC& aResourceFile, const TInt aResourceId)
    {
    TRACER("HBufC* CGlxCollectionPluginBase::LoadLocalizedStringLC()");
    HBufC* string = NULL;
    
    TFileName resourceFile;
    resourceFile.Copy(aResourceFile);
    
    TRAPD(err, string = LoadLocalizedStringFromDriveL(resourceFile, aResourceId));
    
    switch (err)
        {
        case KErrNone:
            break;
            
        case KErrNotFound:
            {
            // Could not find resource file on drive z so try on drive c
            _LIT(KCDrive,"c");
            resourceFile.Copy(KCDrive);
            resourceFile.Append(aResourceFile.Mid(1));
            string = LoadLocalizedStringFromDriveL(resourceFile, aResourceId);
            }
            break;
            
        default:
            {
            GLX_DEBUG2("LEAVING LoadLocalizedStringLC with %d", err);
            User::Leave(err);
            }
            break;
        }
            
    CleanupStack::PushL(string);
    
    return string;
    }
    
// ----------------------------------------------------------------------------
// Loads the String from a drive
// ----------------------------------------------------------------------------
//    
HBufC* CGlxStringCache::LoadLocalizedStringFromDriveL(const TDesC& aResourceFile, const TInt aResourceId)
    {
    TRACER("HBufC* CGlxCollectionPluginBase::LoadLocalizedStringFromDriveL()");
    
    HBufC* string = NULL;
    
	if (NULL != (string = FindL(aResourceId)))
		{
		// String has been cached to return it
		return string;
		}
			
    
    // The string has not been cached to read from resource file
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    // find out if the resource name file has been cached
    TBool resoureFileNameCached = (0 != iResourceFileName.Length());
    TFileName resFile;
    
    if (resoureFileNameCached)
        {
        // Use cached resource file name
        resFile.Copy(iResourceFileName);
        }
    else
        {
        // Caluculate the path and language extension
        TParse parse;
        parse.Set( aResourceFile, &KDC_ECOM_RESOURCE_DIR, NULL );
        resFile.Copy(parse.FullName());
        
    	TLanguage language = ELangNone;
    	BaflUtils::NearestLanguageFile( fs, resFile, language);
    	// Do not cache the file name here as it could be the wrong drive
    	// causing a leave later
        }
	
    RResourceFile resourceFile; 
    resourceFile.OpenL(fs, resFile);
    CleanupClosePushL(resourceFile);
    resourceFile.ConfirmSignatureL(0);          // must use BA_RSS_SIGNATURE in resource file (not RSS_SIGNATURE).
    
    TResourceReader resReader;
    
    HBufC8* buf = resourceFile.AllocReadLC( aResourceId ); 
    resReader.SetBuffer( buf );
    string  = resReader.ReadHBufCL();
    CleanupStack::PopAndDestroy( buf );
    
    // We have succesfully read from the resource so 
    // check to see if we need to cache the file name
    if (!resoureFileNameCached)
        {
        // Cache the resource file name
        iResourceFileName.Copy(resFile);
        }
    
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PopAndDestroy( &fs );
    
    CleanupStack::PushL(string);    
    
    InsertL(aResourceId, string); // InsertL won't create duplicates
    
    CleanupStack::Pop(string);
    
    return string;
    }

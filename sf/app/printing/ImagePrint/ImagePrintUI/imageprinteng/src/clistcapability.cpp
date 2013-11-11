/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <StringLoader.h>
#include <AknQueryValueText.h>
#include <aknpopupsettingpage.h>
#include <badesca.h>
#include <imageprintengine.rsg>

#include "clistcapability.h"
#include "crealfactory.h"
#include "mprintsettings.h"
#include "tprinter.h"
#include "mdiscoveryobserver.h"
#include "printcapabilitycodes.h"

const TInt KArrayGranularity( 4 );
const TInt KTwoPaperSizeAvailable( 2 );
const TInt KTemplateSize = 16;
const TInt KTemplates[] = {1554,1552,1565,1555,1501,1553,1551,1558,1502,1504,1564,1506,1509,1512,1513,1516};

CListCapability::CListCapability( CRealFactory* aFactory ) :
	iFactory( aFactory )
	{
	}
	
// Destructor
CListCapability::~CListCapability()
    {
    iEnumIDs.Close();
    iTexts.ResetAndDestroy();
    }

// Creates text string for the settings list box
HBufC* CListCapability::ListBoxTextL()
    {        
    CDesCArrayFlat* strings = 
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( strings );
    strings->AppendL( iTitle );

    TInt i( 0 );
    TInt num( iEnumIDs.Count() );
    TBool found( EFalse );
    // Find the matching text based on the enumeration value
    for ( i = 0; i < num && !found; i++ )
        {
        if ( iEnumIDs[i] == iValue )
            {
            strings->AppendL( iTexts[i]->Des() );
            found = ETrue;
            }
        }

    if ( !found )
        {
        User::Leave( KErrNotFound );
        }

    HBufC* buf = StringLoader::LoadL( R_QTN_LBOX_FORMAT, *strings ); 
    CleanupStack::PopAndDestroy();  // strings

    return buf;    
    }

// Displays pop-up list for changing the active TInt value
TBool CListCapability::LaunchPopupListL()
    {        
    // Create value array and sets it up
    CAknQueryValueTextArray* queryTextArray = CAknQueryValueTextArray::NewL();
    CleanupStack::PushL( queryTextArray );

    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( array );
    TInt i( 0 );
    TInt num( iTexts.Count() );
    for ( i = 0; i < num; i++ )
        {
        array->AppendL( iTexts[i]->Des() );
        }
    queryTextArray->SetArray( *array );

    CAknQueryValueText* queryValue = CAknQueryValueText::NewL();
    CleanupStack::PushL( queryValue );
	queryValue->SetArrayL( queryTextArray );

    TInt found( KErrNotFound );
    num = iEnumIDs.Count();
    for ( i = 0; i < num && found == KErrNotFound; i++ )
        {
        if ( iEnumIDs[i] == iValue )
            {            
            found = KErrNone;
            }
        }
    i--;
    User::LeaveIfError( found );    
    queryValue->SetCurrentValueIndex( i );
    	
    TBool changed = EFalse;
    if( array->MdcaCount() > KTwoPaperSizeAvailable )
        {
        // Create popup setting page and show it
        CAknPopupSettingPage* page = new ( ELeave ) CAknPopupSettingPage( 
            &iTitle, EAknSettingPageNoOrdinalDisplayed, EAknCtPopupSettingList, 
            R_EMPTY_SETTING_LIST, R_SETTING_GENERIC_PAGE, *queryValue );
        CleanupStack::PushL( page );
        page->ConstructL();
        CleanupStack::Pop();    // page

        changed = page->ExecuteLD();
        // Save new value if needed
        if ( changed )
            {                
            iValue = iEnumIDs[queryValue->CurrentValueIndex()];
            }  
        }
    // If there are only two paper sizes available, another one is automaticly changed
    else if( array->MdcaCount() == KTwoPaperSizeAvailable )
        {
        if ( i == 0 )
            {
            iValue = iEnumIDs[1];            
            }
        else
            {
            iValue = iEnumIDs[0];            
            }
        changed = ETrue;
        }
    // Do nothing
    else
        {      
        }        
        
    CleanupStack::PopAndDestroy( 3 );   // queryTextArray, array, queryArray
    return changed;
    }

// Clones itself
CBaseCapability* CListCapability::CloneL()
    {
    CListCapability* clone = new ( ELeave ) CListCapability( iFactory );
    CleanupStack::PushL( clone );
    
    clone->iValue = iValue;
    clone->iIndexOnList = iIndexOnList;
    clone->iTitle = iTitle;
    clone->iUid = iUid;

    TInt num( iEnumIDs.Count() );
	if ( iUid != EPrintCapabLayout )
		{
	    for ( TInt j = 0; j < num; j++ )
	        {
	        User::LeaveIfError( clone->iEnumIDs.Append( iEnumIDs[j] ) );
	        User::LeaveIfError( clone->iTexts.Append( iTexts[j]->AllocLC() ) );
	        CleanupStack::Pop();      // clone->iTexts
			}		
		}
	else
		{
		// 
		// has to be done here, i.e. remove not-supported templates
		TBool dropTemplates = EFalse;
		if ( iFactory )
			{		
		    if( ( iFactory->SettingsIF()->GetCurrentPrinterProtocol() == MDiscoveryObserver::EWLAN ) ||
		        ( iFactory->SettingsIF()->GetCurrentPrinterProtocol() == MDiscoveryObserver::EBPP )  ||
		        ( iFactory->SettingsIF()->GetCurrentPrinterProtocol() == MDiscoveryObserver::EUSB )  || 
		        ( iFactory->SettingsIF()->GetCurrentPrinterProtocol() == MDiscoveryObserver::EMMC ) )
		        {
		        dropTemplates = ETrue;
		        }
	    	}
	    // Also sort the print layout templates to "standard" order
	    for ( TInt i = 0; i < KTemplateSize; i++ )
	        {
		    for ( TInt j = 0; j < num; j++ )
		        {
                if ( KTemplates[i] == iEnumIDs[j] )
                	{
			        if ( (dropTemplates && (iEnumIDs[j] == EPrintCapabLayout1UpBorderless || iEnumIDs[j] == EPrintCapabLayout1UpBorder || iEnumIDs[j] == EPrintCapabLayout1Up) ) || !dropTemplates ) 
			            {
			            User::LeaveIfError( clone->iEnumIDs.Append( iEnumIDs[j] ) );
				        User::LeaveIfError( clone->iTexts.Append( iTexts[j]->AllocLC() ) ); 	
				        CleanupStack::Pop();	// clone->iTexts       	
						}
                	}
		        }
	        }
		}

    CleanupStack::Pop(); // clone
    return clone;
    }

//  End of File

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
* Description:  Interface to Browser for handling embedded system info requests.
 *
*/

// INCLUDE FILES
#include "CCPixNPPluginInterface.h"
#include "HarvesterServerLogger.h"
#include <string.h>
#include <common.h>

#include "utf.h"

#include "CCPixNPSearcher.h"
#include "CCPixNPPluginEcom.h"
#include "CPixNPExceptions.h"
#include "CMobileInfo.h"

#include "NPUtils.h"
using namespace nputils; 


// ============================ MEMBER FUNCTIONS ===============================

CCPixNPPluginInterface::~CCPixNPPluginInterface()
    {
    }

// -----------------------------------------------------------------------------

const NPUTF8 *CPixNPPluginPropertyNames[] =
	{ 
	"DOCUID",				// 0
	"APPCLASS",
	"EXCERPT",
	"MIMETYPE", 
	"AGGREGATE",			
	"DEFAULT"               // 5
	"TERM",
	"DOCFREQ",
	"ANALYZER_STANDARD",
	"TOKENIZER_STANDARD",   
	"TOKENIZER_WHITESPACE", // 10
	"TOKENIZER_LETTER",     
	"TOKENIZER_KEYWORD",
	"FILTER_STANDARD",
	"FILTER_LOWERCASE",
	"FILTER_ACCENT",        // 15
	"FILTER_STOP",
	"FILTER_STEM",
   	"FILTER_LENGTH",
   	"LANG_EN",
  	"LANG_FI",              // 20
  	"LANG_HU",
  	"LANG_RU"
    };



typedef const TDesC& (CCPixNPPluginInterface::*CPixNPPluginGetPropertyMethod)() const;

const CPixNPPluginGetPropertyMethod CPixNPPluginGetPropertyMethods[] =
	{ 
	&CCPixNPPluginInterface::DocUidFieldName,		// 0
	&CCPixNPPluginInterface::AppClassFieldName,
	&CCPixNPPluginInterface::ExcerptFieldName,
	&CCPixNPPluginInterface::MimeTypeFieldName, 
	&CCPixNPPluginInterface::AggregateFieldName, 
	&CCPixNPPluginInterface::DefaultFieldName,	    // 5
	&CCPixNPPluginInterface::TermTextFieldName,
	&CCPixNPPluginInterface::TermDocFreqFieldName, 
	&CCPixNPPluginInterface::StandardAnalyzerId,
	&CCPixNPPluginInterface::StandardTokenizerId, 
	&CCPixNPPluginInterface::WhitespaceTokenizerId,  // 10
	&CCPixNPPluginInterface::LetterTokenizerId,
	&CCPixNPPluginInterface::KeywordTokenizerId, 
	&CCPixNPPluginInterface::StandardFilterId,
	&CCPixNPPluginInterface::LowercaseFilterId, 
	&CCPixNPPluginInterface::AccentFilterId, 		// 15
	&CCPixNPPluginInterface::StopFilterId, 
	&CCPixNPPluginInterface::StemFilterId, 
	&CCPixNPPluginInterface::LengthFilterId, 
	&CCPixNPPluginInterface::EnglishLanguageId, 
	&CCPixNPPluginInterface::FinnishLanguageId, 		// 20
	&CCPixNPPluginInterface::HungarianLanguageId, 
	&CCPixNPPluginInterface::RussianLanguageId, 
    };


const NPUTF8 *CPixNPPluginMethodNames[] =
	{
	"createSearcher",
	"defineVolume",
	"undefineVolume",
    "openContact",
    "openMessaging",
    "openCalendar",
    "openMaps",
    "getCountryCode",
    "getVersionNumber",
    "openNote",
    "getDriveFromMediaId"
	};

void CCPixNPPluginInterface::InitInterfaceL()
    {
    SetIdentifiersL( CPixNPPluginPropertyNames, sizeof(CPixNPPluginPropertyNames)/sizeof(CPixNPPluginPropertyNames[0]),
    				 CPixNPPluginMethodNames, sizeof(CPixNPPluginMethodNames)/sizeof(CPixNPPluginMethodNames[0]) );
    }

// -----------------------------------------------------------------------------

bool CCPixNPPluginInterface::InvokeL(NPIdentifier name, NPVariant* args, uint32_t argCount, NPVariant *result)
    {
    VOID_TO_NPVARIANT( *result ); // sets the result empty
    
    if ( name == iMethodIdentifiers[0] ) // createSearcher
        {
        if ( argCount == 1 ) 
        	{
            CPIXLOGSTRING("CCPixNPPluginInterface::InvokeL(1) : Requesting search server session");
			TRAPD(err, 
				HBufC16* baseAppClass = VariantToHBufC16LC( args[0] );
				
				CPixNPSearcherObject *object = (CPixNPSearcherObject *)NPN_CreateObject (iInstanceHandle, CPixNPSearcherClass);
				CCPixNPSearcher* customObject = CreateSearcherL( *baseAppClass  );
				customObject->SetInstance( iInstanceHandle, &object->object );
				object->plugin = customObject;
				OBJECT_TO_NPVARIANT( (NPObject*)object, *result );
				
				CleanupStack::PopAndDestroy( baseAppClass ); 
				);
			// TODO: Report err
        	} 
        else if ( argCount == 2 ) 
        	{ 
            CPIXLOGSTRING("CCPixNPPluginInterface::InvokeL(2) : Requesting search server session");
			TRAPD(err, 
				HBufC16* baseAppClass = VariantToHBufC16LC( args[0] );
				HBufC16* defaultField = VariantToHBufC16LC( args[1] );
				
				CPixNPSearcherObject *object = (CPixNPSearcherObject *)NPN_CreateObject (iInstanceHandle, CPixNPSearcherClass);
				CCPixNPSearcher* customObject = CreateSearcherL( *baseAppClass, *defaultField );
				customObject->SetInstance( iInstanceHandle, &object->object );
				object->plugin = customObject;
				OBJECT_TO_NPVARIANT( (NPObject*)object, *result );
				
				CleanupStack::PopAndDestroy( defaultField ); 
				CleanupStack::PopAndDestroy( baseAppClass );
				);
			// TODO Report err
        	}
        else 
        	{
        	// FIXME: Doesn't work
        	NPString string; 
        	// ugly conversion? 
        	string.UTF8Characters = reinterpret_cast<const char*>(KCPixNPNotEnoughArgumentsException().Ptr());
        	string.UTF8Length = KCPixNPNotEnoughArgumentsException().Length(); 
        	//NPN_SetException( this->iNpObject, &string );
        	}		
        return true;
        }
    else if ( name == iMethodIdentifiers[1] ) // defineVolume
        {
        TRAPD( err, InvokeDefineVolumeL( name, args, argCount, result ) );
        if ( KErrNone == err )
            {
            return true; // TODO Report err
            }
        return true;
        }
    else if ( name == iMethodIdentifiers[2] ) // undefineVolume
        {
        TRAPD( err, InvokeUnDefineVolumeL( name, args, argCount, result ) );
        if ( KErrNone == err )
            {
            return true; // TODO Report err
            }
        return true;
        }
    else if ( name == iMethodIdentifiers[3] ) // openContact
        {
    	if (argCount >= 1) 
    		{
			TInt contactId = VariantToInt( args[0] );
			TRAPD(err, OpenContactL(contactId));
			// TODO: report err
    		}
		// TODO: Report err
        return true;
        }    
    else if ( name == iMethodIdentifiers[4] ) // openMessaging
        {
    	if (argCount >= 1) 
    		{
			TInt mesId = VariantToInt( args[0] );
			TRAPD(err, OpenMessagingL(mesId));
			// TODO: report err
    		}
		// TODO: Report err
        return true;
        }    
    else if ( name == iMethodIdentifiers[5] ) // openCalendar
        {
    	if (argCount >= 1) 
    		{
			TInt calendarId = VariantToInt( args[0] );
			TRAPD(err, OpenCalendarL(calendarId));
			// TODO: report err
    		}
		// TODO: Report err
        return true;
        }        
    else if ( name == iMethodIdentifiers[6] ) // openMaps
        {
        if (argCount >= 2) 
            {
            TReal latitude = VariantToDouble( args[0] );
            TReal longitude = VariantToDouble( args[1] );
            TRAPD(err, OpenMapsL(latitude, longitude));
            // TODO: report err
            }
        // TODO: Report err
        return true;
        }        
    else if ( name == iMethodIdentifiers[7] ) // getCountryCode
        {
		TInt mccInt(0);
		TBuf<10> mcc; //MCC code has 3 digits 
		TRAPD(err, CMobileInfo::GetInfoL(mcc));
		if (KErrNone == err)
			{
			TLex lex(mcc);
			lex.Val(mccInt);
			}
		INT32_TO_NPVARIANT(mccInt, *result); 
        return true;
        }
    else if ( name == iMethodIdentifiers[8] ) // getVersionNumber
        {
		TInt versionNumber(0);
		versionNumber = GetVersionNumber();
		INT32_TO_NPVARIANT(versionNumber, *result); 
        return true;
        }
	else if ( name == iMethodIdentifiers[9] ) // openNote
        {
        if (argCount >= 1) 
            {
            TInt noteId = VariantToInt( args[0] );
            TRAPD(err, OpenNoteL(noteId));
            // TODO: report err
            }
        // TODO: Report err
        return true;
        } 
	else if( name == iMethodIdentifiers[10] ) //getDriveFromMediaId
	    {
	    if (argCount >= 1) 
            {
	        //Get arg[0] convert from Variant to Descriptor and get the drive
	        HBufC16* mediaId = VariantToHBufC16LC( args[0] );
	        TBuf<5> bufMedia;
	        GetDriveFromMediaIdL(mediaId,bufMedia);
	        DescriptorToVariantL(bufMedia,*result);
	        //Convert bufMedia to variant
	        CleanupStack::PopAndDestroy(mediaId);
            }
        // TODO: Report err
        return true;
	    }
    return false;
    }

// -----------------------------------------------------------------------------



bool CCPixNPPluginInterface::GetProperty (NPIdentifier name, NPVariant *variant)
    {
    // default variant value maps to javascript undefined
    VOID_TO_NPVARIANT(*variant);
    
	for (int i = 0; i < iMethodCount; i++) 
		{
		if ( name == iPropertyIdentifiers[i] ) 
			{
			TRAP_IGNORE(
				CPixNPPluginGetPropertyMethod method = CPixNPPluginGetPropertyMethods[i];
				DescriptorToVariantL((*this.*method)(), *variant);
			);
			return true; 
			}
		}
	
    return false;
    }

// -----------------------------------------------------------------------------

bool CCPixNPPluginInterface::SetPropertyL( NPIdentifier /*name*/, NPVariant* /*variant*/ )
    {
    return false;
    }


// -----------------------------------------------------------------------------
// NPClass Function

NPObject *CPixPluginAllocate ()
    {
    CPixPluginObject *newInstance = (CPixPluginObject *)User::Alloc (sizeof(CPixPluginObject));       
    return (NPObject *)newInstance;
    }

void CPixPluginInvalidate ()
    {
    
    }

void CPixPluginDeallocate (CPixPluginObject* obj) 
    {    
    obj->plugin->Deallocate();
    User::Free ((void *)obj);
    }    
 
bool CPixPluginHasMethod(CPixPluginObject* obj, NPIdentifier name)
    {
    return obj->plugin->HasMethod(name);
    }

bool CPixPluginInvokeFunctionL(CPixPluginObject* obj, NPIdentifier name, NPVariant *args, uint32_t argCount, NPVariant *result)
    {
    return obj->plugin->InvokeL(name, args, argCount, result);
    }    

bool CPixPluginHasProperty(CPixPluginObject* obj, NPIdentifier name)
    {
    return obj->plugin->HasProperty(name);
    }    
    
bool CPixPluginGetProperty (CPixPluginObject* obj, NPIdentifier name, NPVariant *variant)
    {
    return obj->plugin->GetProperty(name,variant);
    }

void CPixPluginSetProperty (CPixPluginObject* obj, NPIdentifier name, NPVariant *variant)
    {
    TRAP_IGNORE(obj->plugin->SetPropertyL(name,variant));
    }

void CCPixNPPluginInterface::InvokeDefineVolumeL(NPIdentifier /*name*/, NPVariant* args, uint32_t argCount, NPVariant */*result*/)
    {
    if ( 2 == argCount )
        {
        HBufC16* baseAppClass = VariantToHBufC16LC( args[0] );
        HBufC16* indexDbPath = VariantToHBufC16LC( args[1] );

        if ( iInstanceHandle )
            {
            DefineVolumeL( *baseAppClass, *indexDbPath );
            }

        CleanupStack::PopAndDestroy( indexDbPath );
        CleanupStack::PopAndDestroy( baseAppClass );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

void CCPixNPPluginInterface::InvokeUnDefineVolumeL(NPIdentifier /*name*/, NPVariant* args, uint32_t argCount, NPVariant */*result*/)
    {
    if ( 1 == argCount )
        {
        HBufC16* baseAppClass = VariantToHBufC16LC( args[0] );

        if ( iInstanceHandle )
            {
            UnDefineVolumeL( *baseAppClass );
            }

        CleanupStack::PopAndDestroy( baseAppClass );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

//  End of File

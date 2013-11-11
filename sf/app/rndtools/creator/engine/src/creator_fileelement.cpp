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



#include "creator_fileelement.h"
#include <DrmPermission.h>
#include <DrmConstraint.h>
#include "creator_traces.h"
#include "creator_file.h"

using namespace creatorfile;

// 15 minutes per time zone, 60 seconds per minute
const TInt KSecondsPerTimeZone = 900;
const TInt KMinuteInMicroseconds = 60000000;
const TInt KTimeZoneIncrement = 15;

/*
 * 
 */
CCreatorFileElement* CCreatorFileElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext )
    {
    CCreatorFileElement* self = new (ELeave) CCreatorFileElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop(self);
    return self;
    }
/*
 * 
 */
CCreatorFileElement::CCreatorFileElement(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    {
    iIsCommandElement = ETrue;

    TTime currUniversal;
    TTime currLocal;
    TInt64 result( 0 );
    currUniversal.UniversalTime();
    currLocal.HomeTime();
    result = currLocal.Int64() - currUniversal.Int64();
    result /= KMinuteInMicroseconds;
    result /= KTimeZoneIncrement;
    iTimeZone = I64INT( result );
    }
/*
 * 
 */
void CCreatorFileElement::ExecuteCommandL()
    {
    const CCreatorScriptAttribute* amountAttr = FindAttributeByName(KAmount);
    TInt fileAmount = 1;    
    if( amountAttr )
        {
        fileAmount = ConvertStrToIntL(amountAttr->Value());
        }
    // Get 'fields' element 
    CCreatorScriptElement* fieldsElement = FindSubElement(KFields);
    if( fieldsElement && fieldsElement->SubElements().Count() > 0)
        {
        // Get sub-elements
        const RPointerArray<CCreatorScriptElement>& fields = fieldsElement->SubElements();
        // Create files, the amount of files is defined by fileAmount:
        for( TInt cI = 0; cI < fileAmount; ++cI )
            {
            CFilesParameters* param = new (ELeave) CFilesParameters;
            CleanupStack::PushL( param );
            
            for( TInt i = 0; i < fields.Count(); ++i )
                {
                CCreatorScriptElement* field = fields[i];
                TPtrC elemName = field->Name();
                TPtrC elemContent = field->Content();

                const CCreatorScriptAttribute* randomAttr = field->FindAttributeByName( KRandomLength );
                
                if( elemName == KType )
                    {
                    param->iFileCommand = GetFileCommandL(elemContent, randomAttr || elemContent.Length() == 0);
                    }
                else if( elemName == KDirectory )
                    {
                    delete param->iFullFilePath;
                    param->iFullFilePath = 0;
                    if( randomAttr == 0 && elemContent.Length() > 0 )
                    	{
                    	param->iFullFilePath = elemContent.AllocL();
                    	}
                    else
                        {
                        TFileName defaultPath;
                        iEngine->SetDefaultPathForFileCommandL( param->iFileCommand, defaultPath );
                        param->iFullFilePath = defaultPath.AllocL();
                        }
                    }
                else if ( elemName == KEncryption )
                    {
                    ParseDRMElementsL( field, param );
                    }
                }

            if ( !param->iFileCommand )
                {
                param->iFileCommand = ECmdCreateFileEntryEmptyFolder;
                }
            iEngine->AppendToCommandArrayL( param->iFileCommand, param);
            CleanupStack::Pop( param );
            }
        }
    else
    	{
    	_LIT(KDummy, "");
    	for( TInt cI = 0; cI < fileAmount; ++cI )
    		{
    		CFilesParameters* param = new (ELeave) CFilesParameters;
    		CleanupStack::PushL(param);
    		delete param->iFullFilePath;
    		param->iFullFilePath = 0;
    		TFileName defaultPath;
    		param->iFileCommand = GetFileCommandL(KDummy, ETrue);
    		iEngine->SetDefaultPathForFileCommandL(param->iFileCommand, defaultPath);                
    		param->iFullFilePath = defaultPath.AllocL();
    		
    		iEngine->AppendToCommandArrayL(param->iFileCommand, param);
    		CleanupStack::Pop(param);
    		}
    	}
    }

TInt CCreatorFileElement::GetFileCommandL( const TDesC& aFileIdStr, TBool aRandom ) const
    {
    TInt mapCount = sizeof(FileMap) / sizeof(FileMapping);
    
    if( aRandom )
        {
        return FileMap[iEngine->RandomNumber(0, mapCount-1)].CommandId();
        }
    
    for( TInt i = 0; i < mapCount; ++i )
        {
        if( CompareIgnoreCase(FileMap[i].FileName(), aFileIdStr) == 0 )
            return FileMap[i].CommandId();
        }
    LOGSTRING2("CCreatorFileElement::GetFileCommandL: Unknown file id: %S", &aFileIdStr);
    User::Leave(KErrGeneral);
    return 0; // Not reached, but disables compiler warning...
    }

void CCreatorFileElement::ParseDRMElementsL( CCreatorScriptElement* aField, CFilesParameters* aParam )
    {
    const CCreatorScriptAttribute* encryptionTypeAttr = aField->FindAttributeByName( KType );
    if ( encryptionTypeAttr && encryptionTypeAttr->Value().Length() > 0 )
        {
        aParam->iEncrypt = ETrue;
        if ( encryptionTypeAttr->Value() == KDRMCD ) 
            {
            delete aParam->iPermission;
            aParam->iPermission = NULL;
            // ownership transferred:
            aParam->iPermission = CDRMPermission::NewL();
            }
        else if ( encryptionTypeAttr->Value() != KDRMFL )
            {
            // must be either KDRMCD or KDRMFL
            User::Leave( KErrArgument );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    const RPointerArray<CCreatorScriptElement>& rights = aField->SubElements();
    
    if ( encryptionTypeAttr->Value() == KDRMFL && rights.Count() )
        {
        // Right definitions not allowed with DRM Forward Lock
        // User::Leave( KErrArgument ); // Do not leave here
        return; // Just omit rights definitions
        }
    
    // parse right elements
    CDRMPermission* drmPermission = aParam->iPermission;
    for ( TInt r = 0; r < rights.Count(); r++ )
        {
        const CCreatorScriptAttribute* rightAttr = rights[r]->FindAttributeByName( KType );
        if ( rightAttr->Value() == KDRMPlayRight &&
             !( drmPermission->iAvailableRights & ERightsPlay ) )
            {
            drmPermission->iAvailableRights |= ERightsPlay;
            ParseDRMConstraintsL( rights[r], drmPermission->iPlay );
            }
        else if ( rightAttr->Value() == KDRMDisplayRight  &&
                !( drmPermission->iAvailableRights & ERightsDisplay ))
            {
            drmPermission->iAvailableRights |= ERightsDisplay;
            ParseDRMConstraintsL( rights[r], drmPermission->iDisplay );
            }
        else if ( rightAttr->Value() == KDRMPrintRight  &&
                !( drmPermission->iAvailableRights & ERightsPrint ) )
            {
            drmPermission->iAvailableRights |= ERightsPrint;
            ParseDRMConstraintsL( rights[r], drmPermission->iPrint );
            }
        else if ( rightAttr->Value() == KDRMExecuteRight  &&
                !( drmPermission->iAvailableRights & ERightsExecute ) )
            {
            drmPermission->iAvailableRights |= ERightsExecute;
            ParseDRMConstraintsL( rights[r], drmPermission->iExecute );
            }
        }
    }

void CCreatorFileElement::ParseDRMConstraintsL( CCreatorScriptElement* aRight, CDRMConstraint* aConstraint )
    {
    const RPointerArray<CCreatorScriptElement>& constraints = aRight->SubElements();
    for ( TInt c = 0; c < constraints.Count(); c++ )
        {
        TPtrC elemName = constraints[c]->Name();
        TPtrC elemContent = constraints[c]->Content();
        if ( elemName == KDRMCount )
            {
            TInt count = ConvertStrToIntL( elemContent );
            // apply count constraint
            aConstraint->iActiveConstraints |= EConstraintCounter;
            aConstraint->iCounter = count;
            aConstraint->iOriginalCounter = count;
            }
        else if ( elemName == KDRMInterval )
            {
            // apply interval constraint           
            aConstraint->iActiveConstraints |= EConstraintInterval;
            aConstraint->iInterval = ParseTimeInterval( elemContent );
            aConstraint->iIntervalStart = Time::NullTTime();
            }
        else if ( elemName == KDRMStartTime )
            {
            // apply start time constraint           
            aConstraint->iActiveConstraints |= EConstraintStartTime;
            TTimeIntervalSeconds offset( iTimeZone * KSecondsPerTimeZone );
            aConstraint->iStartTime = ConvertToDateTimeL( elemContent ) - offset;
            }
        else if ( elemName == KDRMEndTime )
            {
            // apply end time constraint
            aConstraint->iActiveConstraints |= EConstraintEndTime;
            TTimeIntervalSeconds offset( iTimeZone * KSecondsPerTimeZone );
            aConstraint->iEndTime = ConvertToDateTimeL( elemContent ) - offset;
            }
        else if ( elemName == KDRMAccumulated )
            {
            // apply accumulated time constraint
            aConstraint->iActiveConstraints |= EConstraintAccumulated;
            if ( aConstraint->iEndTime == Time::NullTTime() )
                {
                aConstraint->iEndTime = Time::MaxTTime();
                }
            if ( aConstraint->iStartTime == Time::NullTTime() )
                {
                aConstraint->iStartTime = Time::MinTTime();
                }
            aConstraint->iAccumulatedTime = ParseTimeInterval( elemContent );
            }
        }
    }

TTimeIntervalSeconds CCreatorFileElement::ParseTimeInterval( TDesC& aTimeString )
    {
    // Iso8601 format (P1Y2M3DT4H5M6S) to TimeIntervalSeconds
    TLex lex;
    TInt year = 0;
    TInt month = 0;
    TInt day = 0;
    TInt hour = 0;
    TInt minute = 0;
    TInt second = 0;

    if (aTimeString.Length() > 0)
        {    
        lex = aTimeString;
        lex.Inc(); // skip 'P'
        lex.Val(year);
        lex.Inc();
        lex.Val(month);
        lex.Inc();
        lex.Val(day);
        lex.Inc(2);
        lex.Val(hour);
        lex.Inc();
        lex.Val(minute);
        lex.Inc();
        lex.Val(second);
        }
    TTimeIntervalSeconds result( ( ( ( year * 365 + month * 30 + day ) * 24 + hour )
                * 60 + minute ) * 60 + second );
    return result;
    }

// End of file

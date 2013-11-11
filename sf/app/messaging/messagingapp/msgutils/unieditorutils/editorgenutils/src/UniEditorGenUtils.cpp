/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: General utilities for unified editor and plugin
 *
 */

// INCLUDE FILES
#include <centralrepository.h>          // CRepository
#include <CoreApplicationUIsSDKCRKeys.h>  // offline CR keys
#include <rcustomerserviceprofilecache.h>
#include <MmsEngineDomainCRKeys.h>
#include <MsgMediaResolver.h>
#include <DRMHelper.h>
#include <ccsdefs.h> // for KDefaultGsmNumberMatchLength
#include <telconfigcrkeys.h> // for KCRUidTelephonyConfiguration

#include "MessagingVariant.hrh"
#include "MessagingInternalCRKeys.h"  // Keys
#include "UniEditorGenUtils.h"
#include <xqconversions.h>

// CONSTANTS
const TInt KMuiuCharQuote = '\"';
const TInt KMuiuCharBackSlash = '\\';
const TInt KMuiuCharDot = '.';
const TInt KMuiuCharSpace = ' ';
const TInt KMuiuCharDel = 127;
const TInt KMuiuCharAt = '@';
const TInt KMuiuSpecialCharStrLen = 12;
_LIT( KRFC822Specials,"()<>@,;:\\\"[]");

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// UniEditorGenUtils::UniEditorGenUtils
// @see header
// ----------------------------------------------------
UniEditorGenUtils::UniEditorGenUtils() :
mAbsMaxConcatenatedSms(-1),
mAbsMaxSmsCharacters(-1),
mMaxMmsSize(-1),
mMatchDigitCount(-1)
{

}

// ----------------------------------------------------
// UniEditorGenUtils::~UniEditorGenUtils
// @see header
// ----------------------------------------------------
UniEditorGenUtils::~UniEditorGenUtils()
{

}
// ----------------------------------------------------
// UniEditorGenUtils::ReadEmailOverSmsSettingsL
// @see header
// ----------------------------------------------------
TInt UniEditorGenUtils::ReadEmailOverSmsSettingsL(
    TDes& aSmsc,
    TDes& aDestinationAddress,
    TBool& aModifiable )
    {
    // Create storage
    CRepository* storage = CRepository::NewLC( KCRUidSmum );
    storage->Get( KSumEmailSC, aSmsc );
    storage->Get( KSumEmailGateway, aDestinationAddress );
    storage->Get( KSumEmailModifiable, aModifiable );
    CleanupStack::PopAndDestroy(); // storage
    return KErrNone;
    }

// ----------------------------------------------------
// UniEditorGenUtils::WriteEmailOverSmsSettingsL
// @see header
// ----------------------------------------------------
TInt UniEditorGenUtils::WriteEmailOverSmsSettingsL(
    const TDes& aSmsc,
    const TDes& aDestinationAddress,
    const TBool& aModifiable )
    {
    // Create storage
    CRepository* storage = CRepository::NewLC( KCRUidSmum );
    storage->Set( KSumEmailSC, aSmsc );
    storage->Set( KSumEmailGateway, aDestinationAddress );
    storage->Set( KSumEmailModifiable, aModifiable );
    CleanupStack::PopAndDestroy(); // storage
    return KErrNone;
    }

// ---------------------------------------------------------
// UniEditorGenUtils::IsPhoneOfflineL
// @see header
// ---------------------------------------------------------
TBool UniEditorGenUtils::IsPhoneOfflineL()
    {
    TInt connAllowed ( 1 );
    CRepository* repository ( CRepository::NewL( KCRUidCoreApplicationUIs ) );
    TInt err = repository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed );
    delete repository;
    repository = NULL;
    if ( !err && !connAllowed )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------
// UniEditorGenUtils::AcceptEmailAddressesL
// @see header
// ----------------------------------------------------
TBool UniEditorGenUtils::AcceptEmailAddressesL()
{
	CRepository* repository ( CRepository::NewL( KCRUidMuiuVariation ) );
	TInt features = 0;
    TBool emailOverSmsVariationOn = false;
    TBool acceptEmailAddresses = false;

	 if ( repository->Get( KMuiuSmsFeatures, features ) == KErrNone )
	         {
	         if ( features & KSmsFeatureIdEmailOverSms )
	             {
	             emailOverSmsVariationOn = ETrue;
	             }
	         }

	 delete repository;

	 //TODO chk with Jerry if below is needed
	 RCustomerServiceProfileCache* cspProfile = new (ELeave) RCustomerServiceProfileCache;
     TInt error = cspProfile->Open();

     if ( error == KErrNone )
         {
         if ( emailOverSmsVariationOn )
             {

             // EmailOverSms bit was variated ON so let's check the bit from SIM
             // Get tele services flags from CSP
             RMobilePhone::TCspValueAdded params;
             // Read the CPHS bit so we know if EmailOverSms is supported
             error = cspProfile->CspCPHSValueAddedServices( params );


             if ( error == KErrNone &&
                  params >= 0 &&
                  params & RMobilePhone::KCspSMMOEmail )
                 {
                 // It's supported
            	 acceptEmailAddresses= true;
                 }
             }
         }
     cspProfile->Close();
     delete cspProfile;

     return acceptEmailAddresses;
}

// ----------------------------------------------------
// UniEditorGenUtils::VerifyEmailAddressesL
// @see header
// ----------------------------------------------------
TBool UniEditorGenUtils::VerifyEmailAddressesL( ConvergedMessageAddressList addrList)
{
    TBool emailAddrPresent =  EFalse;
    int addrCount = addrList.count();
    for(int i=0; i< addrCount; i++)
    {
        QString addr = addrList.at(i)->address();
        // check if email address, contains at least 3 characters
        if(addr.size() >= 3)
        {
            // search for @ from the address
            // however, it can't be the first or the last item
            for(int i = 1; i < addr.size() - 1; i++)
            {
                if(addr.at(i) == '@')
                {
                    emailAddrPresent = ETrue;
                    break;
                }
            }
        }
    }
    return emailAddrPresent;
}

// ----------------------------------------------------
// UniEditorGenUtils::UTF8Size
// @see header
// ----------------------------------------------------
TInt UniEditorGenUtils::UTF8Size( QString aText )
    {
    HBufC* text = XQConversions::qStringToS60Desc(aText);
    TPtrC ptr = text->Des();
    
    TInt count = 0;
    TInt sizeInBytes = 0;
    TUint16 charValue;
    while ( count < ptr.Length() )
        {
        charValue = ptr[count];
        if ( charValue < 0x80 )
            {
            sizeInBytes += 1;
            }
        else if ( charValue < 0x800 )
            {
            sizeInBytes += 2;
            }
        else //if ( charValue < 0x10000 )
            {
            sizeInBytes += 3;
            }
        count++;
        }
    return sizeInBytes;
    }

// ----------------------------------------------------
// UniEditorGenUtils::MaxSmsRecipientsL
// @see header
// ----------------------------------------------------
TInt UniEditorGenUtils::MaxSmsRecipientsL()
{
	CRepository* repository ( CRepository::NewL( KCRUidUniEditor ) );
	TInt maxSmsRecipients = KDefaultSmsRecipients;
	
    if ( (repository->Get( KUniEditorSoftLimitRecipientCount, maxSmsRecipients ) 
    		!= KErrNone) ||	(maxSmsRecipients <= 0) )
        {
        // Unreasonable count, change it back to default value
    	maxSmsRecipients = KDefaultSmsRecipients;
        }
    delete repository;
    
    return maxSmsRecipients;
}

// ----------------------------------------------------
// UniEditorGenUtils::MaxMmsRecipientsL
// @see header
// ----------------------------------------------------
TInt UniEditorGenUtils::MaxMmsRecipientsL()
{
	CRepository* repository ( CRepository::NewL( KCRUidUniEditor ) );
	TInt maxMmsRecipients = KDefaultMmsRecipients;
	
    if ( (repository->Get( KUniEditorMaxRecipientCount, maxMmsRecipients )
    		!= KErrNone) || (maxMmsRecipients <= 0))
        {
        // Unreasonable count, change it back to default value
    	maxMmsRecipients = KDefaultMmsRecipients;
        }
    delete repository;

    return maxMmsRecipients;
}

// ----------------------------------------------------
// UniEditorGenUtils::MaxMmsMsgSizeL
// @see header
// ----------------------------------------------------
TInt UniEditorGenUtils::MaxMmsMsgSizeL()
{
    // optimized, to read only once from CR
    if(mMaxMmsSize == -1)
    {// not read yet, so read it now
        mMaxMmsSize = KDefaultMaxSize;
    
        CRepository* repository = CRepository::NewL( KCRUidMmsEngine );
        CleanupStack::PushL( repository );
    
        repository->Get( KMmsEngineMaximumSendSize, mMaxMmsSize );
    
        CleanupStack::PopAndDestroy( repository );
    }
    
    return mMaxMmsSize;
}

// ----------------------------------------------------
// UniEditorGenUtils::getFileInfoL
// @see header
// ----------------------------------------------------
void UniEditorGenUtils::getFileInfoL(QString filePath,
                                    int& size,
                                    QString& mimetype,
                                    TMsgMediaType& mediaType)
{
    HBufC* filepath = XQConversions::qStringToS60Desc(filePath);
    int fileSize = 0;
    
	CMsgMediaResolver* mediaResolver;
	
    if(filepath)  
    { 
    CleanupStack::PushL(filepath);     
    
    mediaResolver = CMsgMediaResolver::NewLC();
    
    RFile file = mediaResolver->FileHandleL(*filepath);    
    file.Size(fileSize);        
    fileSize+= KEstimatedMimeHeaderSize;
    size = fileSize;
    TDataType datatype;
    mediaResolver->RecognizeL( file, datatype );
    mimetype = XQConversions::s60Desc8ToQString(datatype.Des8());
    mediaType = mediaResolver->MediaType(datatype.Des8());
    
    CleanupStack::PopAndDestroy(mediaResolver);
    CleanupStack::PopAndDestroy(filepath);
    }
    
    return;
}

// ----------------------------------------------------
// UniEditorGenUtils::MaxSmsMsgSizeL
// @see header
// ----------------------------------------------------
int UniEditorGenUtils::MaxSmsMsgSizeL(bool unicode)
{
    int maxLength = 0;
    int lengthOne = 0;
    int lengthMany = 0;
    
    getSmsCharacterLimits(lengthOne, lengthMany, unicode);
    maxLength = lengthOne;

    int maxSmsParts = absoluteMaxSmsPartsL();    
    if(maxSmsParts > 1)
        {
        maxLength = maxSmsParts * lengthMany;
        }
/* UniEditor soft limit, may not be needed    
    int maxSmsCharacters = absoluteMaxSmsCharactersL();
    if(maxSmsCharacters > 0)
        {
        maxLength = maxSmsCharacters;
        }
*/
    return maxLength;
}

// ----------------------------------------------------
// UniEditorGenUtils::getSmsCharacterLimits
// @see header
// ----------------------------------------------------
void UniEditorGenUtils::getSmsCharacterLimits(int& singlePartLength,
                                        int& concatenatedPartLength,
                                        bool unicode)
    {
    if(unicode)
        {
        singlePartLength = KFirstUnicodeSmsLength;
        concatenatedPartLength = KUnicodeConcatenationInterval;
        }
    else
        {
        singlePartLength = KFirstNormalSmsLength;
        concatenatedPartLength = KNormalConcatenationInterval;
        }
    }

// ----------------------------------------------------
// UniEditorGenUtils::absoluteMaxSmsPartsL
// @see header
// ----------------------------------------------------
int UniEditorGenUtils::absoluteMaxSmsPartsL()
{
    // optimized, to read only once from CR
    if(mAbsMaxConcatenatedSms == -1)
    { // not read yet, so read it now
        mAbsMaxConcatenatedSms = KDefaultMaxSmsSize;
        CRepository* repository = CRepository::NewL( KCRUidSmum );
        CleanupStack::PushL( repository );

        if ( repository->Get( KSmumMaxSubMsgCount, mAbsMaxConcatenatedSms ) ||
                mAbsMaxConcatenatedSms < 1 || 
                mAbsMaxConcatenatedSms > KMaxSmsSizeByStandard )
        {
            // Unreasonable count, change it back to 30 ( S60 default )
            mAbsMaxConcatenatedSms = KMaxSmsSizeByStandard;
        }
        CleanupStack::PopAndDestroy( repository );
    }
    return mAbsMaxConcatenatedSms;
}

// ----------------------------------------------------
// UniEditorGenUtils::absoluteMaxSmsCharactersL
// @see header
// ----------------------------------------------------
int UniEditorGenUtils::absoluteMaxSmsCharactersL()
{
    // optimized, to read only once from CR
    if(mAbsMaxSmsCharacters == -1)
    {// not read yet, so read it now
        CRepository* repository = CRepository::NewL( KCRUidUniEditor );
        CleanupStack::PushL( repository );
        repository->Get( KUniEditorMaxSmsCharacterCount, mAbsMaxSmsCharacters );
        CleanupStack::PopAndDestroy( repository );    
    }
    return mAbsMaxSmsCharacters;
}

// ---------------------------------------------------------
// UniEditorGenUtils::ConvertDigitsTo
// @see header
// ---------------------------------------------------------
void UniEditorGenUtils::ConvertDigitsTo( TDes& aDes, TDigitType aDigitType )
    {
    TChar toArea = aDigitType;
    TInt length = aDes.Length();
    for(int i=0; i<length; i++)
        {
        TChar ch = aDes[i];
        TChar base = NumberToBase(ch);
        switch (base)
            {
            case EDigitTypeWestern:
            case EDigitTypeArabicIndic:
            case EDigitTypeEasternArabicIndic:
            case EDigitTypeDevanagari:
            case EDigitTypeThai:
                ch += toArea - base;
                aDes[i] = TUint16(ch);
                break;
            default:
                break;
            };
        }
    }

// ---------------------------------------------------------
// UniEditorGenUtils::NumberToBase
// @see header
// ---------------------------------------------------------
TChar UniEditorGenUtils::NumberToBase(TChar ch)
    {
    TDigitType d[] = { EDigitTypeWestern, EDigitTypeArabicIndic, EDigitTypeEasternArabicIndic, EDigitTypeDevanagari, EDigitTypeThai };
    TInt i = 0;
    TInt num = sizeof(d)/sizeof(d[0]);
    while(i<num)
        {
        if (ch>TChar(d[i]) && ch<TChar(d[i]+10)) { return d[i]; }
        i++;
        }
    return ch;
    }

// ---------------------------------------------------------
// UniEditorGenUtils::ReplaceCharacters
// @see header
// ---------------------------------------------------------
void UniEditorGenUtils::ReplaceCharacters(TDes &aDes, const TDesC &aChars, TChar aReplacement)
    {
    TInt src = 0;
    TInt srclength = aDes.Length();
    while(src < srclength)
        {
    TChar c = aDes[src];
    if (aChars.LocateF(c) != KErrNotFound)
        aDes[src] = TUint16(aReplacement);
    ++src;
        }
    }

// ----------------------------------------------------
// UniEditorGenUtils::IsValidEmailAddressL
// @see header
// ----------------------------------------------------
TBool UniEditorGenUtils::IsValidEmailAddress( const TDesC& aAddress )
    {
    TInt c;
    TInt length = aAddress.Length ();
    TBufC<KMuiuSpecialCharStrLen> rfc822Specials ( KRFC822Specials );
        
    // first we validate the name portion (name@domain)
    if ( length && aAddress[0] == KMuiuCharDot )
        {
        return EFalse;
        }
    for ( c = 0 ; c < length ; c++ )
        {
        if ( aAddress[c] == KMuiuCharQuote && ( c == 0 || 
        aAddress[c-1] == KMuiuCharDot || aAddress[c-1] == KMuiuCharQuote ) )
            {
            while ( ++c < length )
                {
                if ( aAddress[c] == KMuiuCharQuote )
                    {
                    if( (c + 1) == length)
                        {
                        return EFalse;
                        }
                    break;
                    }
                if ( aAddress[c] == KMuiuCharBackSlash && 
                    ( aAddress[++c] == KMuiuCharSpace) ) 
                    {
                    continue;
                    }  
                if ( aAddress[c] <= KMuiuCharSpace || 
                    aAddress[c] >= KMuiuCharDel ) 
                    {
                    return EFalse;
                    }
                }
            if ( c++ == length )
                {
                return EFalse;
                }
            if ( aAddress[c] == KMuiuCharAt )
                {
                break;
                }
            if ( aAddress[c] != KMuiuCharDot )
                {
                return EFalse;
                }
            continue;
            }
        if ( aAddress[c] == KMuiuCharAt )
            {
            break; 
            }
        if ( aAddress[c] <= KMuiuCharSpace || aAddress[c] >= KMuiuCharDel )
            {
            return EFalse;
            }    
        if ( rfc822Specials.Locate ( aAddress[c] ) != KErrNotFound )
            {
            return EFalse;
            }
        }
    if ( c == 0 || aAddress[c-1] == KMuiuCharDot )
        {
        return EFalse;
        }
    // next we validate the domain portion (name@domain)
    if ( c == length )
        {
        return EFalse;
        }
    else
        {
        c++; 
        return IsValidDomain ( aAddress.Mid ( ( c ) , length-c ) );
        }
    }

// ----------------------------------------------------
// UniEditorGenUtils::IsValidDomainL
// @see header
// ----------------------------------------------------
TBool UniEditorGenUtils::IsValidDomain ( const TDesC& aDomain )
    {
    TInt c = 0;  
    TInt length = aDomain.Length ();
    TBufC<KMuiuSpecialCharStrLen> rfc822Specials ( KRFC822Specials );
    
    if ( length == 0 )
        {
        return EFalse;   
        }
    
    do
        {    
        if ( aDomain[c] == KMuiuCharDot )
            {
            if ( c == 0 || aDomain[c-1] == KMuiuCharDot )
                {
                return EFalse;
                }
            }
        if ( aDomain[c] <= KMuiuCharSpace || aDomain[c] >= KMuiuCharDel )
            {
            return EFalse;
            }
        if ( rfc822Specials.Locate( aDomain[c] ) != KErrNotFound )
            {
            return EFalse;
            }
        } 
    while ( ++c < length );
  
    return ( aDomain[length-1] != '.' );
    }

// ----------------------------------------------------
// UniEditorGenUtils::MatchPhoneNumber
// @see header
// ----------------------------------------------------
TBool UniEditorGenUtils::MatchPhoneNumberL(
        TDesC& aFirstNumber, TDesC& aSecondNumber)
    {
    // if matching digit count is already read from CR, then don't do that again
    if(mMatchDigitCount == -1)
        {
        mMatchDigitCount = KDefaultGsmNumberMatchLength;
        // Read the amount of digits to be used in contact matching
        // The key is owned by PhoneApp
        CRepository* repository = CRepository::NewL(KCRUidTelConfiguration);
        CleanupStack::PushL(repository);
        if (repository->Get(KTelMatchDigits, mMatchDigitCount) == KErrNone)
            {
            // Min is 7
            mMatchDigitCount = Max(mMatchDigitCount, KDefaultGsmNumberMatchLength);
            }
        CleanupStack::PopAndDestroy(); // repository
        }
    
    // start matching
    if( (aFirstNumber.Length() == 0) || (aSecondNumber.Length() == 0) )
        {
        return EFalse;
        }
    
    if (aFirstNumber.Right(mMatchDigitCount).CompareF(
            aSecondNumber.Right(mMatchDigitCount)) == 0)
        {
        return ETrue;
        }
    return EFalse;
    }

// End of file

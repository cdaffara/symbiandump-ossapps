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

#ifndef __UNIEDITOR_GEN_UTILS_H__
#define __UNIEDITOR_GEN_UTILS_H__

//  INCLUDES
#include <e32base.h>
#include <e32const.h>
#include "muiuutilsdefines.h"
#include "convergedmessage.h"
#include "MsgMedia.hrh"

const TInt KDefaultMaxSmsSize = 10;
// Length of one 7bit normal sms
const TInt KFirstNormalSmsLength = 160;
// Length of one unicode sms
const TInt KFirstUnicodeSmsLength = 70;
// Submsg length for normal 7bit sms
const TInt KNormalConcatenationInterval = 153;
// Submsg length for unicode sms
const TInt KUnicodeConcatenationInterval = 67;

const TInt KMaxSmsSizeByStandard = 30;
const TInt KDefaultSmsRecipients = 20;
const TInt KDefaultMmsRecipients = 100;
const TInt KDefaultMaxSize = 300 * 1024;
const TInt KEstimatedMmsSmilHeaderSize = 2.4 * 1024; // 1Kb buffer for mime headers + 1.4Kb for smil  
const TInt KEstimatedMimeHeaderSize = 1400; 

class CMsgMediaResolver;

// CLASS DECLARATION
class MUIU_UTILS_EXPORT UniEditorGenUtils :public CBase
    {

      public:

      explicit UniEditorGenUtils();

      ~UniEditorGenUtils();

      public:
        /**
        * Returns the current Email-over-Sms settings
        * @param aSmsc
        * @param aDestinationAddress
        * @param aModifiable
        * @return                   Symbian OS standard error code
        */
        TInt ReadEmailOverSmsSettingsL(
            TDes& aSmsc,
            TDes& aDestinationAddress,
            TBool& aModifiable );

        /**
        * Writes the current Email-over-Sms settings to file
        * @param aSmsc service centre, used for Email over Sms
        * @param aDestinationAddress service number, used for Email over Sms
        * @param aModifiable to check if user is allowed to modify setting
        * @return                   Symbian OS standard error code
        */
        TInt WriteEmailOverSmsSettingsL(
            const TDes& aSmsc,
            const TDes& aDestinationAddress,
            const TBool& aModifiable );

        /**
        * Checks if phone is in offline mode or not
        * Make sure that offline feature has been checked
        * using feature manager
        * @return ETrue if phone is in offline mode, otherwise EFalse.
        */
        TBool IsPhoneOfflineL();

        /**
         * Checks if email addresses are supported over sms message type
         * using feature manager
         * @return ETrue if emailoversms is suppoted
         */
        TBool AcceptEmailAddressesL();

        /**
         * Checks if the given addresslist contains a valid email address
         * @param addr converged message address list
         * @return ETrue if list contains a valid email address
         */
        TBool VerifyEmailAddressesL( ConvergedMessageAddressList addr);
        
       /** 
        * Gets byte-size of UTF8 formatted text
        * @param aText
        * @return Size of text in bytes
        */
        TInt UTF8Size( QString aText );
        
        /**
         * Get maximum recipient count for sms from feature manager
         * @return max recipient count
         */
        TInt MaxSmsRecipientsL();
        
        /**
         * Get maximum recipient count for mms from feature manager
         * @return max recipient count
         */
        TInt MaxMmsRecipientsL();
        
        /**
         * Calculates the size of the given message
         * @param msg ConvergedMessage
         * @return size of the converged message
         */
        TInt CalculateMsgSize(ConvergedMessage& msg);

        /**
         * Get the maximum size allowed for mms
         * using feature manager
         * @return max mms size
         */
        TInt MaxMmsMsgSizeL();
        
        /**
         * get file's size and mimetype info
         * @param filePath IN path of file
         * @param size OUT size of the given file
         * Size includes filesize + max mime headers size
         * @param mimetype OUT mimetype of the given file
         * @param mediaType OUT media-type grouping of mimetype
         */
        void getFileInfoL(QString filePath,
                        int& size,
                        QString& mimetype,
                        TMsgMediaType& mediaType);
        
        /**
         * get max sms size limit from feature manager
         * @param unicode if set to true limits 
         *                 will be based on Unicode charset
         * @return max sms size limit
         */
        int MaxSmsMsgSizeL(bool unicode=false);
        
        /**
         * Convert digits from any digit format to another format eg. from
         * european digits to arabic-indic digits.
         * @param aDes        Parameter to change. It can contain digits
         *                    from several digit types.
         * @param aDigitType  Destination digit type.
         */
        void ConvertDigitsTo( TDes& aDes, TDigitType aDigitType );
        
        /**
         * Identifies the digit format
         * @param ch    digit, whose format is to be recognized
         * @return      digit format
         */
        TChar NumberToBase(TChar ch);

        /**
         * Replace all control chars with a single character,
         * usually a whitespace.
         * @param   aDes         Parameter to change
         * @param   aCharacters  A set of characters to remove
         * @param   aReplacement A character used as replacement
         */
        void ReplaceCharacters(TDes &aDes, const TDesC &aChars, TChar aReplacement);

        /**
         * Checks if a given address string is a valid email address
         * @param addr address string
         * @return ETrue if addr is valid email address
         */
        TBool IsValidEmailAddress( const TDesC& aAddress );
        
        /**
         * Verifies if the given two numbers are same
         * @param aFirstNumber, phone number to be matched
         * @param aSecondNumber, phone number to be matched
         * @return TBool, true if the numbers match
         */
        TBool MatchPhoneNumberL(TDesC& aFirstNumber, TDesC& aSecondNumber);
        
      private:
          /**
           * get sms character limits from feature manager
           * @param singlePartLength length of one part
           * @param concatenatedPartLength total length over all parts
           * @param unicode if set to true limits 
           *                 will be based on Unicode charset
           */
          void getSmsCharacterLimits(int& singlePartLength, 
              int& concatenatedPartLength,
              bool unicode);
          
          /**
           * get the maximum number of parts for sms
           * using feature manager
           * @return max number of parts for the sms
           */
          int absoluteMaxSmsPartsL();
          
          /**
           * get the character limit for sms
           * using feature manager
           * @return sms character limit 
           */
          int absoluteMaxSmsCharactersL();
          
          /**
           * validate the domain portion of an email address
           */
          TBool IsValidDomain ( const TDesC& aDomain );
          
      private: //data
          /**
           * Read only once for optimization purpose
           * Max number of parts allowed for sms msgs
           */
          int mAbsMaxConcatenatedSms;
          
          /**
           * Read only once for optimization purpose
           * Character limit for sms msgs
           */
          int mAbsMaxSmsCharacters;
          
          /**
           * Read only once for optimization purpose
           * MMS size limit
           */
          int mMaxMmsSize;
          
          /**
           * The amount of digits to be used in contact matching
           */
          int mMatchDigitCount;
    };


#endif //__UNIEDITOR_GEN_UTILS_H__

// End of file

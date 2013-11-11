/*
* Copyright (c) 2002 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*       Utility methods for UI and engine modules. Provides help for address
*       string parsing, and contact database access.
*       General address format is either 
*           alias<real-address> 
*       or
*           <real-address>
*       as used in the Client MTM API.   
*
*/



#ifndef TMMSGENUTILS_H
#define TMMSGENUTILS_H

//  INCLUDES
#include <badesca.h>             // CDesCArray
#include "mmsconst.h"

// CONSTANTS
_LIT( KSepaOpen, "<" );
_LIT( KSepaClose, ">" );

// This defines the number of digits that will used for matching phone numbers to contacts
// in contact database. The number is counted from the right.
const TInt KMmsNumberOfDigitsToMatch = 7;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CContactMatcher;

class MVPbkContactLink;
class CVPbkContactLinkArray;

/**
*  TMmsGenUtils
*/
class TMmsGenUtils 
    {
    public:  // Constructors and destructor   
        
        /**
        * Constructor.
        */
        IMPORT_C TMmsGenUtils();
        
        /**
        * Destructor.
        */
        IMPORT_C ~TMmsGenUtils();
      

    public: // New functions 

        /**
        * Resolve address type and the real address part excluding alias definition.
        * NOTE: Ipv4 and IPv6 not yet supported.
        * @param aAddress address to be resolved. May include alias part.
        * @param aType address type (OUT)
        * @param aRealAddress real address part (OUT)
        * @param aMaxLength maximum length of the address.
        * @return KErrNone if successful.
        */
        IMPORT_C static TInt AddressTypeAndRealAddress(
            const TDesC& aAddress,
            TMmsAddressType& aType, 
            TDes& aRealAddress,
            TInt aMaxLength,
            const TDesC& aOpen = KSepaOpen,
            const TDesC& aClose = KSepaClose );

        /**
        * Validate the address.
        * The address must be EMail address or MMS global phone number address.
        * @param aAddress address to be resolved.  
        * @param aReal is true if address is real address excluding alias definition. 
        * @return ETrue if real address without alias part
        * @param aOpen real address part start separator. Optional. Open bracket is default.
        * @param aClose real address part end separator. Optional. Close bracket is default.
        */
        IMPORT_C static TBool IsValidAddress( 
            const TDesC& aAddress,
            TBool aReal = ETrue,
            const TDesC& aOpen = KSepaOpen,
            const TDesC& aClose = KSepaClose ); 

        /**
        * Validate that the address contains valid EMail address.
        * @param aAddress address to be resolved.
        * @param aReal is true if address is real address excluding alias definition. 
        * @return ETrue if real address without alias part
        * @param aOpen real address part start separator. Optional. Open bracket is default.
        * @param aClose real address part end separator. Optional. Close bracket is default.
        */        
        IMPORT_C static TBool IsValidEmailAddress( 
            const TDesC& aAddress,
            TBool aReal = ETrue,
            const TDesC& aOpen = KSepaOpen,
            const TDesC& aClose = KSepaClose );

        /**
        * Validate that the address contains valid MMS global phone number address.
        * You do not need to strip off the alias before this method.
        * @param aAddress address to be resolved.
        * @param aReal is true if address is real address without alias definition.
        * @param aOpen real address part start separator. Optional. Open bracket is default.
        * @param aClose real address part end separator. Optional. Close bracket is default.
        * @return ETrue if valid address
        */
        IMPORT_C static TBool IsValidMMSPhoneAddress( 
            const TDesC& aAddress,
            TBool aReal = ETrue,
            const TDesC& aOpen = KSepaOpen,
            const TDesC& aClose = KSepaClose );  

        /**
        * Alias part from the address.
        * @param aAddress address to be resolved.
        * @param aOpen real address part start separator. Optional. Open bracket is default.
        * @param aClose real address part end separator. Optional. Close bracket is default.
        * @return address alias. Zero length if not defined.
        */
        IMPORT_C static TPtrC Alias( 
            const TDesC& aAddress,
            const TDesC& aOpen = KSepaOpen,
            const TDesC& aClose = KSepaClose );

        /**
        * Real address excluding alias part.
        * @param aAddress address to be resolved.
        * @param aOpen real address part start separator. Optional. Open bracket is default.
        * @param aClose real address part end separator. Optional. Close bracket is default.
        * @return real address.
        */
        IMPORT_C static TPtrC PureAddress( 
            const TDesC& aAddress,
            const TDesC& aOpen = KSepaOpen,
            const TDesC& aClose = KSepaClose );

        /**
        * Generate address from given address and alias
        * @param aRealAddress real address
        * @param aAlias alias part 
        * @param aOpen real address part start separator. 
        *        Optional. Open bracket is default.
        * @param aClose real address part end separator.
        *        Optional. Close bracket is default.
        * @return combined address in format: <br>
        *    alias separator1 real_address separator2
        */
        IMPORT_C static HBufC* GenerateAddressL(
            const TDesC& aRealAddress, 
            const TDesC& aAlias,
            const TDesC& aOpen = KSepaOpen,
            const TDesC& aClose = KSepaClose ); 

        /**
        * Get message's description.
        * @param aPath full path of the attachment binary file
        * @param aFileSize attachment size
        * @param aMimeType attachment's MIME type.
        * @param aCharSet attachment character set MIBEnum value.
        * @param aDescription message entry's description (OUT).
        *        Truncated as necessary to fit into return value.
        */
        IMPORT_C static void GetDescriptionL(
            RFs& aFs,
            const TDesC& aPath,
            TInt aFileSize,
            TPtrC8 aMimetype,
            TInt aCharSet,
            TDes& aDescription );

        /**
        * Replaces CRs + LFs with spaces and trims all white spaces.
        * @param aDes string where to replace CRs + LFs (IN/OUT)
        */
        IMPORT_C static void ReplaceCRLFAndTrim( TDes16& aDes );

        /**
        * Replaces CRs + LFs with spaces and trims all white spaces.
        * @param aDes string where to replace CRs + LFs (IN/OUT)
        */
        IMPORT_C static void ReplaceCRLFAndTrim( TDes8& aDes );


        /**
        * Write message to MMS client log
        * @param aFmt like sprintf params in C++.
        */
        IMPORT_C static void Log( 
            TRefByValue<const TDesC> aFmt,...);
        
// The following function is for future use
        /**
        * Removes all escapes from standard URIs, and replaces them with chars.
        *  The user is reponsible for allocating the aOutput object prior to
        *  calling the method.
        *  (e.g. <space> character coding is changed from "%20" to " ".)
        * @param aInput URI received from network (IN).
        * @param aOutput descriptor containing the resulting string (OUT).
        * @return errorvalue.
        */
        /*static TInt ConvertEscapesFromUri(
            const TDesC8& aInput,
            TDes8& aOutput
            );*/
                
        /**
        * Decodes base64 or quoted-printable descriptor into plain unicode.
        * The user is responsible for allocating memory for output parameter.
        * @param[in] aInput 'base64' or 'quoted-printable' encoded string (IN).
        * @param[out] aOutput 16 bit unicode descriptor containing the resulting 
        *   string.
        * @param[in] aFs open file system handle
        * @return errorvalue.
        */
        IMPORT_C static void DecodeAndConvertMessageHeaderL(
            const TDesC8& aInput,
            TDes16& aOutput,
            RFs& aFs
            );
            
        /**
        * Generic method to check if free drive storage
        * space is or will fall below Critical Level (CL).
        * The CL per drive is defined by this module.
        * @param aFs File server session.
        *        Must be given if available in the caller.
        *        If NULL this method will create a temporary session for
        *        a check, but then the check is more expensive.
        * @param aBytesToWrite number of bytes the caller is about to add
        *        FFS.The value 0 checks if the current
        *        space is already below the CL. 
        * @param aDrive number of the drive to be checked.
        *        (see TDriveNumber of f32file.h)
        * @return ETrue if storage space would go below CL after adding
        *         aBytesToWrite more data, EFalse otherwise.
        *         Leaves on error with one of system-wide error code.
        *         e.g. KErrNotReady if drive contais no media.
        */
        IMPORT_C static TBool DiskSpaceBelowCriticalLevelL(
            RFs* aFs, TInt aBytesToWrite, TInt aDrive);
            
        /** 
        * Check if network operations are allowed
        * @return ETrue if network operations are allowed
        *         EFalse if network operations are forbidden
        */
        IMPORT_C static TBool NetworkOperationsAllowed();
        
        /** 
        * Add attribute to an attribute array. No duplicates allowed
        * If attribute exists, its value is changed
        * @param aName Name of the attribute (case sensitive)
        * @param aValue Value of the attribute
        * @param aAttributeList Array of attributes
        * Leaves if memory runs out
        */
        IMPORT_C static void AddAttributeL(
            const TDesC& aName,
            const TDesC& aValue,
            CDesCArray& aAttributeList );
            
        /**
        * Get value for an attribute.
        * @param aName Name of the attribute (case sensitive)
        * @param aAttributeList Array of attributes
        * @return Pointer to the value of the attribute
        * Leaves with KErrNotFound if attribute not found
        */
        IMPORT_C static TPtrC GetAttributeL(
            const TDesC& aName,
            const CDesCArray& aAttributeList );
            
        /**
        * Check if attribute is present
        * @param aName Name of the attribute (case sensitive)
        * @param aAttributeList Array of attributes
        * @return ETrue if the attribute is found, EFalse otherwise
        */
        IMPORT_C static TBool FindAttribute(
            const TDesC& aName,
            const CDesCArray& aAttributeList );
            
        /**
        * Delete named attribute from list
        * @param aName Name of the attribute (case sensitive)
        * @param aAttributeList Array of attributes
        */
        IMPORT_C static void DeleteAttribute(
            const TDesC& aName,
            CDesCArray& aAttributeList );
            
        /**
        * Generate message details (alias) from ContactDb based on given address.
        * Only default separators <> are allowed (compatibility with email)
        *
        * @since 3.1
        * @param aAddress address. May include alias part, which will be the result.
        * @param aAlias generated alias name (OUT). If no match in contact db then 
        *     this is a real address part of aAddress.
        * @param aMaxLength maximum length of the alias
        * @param aFs file system
        * @return KErrNone if no fatal error, Otherwise general error code.
        */
        IMPORT_C static TInt GenerateDetails( 
            const TDesC& aAddress,
            TDes& aAlias,
            TInt aMaxLength,
            RFs& aFs ); 

        /**
        * Get alias from ContactDb based on given address.
        *        
        * @since 3.1
        * @param aAddress address (pure real address). 
        * @param aAlias generated alias name (OUT). 
        * @param aMaxLength maximum length of the alias
        * @param aFs file system
        * @return KErrNone if no fatal error, Otherwise general error code.
        */
        IMPORT_C static TInt GetAlias( 
            const TDesC& aAddress,
            TDes& aAlias,
            TInt aMaxLength,
            RFs& aFs );
            
        /**
        * Get alias from ContactDb based on given address.
        *        
        * @since 3.1
        * @param aAddress Array of addresses (pure real address). 
        * @param aAlias Array of generated alias names (OUT). 
        * @param aMaxLength maximum length of the alias
        * @param aFs file system
        * Leaves in case of error
        */
        IMPORT_C static void GetAliasForAllL(
            const CDesCArray& aAddress,
            CDesCArray& aAlias,
            TInt aMaxLength,
            RFs& aFs );  
            
        /**
        * Read decode logging and dump settings from central repository
        * @since 3.2
        *
        * @param aDecodeLoggingOn tells if detailed logging is needed
        * @param aDumpOn tells if binary dump of message is needed
        */
        IMPORT_C static void GetLoggingSettings( TBool& aDecodeLoggingOn, TBool& aDumpOn );
            

    protected:  // New functions
        

    private:

        // By default, prohibit copy constructor
        TMmsGenUtils( const TMmsGenUtils& );

        // Prohibit assignment operator
        TMmsGenUtils& operator= ( const TMmsGenUtils& );

        /**
        * Generate alias from ContactDb based on given address.
        * @param aFs file system
        * @param aAddress real address
        * @param aAlias generated alias name (OUT)
        * @param aMaxLength maximum length of the alias
        * @param aId contacd db id of the matched contact
        */
        static void DoGetAliasL(
            RFs& aFs,
            const TDesC& aAddress, 
            TDes& aAlias, 
            TInt aMaxLength );
        

        /**
        * Generate aliases from ContactDb based on given address.
        * This is optimized version of the previous DoGetAliasL
        * because the contact db is opened before calling this function.
        * Opening the db is quite an time consuming operation.
        * @param aAddress real address
        * @param aAlias generated alias name (OUT)
        * @param aMaxLength maximum length of the alias
        * @param aContactMatcher pointer to contact matcher
        * @param aId contacd db id of the matched contact
        * @param aDigitsToMatch number of digits to match
        */
        static void  DoGetAliasL(
            const TDesC& aAddress, 
            TDes&  aAlias, 
            TInt aMaxLength,
            CContactMatcher& aContactMatcher,
            TInt aDigitsToMatch  );

        /**
        * Find position of attribute
        * @param aName Name of the attribute (case sensitive)
        * @param aAttributeList Array of attributes
        * @return index of attribute, KerrNot found if not found
        */
        static TInt FindAttributePosition(
            const TDesC& aName,
            const CDesCArray& aAttributeList );
            
        /**
        * Get number of digits to be used when matching phone numbers
        * @return number of digits to match from central repository
        * If reading central repository fails, returns hard coded default
        */   
        static TInt DigitsToMatch();
         
        /**
        * Create contact match wrapper and open all stores
        * Caller must close stores and delete contact matcher when no longer needed
        * @param aFs file system
        * @return pointer to contact matcher for further operations
        * Leaves in case of error
        */
        static CContactMatcher* OpenAllStoresL( RFs& aFs );
        

        /**
        * extracts contact name out of the contact link passed
        * Caller must delete the name buffer returned
        * @param aContactLink - contact link
        * @param aContactMatcher - contact matcher
        * @return pointer to contact name for further operation
        * Leaves in case of error
        */
        static HBufC* GetContactNameL(
                const MVPbkContactLink& aContactLink,
                CContactMatcher &aContactMatcher);


        /**
        * extracts contact name and converts to lower case chars.
        * Caller must delete the name buffer returned
        * @param aContactLink - contact link
        * @param aContactMatcher - contact matcher
        * @return pointer to contact name for further operation
        * Leaves in case of error
        */
        static HBufC* GetContactNameInLowerCaseL(
                const MVPbkContactLink& aContactLink,
                CContactMatcher &aContactMatcher);

        /**
        * checks if contact names in the linkArray are identical. Can be used t
        * Caller must delete the name buffer returned
        * @param aLinkArray- contact link array
        * @param aContactMatcher - contact matcher
        * @param aNameIndex - index of the name to be finally shown(output param)
        * @return true/false, to decide if name has to be shown or not. 
        * Leaves in case of error
        */
        static TBool ShowContactNameL(
                CVPbkContactLinkArray* aLinkArray,
                TInt &aNameIndex,
                CContactMatcher &aContactMatcher);
                

        /**
        * Gets the index of contact match in current store. 
        * If multiple matches are found in current store, then the link Array
        * is modified to include only current store contact matches for further name mapping.
        * else, link array can be used as is for further name mapping
        * @param aLinkArray- contact link array containing all the matches
        * @return Valid name index, if single match in current store is found
        *         else returns KErrMultipleMatchFound, if multiple mathes are found
        *         and also modifies aLinkArray as explained above.
        * Leaves in case of error
        */
        static TInt GetCurrentStoreIndexL( CVPbkContactLinkArray& aLinkArray );

    private:    // Data

    };

#endif      // TMMSGENUTILS_H   
            
// End of File

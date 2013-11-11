/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Key class for message sorting
*
*/

// <qmail> Are AknUtils available in 10.1?
// <cmail>
#include <AknUtils.h>
// </cmail>
// </qmail>

#include "emailtrace.h"
#include "ipsplgheaders.h"

const TInt KLessThan = -1;
const TInt KMoreThan = 1;
const TInt KEqual = 0;

_LIT(KDefaultSubjectPrefixSeparator,": ");
// <cmail>
// <qmail> Cmail bugfix uses AknUtils (available in 10.1?) - commented out until it is clear
//_LIT( KCharsToReplace, "\r\n\t\x2028\x2029" );
// <qmail>
// </cmail>

// ---------------------------------------------------------------------------
// Basic sonstructor
// ---------------------------------------------------------------------------

TIpsPlgMsgKey::TIpsPlgMsgKey( 
    CMsvEntry& aFolderEntry,
    const RArray<TFSMailSortCriteria>& aSortCriteria )
    : iFolderEntry( aFolderEntry ), 
      iSortingCriteria( aSortCriteria ),
      iSubjectPrefixSeparator( KDefaultSubjectPrefixSeparator )
    {
    FUNC_LOG;
    // none
    }

// ---------------------------------------------------------------------------
// Compares the messages under the folder entry according to the sorting
// criteria.
// ---------------------------------------------------------------------------

TInt TIpsPlgMsgKey::Compare( TInt aLeft, TInt aRight ) const
    {
    FUNC_LOG;
    TInt result( KEqual );
    TInt childCount( iFolderEntry.Count() );
    if ( aLeft >= childCount || aRight >= childCount  )
        {
        return result;
        }
    const TMsvEmailEntry leftEntry( iFolderEntry[aLeft] );
    const TMsvEmailEntry rightEntry( iFolderEntry[aRight] );
    
    // Loop over the sorting criteria until the comparison result is known
    TInt i(0);
    while ( ( result == KEqual ) && ( i < iSortingCriteria.Count() ) )
        {
        switch ( iSortingCriteria[i].iField )
            {
            case EFSMailDontCare:
                {
                // If the client does not care, we can return immediately
                return result;
                }
            case EFSMailSortByDate:
                {
                if ( leftEntry.iDate < rightEntry.iDate ) 
                    {
                    result = KLessThan;
                    }
                else if ( leftEntry.iDate > rightEntry.iDate ) 
                    {
                    result = KMoreThan;
                    }
                break;
                }
            case EFSMailSortBySender:
                {
                result = 
                    leftEntry.iDetails.CompareC( rightEntry.iDetails );
                break;
                }
            case EFSMailSortByRecipient:
                {                
                result = 
                    leftEntry.iDetails.CompareC( rightEntry.iDetails );
                break;
                }
            case EFSMailSortBySubject:
                {
                result = CompareSubjects( leftEntry.iDescription,
                    rightEntry.iDescription );
                break;
                }
            case EFSMailSortByPriority:
                // The values of TMsvPriority are defined so that the highest
                // priority has the smallest value
                if ( leftEntry.Priority() > rightEntry.Priority() ) 
                    {
                    result = KLessThan;
                    }
                else if ( leftEntry.Priority() < rightEntry.Priority() ) 
                    {
                    result = KMoreThan;
                    }
                break;
            case EFSMailSortByFlagStatus:
                {
                // The 'flagged' state is supported only in IMAP4
                if ( leftEntry.iMtm == KUidMsgTypeIMAP4 )
                    {
                    if ( !leftEntry.FlaggedIMAP4Flag() && 
                         rightEntry.FlaggedIMAP4Flag() ) 
                        {
                        result = KLessThan;
                        }
                    else if ( leftEntry.FlaggedIMAP4Flag() && 
                              !rightEntry.FlaggedIMAP4Flag() ) 
                        {
                        result = KMoreThan;
                        }
                    }
                break;
                }
            case EFSMailSortByUnread:
                {
                // In this context, an read message is 'greater' than
                // a read one
                if ( !leftEntry.Unread() && rightEntry.Unread() ) 
                    {
                    result = KMoreThan; // <cmail> changed from KLessThan 
                    }
                else if ( leftEntry.Unread() && !rightEntry.Unread() ) 
                    {
                    result = KLessThan; // <cmail> changed from KMoreThan
                    }
                break;
                }
            case EFSMailSortBySize:
                {
                if ( leftEntry.iSize < rightEntry.iSize ) 
                    {
                    result = KLessThan;
                    }
                else if ( leftEntry.iSize > rightEntry.iSize ) 
                    {
                    result = KMoreThan;
                    }
                break;
                }
            case EFSMailSortByAttachment:
                {
                // In this context, a message with an attachment is 'greater'
                // than one without an attachment
                if ( !leftEntry.Attachment() && rightEntry.Attachment() ) 
                    {
                    result = KLessThan;
                    }
                else if ( leftEntry.Attachment() && !rightEntry.Attachment() ) 
                    {
                    result = KMoreThan;
                    }
                break;
                }
            default:
                break;
            }
            
        // Switch the order if the messages are sorted in the descending order
        if ( iSortingCriteria[i].iOrder == EFSMailDescending ) 
            {
            result = -result;
            }
        
        i++;
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// Returns a message based on the index.
// Note, that KIndexPtr index value is not supported as the class is not 
// intended to be used with User::BinarySearch().
// ---------------------------------------------------------------------------

TAny* TIpsPlgMsgKey::At( TInt anIndex ) const
    {
    FUNC_LOG;
    return (TAny*) &iFolderEntry[ anIndex ];
    }

// ---------------------------------------------------------------------------
// Strips the subject prefixes before comparing the strings
// ---------------------------------------------------------------------------

TInt TIpsPlgMsgKey::CompareSubjects(
    const TDesC& aLeft, 
    const TDesC& aRight ) const
    {
    FUNC_LOG;
    TInt  result( KEqual );
    TPtrC leftPtr( aLeft );
    TPtrC rightPtr( aRight );
    TInt  leftOffset( FindSubjectStart( aLeft ) );
    TInt  rightOffset( FindSubjectStart( aRight ) );
    
    leftPtr.Set( 
        leftPtr.Ptr() + leftOffset, leftPtr.Length() - leftOffset );
    rightPtr.Set( 
        rightPtr.Ptr() + rightOffset, rightPtr.Length() - rightOffset );
    
// <qmail> Are AknUtils available in 10.1?
    // <cmail> for unifying with UI - remove all white spaces
    /*HBufC* croppedLeft = leftPtr.AllocLC();
    TPtr croppedLeftPtr = croppedLeft->Des();
    HBufC* croppedRight = rightPtr.AllocLC();
    TPtr croppedRightPtr = croppedRight->Des();
        
    AknTextUtils::ReplaceCharacters( croppedLeftPtr, KCharsToReplace, ' ' );
    croppedLeftPtr.TrimAll();
    AknTextUtils::ReplaceCharacters( croppedRightPtr, KCharsToReplace, ' ' );
    croppedRightPtr.TrimAll();
    
    result = croppedLeftPtr.CompareC( croppedRightPtr );
    
    CleanupStack::PopAndDestroy( croppedRight );
    CleanupStack::PopAndDestroy( croppedLeft );*/
    // </cmail>
// </qmail>

// <qmail> Remove line if above code is valid in 10.1
	result = leftPtr.CompareC( rightPtr );
// </qmail>
        
    return result;
    }
    
// ---------------------------------------------------------------------------
// Finds the starting point of the actual subject text after the subject
// prefixes (Re:, Fwd: etc).
// The algorithm search the last separator string (': ') in most western
// languages and returns the offset from the beginnig of the string to
// the first character following the last separator.
// ---------------------------------------------------------------------------

TInt TIpsPlgMsgKey::FindSubjectStart( const TDesC& aSubject ) const
    {
    FUNC_LOG;
    TInt offset(0);
    // <cmail> removed and replaced with the code from UI
    // TFsEmailUiUtility::CreateSubjectWithoutLocalisedPrefixLC
    // to have the same subject here and there
    // there was sorting problem when subjects where handled
    // different here and in UI while creating mails' list.  
    
    /*TPtrC ptr(aSubject);
    TInt current(0);
    TInt skipLength = iSubjectPrefixSeparator.Length();
    
    // Loop while separators are found
    do
        {
        current = ptr.FindF( iSubjectPrefixSeparator );

        if ( current != KErrNotFound )
            {
            offset += current + skipLength;
            ptr.Set( aSubject.Ptr() + offset, aSubject.Length() - offset );
            }
        } while ( current != KErrNotFound );*/
    
    TBool prefixFound = EFalse;
    TPtrC croppedSubject;
    croppedSubject.Set( aSubject );
    
    do
        {
        prefixFound = EFalse;
    
        // Remove leading white space before trying to find the prefix
        while( croppedSubject.Length() && 
               TChar( croppedSubject[0] ).IsSpace() )
            {
            croppedSubject.Set( croppedSubject.Mid(1) );
            offset++;
            }
    
        // try to find ":" at the beginning
        // Locate : character on location 1,2 and 3
        static const TInt KPrefixMinLength = 1;
        static const TInt KPrefixMaxLength = 3;
        static const TText KPrefixSeparator = ':';
        TInt separatorPosition = croppedSubject.Locate( KPrefixSeparator );
        if ( separatorPosition >= KPrefixMinLength &&
             separatorPosition <= KPrefixMaxLength )
            {
            TPtrC prefixCandidate = croppedSubject.Left( separatorPosition );
            // Only fully alphabetic prefixes are cropped
            TBool isAlpha = ETrue;
            for ( TInt i = 0 ; i < prefixCandidate.Length() ; ++i )
                {
                if ( !TChar( prefixCandidate[i] ).IsAlpha() )
                    {
                    isAlpha = EFalse;
                    }
                }
            if ( isAlpha )
                {
                croppedSubject.Set( croppedSubject.Mid( 
                        separatorPosition + 1 ) );
                offset += separatorPosition + 1;
                prefixFound = ETrue;
                }
            }
        }
    while ( prefixFound );
    // </cmail>

    return offset;
    }

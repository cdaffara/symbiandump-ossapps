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


#ifndef IPSPLGMSGKEY_H
#define IPSPLGMSGKEY_H

#include <e32base.h>          // TKey
//<cmail>
#include "CFSMailCommon.h"    // tfsmailsortcriteria
//</cmail>

class CMsvEntry;

const TInt KMaxSubjectPrefixSeparatorLength = 8;

/**
 *  Comparison class for message objects
 *
 *  @code
 *  @endcode
 *
 *  @lib ipssosplugin.lib
 *  @since 
 */
NONSHARABLE_CLASS( TIpsPlgMsgKey ) : public TKey
    {

public:

    /**
     * Constructor.
     *
     * @since 
     * @param aFolderEntry  Folder to be sorted
     * @param aSortCriteria Sorting criteria
     */
    TIpsPlgMsgKey( 
        CMsvEntry& aFolderEntry,
        const RArray<TFSMailSortCriteria>& aSortCriteria );
        
    /**
     * Finds the offset from which the actual subject starts after 
     * the prefixes
     *
     * @param aSubject Message subject string
     * @return Offset from the beginning of the string. If the string
     *         is not found, returns 0.
     */
    TInt FindSubjectStart( const TDesC& aSubject ) const;

public:

    /**
     * From TKey
     */
    virtual TInt Compare(TInt aLeft, TInt aRight) const;

    /**
     * From TKey
     */
    virtual TAny* At(TInt anIndex) const;

private:
    // Prevent usage of the default constructor
    TIpsPlgMsgKey();
    
private:
    /**
     * Compares the subject strings omitting the reply & forward prefixes
     */
    TInt CompareSubjects( const TDesC& aLeft, const TDesC& aRight ) const;

private:
    // Folder objects the messages of which are sorted. Not owned.    
    CMsvEntry& iFolderEntry;
    
    // Sorting criteria
    const RArray<TFSMailSortCriteria>& iSortingCriteria;
    
    // String which separates the subject prefixes (Re, Fwd etc) 
    // from the actual subject
    TBuf<KMaxSubjectPrefixSeparatorLength> iSubjectPrefixSeparator;
    
    };

#endif // IPSPLGMSGKEY_H

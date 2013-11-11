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
* Description:  Swapper class for message sorting function
*
*/


#ifndef IPSPLGMSGSWAP_H
#define IPSPLGMSGSWAP_H

#include <e32std.h>    // TSwap

class CMsvEntry;

/**
 *  Swaps the message objects inside CMsvEntry. Intended to be used with
 *  User::QuickSort().
 *
 *  @code
 *  @endcode
 *
 *  @lib ipssosplugin.dll
 *  @since 
 */
NONSHARABLE_CLASS( TIpsPlgMsgSwap )
    : public TSwap
    {
public:

    /**
     * Constructor.
     *
     * @since 
     * @param aFolderEntry Folder to be sorted
     */
    TIpsPlgMsgSwap( CMsvEntry& aFolderEntry );

    /**
     * From TSwap
     */
    virtual void Swap(TInt aLeft, TInt aRight) const;
    
private:
    // Inhibit the usage of the default constructor
    TIpsPlgMsgSwap();
    
private:
    // Folder objects the messages of which are sorted. Not owned.    
    CMsvEntry& iFolderEntry;
    };

#endif // IPSPLGMSGSWAP_H

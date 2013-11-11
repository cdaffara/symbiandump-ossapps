/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container store content manager.
*
*/
#ifndef MESSAGESTOREUTILS_H
#define MESSAGESTOREUTILS_H


#include <f32file.h>


class CContainerStoreUtils;


NONSHARABLE_CLASS( MessageStoreUtils )
    {
public:
    /**
     * Replaces the target file with the aPrepend buffer and then copies the
     * contents of the source file to the target file.
     * 
     * Not encryption-aware so must be used only when it is guaranteed the
     * encryptions if off.
     * 
     * @param aSource the source file path, the method expects this file
     * exists. 
     * @param aTarget the target file path, the file will be overwritten if
     * it already exists. 
     */
    static void PrependBufferAndCopyFileL(
        RFs& aRfs,
        CContainerStoreUtils& aUtils,
        const TDesC& aSource,
        const TDesC& aTarget,
        const TDesC8& aPrepend );
    
    /**
     * Replaces the target file with the aPrepend buffer and then copies the
     * contents of the source file to the target file. Afterwards changes
     * the name of the target file to the source.
     * 
     * Not encryption-aware so must be used only when it is guaranteed the
     * encryptions if off.
     * 
     * @param aSource the source file path, the method expects this file
     * exists. 
     * @param aTarget the target file path, the file will be overwritten if
     * it already exists. 
     */
    static void PrependBufferAndMoveFileL(
        RFs& aRfs,
        CContainerStoreUtils& aUtils,
        const TDesC& aSource,
        const TDesC& aTarget,
        const TDesC8& aPrepend );

    static void ReplaceFileWithFileL(
        RFs& aRfs,
        CContainerStoreUtils& aUtils,
        RFile& aSource,
        const TDesC& aTarget );
    
private:
    static void PrependBufferAndCopyFileInternalL(
        RFs& aRfs,
        CContainerStoreUtils& aUtils,
        RFile& aSource,
        const TDesC& aTarget,
        const TDesC8& aPrepend );

    };

#endif //MESSAGESTOREUTILS_H.

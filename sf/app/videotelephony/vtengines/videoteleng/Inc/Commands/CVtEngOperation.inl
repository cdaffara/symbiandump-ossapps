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
* Description:  Operation inline methods 
*
*/


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CVtEngOperation::Command
// -----------------------------------------------------------------------------
inline TVtEngCommandId CVtEngOperation::Command() const { return iCommand; }

// -----------------------------------------------------------------------------
// CVtEngOperation::Parameters
// -----------------------------------------------------------------------------
inline const TDesC8* CVtEngOperation::Parameters() const { return iParams; }

// -----------------------------------------------------------------------------
// TVtEngOpParamUtil::Set
// -----------------------------------------------------------------------------
template <class T>
inline void TVtEngOpParamUtil<T>::Set( 
    T& aRef,
    const CVtEngOperation& aOp ) 
    { 
    TPckgC<T> pckg( aRef );
    pckg.Set( *aOp.Parameters() );
    aRef = pckg();
    }

// End of File

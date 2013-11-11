/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Preset Server session inline method implementations
*
*/


// ---------------------------------------------------------------------------
// Gets all presets of the supplied type.
// ---------------------------------------------------------------------------
//
template <typename T>
void RPSServ::GetPresetsL( RPointerArray<T>& aPresets, TUid aDataHandler )
    {    
    TPckgBuf<TInt> length;
    User::LeaveIfError( SendReceive( EPSOpGetPresetsLength, TIpcArgs( aDataHandler.iUid, &length ) ) );
    
    HBufC8* buf = HBufC8::NewLC( length() ); 
    TPtr8 ptr( buf->Des() );
    
    User::LeaveIfError( SendReceive( EPSOpGetPresets, TIpcArgs( aDataHandler.iUid, &ptr ) ) );
    
    RDesReadStream stream( ptr );
    CleanupClosePushL( stream );
    
    TCardinality count;
    stream >> count;
    
    for ( TInt i = 0; i < count; i++ )
        {
        CPSPresetInterface* preset = OpenPresetL( stream.ReadInt32L() );
        CleanupStack::PushL( preset );
        aPresets.AppendL( static_cast<T*>( preset ) );
        CleanupStack::Pop( preset );
        }         
     
    CleanupStack::PopAndDestroy( 2, buf );
    }


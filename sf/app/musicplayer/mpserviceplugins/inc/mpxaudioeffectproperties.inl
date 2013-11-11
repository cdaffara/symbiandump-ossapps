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
* Description:  Inline implementation of audio effect properties
*
*/

#include  <s32file.h>
#include  <s32std.h>
#include <f32file.h>
#include <mpxlog.h>

_LIT( KMPXAudioEffectFileName, "c:\\system\\data\\mpxaudioeffect.dat" );

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
inline CMPXAudioEffectProperties::~CMPXAudioEffectProperties()
    {
    //iDesValues.DeleteAll();
    }


// -----------------------------------------------------------------------------
// Return balance
// -----------------------------------------------------------------------------
//
inline TInt CMPXAudioEffectProperties::Balance() const
    {
    return Value(EIntBalance);
    }

// -----------------------------------------------------------------------------
// Set balance
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::SetBalance(TInt aBalance)
    {
    SetValue(EIntBalance, aBalance);
    }

// -----------------------------------------------------------------------------
// Reverb value
// -----------------------------------------------------------------------------
//
inline TInt CMPXAudioEffectProperties::Reverb() const
    {
    return Value(EIntReverb);
    }

// -----------------------------------------------------------------------------
// Set reverb
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::SetReverb(TInt aReverb)
    {
    SetValue(EIntReverb, aReverb);
    }

// -----------------------------------------------------------------------------
// Stereo value
// -----------------------------------------------------------------------------
//
inline TBool CMPXAudioEffectProperties::Stereo() const
    {
    return Value(EIntStereo);
    }

// -----------------------------------------------------------------------------
// Set bass boost
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::SetStereo(TBool aStereo)
    {
    SetValue(EIntStereo, aStereo);
    }

// -----------------------------------------------------------------------------
// Bass boost value
// -----------------------------------------------------------------------------
//
inline TBool CMPXAudioEffectProperties::BassBoost() const
    {
    return Value(EIntBassBoost);
    }

// -----------------------------------------------------------------------------
// Set bass boost
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::SetBassBoost(TBool aBassBoost)
    {
    SetValue(EIntBassBoost, aBassBoost);
    }

// -----------------------------------------------------------------------------
// Bass boost value
// -----------------------------------------------------------------------------
//
inline TBool CMPXAudioEffectProperties::Loudness() const
    {
    return Value(EIntLoudness);
    }

// -----------------------------------------------------------------------------
// Set loudness
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::SetLoudness(TBool aLoudness)
    {
    SetValue(EIntLoudness, aLoudness);
    }
    
// -----------------------------------------------------------------------------
// Externalize properties
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::ExternalizeL(RWriteStream& aStream) const
    {
    MPX_DEBUG1("CMPXAudioEffectProperties::ExternalizeL <---");
    for (TInt i=0;i<EIntNumItems;++i)
        {
        aStream.WriteInt32L(iIntValues[i]);
        MPX_DEBUG2("CMPXAudioEffectProperties::ExternalizeL  -- %i", iIntValues[i]);
        }

    //for (TInt ii=0;ii<EDesNumItems;++ii)
    //    {
    //    aStream.WriteInt32L(Value(static_cast<TDesProperty>(ii)).Length());
    //    aStream << Value(static_cast<TDesProperty>(ii));
    //    }
    MPX_DEBUG1("CMPXAudioEffectProperties::ExternalizeL --->");
    }

// -----------------------------------------------------------------------------
// Internalize properties
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::InternalizeL(RReadStream& aStream)
    {
    MPX_DEBUG1("CMPXAudioEffectProperties::InternalizeL <---");
    for (TInt i=0;i<EIntNumItems;++i)
        {
        iIntValues[i]=aStream.ReadInt32L();
        MPX_DEBUG2("CMPXAudioEffectProperties::InternalizeL  -- %i", iIntValues[i]);
        }
    //for (TInt ii=0;ii<EDesNumItems;++ii)
    //    {
    //    delete iDesValues[ii];
    //    iDesValues[ii] = NULL;
    //    iDesValues[ii] = HBufC::NewL(aStream,aStream.ReadInt32L());
    //    }
    MPX_DEBUG1("CMPXAudioEffectProperties::InternalizeL --->");
    }

// -----------------------------------------------------------------------------
// Get value of a descriptor property
// -----------------------------------------------------------------------------
//
inline const TDesC& CMPXAudioEffectProperties::Value(TDesProperty /*aProperty*/) const
    {
    //return iDesValues[aProperty] ? 
    //       static_cast<const TDesC&>(*iDesValues[aProperty]) : KNullDesC;
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// Get value of an integer property
// -----------------------------------------------------------------------------
//
inline TInt CMPXAudioEffectProperties::Value(TIntProperty aProperty) const
    {
    return iIntValues[aProperty];
    }
    
// -----------------------------------------------------------------------------
// Set value of a descriptor property
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::SetValue(TDesProperty /*aProperty*/,
                                          const TDesC& /*aValue*/)
    {
    //delete iDesValues[aProperty];
    //iDesValues[aProperty]=aValue.Alloc();
    }
    
// -----------------------------------------------------------------------------
// Set value of an integer property
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::SetValue(TIntProperty aProperty,TInt aValue)
    {
    iIntValues[aProperty]=aValue;
    }

// -----------------------------------------------------------------------------
// Reset all values to 0 or NULL
// -----------------------------------------------------------------------------
//
inline void CMPXAudioEffectProperties::Reset()
    {
    MPX_DEBUG1("CMPXAudioEffectProperties::Reset");
    for (TInt i=0;i<EIntNumItems;++i)
        {
        iIntValues[i] =0;
        }
    //for (TInt ii=0;ii<EDesNumItems;++ii)
    //    {
    //    delete iDesValues[ii];
    //    iDesValues[ii]=NULL;
    //    }
    }
   
// -----------------------------------------------------------------------------
// Save to file
// -----------------------------------------------------------------------------
// 
inline void CMPXAudioEffectProperties::SaveToFileL() const
    {
    MPX_DEBUG1("CMPXAudioEffectProperties::SaveToFileL <---");
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    fs.MkDirAll( KMPXAudioEffectFileName  );
    TParse parsedName;
    fs.Parse( KMPXAudioEffectFileName, parsedName );
    CFileStore* store = CDirectFileStore::ReplaceLC(fs, 
                                                    parsedName.FullName(), 
                                                    EFileWrite);
    store->SetTypeL( KDirectFileStoreLayoutUid );
    RStoreWriteStream stream;
    TStreamId id = stream.CreateLC( *store );
    this->ExternalizeL(stream);
    stream.CommitL();
    CleanupStack::PopAndDestroy();
    store->SetRootL( id );
    store->CommitL();
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy(&fs);
    MPX_DEBUG1("CMPXAudioEffectProperties::SaveToFileL --->");
    }

// -----------------------------------------------------------------------------
// Load from file
// -----------------------------------------------------------------------------
//  
inline void CMPXAudioEffectProperties::LoadFromFileL()
    {
    MPX_DEBUG1("CMPXAudioEffectProperties::LoadFromFileL <---");
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    fs.MkDirAll( KMPXAudioEffectFileName );
    TParse parsedName;

    fs.Parse( KMPXAudioEffectFileName, parsedName );
    CFileStore* store = CDirectFileStore::OpenLC(fs, 
                                                 parsedName.FullName(), 
                                                 EFileRead );
    RStoreReadStream stream;
    stream.OpenLC( *store, store->Root() );
    this->InternalizeL(stream);
    CleanupStack::PopAndDestroy(); //store
    CleanupStack::PopAndDestroy(); //stream
    CleanupStack::PopAndDestroy(&fs);
    MPX_DEBUG1("CMPXAudioEffectProperties::LoadFromFileL --->");
    }


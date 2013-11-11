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
* Description:  String loader class.
*
*/

#include <exception> // must be before e32base.h so uncaught_exception gets defined
#include <StringLoader.h>
#include "phoneresourceadapter.h"
#include "phoneaction.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// StringLoader::StringLoader
// -----------------------------------------------------------------------------
//
/*
StringLoader::StringLoader(  )
    {
    
    }
*/

// -----------------------------------------------------------------------------
// StringLoader::Load
// -----------------------------------------------------------------------------
//
EXPORT_C void StringLoader::Load( 
        TDes & aDest,
        TInt aResourceId,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        aDest.Copy (text->text ().utf16 ());
        delete text;        
        }
    }

// -----------------------------------------------------------------------------
// StringLoader::LoadL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadL( 
        TInt aResourceId,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewL (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::New (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadL( 
        TInt aResourceId,
        TInt aInt,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, aInt);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewL (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::New (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadL( 
        TInt aResourceId,
        const TDesC & aString,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aString);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewL (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::New (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadL( 
        TInt aResourceId,
        const TDesC & aString,
        TInt aInt,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aString, aInt);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewL (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::New (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadL( 
        TInt aResourceId,
        const CArrayFix<TInt> & aInts,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aInts);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewL (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::New (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadL( 
        TInt aResourceId,
        const MDesCArray & aStrings,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aStrings);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewL (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::New (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadL( 
        TInt aResourceId,
        const MDesCArray & aStrings,
        const CArrayFix<TInt> & aInts,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aStrings, &aInts);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewL (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::New (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadLC( 
        TInt aResourceId,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewLC (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::NewLC (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadLC( 
        TInt aResourceId,
        TInt aInt,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, aInt);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewLC (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::NewLC (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadLC( 
        TInt aResourceId,
        const TDesC & aString,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aString);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewLC (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::NewLC (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadLC( 
        TInt aResourceId,
        const TDesC & aString,
        TInt aInt,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aString, aInt);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewLC (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::NewLC (1);
    }
    return buf;
    }



// -----------------------------------------------------------------------------
// StringLoader::LoadLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadLC( 
        TInt aResourceId,
        const CArrayFix<TInt> & aInts,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aInts);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewLC (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::NewLC (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadLC( 
        TInt aResourceId,
        const MDesCArray & aStrings,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aStrings);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewLC (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::NewLC (1);
    }
    return buf;
    }


// -----------------------------------------------------------------------------
// StringLoader::LoadLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC * StringLoader::LoadLC( 
        TInt aResourceId,
        const MDesCArray & aStrings,
        const CArrayFix<TInt> & aInts,
        CCoeEnv * aLoaderEnv )
    {
    Q_UNUSED (aLoaderEnv);
    HBufC *buf;
    QMap<PhoneAction::ActionType, PhoneAction *> map = PhoneResourceAdapter::Instance ()->convert (aResourceId, &aStrings, &aInts);
    if (map.contains (PhoneAction::Text)) {
        PhoneAction *text = map [PhoneAction::Text];
        buf = HBufC::NewLC (text->text ().size ());
        *buf = text->text ().utf16 ();
        delete text;
    } else {
        // TODO: this else branch MUST be removed after all strings are localized!
        //       now here only to prevent crashing
        buf = HBufC::NewLC (1);
    }
    return buf;
    }

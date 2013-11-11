/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of interface for playbackutility
*
*/

// Version : %version:  4 %

#ifndef CMPXPLAYBACKUTILITY_H
#define CMPXPLAYBACKUTILITY_H

#include <e32base.h>
#include <mpxcommand.h>
#include <mpxattributespecs.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxplaybackframeworkdefs.h>

class TMPXAttribute;
class MMPXPlaybackObserver;
class CMPXCollectionPlaylist;
class MMPXPlaybackCallback;
class CMPXPlaybackUtility;

class MMPXSource
{
    public:
        virtual CMPXCollectionPlaylist* PlaylistL() = 0;

        virtual void MediaL( const TArray<TMPXAttribute>& aAttrs,
                             MMPXPlaybackCallback& aCallback,
                             CMPXAttributeSpecs* aSpecs ) = 0;

        virtual void MediaL( const TArray<TMPXAttribute>& aAttrs,
                             MMPXPlaybackCallback& aCallback ) = 0;
};

class MMPXPlaybackUtility : public CBase
{
    public:
        static MMPXPlaybackUtility* UtilityL( const TMPXCategory aCategory,
                                              const TUid& aModeId = KPbModeDefault );

        virtual TMPXPlaybackState StateL() const = 0;
        virtual void AddObserverL( MMPXPlaybackObserver& aObs ) = 0;
        virtual void CommandL( CMPXCommand& aCmd, MMPXPlaybackCallback* aCallback=NULL ) = 0;
        virtual void CommandL( TMPXPlaybackCommand aCmd ) = 0;
        virtual MMPXSource* Source() = 0;
        virtual void PropertyL( MMPXPlaybackCallback& aCallback, TMPXPlaybackProperty aProperty ) = 0;
        virtual void RemoveObserverL( MMPXPlaybackObserver& aObs ) = 0;
        virtual void Close() = 0;
        virtual void SetPrimaryClientL() = 0;
        virtual void SetL( TMPXPlaybackProperty aProperty,TInt aValue ) = 0;
        virtual void CancelRequest() = 0;

    public:
        TMPXPlaybackState iState;
        TMPXPlaybackProperty iProperty;
        TInt iPropertyValue;
        TInt iCommand;
};

class CMPXPlaybackUtility : public MMPXPlaybackUtility,
                            public MMPXSource
{
    public:

        static CMPXPlaybackUtility* NewL();

        ~CMPXPlaybackUtility();

    private:

        CMPXPlaybackUtility();

        void ConstructL();

        void AddObserverL( MMPXPlaybackObserver& aObs );

        void RemoveObserverL( MMPXPlaybackObserver& aObs );

        void Close();

        void CommandL( CMPXCommand& aCmd, MMPXPlaybackCallback* aCallback );
        void CommandL( TMPXPlaybackCommand aCmd );

        TMPXPlaybackState StateL() const;

        MMPXSource* Source();

        void SetL( TMPXPlaybackProperty aProperty,TInt aValue );

        void PropertyL( MMPXPlaybackCallback& aCallback, TMPXPlaybackProperty aProperty );

        CMPXCollectionPlaylist* PlaylistL();

        void MediaL( const TArray<TMPXAttribute>& aAttrs,
                     MMPXPlaybackCallback& aCallback,
                     CMPXAttributeSpecs* aSpecs );

        void MediaL( const TArray<TMPXAttribute>& aAttrs,
                     MMPXPlaybackCallback& aCallback );

        void SetPrimaryClientL();

        void CancelRequest();
};

#endif // CMPXPLAYBACKUTILITY_H

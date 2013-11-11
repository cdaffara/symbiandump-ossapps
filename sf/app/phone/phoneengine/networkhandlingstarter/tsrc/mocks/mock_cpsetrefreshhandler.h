/** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef CPSetRefreshHandler_h
#define CPSetRefreshHandler_h


#include <psetrefreshhandler.h> 
   
class CPSetRefreshHandlerMock : public CPSetRefreshHandler
    {
    public:
        ~CPSetRefreshHandlerMock();
        static CPSetRefreshHandlerMock* NewL();
        void NotifyFileChangeL( MSSSettingsRefreshObserver & aObserver, TSatElementaryFiles aObservedFile, TSatRefreshType aObservedRefreshType );
        void CancelNotify( );
        TInt ChangedCspSettings( TPSetChangedCspSetting & aChangedCspSettings, TPSetChangedCspSetting & aNewValues );
    };

#endif

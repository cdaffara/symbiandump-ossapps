/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Debug print macros*
*/


// Version : %version: ou1cpsw#5 %




#ifndef __MPXVIDEO_DEBUG_H__
#define __MPXVIDEO_DEBUG_H__

// INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <e32des16.h>
#include <e32property.h>
#include <flogger.h>
#include <e32def.h>



// #define _MPX_FILE_LOGGING_

#ifdef _DEBUG
    #define FU_DEBUG RDebug::Print
#else
    #ifdef _MPX_FILE_LOGGING_
        #define FU_DEBUG MPXDebug::FileLog
    #else
        #define FU_DEBUG
    #endif
#endif


class MPXDebug
{
    public:
        inline static void FileLog( TRefByValue<const TDesC16> aFmt, ... )
        {
            VA_LIST list;
            VA_START(list,aFmt);
            RFileLogger::WriteFormat( _L("Fusion"),
                                      _L("fusion.log"),
                                      EFileLoggingModeAppend,
                                      aFmt,
                                      list );
        }
};



// MACROS
#ifdef _DEBUG
    #define MPX_DEBUG             TFusionLog::FusionLog
    #define MPX_ENTER_EXIT        TEnterExitLog _s
#else
    #define MPX_DEBUG
    #define MPX_ENTER_EXIT
#endif


class TFusionLog : public TDes16Overflow
{
    public:

        inline static void FusionLog( TRefByValue<const TDesC16> aFmt, ... )
        {
            TBuf< 496 > buffer;

            VA_LIST list;
            VA_START( list, aFmt );
            buffer.AppendFormatList( aFmt, list );
            VA_END(list);

            FU_DEBUG(_L("#Fu# %S"), &buffer );
        }
};

class TEnterExitLog : public TDes16Overflow
{
    public:

        void Overflow(TDes16& /*aDes*/)
        {
            FU_DEBUG(_L("%S Logging Overflow"), &iFunctionName);
        }

        TEnterExitLog( TRefByValue<const TDesC> aFunctionName,
                       TRefByValue<const TDesC> aFmt, ... )
        {
            iFunctionName = HBufC::New( TDesC(aFunctionName).Length() );

            if ( iFunctionName )
            {
                iFunctionName->Des().Copy(aFunctionName);
            }

            TBuf< 496 > buffer;

            VA_LIST list;
            VA_START( list, aFmt );
            buffer.AppendFormatList( aFmt, list, this );
            VA_END(list);

            FU_DEBUG(_L("#Fu# --> %S %S"), iFunctionName, &buffer );
        }

        TEnterExitLog( TRefByValue<const TDesC> aFunctionName )
        {
            iFunctionName = HBufC::New( TDesC(aFunctionName).Length() );

            if ( iFunctionName )
            {
                iFunctionName->Des().Copy(aFunctionName);
            }

            FU_DEBUG(_L("#Fu# --> %S"), iFunctionName );
        }

        ~TEnterExitLog()
        {
            FU_DEBUG(_L("#Fu# <-- %S"), iFunctionName );
            delete iFunctionName;
        }

    private:
        HBufC*    iFunctionName;
};

_LIT(_KMPXErrorInfo, "#Fu# MPXVideo Error : error %d file %s line %d");

#define MPX_S(a) _S(a)

#define MPX_ERROR_LOG(aErr) \
    {\
        if (aErr) \
            FU_DEBUG(_KMPXErrorInfo, aErr, MPX_S(__FILE__), __LINE__);\
    }

#ifdef _DEBUG
    #define MPX_TRAP(_r, _s) TRAP(_r,_s);MPX_ERROR_LOG(_r);
    #define MPX_TRAPD(_r, _s) TRAPD(_r,_s);MPX_ERROR_LOG(_r);
#else // _DEBUG
    #define MPX_TRAP(_r, _s) TRAP(_r,_s);(_r=_r);
    #define MPX_TRAPD(_r, _s) TRAPD(_r,_s);(_r=_r);
#endif // _DEBUG

#endif  // __MPXVIDEO_DEBUG_H__

// End of File

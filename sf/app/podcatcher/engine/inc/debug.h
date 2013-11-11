/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

// dbg_dp.h
// debug printouts

/*************************************************

    USAGE
    DP("Hi there");
    DP1("My name is %S", &name);
    DPV1("My detailed info is %S", &detailedInfo);
    DP_MSG(aMsg);
    DP_MSG_COMPL(aMsg, r);
    DP_EXC(aType);

    CUSTOMISATION
    With SUPPORT_DP_CLIENT_SERVER_V2 defined, a
    proper opcode type (default TOpcode) needs to
    be specified.

    NOTE
    This file may be included by many components
    within one application and defining something
    here will affect all such components. In such
    cases it might be better to state a MACRO in
    the intended component's MMP file.

**************************************************/

#ifndef ___DBG_DP_H
#define ___DBG_DP_H

// This file may be included butThese defines may be individual

//#if defined (_DEBUG) && (__WINS__)
//#if defined _DEBUG
#   define SUPPORT_DP_STANDARD
#   define SUPPORT_DP_FILE
//#endif

// #define SUPPORT_RDEBUG_PRINT
// #define SUPPORT_DP_VERBOSE
// #define SUPPORT_DP_THREAD_NAME
// #define SUPPORT_DP_CLIENT_SERVER_V2
// #define SUPPORT_DP_EXCEPTION
// #define SUPPORT_DP_COMMDB

// ------------------------------------------------- no need to modify

#if defined (SUPPORT_RDEBUG_PRINT)\
 || defined (SUPPORT_DP_VERBOSE)\
 || defined(SUPPORT_DP_CLIENT_SERVER_V2)\
 || defined(SUPPORT_DP_EXCEPTION)
#   define SUPPORT_DP_STANDARD
#endif

#if defined(SUPPORT_RDEBUG_PRINT)
#   include <e32svr.h> // RDebug
#endif

#if defined(SUPPORT_DP_FILE)
#   include <flogger.h>
#endif

#if defined(SUPPORT_DP_CLIENT_SERVER_V2)
#   include "dbg_cmn.h" // TOpcode
#endif

#if defined(SUPPORT_DP_COMMDB)
#   include <commdbconnpref.h> // TCommDbConnPref
#endif

namespace Dbg
{

// ------------------------------------------------- file output
#ifdef SUPPORT_DP_FILE
_LIT(KLogDir, "podcatcher");
_LIT(KLogFile, "debug.log");
#   define DPF(a); RFileLogger::WriteFormat(Dbg::KLogDir, Dbg::KLogFile, EFileLoggingModeAppend, _L("%S %s"), &s, L##a);
#   define DPF1(a,b); RFileLogger::WriteFormat(Dbg::KLogDir, Dbg::KLogFile, EFileLoggingModeAppend, _L("%S " L##a), &s, b);
#   define DPF2(a,b,c); RFileLogger::WriteFormat(Dbg::KLogDir, Dbg::KLogFile, EFileLoggingModeAppend, _L("%S " L##a), &s, b,c);
#   define DPF3(a,b,c,d); RFileLogger::WriteFormat(Dbg::KLogDir, Dbg::KLogFile, EFileLoggingModeAppend, _L("%S " L##a), &s, b,c,d);
#   define DPF4(a,b,c,d,e); RFileLogger::WriteFormat(Dbg::KLogDir, Dbg::KLogFile, EFileLoggingModeAppend, _L("%S " L##a), &s, b,c,d,e);
#   define DPF5(a,b,c,d,e,f); RFileLogger::WriteFormat(Dbg::KLogDir, Dbg::KLogFile, EFileLoggingModeAppend, _L("%S " L##a), &s, b,c,d,e,f);
#   define DPF6(a,b,c,d,e,f,g); RFileLogger::WriteFormat(Dbg::KLogDir, Dbg::KLogFile, EFileLoggingModeAppend, _L("%S " L##a), &s, b, c, d,e,f,g);
#	define HEXDUMP(aHeader, aMargin, aPtr, aLen); RFileLogger::HexDump(Dbg::KLogDir, Dbg::KLogFile, EFileLoggingModeAppend, aHeader, aMargin, aPtr, aLen);
#   define DPDT(dt); RFileLogger::WriteFormat(Dbg::KLogDir, Dbg::KLogFile, EFileLoggingModeAppend, _L("DateTime: %d-%d-%d %d:%d:%d "), dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second() );

#else
#   define DPF(a);
#   define DPF1(a,b);
#   define DPF2(a,b,c);
#   define DPF3(a,b,c,d);
#   define DPF4(a,b,c,d,e);
#   define DPF5(a,b,c,d,e,f);
#   define DPF6(a,b,c,d,e,f,g);
#	define HEXDUMP(aHeader, aMargin, aPtr, aLen);
#   define DPDT(dt); 
#endif
// ------------------------------------------------- standard debugprint
#ifdef SUPPORT_DP_STANDARD

inline void FormatThreadName(TFullName& aName)
{
    const TChar KLeftBracket('[');
    const TChar KScope(':');
    TInt a = aName.LocateReverse(KLeftBracket);
    if (a==KErrNotFound)
        { return; }
    TInt b = aName.Locate(KScope);
    if (b==KErrNotFound || b<a)
        { return; }
    aName.Delete(a,b-a);
}

#ifdef SUPPORT_RDEBUG_PRINT
#   define DP(a);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        RDebug::Print(_L("%S %s"), &s, L##a);\
        DPF(a);\
    }
#   define DP1(a,b);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        RDebug::Print(_L("%S " L##a), &s, b);\
        DPF1(a,b);\
    }
#   define DP2(a,b,c);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        RDebug::Print(_L("%S " L##a), &s, b,c);\
        DPF2(a,b,c);\
    }
#   define DP3(a,b,c,d);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        RDebug::Print(_L("%S " L##a), &s, b,c,d);\
        DPF3(a,b,c,d);\
    }
#   define DP4(a,b,c,d,e);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        RDebug::Print(_L("%S " L##a), &s, b,c,d,e);\
        DPF4(a,b,c,d,e);\
    }
#   define DP5(a,b,c,d,e,f);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        RDebug::Print(_L("%S " L##a), &s, b,c,d,e,f);\
        DPF5(a,b,c,d,e,f);\
    }
#   define DP6(a,b,c,d,e,f,g);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        RDebug::Print(_L("%S " L##a), &s, b,c,d,e,f,g);\
        DPF6(a,b,c,d,e,f,g);\
    }
#else	//SUPPORT_RDEBUG_PRINT
#   define DP(a);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        DPF(a);\
    }
#   define DP1(a,b);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        DPF1(a,b);\
    }
#   define DP2(a,b,c);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        DPF2(a,b,c);\
    }
#   define DP3(a,b,c,d);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        DPF3(a,b,c,d);\
    }
#   define DP4(a,b,c,d,e);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        DPF4(a,b,c,d,e);\
    }
#   define DP5(a,b,c,d,e,f);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        DPF5(a,b,c,d,e,f);\
    }
#   define DP6(a,b,c,d,e,f,g);\
    {\
        TFullName s = RThread().FullName();\
        Dbg::FormatThreadName(s);\
        DPF6(a,b,c,d,e,f,g);\
    }
#endif	//SUPPORT_RDEBUG_PRINT
#   define ASSERT_DP(a, b); {if(!(a)){DP(b);}}
#   define ASSERT_DP1(a,b,c); {if(!(a)){DP1(b,c);}}
#   define ASSERT_DP2(a,b,c,d); {if(!(a)){DP2(b,c,d);}}
#   define ASSERT_DP3(a,b,c,d,e); {if(!(a)){DP3(b,c,d,e);}}
#   define ASSERT_DP4(a,b,c,d,e,f); {if(!(a)){DP4(b,c,d,e,f);}}
#   define ASSERT_DP5(a,b,c,d,e,f,g); {if(!(a)){DP5(b,c,d,e,f,g);}}
#   define ASSERT_DP6(a,b,c,d,e,f,g,h); {if(!(a)){DP6(b,c,d,e,f,g,h);}}
// @brief stuff to get enum strings from enums
#   define ENUM_CASE(n,c)\
        case c: { _LIT(KTxt##c, #c); n.Set(KTxt##c); break; }
#   define DEFAULT(n)\
        default: { _LIT(KTxtUnknown, "Unknown"); n.Set(KTxtUnknown); break; }
#else
#   define DP(a);
#   define DP1(a,b);;
#   define DP2(a,b,c);
#   define DP3(a,b,c,d);
#   define DP4(a,b,c,d,e);
#   define DP5(a,b,c,d,e,f);
#   define DP6(a,b,c,d,e,f,g);
#   define ASSERT_DP(a, b);
#   define ASSERT_DP1(a,b,c);
#   define ASSERT_DP2(a,b,c,d);
#   define ASSERT_DP3(a,b,c,d,e);
#   define ASSERT_DP4(a,b,c,d,e,f);
#   define ASSERT_DP5(a,b,c,d,e,f,g);
#   define ASSERT_DP6(a,b,c,d,e,f,g,h);
#   define ENUM_CASE(n,c)
#   define DEFAULT(n)
#endif
// ------------------------------------------------- detailed debugprint
#ifdef SUPPORT_DP_VERBOSE
#   define DPV(a); { DP(a); }
#   define DPV1(a,b); { DP1(a,b); }
#   define DPV2(a,b,c); { DP2(a,b,c); }
#   define DPV3(a,b,c,d); { DP3(a,b,c,d); }
#   define DPV4(a,b,c,d,e); { DP4(a,b,c,d,e); }
#   define DPV5(a,b,c,d,e,f); { DP5(a,b,c,d,e,f); }
#   define DPV6(a,b,c,d,e,f,g); { DP6(a,b,c,d,e,f,g); }
#	define DPDTV(dt); { DPDT(dt); }
#else
#   define DPV(a);
#   define DPV1(a,b);
#   define DPV2(a,b,c);
#   define DPV3(a,b,c,d);
#   define DPV4(a,b,c,d,e);
#   define DPV5(a,b,c,d,e,f);
#   define DPV6(a,b,c,d,e,f,g);
#	define DPDTV(dt);
#endif
// ------------------------------------------------- client/server V2
#ifdef SUPPORT_DP_CLIENT_SERVER_V2

// @brief Get client thread name from a message
static void GetClientName(const RMessage2& aMsg, TFullName& aName)
{
    RThread thread;
    aMsg.Client(thread);
    aName = thread.FullName();
    Dbg::FormatThreadName(aName);
}

// @brief Get server opcode from any message
static TOpcode GetServerOpcode(const RMessage2& aMsg)
{
    return (TOpcode)aMsg.Function();
}

// @brief Get server opcode name from a server opcode
static TPtrC GetServerOpcodeName(TOpcode aOpcode)
{
    TPtrC name;
    switch(aOpcode)
    {
        // ENUM_CASE(name, EOpcodeOne);
        // ENUM_CASE(name, EOpcodeTwo);
        DEFAULT(name);
    }
    return name;
}

// @brief Print generic message.
inline void PrintMsg(const RMessage2& aMsg)
{
    TFullName threadName;
    GetClientName(aMsg, threadName);
    TOpcode sOpcode = GetServerOpcode(aMsg);
    TPtrC sOpcodeName = GetServerOpcodeName(sOpcode);
    DP3("? %S %S(%d)", &threadName, &sOpcodeName, sOpcode);
}

// @brief Print generic message completion.
inline void PrintMsgCompl(const RMessage2& aMsg, TInt aErr)
{
    TFullName threadName;
    GetClientName(aMsg, threadName);
    TOpcode sOpcode = GetServerOpcode(aMsg);
    TPtrC sOpcodeName = GetServerOpcodeName(sOpcode);
    DP4("! %S %S(%d) r=%d", &threadName, &sOpcodeName, sOpcode, aErr);
}

// m= message, r=error
#   define DP_MSG(m); PrintMsg(m);
#   define DP_MSG_COMPL(m,r); PrintMsgCompl(m,r);
#else
#   define DP_MSG(m);
#   define DP_MSG_COMPL(m,r);
#endif
// ------------------------------------------------- exception debugprint
#ifdef SUPPORT_DP_EXCEPTION

// @brief Get an exception type
static TPtrC GetExcName(TExcType aType)
{
    TPtrC name;
    switch(aType)
    {
        ENUM_CASE(name, EExcGeneral);
        ENUM_CASE(name, EExcIntegerDivideByZero);
        ENUM_CASE(name, EExcSingleStep);
        ENUM_CASE(name, EExcBreakPoint);
        ENUM_CASE(name, EExcIntegerOverflow);
        ENUM_CASE(name, EExcBoundsCheck);
        ENUM_CASE(name, EExcInvalidOpCode);
        ENUM_CASE(name, EExcDoubleFault);
        ENUM_CASE(name, EExcStackFault);
        ENUM_CASE(name, EExcAccessViolation);
        ENUM_CASE(name, EExcPrivInstruction);
        ENUM_CASE(name, EExcAlignment);
        ENUM_CASE(name, EExcPageFault);
        ENUM_CASE(name, EExcFloatDenormal);
        ENUM_CASE(name, EExcFloatDivideByZero);
        ENUM_CASE(name, EExcFloatInexactResult);
        ENUM_CASE(name, EExcFloatInvalidOperation);
        ENUM_CASE(name, EExcFloatOverflow);
        ENUM_CASE(name, EExcFloatStackCheck);
        ENUM_CASE(name, EExcFloatUnderflow);
        ENUM_CASE(name, EExcAbort);
        ENUM_CASE(name, EExcKill);
        ENUM_CASE(name, EExcUserInterrupt);
        ENUM_CASE(name, EExcDataAbort);
        ENUM_CASE(name, EExcCodeAbort);
        ENUM_CASE(name, EExcMaxNumber);
        ENUM_CASE(name, EExcInvalidVector);
        DEFAULT(name);
    }
    return name;
}

// @brief Print exception info.
inline void PrintExc(TExcType aType)
{
    TPtrC excName = GetExcName(aType);
    DP2("### Exception %S(%d) ###", &excName, aType);
}

#   define DP_EXC(t); PrintExc(t);
#else
#   define DP_EXC(t);
#endif
// ------------------------------------------------- 
#ifdef SUPPORT_DP_COMMDB

inline void LogIap(const TDesC* aName, const TCommDbConnPref& aPref)
{
    TUint32 iapId=aPref.IapId();
    TUint32 netId=aPref.NetId();
    TCommDbDialogPref dlgPref=aPref.DialogPreference();
    TCommDbConnectionDirection dir=aPref.Direction();
    TUint32 bearerSet=aPref.BearerSet();
    if(aName==0)
    {
        UI_FLOG(Log::Printf(
            _L("iapId=%u, netId=%u, dlgPref=%d, dir=%d, bearerSet=0x%02x"),
            iapId, netId, dlgPref, dir, bearerSet));
    }
    else
    {
        UI_FLOG(Log::Printf(
            _L("\"%S\", iapId=%u, netId=%u, dlgPref=%d, dir=%d, bearerSet=0x%02x"),
            aName, iapId, netId, dlgPref, dir, bearerSet));
    }
}

#   define DP_COMMDBCONNPREF(a,b); LogIap(a,b);
#else
#   define DP_COMMDBCONNPREF(a,b);
#endif
// ------------------------------------------------- 

}
#endif //___DBG_DP_H


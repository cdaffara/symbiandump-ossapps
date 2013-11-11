/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Remote video handler.
*
*/


// INCLUDE FILES

#include    "CVtEngDrawDP.h"
#include    "CVtEngEventManager.h"
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"
#include    "MVtEngFrameObserver.h"
#include    <videotelephonyvariant.hrh>
#include    <tdisplaysinkparamsdp.h>
#include    <cvtlogger.h>
#include    <Posting_Surface_Factory.h>

#include <cvtimagebitmap.h>

// LOCAL CONSTANTS AND MACROS

// YCbCr conversion constants

// Luma
static const TUint32 KYLut[ 256 ] =
    {
    0x00000000, 0x00400801, 0x00801002, 0x00C01803,
    0x01002004, 0x01402805, 0x01803006, 0x01C03807,
    0x02004008, 0x02404809, 0x0280500A, 0x02C0580B,
    0x0300600C, 0x0340680D, 0x0380700E, 0x03C0780F,
    0x04008010, 0x04408811, 0x04809012, 0x04C09813,
    0x0500A014, 0x0540A815, 0x0580B016, 0x05C0B817,
    0x0600C018, 0x0640C819, 0x0680D01A, 0x06C0D81B,
    0x0700E01C, 0x0740E81D, 0x0780F01E, 0x07C0F81F,
    0x08010020, 0x08410821, 0x08811022, 0x08C11823,
    0x09012024, 0x09412825, 0x09813026, 0x09C13827,
    0x0A014028, 0x0A414829, 0x0A81502A, 0x0AC1582B,
    0x0B01602C, 0x0B41682D, 0x0B81702E, 0x0BC1782F,
    0x0C018030, 0x0C418831, 0x0C819032, 0x0CC19833,
    0x0D01A034, 0x0D41A835, 0x0D81B036, 0x0DC1B837,
    0x0E01C038, 0x0E41C839, 0x0E81D03A, 0x0EC1D83B,
    0x0F01E03C, 0x0F41E83D, 0x0F81F03E, 0x0FC1F83F,
    0x10020040, 0x10420841, 0x10821042, 0x10C21843,
    0x11022044, 0x11422845, 0x11823046, 0x11C23847,
    0x12024048, 0x12424849, 0x1282504A, 0x12C2584B,
    0x1302604C, 0x1342684D, 0x1382704E, 0x13C2784F,
    0x14028050, 0x14428851, 0x14829052, 0x14C29853,
    0x1502A054, 0x1542A855, 0x1582B056, 0x15C2B857,
    0x1602C058, 0x1642C859, 0x1682D05A, 0x16C2D85B,
    0x1702E05C, 0x1742E85D, 0x1782F05E, 0x17C2F85F,
    0x18030060, 0x18430861, 0x18831062, 0x18C31863,
    0x19032064, 0x19432865, 0x19833066, 0x19C33867,
    0x1A034068, 0x1A434869, 0x1A83506A, 0x1AC3586B,
    0x1B03606C, 0x1B43686D, 0x1B83706E, 0x1BC3786F,
    0x1C038070, 0x1C438871, 0x1C839072, 0x1CC39873,
    0x1D03A074, 0x1D43A875, 0x1D83B076, 0x1DC3B877,
    0x1E03C078, 0x1E43C879, 0x1E83D07A, 0x1EC3D87B,
    0x1F03E07C, 0x1F43E87D, 0x1F83F07E, 0x1FC3F87F,
    0x20040080, 0x20440881, 0x20841082, 0x20C41883,
    0x21042084, 0x21442885, 0x21843086, 0x21C43887,
    0x22044088, 0x22444889, 0x2284508A, 0x22C4588B,
    0x2304608C, 0x2344688D, 0x2384708E, 0x23C4788F,
    0x24048090, 0x24448891, 0x24849092, 0x24C49893,
    0x2504A094, 0x2544A895, 0x2584B096, 0x25C4B897,
    0x2604C098, 0x2644C899, 0x2684D09A, 0x26C4D89B,
    0x2704E09C, 0x2744E89D, 0x2784F09E, 0x27C4F89F,
    0x280500A0, 0x284508A1, 0x288510A2, 0x28C518A3,
    0x290520A4, 0x294528A5, 0x298530A6, 0x29C538A7,
    0x2A0540A8, 0x2A4548A9, 0x2A8550AA, 0x2AC558AB,
    0x2B0560AC, 0x2B4568AD, 0x2B8570AE, 0x2BC578AF,
    0x2C0580B0, 0x2C4588B1, 0x2C8590B2, 0x2CC598B3,
    0x2D05A0B4, 0x2D45A8B5, 0x2D85B0B6, 0x2DC5B8B7,
    0x2E05C0B8, 0x2E45C8B9, 0x2E85D0BA, 0x2EC5D8BB,
    0x2F05E0BC, 0x2F45E8BD, 0x2F85F0BE, 0x2FC5F8BF,
    0x300600C0, 0x304608C1, 0x308610C2, 0x30C618C3,
    0x310620C4, 0x314628C5, 0x318630C6, 0x31C638C7,
    0x320640C8, 0x324648C9, 0x328650CA, 0x32C658CB,
    0x330660CC, 0x334668CD, 0x338670CE, 0x33C678CF,
    0x340680D0, 0x344688D1, 0x348690D2, 0x34C698D3,
    0x3506A0D4, 0x3546A8D5, 0x3586B0D6, 0x35C6B8D7,
    0x3606C0D8, 0x3646C8D9, 0x3686D0DA, 0x36C6D8DB,
    0x3706E0DC, 0x3746E8DD, 0x3786F0DE, 0x37C6F8DF,
    0x380700E0, 0x384708E1, 0x388710E2, 0x38C718E3,
    0x390720E4, 0x394728E5, 0x398730E6, 0x39C738E7,
    0x3A0740E8, 0x3A4748E9, 0x3A8750EA, 0x3AC758EB,
    0x3B0760EC, 0x3B4768ED, 0x3B8770EE, 0x3BC778EF,
    0x3C0780F0, 0x3C4788F1, 0x3C8790F2, 0x3CC798F3,
    0x3D07A0F4, 0x3D47A8F5, 0x3D87B0F6, 0x3DC7B8F7,
    0x3E07C0F8, 0x3E47C8F9, 0x3E87D0FA, 0x3EC7D8FB,
    0x3F07E0FC, 0x3F47E8FD, 0x3F87F0FE, 0x3FC7F8FF
    };

// Chroma1 (Cb)
static const TUint32 KCbLut[ 256 ] =
    {
    0x0001531C, 0x00014B1E, 0x00014B20, 0x00014B22,
    0x00014324, 0x00014325, 0x00014327, 0x00013B29,
    0x00013B2B, 0x00013B2D, 0x0001332E, 0x00013330,
    0x00013332, 0x00012B34, 0x00012B35, 0x00012B37,
    0x00012339, 0x0001233B, 0x0001233D, 0x00011B3E,
    0x00011B40, 0x00011B42, 0x00011344, 0x00011345,
    0x00011347, 0x00010B49, 0x00010B4B, 0x00010B4D,
    0x0001034E, 0x00010350, 0x00010352, 0x0000FB54,
    0x0000FB55, 0x0000FB57, 0x0000F359, 0x0000F35B,
    0x0000F35D, 0x0000EB5E, 0x0000EB60, 0x0000EB62,
    0x0000E364, 0x0000E366, 0x0000E367, 0x0000DB69,
    0x0000DB6B, 0x0000DB6D, 0x0000D36E, 0x0000D370,
    0x0000D372, 0x0000CB74, 0x0000CB76, 0x0000CB77,
    0x0000C379, 0x0000C37B, 0x0000C37D, 0x0000BB7E,
    0x0000BB80, 0x0000BB82, 0x0000B384, 0x0000B386,
    0x0000B387, 0x0000AB89, 0x0000AB8B, 0x0000AB8D,
    0x0000AB8E, 0x0000A390, 0x0000A392, 0x0000A394,
    0x00009B96, 0x00009B97, 0x00009B99, 0x0000939B,
    0x0000939D, 0x0000939F, 0x00008BA0, 0x00008BA2,
    0x00008BA4, 0x000083A6, 0x000083A7, 0x000083A9,
    0x00007BAB, 0x00007BAD, 0x00007BAF, 0x000073B0,
    0x000073B2, 0x000073B4, 0x00006BB6, 0x00006BB7,
    0x00006BB9, 0x000063BB, 0x000063BD, 0x000063BF,
    0x00005BC0, 0x00005BC2, 0x00005BC4, 0x000053C6,
    0x000053C7, 0x000053C9, 0x00004BCB, 0x00004BCD,
    0x00004BCF, 0x000043D0, 0x000043D2, 0x000043D4,
    0x00003BD6, 0x00003BD8, 0x00003BD9, 0x000033DB,
    0x000033DD, 0x000033DF, 0x00002BE0, 0x00002BE2,
    0x00002BE4, 0x000023E6, 0x000023E8, 0x000023E9,
    0x00001BEB, 0x00001BED, 0x00001BEF, 0x000013F0,
    0x000013F2, 0x000013F4, 0x00000BF6, 0x00000BF8,
    0x00000BF9, 0x000003FB, 0x000003FD, 0x000003FF,
    0x00000000, 0x00000001, 0x00000003, 0x00000005,
    0x001FF807, 0x001FF808, 0x001FF80A, 0x001FF00C,
    0x001FF00E, 0x001FF010, 0x001FE811, 0x001FE813,
    0x001FE815, 0x001FE017, 0x001FE018, 0x001FE01A,
    0x001FD81C, 0x001FD81E, 0x001FD820, 0x001FD021,
    0x001FD023, 0x001FD025, 0x001FC827, 0x001FC828,
    0x001FC82A, 0x001FC02C, 0x001FC02E, 0x001FC030,
    0x001FB831, 0x001FB833, 0x001FB835, 0x001FB037,
    0x001FB039, 0x001FB03A, 0x001FA83C, 0x001FA83E,
    0x001FA840, 0x001FA041, 0x001FA043, 0x001FA045,
    0x001F9847, 0x001F9849, 0x001F984A, 0x001F904C,
    0x001F904E, 0x001F9050, 0x001F8851, 0x001F8853,
    0x001F8855, 0x001F8057, 0x001F8059, 0x001F805A,
    0x001F785C, 0x001F785E, 0x001F7860, 0x001F7061,
    0x001F7063, 0x001F7065, 0x001F6867, 0x001F6869,
    0x001F686A, 0x001F606C, 0x001F606E, 0x001F6070,
    0x001F5872, 0x001F5873, 0x001F5875, 0x001F5877,
    0x001F5079, 0x001F507A, 0x001F507C, 0x001F487E,
    0x001F4880, 0x001F4882, 0x001F4083, 0x001F4085,
    0x001F4087, 0x001F3889, 0x001F388A, 0x001F388C,
    0x001F308E, 0x001F3090, 0x001F3092, 0x001F2893,
    0x001F2895, 0x001F2897, 0x001F2099, 0x001F209A,
    0x001F209C, 0x001F189E, 0x001F18A0, 0x001F18A2,
    0x001F10A3, 0x001F10A5, 0x001F10A7, 0x001F08A9,
    0x001F08AB, 0x001F08AC, 0x001F00AE, 0x001F00B0,
    0x001F00B2, 0x001EF8B3, 0x001EF8B5, 0x001EF8B7,
    0x001EF0B9, 0x001EF0BB, 0x001EF0BC, 0x001EE8BE,
    0x001EE8C0, 0x001EE8C2, 0x001EE0C3, 0x001EE0C5,
    0x001EE0C7, 0x001ED8C9, 0x001ED8CB, 0x001ED8CC,
    0x001ED0CE, 0x001ED0D0, 0x001ED0D2, 0x001EC8D3,
    0x001EC8D5, 0x001EC8D7, 0x001EC0D9, 0x001EC0DB,
    0x001EC0DC, 0x001EB8DE, 0x001EB8E0, 0x001EB8E2
    };

// Chroma1 (Cr)
static const TUint32 KCrLut[ 256 ] =
    {
    0xD302D000, 0xD382C800, 0xD3C2C000, 0xD442B800,
    0xD482B800, 0xD502B000, 0xD542A800, 0xD582A800,
    0xD602A000, 0xD6429800, 0xD6C29000, 0xD7029000,
    0xD7428800, 0xD7C28000, 0xD8028000, 0xD8827800,
    0xD8C27000, 0xD9027000, 0xD9826800, 0xD9C26000,
    0xDA425800, 0xDA825800, 0xDAC25000, 0xDB424800,
    0xDB824800, 0xDC024000, 0xDC423800, 0xDC823800,
    0xDD023000, 0xDD422800, 0xDDC22000, 0xDE022000,
    0xDE421800, 0xDEC21000, 0xDF021000, 0xDF820800,
    0xDFC20000, 0xE041F800, 0xE081F800, 0xE0C1F000,
    0xE141E800, 0xE181E800, 0xE201E000, 0xE241D800,
    0xE281D800, 0xE301D000, 0xE341C800, 0xE3C1C000,
    0xE401C000, 0xE441B800, 0xE4C1B000, 0xE501B000,
    0xE581A800, 0xE5C1A000, 0xE601A000, 0xE6819800,
    0xE6C19000, 0xE7418800, 0xE7818800, 0xE7C18000,
    0xE8417800, 0xE8817800, 0xE9017000, 0xE9416800,
    0xE9816800, 0xEA016000, 0xEA415800, 0xEAC15000,
    0xEB015000, 0xEB814800, 0xEBC14000, 0xEC014000,
    0xEC813800, 0xECC13000, 0xED412800, 0xED812800,
    0xEDC12000, 0xEE411800, 0xEE811800, 0xEF011000,
    0xEF410800, 0xEF810800, 0xF0010000, 0xF040F800,
    0xF0C0F000, 0xF100F000, 0xF140E800, 0xF1C0E000,
    0xF200E000, 0xF280D800, 0xF2C0D000, 0xF300D000,
    0xF380C800, 0xF3C0C000, 0xF440B800, 0xF480B800,
    0xF4C0B000, 0xF540A800, 0xF580A800, 0xF600A000,
    0xF6409800, 0xF6C09000, 0xF7009000, 0xF7408800,
    0xF7C08000, 0xF8008000, 0xF8807800, 0xF8C07000,
    0xF9007000, 0xF9806800, 0xF9C06000, 0xFA405800,
    0xFA805800, 0xFAC05000, 0xFB404800, 0xFB804800,
    0xFC004000, 0xFC403800, 0xFC803800, 0xFD003000,
    0xFD402800, 0xFDC02000, 0xFE002000, 0xFE401800,
    0xFEC01000, 0xFF001000, 0xFF800800, 0xFFC00000,
    0x00000000, 0x00400000, 0x009FF800, 0x011FF000,
    0x015FF000, 0x01DFE800, 0x021FE000, 0x025FE000,
    0x02DFD800, 0x031FD000, 0x039FC800, 0x03DFC800,
    0x041FC000, 0x049FB800, 0x04DFB800, 0x055FB000,
    0x059FA800, 0x05DFA800, 0x065FA000, 0x069F9800,
    0x071F9000, 0x075F9000, 0x079F8800, 0x081F8000,
    0x085F8000, 0x08DF7800, 0x091F7000, 0x095F7000,
    0x09DF6800, 0x0A1F6000, 0x0A9F5800, 0x0ADF5800,
    0x0B5F5000, 0x0B9F4800, 0x0BDF4800, 0x0C5F4000,
    0x0C9F3800, 0x0D1F3000, 0x0D5F3000, 0x0D9F2800,
    0x0E1F2000, 0x0E5F2000, 0x0EDF1800, 0x0F1F1000,
    0x0F5F1000, 0x0FDF0800, 0x101F0000, 0x109EF800,
    0x10DEF800, 0x111EF000, 0x119EE800, 0x11DEE800,
    0x125EE000, 0x129ED800, 0x12DED800, 0x135ED000,
    0x139EC800, 0x141EC000, 0x145EC000, 0x149EB800,
    0x151EB000, 0x155EB000, 0x15DEA800, 0x161EA000,
    0x169E9800, 0x16DE9800, 0x171E9000, 0x179E8800,
    0x17DE8800, 0x185E8000, 0x189E7800, 0x18DE7800,
    0x195E7000, 0x199E6800, 0x1A1E6000, 0x1A5E6000,
    0x1A9E5800, 0x1B1E5000, 0x1B5E5000, 0x1BDE4800,
    0x1C1E4000, 0x1C5E4000, 0x1CDE3800, 0x1D1E3000,
    0x1D9E2800, 0x1DDE2800, 0x1E1E2000, 0x1E9E1800,
    0x1EDE1800, 0x1F5E1000, 0x1F9E0800, 0x1FDE0800,
    0x205E0000, 0x209DF800, 0x211DF000, 0x215DF000,
    0x21DDE800, 0x221DE000, 0x225DE000, 0x22DDD800,
    0x231DD000, 0x239DC800, 0x23DDC800, 0x241DC000,
    0x249DB800, 0x24DDB800, 0x255DB000, 0x259DA800,
    0x25DDA800, 0x265DA000, 0x269D9800, 0x271D9000,
    0x275D9000, 0x279D8800, 0x281D8000, 0x285D8000,
    0x28DD7800, 0x291D7000, 0x295D7000, 0x29DD6800,
    0x2A1D6000, 0x2A9D5800, 0x2ADD5800, 0x2B1D5000,
    0x2B9D4800, 0x2BDD4800, 0x2C5D4000, 0x2C9D3800
    };


// Posting surface factory DLL's name
_LIT( KDPAPIDLLName, "PostingSurfaceFactory.dll" );

// Reversed enumeration for the bitmaps.
static const MDisplaySink::TBitmapNo KVtEngBitmapNoReversed[] =
        {
        MDisplaySink::ESecondBitmap,
        MDisplaySink::EFirstBitmap
        };

// Default posting format (if this or KDefaultPostingFormat420 is not
// supported, BaseConstructL() will leave with KErrNotSupported)
const CPostingSurface::TPostingFormat KDefaultPostingFormat422 =
    CPostingSurface::EYuv422LeBt601Range0;

// Default posting format (if this or KDefaultPostingFormat422 is not
// supported, BaseConstructL() will leave with KErrNotSupported)
const CPostingSurface::TPostingFormat KDefaultPostingFormat420 =
    CPostingSurface::EYuv420PlanarBt601Range0;

// Default posting buffering flags.
const TUint KDefaultPostingBufferingFlags =
        // Double buffering
        CPostingSurface::EDoubleBuffering |
        // Progressive frames (no interlacing)
        CPostingSurface::EProgressiveFrames |
        // Do not allow frameskip
        CPostingSurface::EDisAllowFrameSkip |
        // Use internal buffers (allocated by DP)
        CPostingSurface::EInternalBuffers;

// Default pixel aspect ratio num. Aspect ratio = Num/Denom.
const TUint16 KDefaultPixelAspectRatioNum = 1;

// Default pixel aspect ratio denom. Aspect ratio = Num/Denom.
const TUint16 KDefaultPixelAspectRatioDenom = 1;

// LOCAL METHODS

// -----------------------------------------------------------------------------
// DoConvertYuv420toXRGB0888
// -----------------------------------------------------------------------------
//
inline void DoConvertYuv420toXRGB0888(
    TInt aWidth,                    // width in pixels
    const TUint8*& aSourceDataPtrY1, // pointer to Y data row 1
    const TUint8*& aSourceDataPtrY2, // pointer to Y data row 2
    const TUint8*& aSourceDataPtrU,  // pointer to U data row
    const TUint8*& aSourceDataPtrV,  // pointer to V data row
    TUint8* aDestDataPtr1,          // pointer to destination row 1
    TUint8* aDestDataPtr2           // pointer to destination row 2
    )
    {
    TUint32* destDataPtr1 = reinterpret_cast< TUint32* >( aDestDataPtr1 );
    TUint32* destDataPtr2 = reinterpret_cast< TUint32* >( aDestDataPtr2 );
    // width assumed to be even
    TInt pixelsLeft = aWidth / 2;

    TUint32 r, g, b;
    TUint32 LutY0, LutY1, LutC;
    TUint32 pixel;
    TUint32 OverflowMask = 0xC0180300U;

    // One scanline at the time
    while ( pixelsLeft-- )
        {

        // row 1, 2 pixels

        LutY0 = KYLut[*aSourceDataPtrY1++];
        LutY1 = KYLut[*aSourceDataPtrY1++];
        LutC  = KCbLut[*aSourceDataPtrU++];
        LutC += KCrLut[*aSourceDataPtrV++];

        pixel = LutY0 + LutC;

        if (pixel & OverflowMask)
            {
            if (pixel & 0xC0000000U)
                {
                if (pixel & 0x80000000U)
                    {
                    pixel &= ~0x3FC00000U;
                    }
                else
                    {
                    pixel |=  0x3FC00000U;
                    }
                }
            if (pixel & 0x00180000U)
                {
                if (pixel & 0x00100000U)
                    {
                    pixel &= ~0x0007F800U;
                    }
                else
                    {
                    pixel |=  0x0007F800U;
                    }
                }
            if (pixel & 0x00000300U)
                {
                if (pixel & 0x00000200U)
                    {
                    pixel &= ~0x000000FFU;
                    }
                else
                    {
                    pixel |=  0x000000FFU;
                    }
                }
            }

        r = (pixel >> 6) & 0xFF0000;
        g = (pixel >> 3) & 0x00FF00;
        b =  pixel       & 0x0000FF;

        *destDataPtr1++ = 0xFF000000 | r | g | b;

        pixel = LutY1 + LutC;

        if (pixel & OverflowMask)
            {
            if (pixel & 0xC0000000U)
                {
                if (pixel & 0x80000000U)
                    {
                    pixel &= ~0x3FC00000U;
                    }
                else
                    {
                    pixel |=  0x3FC00000U;
                    }
                }
            if (pixel & 0x00180000U)
                {
                if (pixel & 0x00100000U)
                    {
                    pixel &= ~0x0007F800U;
                    }
                else
                    {
                    pixel |=  0x0007F800U;
                    }
                }
            if (pixel & 0x00000300U)
                {
                if (pixel & 0x00000200U)
                    {
                    pixel &= ~0x000000FFU;
                    }
                else
                    {
                    pixel |=  0x000000FFU;
                    }
                }
            }

        r = (pixel >> 6) & 0xFF0000;
        g = (pixel >> 3) & 0x00FF00;
        b =  pixel       & 0x0000FF;

        *destDataPtr1++ = 0xFF000000 | r | g | b;

        // row, 2 pixels

        LutY0 = KYLut[*aSourceDataPtrY2++];
        LutY1 = KYLut[*aSourceDataPtrY2++];

        pixel = LutY0 + LutC;

        if (pixel & OverflowMask)
            {
            if (pixel & 0xC0000000U)
                {
                if (pixel & 0x80000000U)
                    {
                    pixel &= ~0x3FC00000U;
                    }
                else
                    {
                    pixel |=  0x3FC00000U;
                    }
                }
            if (pixel & 0x00180000U)
                {
                if (pixel & 0x00100000U)
                    {
                    pixel &= ~0x0007F800U;
                    }
                else
                    {
                    pixel |=  0x0007F800U;
                    }
                }
            if (pixel & 0x00000300U)
                {
                if (pixel & 0x00000200U)
                    {
                    pixel &= ~0x000000FFU;
                    }
                else
                    {
                    pixel |=  0x000000FFU;
                    }
                }
            }

        r = (pixel >> 6) & 0xFF0000;
        g = (pixel >> 3) & 0x00FF00;
        b =  pixel       & 0x0000FF;

        *destDataPtr2++ = 0xFF000000 | r | g | b;

        pixel = LutY1 + LutC;

        if (pixel & OverflowMask)
            {
            if (pixel & 0xC0000000U)
                {
                if (pixel & 0x80000000U)
                    {
                    pixel &= ~0x3FC00000U;
                    }
                else
                    {
                    pixel |=  0x3FC00000U;
                    }
                }
            if (pixel & 0x00180000U)
                {
                if (pixel & 0x00100000U)
                    {
                    pixel &= ~0x0007F800U;
                    }
                else
                    {
                    pixel |=  0x0007F800U;
                    }
                }
            if (pixel & 0x00000300U)
                {
                if (pixel & 0x00000200U)
                    {
                    pixel &= ~0x000000FFU;
                    }
                else
                    {
                    pixel |=  0x000000FFU;
                    }
                }
            }

        r = (pixel >> 6) & 0xFF0000;
        g = (pixel >> 3) & 0x00FF00;
        b =  pixel       & 0x0000FF;

        *destDataPtr2++ = 0xFF000000 | r | g | b;

        }
    }

// -----------------------------------------------------------------------------
// DoConvertYuv422toXRGB0888
// -----------------------------------------------------------------------------
//
inline void DoConvertYuv422toXRGB0888(
    TInt aWidth,                    // width in pixels
    const TUint8*& aSourceDataPtr,   // pointer to source row
    TUint8* aDestDataPtr            // pointer to destination row
    )
    {
    TUint32* destDataPtr = reinterpret_cast< TUint32* >( aDestDataPtr );
    // width assumed to be even
    TInt pixelsLeft = aWidth / 2;

    TUint32 r, g, b;
    TUint32 LutY0, LutY1, LutC;
    TUint32 pixel;
    TUint32 OverflowMask = 0xC0180300U;

    // One scanline at the time
    while ( pixelsLeft-- )
        {

        LutC  = KCbLut[*aSourceDataPtr++];
        LutY0 = KYLut[*aSourceDataPtr++];
        LutC += KCrLut[*aSourceDataPtr++];
        LutY1 = KYLut[*aSourceDataPtr++];

        pixel = LutY0 + LutC;

        if (pixel & OverflowMask)
            {
            if (pixel & 0xC0000000U)
                {
                if (pixel & 0x80000000U)
                    {
                    pixel &= ~0x3FC00000U;
                    }
                else
                    {
                    pixel |=  0x3FC00000U;
                    }
                }
            if (pixel & 0x00180000U)
                {
                if (pixel & 0x00100000U)
                    {
                    pixel &= ~0x0007F800U;
                    }
                else
                    {
                    pixel |=  0x0007F800U;
                    }
                }
            if (pixel & 0x00000300U)
                {
                if (pixel & 0x00000200U)
                    {
                    pixel &= ~0x000000FFU;
                    }
                else
                    {
                    pixel |=  0x000000FFU;
                    }
                }
            }

        r = (pixel >> 6) & 0xFF0000;
        g = (pixel >> 3) & 0x00FF00;
        b =  pixel       & 0x0000FF;

        *destDataPtr++ = 0xFF000000 | r | g | b;

        pixel = LutY1 + LutC;

        if (pixel & OverflowMask)
            {
            if (pixel & 0xC0000000U)
                {
                if (pixel & 0x80000000U)
                    {
                    pixel &= ~0x3FC00000U;
                    }
                else
                    {
                    pixel |=  0x3FC00000U;
                    }
                }
            if (pixel & 0x00180000U)
                {
                if (pixel & 0x00100000U)
                    {
                    pixel &= ~0x0007F800U;
                    }
                else
                    {
                    pixel |=  0x0007F800U;
                    }
                }
            if (pixel & 0x00000300U)
                {
                if (pixel & 0x00000200U)
                    {
                    pixel &= ~0x000000FFU;
                    }
                else
                    {
                    pixel |=  0x000000FFU;
                    }
                }
            }

        r = (pixel >> 6) & 0xFF0000;
        g = (pixel >> 3) & 0x00FF00;
        b =  pixel       & 0x0000FF;

        *destDataPtr++ = 0xFF000000 | r | g | b;
        }
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::NewL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CVtEngDrawDP* CVtEngDrawDP::NewL(
    MDisplaySinkObserver* aObserver )
    {
    __VTPRINTENTER( "RVD(DP).NewL" )
    CVtEngDrawDP* self = new ( ELeave ) CVtEngDrawDP( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    __VTPRINTEXIT( "RVD(DP).NewL" )
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::~CVtEngDrawDP
// Destructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngDrawDP::~CVtEngDrawDP()
    {
    __VTPRINTENTER( "RVD(DP).~" )
    Cancel();
    delete iFrameBuffer;
    delete iBufferWaiter;
    delete iPostingSurface;
    __VTPRINT( DEBUG_MEDIA , "RVD(DP).~ RRegion::Close()" )
    iClipRegion.Close();
    CloseDPLibrary();

    delete iTarget;
    delete iSource;
    delete iImageScaler;
    __VTPRINTEXIT( "RVD(DP).~" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CVtEngDraw
// C++ constructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngDrawDP::CVtEngDrawDP( MDisplaySinkObserver* aObserver ) :
    CVtEngDraw( aObserver ), iClipRegion( 1 ),
    iPostingFormat( KDefaultPostingFormat422 )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseConstructL
// Base construction.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseConstructL()
    {
    __VTPRINTENTER( "RVD(DP).BaseConstructL" )
    if ( IsFlag( EFlagUIForeground ) )
        {
        DoBaseConstructL();
        }
    else
        {
        SetFlag( EBaseConstructCalled );
        }
    __VTPRINTEXIT( "RVD(DP).BaseConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CreateSinkL
// Instantiates display sink.
// -----------------------------------------------------------------------------
//
MDisplaySink* CVtEngDrawDP::CreateSinkL()
    {
    __VTPRINTENTER( "RVD(DP).CreateSinkL" )
    TDisplaySinkParamsDP params;
    GetSinkParams( params );
    MDisplaySink* sink = ::CreateSinkL( params, KNullDesC8 );
    if ( IsFlag( EBaseConstructCalled ) )
        {
        sink->Pause();
        SetFlag( ESinkParamUpdateRequired | ESinkResumeRequired );
        }
    __VTPRINTEXIT( "RVD(DP).CreateSinkL" )
    return sink;
    }

// -----------------------------------------------------------------------------
// CVtEngRemoteVideo::CVtEngDraw::BaseRefreshL
// Draw a frame.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseRefreshL()
    {
    __VTPRINTENTER( "RVD(DP).BaseRefreshL" )
    __VTPRINTEXIT( "RVD(DP).BaseRefreshL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseRefreshBitmapsL
// Refreshes bitmaps sizes and display mode if needed.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseRefreshBitmapsL()
    {
    __VTPRINTENTER( "RVD(DP).BaseRefreshBitmapsL" )
    BaseConstructL();
    InitializePostingSurfaceL();
    // set the params for DS
    UpdateSinkParamsL();
    ClearFlag( EFirstFrameReceived );
    __VTPRINTEXIT( "RVD(DP).BaseRefreshBitmapsL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseDoCancel
// DoCancel for derived class.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseDoCancel()
    {
    __VTPRINTENTER( "RVD(DP).BaseDoCancel" )
    __VTPRINTEXIT( "RVD(DP).BaseDoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseVerifyConfigL
// Checks current configuration against DP implementation.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseVerifyConfigL()
    {
    __VTPRINTENTER( "RVD(DP).BaseVerifyConfigL" )
    // Check support for requested size
    if ( !SupportsSize( iSourceSize ) )
        {
        __VTPRINT( DEBUG_MEDIA ,
            "RVD(DP).BaseVerifyConfigL Size NOT supported" )
        User::Leave( KErrNotSupported );
        }
    RecalculateClipRegionL();
    __VTPRINTEXIT( "RVD(DP).BaseVerifyConfigL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseRestartL
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseRestartL()
    {
    __VTPRINTENTER( "RVD(DP).BaseRestartL" )
    if ( IsFlag( EFlagUIForeground ) )
        {
        RecalculateClipRegionL();
        iBufferWaiter->FetchBuffer();
        }
    __VTPRINTEXIT( "RVD(DP).BaseRestartL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseAbortNow
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseAbortNow()
    {
    __VTPRINTENTER( "RVD(DP).BaseAbortNow" )
    UpdateVtFrame();
    __VTPRINTEXIT( "RVD(DP).BaseAbortNow" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::StartDrawL
// Starts the drawing.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseStartDrawL()
    {
    __VTPRINTENTER( "RVD(DP).BaseStartDrawL" )
    if ( IsFlag( EBaseConstructCalled ) )
        {
        SetFlag( EBaseBaseStartDrawCalled );
        }
    else
        {
        DoBaseStartDrawL();
        }
    __VTPRINTEXIT( "RVD(DP).BaseStartDrawL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseStopDraw
// Stop the drawing.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseStopDraw()
    {
    // Cancel the timer in any case.
    __VTPRINTENTER( "RVD(DP).BaseStopDraw" )
    ClearFlag( EBaseBaseStartDrawCalled );
    iWatcher->Cancel();
    if ( iDSA )
        {
        ClearFlag( ERemoteVideoDrawing | EStarted );
        iDSA->Cancel();
        }
    iBufferWaiter->Cancel();
    Cancel();
    __VTPRINTEXIT( "RVD(DP).BaseStopDraw" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseSetConfigL
// Sets new configuration.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseSetConfigL( const TVtEngRenderConfig& aConfig )
    {
    __VTPRINTENTER( "RVD(DP).BaseSetConfigL" )
    Mem::FillZ( &iConfig, sizeof( TVtEngRenderConfigDP ) );
    iConfig = aConfig;
    if ( IsFlag( EBaseConstructCalled ) )
        {
        SetFlag( EBaseSetConfigCalled );
        }
    else
        {
        ConfigUpdatedL();
        }
    __VTPRINTEXIT( "RVD(DP).BaseSetConfigL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseSetUIForegroundL
// Sets UI foreground status.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseSetUIForegroundL( TBool aForeground )
    {
    __VTPRINTENTER( "RVD(DP).BaseSetUIForegroundL" )
    if( aForeground )
        {
        TBool wasSentToBackground( IsFlag( ESentToBackground ) );
        SetFlag( EFlagUIForeground );
        ClearFlag( ESentToBackground );
        if( IsFlag( EDelayedCallsMask ) )
            {
            HandleDelayedCallsL();
            }      
        if ( wasSentToBackground )
            {
            BaseRefreshBitmapsL();
            Restart( RDirectScreenAccess::ETerminateCancel );
            BaseStartDrawL();
            }       
        }
    else
        {
        if ( !IsFlag( EBaseConstructCalled ) )
            {
            BaseStopDraw();
            delete iPostingSurface;
            iPostingSurface = 0;
            for ( TInt i = MDisplaySink::EFirstBitmap;
                  i < MDisplaySink::EFirstBitmap + KVtEngMaxBuffers; i++ )
                {
                iBuffers[ i ].UnSet();
                }
            SetFlag( ESentToBackground );
            }
        ClearFlag( EFlagUIForeground );
        }
    __VTPRINTEXIT( "RVD(DP).BaseSetUIForegroundL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseRequestLastFrame
//  Request update last remote video frame through MVtEngFrameObserver::vtSetFrame
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseRequestLastFrame()
    {
    __VTPRINTENTER( "RVD(DP).BaseRequestLastFrame" )
    UpdateVtFrame();
    __VTPRINTEXIT( "RVD(DP).BaseRequestLastFrame" )    
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BaseVideoFrameSizeChangedL
// Called when incoming frame buffer size changes.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BaseVideoFrameSizeChangedL( const TSize& aTo )
    {
    __VTPRINTENTER( "RVD(DP).BaseVideoFrameSizeChangedL" )
    CreateFrameBufferL( aTo );
    __VTPRINTEXIT( "RVD(DP).BaseVideoFrameSizeChangedL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::Config
// Returns current configuration.
// -----------------------------------------------------------------------------
//
TVtEngRenderConfig& CVtEngDrawDP::Config()
    {
    return iConfig;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::RunL
// CActive heir execution method.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::RunL()
    {
    __VTPRINTENTER( "RVD(DP).RunL" )

    TInt bitmapNo( iStatus.Int() );

    if ( bitmapNo < KErrNone )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(DP).RunL notify rvd problem" )
        CVtEngEventManager::NotifyEvent( KVtEngRemoteRenderingProblem );
        return;
        }

    TVtEngBuffer& buffer = iBuffers[ bitmapNo ];

    if ( !iDSUpdated )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(DP).RunL !Updated" )
        __VTPRINTEXIT( "RVD(DP).RunL" )
        return;
        }
    else if ( !IsFlag( EFlagUIForeground ) )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(DP).RunL !Flag( EFlagUIForeground )" )
        __VTPRINTEXIT( "RVD(DP).RunL" )
        if ( iPostingSurface )
            {
            iPostingSurface->PostBuffer( buffer.Buffer() );
            buffer.UnSet();
            iBufferWaiter->FetchBuffer();
            }
        return;
        }
    else if ( buffer.Buffer() == NULL )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(DP).RunL signalled with NULL buffer" )
        TInt result( iBufferWaiter->FetchBuffer() );
        __VTPRINT2( DEBUG_MEDIA,
            "RVD(DP).RunL FetchBuffer %d", result )
        User::LeaveIfError( result );
        __VTPRINTEXIT( "RVD(DP).RunL" )
        return;
        }

    // Image received, reset counter.
    iCheckpoint = KVtEngWatcherThreshold;

    StoreFrameBuffer( buffer.Buffer() );
    __VTPRINT2( DEBUG_MEDIA, "RVD(DP).RunL flags=%d", iFlags )
    TBool firstFrame( !IsFlag ( EFirstFrameReceived ) );
    SetFlag( EFirstFrameReceived );
    // Remote video is about to start?
    if ( !IsFlag( EStarted ) && IsFlag( EReadyInternal ) )
        {
        if ( !iDSA->IsActive() )
            {
            iDSA->StartL();
            }
        SetFlag( EStarted );
        if ( !iConfig.iClipRect.IsEmpty() )
            {
            SetFlag( EReadyForeground );
            iDSA->Gc()->SetClippingRect( iConfig.iClipRect );
            RecalculateClipRegionL();
            }
        __VTPRINT( DEBUG_MEDIA, "RVD(DP).RunL notify rvd start" )
        //CVtEngEventManager::NotifyEvent( KVtEngRemoteVideoStarted );
        }
    if ( firstFrame )
        {
        CVtEngEventManager::NotifyEvent( KVtEngRemoteVideoStarted );
        }
    TRAPD( setClipRegResult, SetClipRegionL() );
    if ( setClipRegResult )
        {
        __VTPRINT2( DEBUG_MEDIA, "RVD(DP).RunL SetClipRegionL failed %d",
            setClipRegResult );
        }
    __VTPRINT3( DEBUG_MEDIA, "RVD(DP).RunL received buffer %d $%x",
        bitmapNo,
        reinterpret_cast< TUint >( buffer.Buffer() ) )
    TInt result
        ( iPostingSurface->PostBuffer( buffer.Buffer() ) );
    __VTPRINT2( DEBUG_MEDIA, "RVD(DP).RunL PostBuffer %d", result )
    buffer.UnSet();
    if ( iBuffers[ KVtEngBitmapNoReversed[ bitmapNo ] ].IsSet() )
        {
        __VTPRINT2( DEBUG_MEDIA, "RVD(DP).RunL reactivating for buffer %d",
            KVtEngBitmapNoReversed[ bitmapNo ] )
        Activate();
        DisplaySink().NextFrame();
        }
        
    if ( result == KErrNotReady )
        {
        BaseRefreshBitmapsL();
        result = KErrNone;
        }
        
    User::LeaveIfError( result );
    result = iBufferWaiter->FetchBuffer();
    __VTPRINT2( DEBUG_MEDIA,
        "RVD(DP).RunL FetchBuffer %d", result )
    User::LeaveIfError( result );

    __VTPRINTEXIT( "RVD(DP).RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::BufferReady
// BufferReady
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::BufferReady( CPostingSurface::TPostingBuff* aBuffer )
    {
    __VTPRINTENTER( "RVD(DP).BufferReady" )
    SetAvailableToSink( aBuffer );
    __VTPRINTEXIT( "RVD(DP).BufferReady" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::FetchError
// Buffer fetching error
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::FetchError( TInt )
    {
    __VTPRINTENTER( "RVD(DP).FetchError" )
    CVtEngEventManager::NotifyEvent( KVtEngRemoteRenderingProblem );
    __VTPRINTEXIT( "RVD(DP).FetchError" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::HandleDelayedCallsL
//
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::HandleDelayedCallsL()
    {
    __VTPRINTENTER( "RVD(DP).HandleDelayedCallsL" )
    if ( IsFlag( EBaseConstructCalled ) )
        {
        DoBaseConstructL();
        }
    if ( IsFlag( EBaseSetConfigCalled ) )
        {
        ConfigUpdatedL();
        ClearFlag( EBaseSetConfigCalled );
        }
    if ( IsFlag( EInitializePostingSurfaceCalled ) )
        {
        DoInitializePostingSurfaceL();
        }
    if ( IsFlag( ESinkParamUpdateRequired ) )
        {
        DoUpdateSinkParamsL();
        }
    if ( IsFlag( EBaseBaseStartDrawCalled ) )
        {
        DoBaseStartDrawL();
        }
    __VTPRINTEXIT( "RVD(DP).HandleDelayedCallsL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::DoBaseConstructL
// Base construction implementation
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::DoBaseConstructL()
    {
    __VTPRINTENTER( "RVD(DP).DoBaseConstructL" )

    if ( !iImageScaler )
        {
        iImageScaler =
            CVtImageScaler::NewL( *this, CVtImageScaler::EBilinear );
        }
    delete iBufferWaiter;
    iBufferWaiter = NULL;
    delete iPostingSurface;
    iPostingSurface = NULL;

    // Load DP API library
    LoadDPLibraryL();

    // Create posting surface instance
    iPostingSurface = CreatePostingSurfaceL();

    CPostingSurface::TPostingFormat previousPostingFormat( iPostingFormat );

    // Check support for format
    if ( !SupportsFormat( KDefaultPostingFormat420 ) )
        {
        if ( !SupportsFormat( KDefaultPostingFormat422 ) )
            {
            __VTPRINT( DEBUG_MEDIA ,
                "RVD(DP).BaseConstructL Format NOT supported" )
            User::Leave( KErrNotSupported );
            }
        else
            {
            __VTPRINT( DEBUG_MEDIA , "RVD(DP).BaseConstructL using 422" )
            iPostingFormat = KDefaultPostingFormat422;
            }
        }
    else
        {
        __VTPRINT( DEBUG_MEDIA , "RVD(DP).BaseConstructL using 420" )
        iPostingFormat = KDefaultPostingFormat420;
        }
    iPostingSurface->GetCapabilities( iPostingCaps );
    iBufferWaiter =
        new ( ELeave ) CVtEngBufferWaiter( *iPostingSurface, *this );
    for ( TInt i = MDisplaySink::EFirstBitmap;
          i < MDisplaySink::EFirstBitmap + KVtEngMaxBuffers; i++ )
        {
        iBuffers[ i ].UnSet();
        }

    // If posting for is different from previous, then free frame buffer
    if ( ( previousPostingFormat != iPostingFormat ) || !iFrameBuffer )
        {
        CreateFrameBufferL( iSourceSize );
        }
    ClearFlag( EBaseConstructCalled );
    __VTPRINTEXIT( "RVD(DP).DoBaseConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::UpdateSinkParamsL
// Updates sink params
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::UpdateSinkParamsL()
    {
    __VTPRINTENTER( "RVD(DP).UpdateSinkParamsL" )
    if ( IsFlag( EBaseConstructCalled ) )
        {
        SetFlag( ESinkParamUpdateRequired );
        }
    else
        {
        DoUpdateSinkParamsL();
        }
    __VTPRINTEXIT( "RVD(DP).UpdateSinkParamsL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::DoBaseStartDrawL
//
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::DoBaseStartDrawL()
    {
    __VTPRINTENTER( "RVD(DP).DoBaseStartDrawL" )
    UpdateVtFrame();
    if ( iDSA )
        {
        SetFlag( EReadyInternal );
        if ( !iWatcher->IsActive() )
            {
            iWatcher->Start( KVtEngWatcherTimeout, KVtEngWatcherTimeout,
                iCallback );
            }
        User::LeaveIfError( iBufferWaiter->FetchBuffer() );
        SetFlag( ERemoteVideoDrawing );
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    ClearFlag( EBaseBaseStartDrawCalled );
    __VTPRINTEXIT( "RVD(DP).DoBaseStartDrawL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::DoInitializePostingSurfaceL
// Initializes posting surface.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::DoInitializePostingSurfaceL()
    {
    __VTPRINTENTER( "RVD(DP).DoInitializePostingSurfaceL" )
    CPostingSurface::TPostingSourceParams postingSourceParams;
    GetPostingSourceParams( postingSourceParams );
    CPostingSurface::TPostingParams postingParams;
    GetPostingParams( postingParams );
    // Create and start DSA for getting access to DP
    CreateAndStartDSAL();
    
    /*S60 Display Posting API has a limitation that only one surface instance can be 
    created at a time. Unfortunately there's no way to check when posting is really 
    free. PostingSurface initialization maybe returns KErrInUse.
    Please refer to TSW error EAKC-7R3TME.
	The transition effect between landscape and portrait uses ATID to achieve smooth 
	transition. ATID and PostingSurface share the same buffers and cannot be used 
	simultaneously. PostingSurface initialization maybe returns KErrNotReady. 
	Please refer to TSW error EWXO-7P4DV3.
	So it probably requires some kind of timer based retry to bypass these cases.*/
    TInt err;
    while (1)
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(DP).DoInitializePostingSurfaceL Initialize PostingSurface" );
        TRAP( err, iPostingSurface->InitializeL(postingSourceParams, postingParams) );
        if ( err == KErrNone)
            {
            break;
            }
        else
            {
            //time interval for each PostingSurface initialization  is 0.2 second.
            __VTPRINT2( DEBUG_MEDIA, "RVD(DP).DoInitializePostingSurfaceL InitPS error = %d", err )
            User::After(200000);
            }
        }
        
    delete iTarget; iTarget = NULL;
    iTarget = CVtImageBitmap::NewL(
        postingParams.iDisplayedRect.Size(), EColor16MU );
    if ( iSource )
        {
        if ( IsFlag( EFrameBufferFilled ) &&
             !IsFlag( EFrameBufferNeedsConvertAndScale ) )
            {
            iImageScaler->ScaleNowL( *iSource, *iTarget );
            Config().iObserver->vtSetFrame(
                MVtEngFrameObserver::ERemoteVideoFrame,
                &iTarget->Bitmap() );
            }
        else
            {
            Config().iObserver->vtSetFrame(
                MVtEngFrameObserver::ERemoteVideoFrame, NULL );
            }
        }
    else
        {
        //delete iSource; iSource = NULL;
        iSource = CVtImageBitmap::NewL( iSourceSize, EColor16MU );
        Config().iObserver->vtSetFrame( MVtEngFrameObserver::ERemoteVideoFrame,
            NULL );
        }
    RecalculateClipRegionL();
    ClearFlag( EInitializePostingSurfaceCalled );
    __VTPRINTEXIT( "RVD(DP).DoInitializePostingSurfaceL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::DoUpdateSinkParamsL
// Updates sink params
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::DoUpdateSinkParamsL()
    {
    __VTPRINTENTER( "RVD(DP).DoUpdateSinkParamsL" )
    TDisplaySinkParamsDP params;
    GetSinkParams( params );
    DisplaySink().UpdateSinkParamsL( params, iDSUpdated );
    ClearFlag( ESinkParamUpdateRequired );
    if ( IsFlag( ESinkResumeRequired ) )
        {
        DisplaySink().Resume();
        ClearFlag( ESinkResumeRequired );
        }
    __VTPRINTEXIT( "RVD(DP).DoUpdateSinkParamsL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::IsAlphaTrashed
// Checks if the alpha has been trashed on screen buffer.
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawDP::IsAlphaTrashed() const
    {
    __VTPRINTENTER( "RVD(DP).IsAlphaTrashed" )
    TBool isTrashed( EFalse );
    const CFbsScreenDevice& sdev( *iDSA->ScreenDevice() );
    if ( sdev.DisplayMode() == EColor16MA )
        {
        TRgb color;
        TPoint point( iClipRegion.BoundingRect().Center() );
        sdev.GetPixel( color, point );
        isTrashed = ( color.Alpha() != 0 );
        }
    __VTPRINTEXITR( "RVD(DP).IsAlphaTrashed %d", isTrashed )
    return isTrashed;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SetClipRegionL
// Sets clipping region for DP.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::SetClipRegionL()
    {
    __VTPRINTENTER( "RVD(DP).SetClipRegionL" )
    if ( iClipRegionUpdated || IsAlphaTrashed() )
        {
        iClipRegionUpdated = EFalse;
        CFbsBitGc* gc = iDSA->Gc();
        gc->SetClippingRegion( iDSA->DrawingRegion() );
        gc->SetDrawMode( CGraphicsContext::EDrawModeAND );
        gc->SetBrushColor( TRgb( 255, 255, 255, 0 ) );
        gc->Clear();
        iDSA->ScreenDevice()->Update();
        User::LeaveIfError(
            iPostingSurface->SetClipRegion( iClipRegion ) );
        }
    __VTPRINTEXIT( "RVD(DP).SetClipRegionL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::InitializePostingSurfaceL
// Initializes posting surface.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::InitializePostingSurfaceL()
    {
    __VTPRINTENTER( "RVD(DP).InitializePostingSurfaceL" )
    if ( IsFlag( EBaseConstructCalled ) )
        {
        SetFlag( EInitializePostingSurfaceCalled );
        }
    else
        {
        DoInitializePostingSurfaceL();
        }
    __VTPRINTEXIT( "RVD(DP).InitializePostingSurfaceL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::GetPostingSourceParams
// Fills posting source params structure.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::GetPostingSourceParams(
    CPostingSurface::TPostingSourceParams& aParams ) const
    {
    __VTPRINTENTER( "RVD(DP).GetPostingSourceParams" )
    Mem::FillZ( &aParams, sizeof( CPostingSurface::TPostingSourceParams ) );
    aParams.iPostingBuffering = KDefaultPostingBufferingFlags;
    aParams.iBufferType = CPostingSurface::EStandardBuffer;
    aParams.iPostingUsage = CPostingSurface::EVideoCapture;
    aParams.iSourceImageSize = iSourceSize;
    aParams.iPostingFormat = iPostingFormat;
    aParams.iPixelAspectRatioNum = KDefaultPixelAspectRatioNum;
    aParams.iPixelAspectRatioDenom = KDefaultPixelAspectRatioDenom;
    aParams.iContentCopyRestricted = EFalse;
    __VTPRINTEXIT( "RVD(DP).GetPostingSourceParams" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::GetPostingParams
// Fills posting params structure.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::GetPostingParams(
    CPostingSurface::TPostingParams& aParams ) const
    {
    __VTPRINTENTER( "RVD(DP).GetPostingParams" )
    Mem::FillZ( &aParams, sizeof( CPostingSurface::TPostingParams ) );

    aParams.iDisplayedRect = iConfig.iClipRect;

    __VTPRINT3( DEBUG_MEDIA, "RVD(DP).GetPostingParams DRect.iTl (%d, %d)",
        aParams.iDisplayedRect.iTl.iX, aParams.iDisplayedRect.iTl.iY );
    __VTPRINT3( DEBUG_MEDIA, "RVD(DP).GetPostingParams DRect.iBr (%d, %d)",
        aParams.iDisplayedRect.iBr.iX, aParams.iDisplayedRect.iBr.iY );

    TRect& dispRect( aParams.iDisplayedRect );
    dispRect.Move( iConfig.iOffset );
    FixRectForDP( dispRect );
    __VTPRINT3( DEBUG_MEDIA, "RVD(DP).GetPostingParams OffsDRect.iTl (%d, %d)",
        dispRect.iTl.iX, dispRect.iTl.iY );
    __VTPRINT3( DEBUG_MEDIA, "RVD(DP).GetPostingParams OffsDRect.iBr (%d, %d)",
        dispRect.iBr.iX, dispRect.iBr.iY );

    aParams.iScaleToRect = TRect( aParams.iDisplayedRect.Size() );

    aParams.iInputCrop = iSourceSize;

    __VTPRINT3( DEBUG_MEDIA, "RVD(DP).GetPostingParams InputCrop.iTl (%d, %d)",
        aParams.iInputCrop.iTl.iX, aParams.iInputCrop.iTl.iY );
    __VTPRINT3( DEBUG_MEDIA, "RVD(DP).GetPostingParams InputCrop.iBr (%d, %d)",
        aParams.iInputCrop.iBr.iX, aParams.iInputCrop.iBr.iY );

    aParams.iBackGround = iConfig.iBackGround;
    __VTPRINTEXIT( "RVD(DP).GetPostingParams" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SupportsFormat
//
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawDP::SupportsFormat(
    const CPostingSurface::TPostingFormat& aFormat ) const
    {
    return iPostingSurface->FormatSupported( aFormat );
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SupportsSize
//
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawDP::SupportsSize( const TSize& aSize ) const
    {
    return ( iPostingCaps.iMaxPixelSize.iWidth >= aSize.iWidth ) &&
        ( iPostingCaps.iMaxPixelSize.iHeight >= aSize.iHeight );
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SupportsRotation
//
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawDP::SupportsRotation(
    CPostingSurface::TRotationType aRotation ) const
    {
    return ( ( aRotation & iPostingCaps.iSupportedRotations ) == aRotation );
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SupportsMirroring
//
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawDP::SupportsMirroring() const
    {
    return iPostingCaps.iSupportsMirroring;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SupportsScaling
//
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawDP::SupportsScaling() const
    {
    return iPostingCaps.iSupportsScaling;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SupportsBrightness
//
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawDP::SupportsBrightness() const
    {
    return iPostingCaps.iSupportsBrightnessControl;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SupportsContrast
//
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawDP::SupportsContrast() const
    {
    return iPostingCaps.iSupportsContrastControl;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::GetFrameBufferSize
// Returns size of frame buffer in bytes for given pixel size and current
// posting format.
// -----------------------------------------------------------------------------
//
TInt CVtEngDrawDP::GetFrameBufferSize( const TSize& aSize ) const
    {
    TInt size( 0 );
    __VTPRINTENTER( "RVD(DP).GetFrameBufferSize" )
    switch( iPostingFormat )
        {
    case KDefaultPostingFormat420:
        size = ( aSize.iWidth * aSize.iHeight * 3 ) >> 1;
        break;

    case KDefaultPostingFormat422:
        size = ( aSize.iWidth * aSize.iHeight ) * 2;
        break;

    default:
        break;
        }
    __VTPRINTEXITR( "RVD(DP).GetFrameBufferSize %d", size )
    return size;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::RecalculateClipRegionL
// Updates clipregion.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::RecalculateClipRegionL()
    {
    __VTPRINTENTER( "RVD(DP).RecalculateClipRegionL" )
    // Clip region needs to be updated
    iClipRegionUpdated = ETrue;
    iClipRegion.Clear();
    TRect clippingRect( iConfig.iClipRect );
    clippingRect.Move( iConfig.iOffset );
    FixRectForDP( clippingRect );
    iClipRegion.AddRect( clippingRect );
    if ( iClipRegion.CheckError() )
        {
        __VTPRINT( DEBUG_MEDIA ,
            "RVD(DP).RecalculateClipRegionL failed to add cliprect" )
        User::Leave( KErrNoMemory );
        }

    if ( IsFlag( EStarted ) && iDSA && iDSA->DrawingRegion() )
        {
        iClipRegion.Intersect( *iDSA->DrawingRegion() );
        if ( iClipRegion.CheckError() )
            {
            __VTPRINT( DEBUG_MEDIA ,
                "RVD(DP).RecalculateClipRegionL failed to intersect" )
            User::Leave( KErrNoMemory );
            }

#ifdef VTDEBUG

    TRect bRect( iClipRegion.BoundingRect() );
    __VTPRINT3( DEBUG_MEDIA, "RVD(DP).RecalculateClipRegionL BRect.iTl (%d, %d)",
        bRect.iTl.iX, bRect.iTl.iY );
    __VTPRINT3( DEBUG_MEDIA, "RVD(DP).RecalculateClipRegionL BRect.iBr (%d, %d)",
        bRect.iBr.iX, bRect.iBr.iY );
#endif // VTDEBUG
        }
    __VTPRINTEXIT( "RVD(DP).RecalculateClipRegionL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::FixRectForDP
// Fixes rect for DP (i.e. width is made even and starting X coordinate is made
// even as well)
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::FixRectForDP( TRect& aRect ) const
    {
    __VTPRINTENTER( "RVD(DP).FixRectForDP" )
    if ( aRect.iTl.iX & 1 )
        {
        aRect.Move( 1, 0 );
        aRect.SetWidth( Max( 0, aRect.Width() - 1 ) );
        }
    if ( aRect.Width() & 1 )
        {
        aRect.SetWidth( aRect.Width() - 1 );
        }
    __VTPRINTEXIT( "RVD(DP).FixRectForDP" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::RecalculateClipRegionL
// Fills sink params.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::GetSinkParams( TDisplaySinkParamsDP& aSinkParams )
    {
    __VTPRINTENTER( "RVD(DP).GetSinkParams" )
    Mem::FillZ( &aSinkParams, sizeof( TDisplaySinkParamsDP ) );
    aSinkParams.iThreadId = RThread().Id();
    aSinkParams.iRequestStatusPtr = &iStatus;
    aSinkParams.iObserver = iObserver;
    aSinkParams.iPostingBuffer1 = &iBuffers[ 0 ].Buffer();
    aSinkParams.iPostingBuffer2 = &iBuffers[ 1 ].Buffer();
    aSinkParams.iFrameSize = iSourceSize;
    aSinkParams.iPostingFormat = iPostingFormat;
    aSinkParams.iFlags = TDisplaySinkParams::EDisplaySinkDP;
    __VTPRINTEXITR( "RVD(DP).GetSinkParams flags=%d",
        (TInt) aSinkParams.iFlags )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::DisplaySink
// Returns reference to sink.
// -----------------------------------------------------------------------------
//
MDisplaySink& CVtEngDrawDP::DisplaySink()
    {
    return *iDisplaySink;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::StoreFrameBuffer
// Stores given frame buffer.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::StoreFrameBuffer(
    CPostingSurface::TPostingBuff* aPostingBuffer )
    {
    __VTPRINTENTER( "RVD(DP).StoreFrameBuffer" )
    if ( iPostingFormat == KDefaultPostingFormat422 )
        {
        const TInt height( iSourceSize.iHeight );
        const TInt srcStride( aPostingBuffer->GetStride() );
        const TInt dstStride( iSourceSize.iWidth * 2 );
        TUint8* dst =
            const_cast< TUint8* >( iFrameBuffer->Des().Ptr() );
        const TUint8* src =
            reinterpret_cast< const TUint8* >( aPostingBuffer->GetBuffer() );
        for ( TInt y = height - 1; y >= 0; y-- )
            {
            dst = Mem::Copy( dst, src, dstStride );
            src += srcStride;
            }
        }
    else
        {
        TInt frameBufferSizeInBytes( GetFrameBufferSize( iSourceSize ) );
        TPtrC8 postingBufferDesC(
            reinterpret_cast< const TUint8* >( aPostingBuffer->GetBuffer() ),
            frameBufferSizeInBytes );
        *iFrameBuffer = postingBufferDesC;
        }
    SetFlag( EFrameBufferFilled | EFrameBufferNeedsConvertAndScale );
    __VTPRINTEXIT( "RVD(DP).StoreFrameBuffer" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CreateFrameBufferL
// Creates frame buffer.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::CreateFrameBufferL( const TSize& aSize )
    {
    __VTPRINTENTER( "RVD(DP).CreateFrameBufferL" )
    delete iFrameBuffer; iFrameBuffer = NULL;
    iFrameBuffer = HBufC8::NewL( GetFrameBufferSize( aSize ) );
    ClearFlag( EFrameBufferFilled | EFrameBufferNeedsConvertAndScale );
    __VTPRINTEXIT( "RVD(DP).CreateFrameBufferL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SetAvailableToSink
// Sets posting buffer available to sink.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::SetAvailableToSink( CPostingSurface::TPostingBuff* aBuffer )
    {
    __VTPRINTENTER( "RVD(DP).SetAvailableToSink" )
    for ( TInt i = MDisplaySink::EFirstBitmap;
          i < MDisplaySink::EFirstBitmap + KVtEngMaxBuffers; i++ )
        {
        if ( !iBuffers[ i ].IsSet() )
            {
            __VTPRINT3( DEBUG_MEDIA,
                "RVD(DP).SetAvailableToSink buffer %d: $%x",
                i, reinterpret_cast< TUint >( aBuffer->GetBuffer() ) );
            iBuffers[ i ].Set( aBuffer );
            Activate();
            DisplaySink().SetBitmapAvailable(
                static_cast< MDisplaySink::TBitmapNo >( i ) );
            break;
            }
        }
    __VTPRINTEXIT( "RVD(DP).SetAvailableToSink" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::SetAvailableToSink
// Sets posting buffer available to sink.
// -----------------------------------------------------------------------------
//
CPostingSurface* CVtEngDrawDP::CreatePostingSurfaceL()
    {
    __VTPRINTENTER( "RVD(DP).CreatePostingSurfaceL" )
    CPostingSurface* postingSurface = NULL;
    CPostingSurfaceFactory* ( *createFactoryL )()  =
        reinterpret_cast< CPostingSurfaceFactory*(*)() >( iDPLib->Lookup( 1 ) );
    if ( createFactoryL )
        {
        CPostingSurfaceFactory* factory = createFactoryL();
        CleanupStack::PushL( factory );
        postingSurface = factory->NewPostingSurfaceL( EMainDisplay );
        CleanupStack::PopAndDestroy(); // factory
        }
    __VTPRINTEXIT( "RVD(DP).CreatePostingSurfaceL" )
    return postingSurface;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CloseDPLibrary
// Close the DP API library and free the allocated instance.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::CloseDPLibrary()
    {
    if ( iDPLib )
        {
        iDPLib->Close();
        delete iDPLib;
        iDPLib = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::LoadDPLibraryL
// Loads DP API library.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::LoadDPLibraryL()
    {
    CloseDPLibrary();
    iDPLib = new ( ELeave ) RLibrary();
    TInt error( iDPLib->Load( KDPAPIDLLName() ) );
    if ( error )
        {
        delete iDPLib;
        iDPLib = NULL;
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::Convert420ToVtImg
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::Convert420ToVtImg( HBufC8* aSource,
    CVtImageBitmap& aTarget )
    {
    __VTPRINTENTER( "RVD(DP).Convert420ToVtImg" )

    CFbsBitmap& bitmap( aTarget.Bitmap() );
    TSize size( bitmap.SizeInPixels() );
    TInt height( size.iHeight );
    TInt width( size.iWidth );
    TInt bytes( bitmap.ScanLineLength( width, bitmap.DisplayMode() ) );

    const TUint8* y1ptr = aSource->Ptr();
    const TUint8* y2ptr = y1ptr + width;
    const TUint8* uptr = y1ptr + ( width * height );
    const TUint8* vptr = uptr + ( width * height ) / 4;

    bitmap.LockHeap();

    TUint8* d1ptr = reinterpret_cast< TUint8* >( bitmap.DataAddress() );
    TUint8* d2ptr = d1ptr + bytes;

    height /= 2;    // divide by 2 because we process to rows at time
    bytes *= 2;     // multiply by 2 so we don't need to do it in loop

    while ( height-- )
        {
        DoConvertYuv420toXRGB0888( width, y1ptr, y2ptr, uptr, vptr, d1ptr,
            d2ptr );
        y1ptr = y2ptr;
        y2ptr += width;
        d1ptr += bytes;
        d2ptr += bytes;
        }

    bitmap.UnlockHeap();

    __VTPRINTEXIT( "RVD(DP).Convert420ToVtImg" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::Convert422ToVtImg
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::Convert422ToVtImg( HBufC8* aSource, CVtImageBitmap& aTarget )
    {
    __VTPRINTENTER( "RVD(DP).Convert422ToVtImg" )
    CFbsBitmap& bitmap( aTarget.Bitmap() );
    TSize size( bitmap.SizeInPixels() );
    TInt height( size.iHeight );
    TInt width( size.iWidth );
    TInt bytes( bitmap.ScanLineLength( width, bitmap.DisplayMode() ) );

    const TUint8* sptr = aSource->Ptr();

    bitmap.LockHeap();

    TUint8* dptr = reinterpret_cast< TUint8* >( bitmap.DataAddress() );
    while ( height-- )
        {
        DoConvertYuv422toXRGB0888( width, sptr, dptr );
        dptr += bytes;
        }

    bitmap.UnlockHeap();

    __VTPRINTEXIT( "RVD(DP).Convert422ToVtImg" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::UpdateVtFrame
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::UpdateVtFrame()
    {
    __VTPRINTENTER( "RVD(DP).UpdateVtFrame" )
    if ( IsFlag( EFrameBufferFilled ) )
        {
        CFbsBitmap* bitmap = NULL;
        if ( IsFlag( EFrameBufferNeedsConvertAndScale ) )
            {
            if ( iPostingFormat == KDefaultPostingFormat422 )
                {
                Convert422ToVtImg( iFrameBuffer, *iSource );
                }
            else
                {
                Convert420ToVtImg( iFrameBuffer, *iSource );
                }
            TRAPD( error, iImageScaler->ScaleNowL( *iSource, *iTarget ) );
            if ( !error )
                {
                bitmap = &iTarget->Bitmap();
                }
            ClearFlag( EFrameBufferNeedsConvertAndScale );
            }
        else
            {
            bitmap = &iTarget->Bitmap();
            }
        Config().iObserver->vtSetFrame( MVtEngFrameObserver::ERemoteVideoFrame,
            bitmap );
        }
    __VTPRINTEXIT( "RVD(DP).UpdateVtFrame" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::ScalingFinished
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::ScalingFinished( TInt )
    {
    }

// Implementation for CVtEngDrawDP::TVtEngBuffer

// -----------------------------------------------------------------------------
// CVtEngDrawDP::TVtEngBuffer::TVtEngBuffer
//
// -----------------------------------------------------------------------------
//
CVtEngDrawDP::TVtEngBuffer::TVtEngBuffer()
    : iBuffer( 0 )
    {
    __VTPRINTENTER( "RVD(DP).Buffer.Ctor" )
    __VTPRINTEXIT( "RVD(DP).Buffer.Ctor" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::TVtEngBuffer::Set
//
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::TVtEngBuffer::Set( CPostingSurface::TPostingBuff* aBuffer )
    {
    __VTPRINTENTER( "RVD(DP).Buffer.Set" )
    __VTPRINT2( DEBUG_MEDIA, "RVD(DP).Buffer.Set $%x",
        reinterpret_cast< TUint >( aBuffer->GetBuffer() ) )
    iBuffer = aBuffer;
    __VTPRINTEXIT( "RVD(DP).Buffer.Set" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::TVtEngBuffer::UnSet
//
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::TVtEngBuffer::UnSet()
    {
    __VTPRINTENTER( "RVD(DP).Buffer.UnSet" )
    iBuffer = 0;
    __VTPRINTEXIT( "RVD(DP).Buffer.UnSet" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::TVtEngBuffer::IsSet
//
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawDP::TVtEngBuffer::IsSet() const
    {
    return ( iBuffer != 0 );
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::TVtEngBuffer::Buffer
//
// -----------------------------------------------------------------------------
//
CPostingSurface::TPostingBuff*& CVtEngDrawDP::TVtEngBuffer::Buffer()
    {
    return iBuffer;
    }

// Implementation for CVtEngDrawDP::CVtEngBufferWaiter

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CVtEngBufferWaiter::CVtEngBufferWaiter
// -----------------------------------------------------------------------------
//
CVtEngDrawDP::CVtEngBufferWaiter::CVtEngBufferWaiter
    ( CPostingSurface& aPostingSurface, MVtEngBufferWaiterObserver& aObserver )
    : CActive( CActive::EPriorityStandard ),
      iPostingSurface( &aPostingSurface ), iObserver( &aObserver )
    {
    __VTPRINTENTER( "RVD(DP).BufferWaiter.Ctor" )
    CActiveScheduler::Add( this );
    __VTPRINTEXIT( "RVD(DP).BufferWaiter.Ctor" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CVtEngBufferWaiter::~CVtEngBufferWaiter
// -----------------------------------------------------------------------------
//
CVtEngDrawDP::CVtEngBufferWaiter::~CVtEngBufferWaiter()
    {
    __VTPRINTENTER( "RVD(DP).BufferWaiter.~" )
    Cancel();
    __VTPRINTEXIT( "RVD(DP).BufferWaiter.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CVtEngBufferWaiter::Activate
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::CVtEngBufferWaiter::Activate()
    {
    __VTPRINTENTER( "RVD(DP).BufferWaiter.Activate" )
    iStatus = KRequestPending;
    SetActive();
    __VTPRINTEXIT( "RVD(DP).BufferWaiter.Activate" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CVtEngBufferWaiter::FetchBuffer
// -----------------------------------------------------------------------------
//
TInt CVtEngDrawDP::CVtEngBufferWaiter::FetchBuffer()
    {
    __VTPRINTENTER( "RVD(DP).BufferWaiter.FetchBuffer" )
    // do nothing if we're already active
    if ( IsActive() )
        {
        __VTPRINTEXIT( "RVD(DP).BufferWaiter.FetchBuffer" )
        return KErrNone;
        }
    // else start fetching
    Activate();
    __VTPRINTEXIT( "RVD(DP).BufferWaiter.FetchBuffer" )
    return iPostingSurface->WaitForNextBuffer( iStatus );
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CVtEngBufferWaiter::RunL
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::CVtEngBufferWaiter::RunL()
    {
    __VTPRINTENTER( "RVD(DP).BufferWaiter.RunL" )
    __VTPRINT2( DEBUG_MEDIA, "RVD(DP).BufferWaiter.RunL %d", iStatus.Int() );
    TInt result( iStatus.Int() );
    if (  result == KErrNone )
        {
        iObserver->BufferReady( iPostingSurface->NextBuffer() );
        }
    else
        {
        iObserver->FetchError( result );
        }
    __VTPRINTEXIT( "RVD(DP).BufferWaiter.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDP::CVtEngBufferWaiter::DoCancel
// -----------------------------------------------------------------------------
//
void CVtEngDrawDP::CVtEngBufferWaiter::DoCancel()
    {
    __VTPRINTENTER( "RVD(DP).BufferWaiter.DoCancel" )
    iPostingSurface->CancelBuffer();
    __VTPRINTEXIT( "RVD(DP).BufferWaiter.DoCancel" )
    }

//  End of File

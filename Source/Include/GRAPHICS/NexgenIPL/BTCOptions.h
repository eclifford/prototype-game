/////////////////////////////////////////////////////////////////////////////
//
// NexgenIPL - Next Generation Image Processing Library
// Copyright (c) 1999-2003 Binary Technologies
// All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in this file is used in any commercial application 
// then a simple email would be nice.
//
// THIS SOFTWARE IS PROVIDED BY BINARY TECHNOLOGIES ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BINARY TECHNOLOGIES OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// Binary Technologies
// http://www.binary-technologies.com
// info@binary-technologies.com
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BTCOPTIONS_INCLUDED
#define BTCOPTIONS_INCLUDED

#pragma once

#include "BTDefines.h"
#include "BTNexgenIPL.h"
#include "BTCColor.h"

#ifndef BTGETOPTION
#define BTGETOPTION(opt,id,val,def) if(NULL != (opt)) { (void)(opt)->GetOption(id,val); } else { val = def; }
#endif
#ifndef BTSETOPTION
#define BTSETOPTION(opt,id,val) if(NULL != (opt)) { (void)(opt)->SetOption(id,val); }
#endif

class BTNEXGENIPL_CLASS_EXPORT BTCOptions
{
public:
	BTCOptions() {}
	virtual ~BTCOptions() {}

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT GetOption(const long lId, long &lValue);
	virtual BTRESULT GetOption(const long lId, double &dValue);
	virtual BTRESULT GetOption(const long lId, char *pszValue);
	virtual BTRESULT GetOption(const long lId, bool &bValue);
	virtual BTRESULT GetOption(const long lId, BTColorRGB &rgbValue);

	virtual BTRESULT SetOption(const long lId, const int nValue);
	virtual BTRESULT SetOption(const long lId, const long lValue);
	virtual BTRESULT SetOption(const long lId, const double dValue);
	virtual BTRESULT SetOption(const long lId, const char *pszValue);
	virtual BTRESULT SetOption(const long lId, const bool bValue);
	virtual BTRESULT SetOption(const long lId, const BTColorRGB rgbValue);
};

class BTNEXGENIPL_CLASS_EXPORT BTCDecoderOptions : public BTCOptions
{
public:
	BTCDecoderOptions() {}
	virtual ~BTCDecoderOptions() {}
};

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptions : public BTCOptions
{
public:
	BTCEncoderOptions() {}
	virtual ~BTCEncoderOptions() {}
};

/////////////////////////////////////////////////////////////////////////////
// BMP options
/////////////////////////////////////////////////////////////////////////////

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptionsBMP : public BTCEncoderOptions
{
public:
	BTCEncoderOptionsBMP();
	virtual ~BTCEncoderOptionsBMP();

	enum { BTEO_BMP_ENABLERLE = 0 };

	virtual BTRESULT GetOption(const long lId, bool &bValue);
	virtual BTRESULT SetOption(const long lId, const bool bValue);

private:
	bool m_bEnableRLE; // [in] Enable/disable RLE (run length encryption) compression.
};

/////////////////////////////////////////////////////////////////////////////
// TGA options
/////////////////////////////////////////////////////////////////////////////

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptionsTGA : public BTCEncoderOptions
{
public:
	BTCEncoderOptionsTGA();
	virtual ~BTCEncoderOptionsTGA();

	enum { BTEO_TGA_ENABLERLE = 0 };

	virtual BTRESULT GetOption(const long lId, bool &bValue);
	virtual BTRESULT SetOption(const long lId, const bool bValue);

private:
	bool m_bEnableRLE; // [in] Enable/disable RLE (run length encryption) compression.
};

/////////////////////////////////////////////////////////////////////////////
// MNG options
/////////////////////////////////////////////////////////////////////////////

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptionsMNG : public BTCEncoderOptions
{
public:
	BTCEncoderOptionsMNG();
	virtual ~BTCEncoderOptionsMNG();

	enum { BTEO_MNG_COMPRESSIONLEVEL = 0 };

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT SetOption(const long lId, const int nValue);

private:
	int m_nCompressionLevel; // [in] 1...9 = Speed...Best, 0 = no compression, -1 = default.
};

/////////////////////////////////////////////////////////////////////////////
// JPEG options
/////////////////////////////////////////////////////////////////////////////

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptionsJPEG : public BTCEncoderOptions
{
public:
	BTCEncoderOptionsJPEG();
	virtual ~BTCEncoderOptionsJPEG();

	enum {	BTEO_JPEG_QUALITY = 0,
			BTEO_JPEG_SMOOTHINGFACTOR,
			BTEO_JPEG_DCTMETHOD,
			BTEO_JPEG_OPTIMIZECODING };

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT GetOption(const long lId, bool &bValue);
	
	virtual BTRESULT SetOption(const long lId, const int nValue);
	virtual BTRESULT SetOption(const long lId, const bool bValue);

private:
	int  m_nQuality;         // [in] Quality of compression on a scale from 0..100 [low..best].
	int  m_nSmoothingFactor; // [in] 1..100, or 0 for no input smoothing.
	int  m_nDCTMethod;       // [in] DCT/IDCT algorithm.
	bool m_bOptimizeCoding;  // [in] true = optimize entropy encoding parms.
};

class BTNEXGENIPL_CLASS_EXPORT BTCDecoderOptionsJPEG : public BTCDecoderOptions
{
public:
	BTCDecoderOptionsJPEG();
	virtual ~BTCDecoderOptionsJPEG();

	enum {	BTDO_JPEG_DCTMETHOD = 0,
			BTDO_JPEG_USEEMBEDDEDPROFILE };

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT GetOption(const long lId, bool &bValue);
	
	virtual BTRESULT SetOption(const long lId, const int nValue);
	virtual BTRESULT SetOption(const long lId, const bool bValue);

private:
	int  m_nDCTMethod;          // [in] DCT/IDCT algorithm.
	bool m_bUseEmbeddedProfile; // [in] Use embedded color profile if available.
};

/////////////////////////////////////////////////////////////////////////////
// PBM/PGM/PPM options
/////////////////////////////////////////////////////////////////////////////

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptionsPXM : public BTCEncoderOptions
{
public:
	BTCEncoderOptionsPXM();
	virtual ~BTCEncoderOptionsPXM();

	enum {	BTEO_PXM_ENCODINGTYPE = 0,
			BTEO_PXM_DESCRIPTION };

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT GetOption(const long lId, char *pszValue);
	
	virtual BTRESULT SetOption(const long lId, const int nValue);
	virtual BTRESULT SetOption(const long lId, const char *pszValue);

private:
	int   m_nEncodingType;  // [in] The encoding type of the image (Ascii or Binary/Raw).
	char *m_pszDescription; // [in] Text that will be saved in the file. Each line
                            // must start with a '#':
                            //
                            // BTCEncoderOptionsPXM Param;
                            // Param.SetOption( BTEO_PXM_DESCRIPTION, "# 1. Line\n# 2. Line\n# 3. Line");
};

class BTNEXGENIPL_CLASS_EXPORT BTCDecoderOptionsPXM : public BTCDecoderOptions
{
public:
	BTCDecoderOptionsPXM();
	virtual ~BTCDecoderOptionsPXM();

	enum { BTDO_PXM_ENCODINGTYPE = 0 };

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT SetOption(const long lId, const int nValue);

private:
	int m_nEncodingType; // [out] The encoding type of the image (Ascii or Binary/Raw).
};

/////////////////////////////////////////////////////////////////////////////
// TIFF options
/////////////////////////////////////////////////////////////////////////////

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptionsTIFF : public BTCEncoderOptions
{
public:
	BTCEncoderOptionsTIFF();
	virtual ~BTCEncoderOptionsTIFF();

	enum {	BTEO_TIFF_TITLE = 0,
			BTEO_TIFF_AUTHOR,
			BTEO_TIFF_DESCRIPTION,
			BTEO_TIFF_SOFTWARE,
			BTEO_TIFF_COPYRIGHT,
			BTEO_TIFF_CODEC };

	enum BTTIFFCodec {	BTTIFFCODEC_NONE          = 1,
						BTTIFFCODEC_CCITTRLE      = 2,
						BTTIFFCODEC_CCITTRLEW     = 32771,
						BTTIFFCODEC_CCITTFAX3     = 3,
						BTTIFFCODEC_CCITT_T4      = 3,
						BTTIFFCODEC_CCITTFAX4     = 4,
						BTTIFFCODEC_CCITT_T6      = 4,
						BTTIFFCODEC_JPEG          = 7,
						BTTIFFCODEC_PACKBITS      = 32773,
						BTTIFFCODEC_DEFLATE       = 32946,
						BTTIFFCODEC_ADOBE_DEFLATE = 8 };

	virtual BTRESULT GetOption(const long lId, long &lValue);
	virtual BTRESULT GetOption(const long lId, char *pszValue);
	
	virtual BTRESULT SetOption(const long lId, const long lValue);
	virtual BTRESULT SetOption(const long lId, const char *pszValue);

private:
	char* m_pszTitle;       // [in]
	char* m_pszAuthor;      // [in]
	char* m_pszDescription; // [in]
	char* m_pszSoftware;    // [in]
	char* m_pszCopyright;   // [in]
	long  m_lCodec;         // [in] Codec used to encode image.
};

class BTNEXGENIPL_CLASS_EXPORT BTCDecoderOptionsTIFF : public BTCDecoderOptions
{
public:
	BTCDecoderOptionsTIFF();
	virtual ~BTCDecoderOptionsTIFF();

	enum { BTDO_TIFF_USEEMBEDDEDPROFILE = 0};

	virtual BTRESULT GetOption(const long lId, bool &bValue);
	virtual BTRESULT SetOption(const long lId, const bool bValue);

private:
	bool m_bUseEmbeddedProfile; // [in] Use embedded color profile if available.
};

/////////////////////////////////////////////////////////////////////////////
// PNG options
/////////////////////////////////////////////////////////////////////////////

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptionsPNG : public BTCEncoderOptions
{
public:
	BTCEncoderOptionsPNG();
	virtual ~BTCEncoderOptionsPNG();

	enum {	BTEO_PNG_TITLE = 0,
			BTEO_PNG_AUTHOR,
			BTEO_PNG_DESCRIPTION,
			BTEO_PNG_SOFTWARE,
			BTEO_PNG_COPYRIGHT,
			BTEO_PNG_DISCLAIMER,
			BTEO_PNG_COMMENT,
			BTEO_PNG_TRANSPARENTCOLORINDEX,
			BTEO_PNG_INTERLACE };

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT GetOption(const long lId, char *pszValue);
	virtual BTRESULT GetOption(const long lId, bool &bValue);

	virtual BTRESULT SetOption(const long lId, const int nValue);
	virtual BTRESULT SetOption(const long lId, const char *pszValue);
	virtual BTRESULT SetOption(const long lId, const bool bValue);

private:
	char* m_pszTitle;               // [in]
	char* m_pszAuthor;              // [in]
	char* m_pszDescription;         // [in]
	char* m_pszSoftware;            // [in]
	char* m_pszCopyright;           // [in]
	char* m_pszDisclaimer;          // [in]
	char* m_pszComment;             // [in]
	int   m_nTransparentColorIndex; // [in] Index of transparent color (only for indexed images).
	bool  m_bInterlace;             // [in] Enable interlacing.
};

class BTNEXGENIPL_CLASS_EXPORT BTCDecoderOptionsPNG : public BTCDecoderOptions
{
public:
	BTCDecoderOptionsPNG();
	virtual ~BTCDecoderOptionsPNG();

	enum {	BTDO_PNG_TRANSPARENTCOLORAVAILABLE = 0,
			BTDO_PNG_TRANSPARENTCOLORINDEX,
			BTDO_PNG_TRANSPARENTCOLOR,
			BTDO_PNG_BACKGROUNDCOLOR };

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT GetOption(const long lId, bool &bValue);
	virtual BTRESULT GetOption(const long lId, BTColorRGB &rgbValue);

	virtual BTRESULT SetOption(const long lId, const int nValue);
	virtual BTRESULT SetOption(const long lId, const bool bValue);
	virtual BTRESULT SetOption(const long lId, const BTColorRGB rgbValue);

private:
	bool       m_bTransparentColor;      // [out] Is transparent color available ?
	int        m_nTransparentColorIndex; // [out] Index of transparent color (only for indexed images).
	BTColorRGB m_rgbTransparentColor;    // [out] Transparent color (only for RGB images).
	BTColorRGB m_rgbBackgroundColor;     // [in]  Color that should be used as background color (Only necessary for paletted images).
};

/////////////////////////////////////////////////////////////////////////////
// GIF options
/////////////////////////////////////////////////////////////////////////////

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptionsGIF : public BTCEncoderOptions
{
public:
	BTCEncoderOptionsGIF();
	virtual ~BTCEncoderOptionsGIF();

	enum {	BTEO_GIF_TRANSPARENTCOLOR = 0,
			BTEO_GIF_COMMENT };

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT GetOption(const long lId, char *pszValue);
	
	virtual BTRESULT SetOption(const long lId, const int nValue);
	virtual BTRESULT SetOption(const long lId, const char *pszValue);

private:
	int   m_nTransparentColor; // [in] Palette index of transparent color.
	char *m_pszComment;        // [in]
};

class BTNEXGENIPL_CLASS_EXPORT BTCDecoderOptionsGIF : public BTCDecoderOptions
{
public:
	BTCDecoderOptionsGIF();
	virtual ~BTCDecoderOptionsGIF();

	enum {	BTDO_GIF_IMAGEINDEX = 0,
			BTDO_GIF_IMAGECOUNT,
			BTDO_GIF_TRANSPARENTCOLORAVAILABLE,
			BTDO_GIF_TRANSPARENTCOLOR,
			BTDO_GIF_BACKGROUNDCOLOR,
			BTDO_GIF_DELAY,
			BTDO_GIF_COMMENT };

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT GetOption(const long lId, bool &bValue);
	virtual BTRESULT GetOption(const long lId, char *pszValue);
	virtual BTRESULT GetOption(const long lId, BTColorRGB &rgbValue);
	
	virtual BTRESULT SetOption(const long lId, const int nValue);
	virtual BTRESULT SetOption(const long lId, const bool bValue);
	virtual BTRESULT SetOption(const long lId, const char *pszValue);
	virtual BTRESULT SetOption(const long lId, const BTColorRGB rgbValue);

private:
	int        m_nImageIndex;         // [in] Index of requested image.
	int        m_nImageCount;         // [out] Number of images.
	bool       m_bTransparentColor;   // [out] Is transparent color available ?
	BTColorRGB m_rgbTransparentColor; // [out] Transparent color.
	BTColorRGB m_rgbBackgroundColor;  // [out]
	int        m_nDelay;              // [out]
	char       m_szComment[256];      // [out]
};

/////////////////////////////////////////////////////////////////////////////
// JP2 (JPEG 2000) options
/////////////////////////////////////////////////////////////////////////////

class BTNEXGENIPL_CLASS_EXPORT BTCEncoderOptionsJP2 : public BTCEncoderOptions
{
public:
	BTCEncoderOptionsJP2();
	virtual ~BTCEncoderOptionsJP2();

	enum {	BTEO_JP2_COMPRESSIONMODE = 0,
			BTEO_JP2_COMPRESSIONRATE,
			BTEO_JP2_CODEC };

	enum BTJP2CompressionMode {	BTJP2COMPRESSIONMODE_INT = 0,
								BTJP2COMPRESSIONMODE_REAL };

	enum BTJP2Codec {	BTJP2CODEC_JP2 = 0, // File Format Syntax
						BTJP2CODEC_JPC };   // Code Stream Syntax

	virtual BTRESULT GetOption(const long lId, int &nValue);
	virtual BTRESULT GetOption(const long lId, double &dValue);
	
	virtual BTRESULT SetOption(const long lId, const int nValue);
	virtual BTRESULT SetOption(const long lId, const double dValue);

private:
	int    m_nCodec;           // [in] Codec used to encode image.
	int    m_nCompressionMode; // [in] Compression mode (Real or Int).
	double m_dCompressionRate; // [in] e.g. 100:1 = 0.01, 200:1 = 0.005.
};

#endif // BTCOPTIONS_INCLUDED

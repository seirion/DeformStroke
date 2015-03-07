// BaboImage.cpp: implementation of the CBaboImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Deform.h"
#include "BaboImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaboImage::CBaboImage()
{
	m_ImgType = IMG_NONE;
	m_Buffer = NULL;
}

CBaboImage::~CBaboImage()
{
	ReleaseImage();
}

void CBaboImage::ReleaseImage()
{
	m_ImgType = IMG_NONE;
	if( m_Buffer ) delete [] m_Buffer;
	m_Buffer = NULL;
}

//
// Load Image with file path 
//		if loading is successful return TRUE, otherwise FALSE
BOOL CBaboImage::LoadImage(CString path)
{
	CFile file;
	CFileException e;

	// BMP file open
	if( !file.Open( path , CFile::modeRead, &e ) )
	{
		AfxMessageBox("Can't Open File");
		return FALSE;
	}

	file.Read(&m_bitMapFileHeader , sizeof(m_bitMapFileHeader) );
	file.Read(&m_bitMapInfoHeader , sizeof(m_bitMapInfoHeader) );
	
	// only 24bit depth is useful
	if( m_bitMapInfoHeader.biBitCount == 24 )
	{
		long size = GetBufferSize();
		m_Buffer = new BYTE[size];	

		file.Read( m_Buffer , size);
		file.Close();
		m_ImgType = IMG_RGB;
		return TRUE;
	}
	else 
	{
		AfxMessageBox("24 bit TRUE COLOR is only used");
		return FALSE;
	}
	m_ImgType = IMG_RGB;
	return TRUE;
}

//
// Save BMP file
// path is target file path
BOOL CBaboImage::SaveBMP(CString path)
{
	if( m_Buffer == NULL) return FALSE;
	
	CFile file ;
	CFileException fe;

	if (!file.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, &fe)) return FALSE;

	TRY
	{
		// write header on target bitmap file
		file.Write((LPSTR)&m_bitMapFileHeader, sizeof(BITMAPFILEHEADER) );
		file.Write((LPSTR)&m_bitMapInfoHeader , sizeof(BITMAPINFOHEADER) );
		// write image data on target bitmap file
		file.Write( (LPSTR)m_Buffer , GetBufferSize() ) ;
		file.Close();
	}
	CATCH( CFileException, e)
	{ 
		return FALSE;
	}
	END_CATCH

	return TRUE;
}

//
// return current BMP buffer size 
long CBaboImage::GetBufferSize()
{
	long width = m_bitMapInfoHeader.biWidth ;
	long height = m_bitMapInfoHeader.biHeight ;
	
	int widthDW = (int) ((width*24+31)/32)*4;
	
	if( m_bitMapInfoHeader.biBitCount == 24) return widthDW*height;
	else return m_bitMapInfoHeader.biSizeImage;
}

//
// get RGB value of specified position
COLORREF CBaboImage::GetAt(int x, int y)
{
	int numW = GetWidth()*GetChannel()+3;
	numW &= 0xfffc;

	int index = (m_bitMapInfoHeader.biHeight - y - 1)*numW + x*GetChannel();
	return RGB(m_Buffer[index+2],m_Buffer[index+1],m_Buffer[index]);
}

//
// color is 0x00bbggrr
void CBaboImage::SetAt(int x, int y, COLORREF color)
{
	int numW = GetWidth()*GetChannel()+3;
	numW &= 0xfffc;
	
	int index = (m_bitMapInfoHeader.biHeight - y - 1)*numW + x*GetChannel();

	m_Buffer[index+2] = (BYTE)(color & 0xff);				// r
	m_Buffer[index+1] = (BYTE)((color & 0xff00) >> 8);		// g
	m_Buffer[index] = (BYTE)((color & 0xff0000) >> 16);		// b
}

//////////////////////////////////////////////////////////////////////////
// Drawing functions

//
// x and y is destinate position
// pSrcDC is pointer to CDC
BOOL CBaboImage::DrawBitmap(int x, int y, CDC *pSrcDC, int xSrc, int ySrc, DWORD dwRop = SRCCOPY )
{
	BOOL bSuccess = FALSE;
	::SetStretchBltMode(pSrcDC->m_hDC , COLORONCOLOR);

	bSuccess = ::SetDIBitsToDevice
		(pSrcDC->m_hDC,						// hDC
		x,	 								// DestX
		y,		 							// DestY
		m_bitMapInfoHeader.biWidth,			// nDestWidth
		m_bitMapInfoHeader.biHeight,		// nDestHeight
		xSrc,	  							// SrcX
		ySrc,								// SrcY
		0,									// nStartScan
		m_bitMapInfoHeader.biHeight,		// nNumScans
		m_Buffer,							// lpBits
		(LPBITMAPINFO)&m_bitMapInfoHeader,	// lpBitsInfo
		DIB_RGB_COLORS);					// wUsage

	return bSuccess;
}

//
// x and y is destinate position
// pSrcDC is pointer to CDC
BOOL CBaboImage::DrawBitmap(int x, int y, int nWidth, int nHeight, CDC *pSrcDC, int xSrc, int ySrc, DWORD dwRop = SRCCOPY )
{
	BOOL bSuccess = FALSE;
	::SetStretchBltMode(pSrcDC->m_hDC , COLORONCOLOR);

	bSuccess = ::SetDIBitsToDevice
		(pSrcDC->m_hDC,						// hDC
		x,	 								// DestX
		y,		 							// DestY
		nWidth,								// nDestWidth
		nHeight,							// nDestHeight
		xSrc,	  							// SrcX
		ySrc,								// SrcY
		0,									// nStartScan
		m_bitMapInfoHeader.biHeight,		// nNumScans
		m_Buffer,							// lpBits
		(LPBITMAPINFO)&m_bitMapInfoHeader,	// lpBitsInfo
		DIB_RGB_COLORS);					// wUsage

	return bSuccess;
}

//
// x and y is destinate position
// pSrcDC is pointer to CDC
// nSrcWidth and nSrcHeight is image size
BOOL CBaboImage::DrawStretchBitmap(int x, int y, CDC *pSrcDC,int nSrcWidth, int nSrcHeight, DWORD dwRop = SRCCOPY)
{
	BOOL bSuccess = FALSE;
	::SetStretchBltMode(pSrcDC->m_hDC , COLORONCOLOR);

	bSuccess = ::StretchDIBits
				(pSrcDC->m_hDC, 					// hDC
				x,									// DestX
				y,									// DestY
				m_bitMapInfoHeader.biWidth,			// nDestWidth
				m_bitMapInfoHeader.biHeight,	  	// nDestHeight
				0,									// SrcX
				0,									// SrcY
				nSrcWidth ,							// wSrcWidth
				nSrcHeight ,						// wSrcHeight
				m_Buffer,							// lpBits
				(LPBITMAPINFO)&m_bitMapInfoHeader,	// lpBitsInfo
				DIB_RGB_COLORS,						// wUsage
				dwRop);								// dwROP

	return bSuccess;
		
}




void CBaboImage::SetImageType(int type) { m_ImgType = type; }
//////////////////////////////////////////////////////////////////////////
// functions - getting properties
int CBaboImage::GetChannel() { return m_bitMapInfoHeader.biBitCount/8; }
int CBaboImage::GetImageType() { return m_ImgType; }
int CBaboImage::GetWidth() { return m_bitMapInfoHeader.biWidth; }
int CBaboImage::GetHeight() { return m_bitMapInfoHeader.biHeight; }
BYTE *CBaboImage::GetBuffer() { return m_Buffer; }
//////////////////////////////////////////////////////////////////////////
// functions - conversion image types

//
// parameter : another image pointer
// return TRUE, image converting has been successful
// return FALSE, otherwise
BOOL CBaboImage::MakeYUVImage(CBaboImage *targetImage)
{
	// check whether image is valid
	if( m_ImgType != IMG_RGB ) return FALSE;
	if( GetWidth() <= 0 || GetHeight() <= 0 ) return FALSE;

	// image converting
	if( targetImage->m_ImgType == IMG_NONE )
		targetImage->ReleaseImage();
	else if( targetImage->GetWidth() != GetWidth() || targetImage->GetHeight() !=GetHeight() )
		targetImage->ReleaseImage();

	int size = GetBufferSize();
	BYTE *tBuffer = new BYTE[size];

	//Y = 0.3R + 0.59G + 0.11B 
	//U = (B-Y) x 0.493
	//V = (R-Y) x 0.877
	// ========================= //
	//R = Y + 0.956U + 0.621V
	//G = Y + 0.272U + 0.647V 
	//B = Y + 1.1061U + 1.703V
	for( int i = 0; i < size; i += 3)
	{
		//r = m_Buffer[i+2];
		//g = m_Buffer[i+1];
		//b = m_Buffer[i];
		tBuffer[i+2] = (BYTE)(0.3*m_Buffer[i+2] + 0.59*m_Buffer[i+1] + 0.11*m_Buffer[i] +.5); // Y
		tBuffer[i+1] = (BYTE)((m_Buffer[i]-tBuffer[i+2]) * 0.493 +.5); // U
		tBuffer[i] = (BYTE)((m_Buffer[i+2]-tBuffer[i+2]) * 0.877 +.5); // V
		tBuffer[i+2] = 0;
	}

	targetImage->SetImageType( IMG_YUV );
	targetImage->SetImage( m_bitMapFileHeader, m_bitMapInfoHeader, tBuffer );

	delete [] tBuffer;
	return TRUE;
}

void CBaboImage::SetImage(BITMAPFILEHEADER &fileHeader, BITMAPINFOHEADER &infoHeader, BYTE* byte )
{
	m_bitMapFileHeader = fileHeader;
	m_bitMapInfoHeader = infoHeader;

	
	int size = GetBufferSize();
	m_Buffer = new BYTE[size];

	for( int i = 0; i < size; i++)
		m_Buffer[i] = byte[i];
}

//
// remove Luminance in the image
// return TRUE, removing luminance has been successful
// return FALSE, otherwise
BOOL CBaboImage::RemoveLuminance()
{
	// check whether image is valid
	if( m_ImgType != IMG_RGB ) return FALSE;
	if( GetWidth() <= 0 || GetHeight() <= 0 ) return FALSE;

	int y,u,v;

	int size = GetBufferSize();

	//Y = 0.3R + 0.59G + 0.11B 
	//U = (B-Y) x 0.493
	//V = (R-Y) x 0.877
	// ========================= //
	//R = Y + 0.956U + 0.621V
	//G = Y + 0.272U + 0.647V 
	//B = Y + 1.1061U + 1.703V
	for( int i = 0; i < size; i += 3)
	{
		//r = m_Buffer[i+2];
		//g = m_Buffer[i+1];
		//b = m_Buffer[i];
		y = (BYTE)(0.3*m_Buffer[i+2] + 0.59*m_Buffer[i+1] + 0.11*m_Buffer[i] +.5); // Y
		u = (BYTE)((m_Buffer[i]-y) * 0.493 +.5); // U
		v = (BYTE)((m_Buffer[i+2]-y) * 0.877 +.5); // V
		y = 0;
		m_Buffer[i+2] = y + (BYTE)( 0.956*u + 0.621*v +.5); // r
		m_Buffer[i+1] = y + (BYTE)( 0.272*u + 0.647*v +.5); //g
		m_Buffer[i] = y + (BYTE)( 1.1061*u + 1.703*v +.5); // b
		
//		m_Buffer[i] = m_Buffer[i+1] = m_Buffer[i+2] = y;

	}
	return TRUE;
}

void CBaboImage::MakeBitmap( int width, int height )
{
	if( m_ImgType != IMG_NONE ) ReleaseImage();
	

	// BITMAPINFOHEADER
	m_bitMapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bitMapInfoHeader.biWidth = width; 
	m_bitMapInfoHeader.biHeight = height; 
	m_bitMapInfoHeader.biPlanes = 1; 
	m_bitMapInfoHeader.biBitCount = 24; 
	m_bitMapInfoHeader.biCompression = BI_RGB; 
	m_bitMapInfoHeader.biSizeImage = 0; 
	m_bitMapInfoHeader.biXPelsPerMeter = 0; 
	m_bitMapInfoHeader.biYPelsPerMeter = 0; 
	m_bitMapInfoHeader.biClrUsed = 0; 
	m_bitMapInfoHeader.biClrImportant = 0; 

	// BITMAPFILEHEADER
	m_bitMapFileHeader.bfType = 19778;
	m_bitMapFileHeader.bfSize = GetBufferSize() + sizeof(BITMAPFILEHEADER);
	m_bitMapFileHeader.bfReserved1 = 0;
	m_bitMapFileHeader.bfReserved2 = 0;
	m_bitMapFileHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);


	// image buffer
	int size = GetBufferSize();
	m_Buffer = new BYTE [size];
	for( int i = 0; i < size; i++) m_Buffer[i] = 0;

	m_ImgType = IMG_RGB;
 
}

//////////////////////////////////////////////////////////////////////////
// operator overlaoding
//////////////////////////////////////////////////////////////////////////

//
// operator overlaoding = (assign operator)
// parameter is original image
BOOL CBaboImage::operator=(CBaboImage* originImage)
{
	if( m_Buffer ) delete [] m_Buffer;

	int i;
	long bufferSize = originImage->GetBufferSize();
	BYTE *orgBuffer = originImage->GetBuffer();
	m_bitMapFileHeader = originImage->GetBitmapFileHeader();
	m_bitMapInfoHeader = originImage->GetBitmapInfoHeader();
	m_ImgType = originImage->m_ImgType;
		
	m_Buffer = new BYTE [bufferSize];
	for( i = 0; i < bufferSize; i++) m_Buffer[i] = orgBuffer[i];

	return TRUE;
}
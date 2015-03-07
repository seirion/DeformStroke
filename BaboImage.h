 // BaboImage.h: interface for the CBaboImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYBITMAP_H__FE0401BA_AF77_4A72_93EA_8E71C0EFF1C5__INCLUDED_)
#define AFX_MYBITMAP_H__FE0401BA_AF77_4A72_93EA_8E71C0EFF1C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
// Image Type
#define IMG_NONE	0x00
#define IMG_RGB		0x13
#define IMG_YUV		0x23
#define IMG_YCbCr	0x33
#define IMG_GRAY	0x01


class CBaboImage  
{
public:
	BOOL operator=(CBaboImage*);
	void MakeBitmap(int width, int height);
	BOOL RemoveLuminance();
	BOOL MakeYUVImage(CBaboImage* targetImage);

	BOOL LoadImage(CString path);
	BOOL SaveBMP(CString path);
	BOOL DrawStretchBitmap(int x, int y, CDC *pSrcDC,int nSrcWidth, int nSrcHeight, DWORD dwRop);
	BOOL DrawBitmap( int x, int y,CDC* pSrcDC, int xSrc, int ySrc, DWORD dwRop );
	BOOL DrawBitmap(int x, int y, int nWidth, int nHeight, CDC *pSrcDC, int xSrc, int ySrc, DWORD dwRop );

	void SetImage(BITMAPFILEHEADER &fileHeader, BITMAPINFOHEADER &infoHeader, BYTE* byte);
	void SetImageType(int type);
	void SetAt(int x, int y, COLORREF color);
	COLORREF GetAt(int x, int y);
	long GetBufferSize();
	int GetChannel();
	int GetImageType();
	int GetWidth();
	int GetHeight();
	BYTE *GetBuffer();
	BITMAPFILEHEADER &GetBitmapFileHeader() { return m_bitMapFileHeader; }
	BITMAPINFOHEADER &GetBitmapInfoHeader() { return m_bitMapInfoHeader; }
	CBaboImage();
	virtual ~CBaboImage();
	void ReleaseImage();
	

private:
	
	int m_ImgType;
	BYTE* m_Buffer;

	BITMAPFILEHEADER m_bitMapFileHeader;
	BITMAPINFOHEADER m_bitMapInfoHeader;
};

#endif // !defined(AFX_MYBITMAP_H__FE0401BA_AF77_4A72_93EA_8E71C0EFF1C5__INCLUDED_)

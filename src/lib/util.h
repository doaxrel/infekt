/**
 * Copyright (C) 2010 cxxjoe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 **/

#ifndef _UTIL_H
#define _UTIL_H


class CUtil
{
public:
	static std::string FromWideStr(const std::wstring& a_wideStr, unsigned int a_targetCodePage);
	static std::wstring ToWideStr(const std::string& a_str, unsigned int a_originCodePage);
	static bool OneCharWideToUtf8(wchar_t a_char, char* a_buf);

#ifdef _WIN32_UI
	static int AddPngToImageList(HIMAGELIST a_imgList,
		HINSTANCE a_instance, int a_resourceId, int a_width, int a_height);
#endif
};


template <typename T> class TwoDimVector
{
public:
	TwoDimVector(size_t a_rows, size_t a_cols, T a_initial) :
	  m_data(a_rows, std::vector<T>(a_cols))
	{
		m_rows = a_rows;
		m_cols = a_cols;

		for(size_t r = 0; r < m_rows; r++)
		{
			for(size_t c = 0; c < m_cols; c++)
			{
				(*this)[r][c] = a_initial;
			}
		}
	}

	std::vector<T> & operator[](size_t i) 
	{ 
		return m_data[i];
	}

	const std::vector<T> & operator[](size_t i) const
	{ 
		return m_data[i];
	}

	const size_t GetRows() const { return m_rows; }
	const size_t GetCols() const { return m_cols; }
private:
	std::vector<std::vector<T> > m_data;
	size_t m_rows, m_cols;

	TwoDimVector() {}
};


/* gutf8.c exports */
extern "C"
{
	int g_utf8_validate(const char *str, size_t max_len, const char **end);
	char *g_utf8_find_next_char(const char *p, const char *end = NULL);
	size_t g_utf8_strlen(const char *p, long max_bytes);
}


/* useful macros */
#ifdef HAVE_BOOST
#define FORMAT(FORMAT_FORMAT, FORMAT_DATA) boost::str(boost::wformat(FORMAT_FORMAT) % FORMAT_DATA)
#endif


/* Win32++ helpers */
#ifdef _WIN32_UI

extern HMODULE g_hUxThemeLib;

class CNonThemedTab : public CTab
{
protected:
	virtual inline LRESULT WndProcDefault(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(uMsg != WM_PAINT && uMsg != WM_ERASEBKGND)
		{
			return CTab::WndProcDefault(uMsg, wParam, lParam);
		}
		else
		{
			return CWnd::WndProcDefault(uMsg, wParam, lParam);
		}
	}

public:
	virtual inline int AddTabPage(CWnd* pWnd, LPCTSTR szTitle, HICON hIcon = 0)
	{
		int l_newIdx = CTab::AddTabPage(pWnd, szTitle, hIcon);

		LONG_PTR l_style = GetWindowLongPtr(GWL_STYLE);
		if((l_style & TCS_OWNERDRAWFIXED) != 0)
		{
			SetWindowLongPtr(GWL_STYLE, l_style & ~TCS_OWNERDRAWFIXED);
		}

		if(SendMessage(CCM_GETVERSION, 0, 0) >= 6)
		{
			// adjust XP style background...
			typedef HRESULT (WINAPI *fEnThDiTe)(HWND hwnd, DWORD dwFlags);

			if(!g_hUxThemeLib)
			{
				g_hUxThemeLib = LoadLibrary(_T("uxtheme.dll"));
			}

			if(g_hUxThemeLib)
			{
				if(fEnThDiTe etdt = (fEnThDiTe)GetProcAddress(g_hUxThemeLib, "EnableThemeDialogTexture"))
				{
					etdt(GetTabPageInfo(l_newIdx).pWnd->GetHwnd(), ETDT_ENABLETAB);
				}
			}
		}

		return l_newIdx;
	}
};

#endif


#endif /* !_UTIL_H */

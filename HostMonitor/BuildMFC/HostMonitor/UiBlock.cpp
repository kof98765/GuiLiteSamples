#include "stdafx.h"
#include "UiBlock.h"

CUiBlock::CUiBlock(int index, int color_bytes)
{
	m_index = index;
	m_color_bytes = color_bytes;
	m_is_mouse_down = false;
	m_ui_width = m_ui_height = 0;
}

void CUiBlock::renderUI(CRect& rect, CDC* pDC)
{
	void* data = 0;
	if (rect.EqualRect(m_block_rect) && m_ui_width != 0)
	{
		data = getUiOfHostMonitor(m_index, 0, 0);
		goto RENDER;
	}

	m_block_rect = rect;
	data = getUiOfHostMonitor(m_index, &m_ui_width, &m_ui_height, true);

	memset(&m_ui_bm_info, 0, sizeof(m_ui_bm_info));
	m_ui_bm_info.bmiHeader.biSize = sizeof(MYBITMAPINFO);
	m_ui_bm_info.bmiHeader.biWidth = m_ui_width;
	m_ui_bm_info.bmiHeader.biHeight = (0 - m_ui_height);//fix upside down. 
	m_ui_bm_info.bmiHeader.biBitCount = m_color_bytes * 8;
	m_ui_bm_info.bmiHeader.biPlanes = 1;
	m_ui_bm_info.bmiHeader.biSizeImage = m_ui_width * m_ui_height * m_color_bytes;

	switch (m_color_bytes)
	{
	case 2:
		m_ui_bm_info.bmiHeader.biCompression = BI_BITFIELDS;
		m_ui_bm_info.biRedMask = 0xF800;
		m_ui_bm_info.biGreenMask = 0x07E0;
		m_ui_bm_info.biBlueMask = 0x001F;
		break;
	case 4:
		m_ui_bm_info.bmiHeader.biCompression = BI_RGB;
		break;
	default:
		ASSERT(false);
		break;
	}

	m_memDC.DeleteDC();
	m_blockBmp.DeleteObject();

	m_memDC.CreateCompatibleDC(pDC);
	m_blockBmp.CreateCompatibleBitmap(pDC, m_block_rect.Width(), m_block_rect.Height());
	m_memDC.SelectObject(&m_blockBmp);
	::SetStretchBltMode(m_memDC, STRETCH_HALFTONE);

RENDER:
	if (!data)
	{
		return;
	}
	register int block_width = m_block_rect.Width();
	register int block_height = m_block_rect.Height();
	::StretchDIBits(m_memDC.m_hDC, 0, 0, block_width, block_height,
		0, 0, m_ui_width, m_ui_height, data, (BITMAPINFO*)&m_ui_bm_info, DIB_RGB_COLORS, SRCCOPY);
	pDC->BitBlt(m_block_rect.left, m_block_rect.top, block_width, block_height, &m_memDC, 0, 0, SRCCOPY);
}

void CUiBlock::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_is_mouse_down = true;
	CPoint guilitePos = pointMFC2GuiLite(point);
	if (guilitePos.x < 0 || guilitePos.y < 0)
	{
		return;
	}

	OUTMSGINFO msg;
	msg.dwMsgId = 0x4700;
	msg.dwParam1 = guilitePos.x;
	msg.dwParam2 = guilitePos.y;
	sendTouch2HostMonitor(&msg, sizeof(msg), m_index);
}

void CUiBlock::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_is_mouse_down = false;
	CPoint guilitePos = pointMFC2GuiLite(point);
	if (guilitePos.x < 0 || guilitePos.y < 0)
	{
		return;
	}

	OUTMSGINFO msg;
	msg.dwMsgId = 0x4600;
	msg.dwParam1 = guilitePos.x;
	msg.dwParam2 = guilitePos.y;
	sendTouch2HostMonitor(&msg, sizeof(msg), m_index);
}

void CUiBlock::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_is_mouse_down == false)
	{
		return;
	}
	OnLButtonDown(nFlags, point);
}

CPoint CUiBlock::pointMFC2GuiLite(CPoint point)
{
	if (!m_block_rect.PtInRect(point))
	{
		return CPoint(-1, -1);
	}
	return CPoint(m_ui_width * (point.x - m_block_rect.left) / m_block_rect.Width(), m_ui_height * (point.y - m_block_rect.top) / m_block_rect.Height());
}

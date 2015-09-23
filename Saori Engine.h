#pragma once

#define Color(r, g, b, a) r / 255, g / 255, b / 255, a / 255
#define optFunction(type) inline type
#define Font IDWriteTextFormat
#undef CreateFont

class Draw
{
private:
	ID2D1SolidColorBrush* m_pColor = nullptr;
	ID2D1HwndRenderTarget* m_pRenderTarget = nullptr;
	IDWriteFactory* m_pDWriteFactory = nullptr;
	Font* m_pTextFormat = nullptr;

public:
	Draw(ID2D1HwndRenderTarget* pRenderTarget)
	{
		if (pRenderTarget == nullptr)
			return;

		m_pRenderTarget = pRenderTarget;
		SetDrawColor(255, 255, 255, 255);
	}

	~Draw()
	{
		if (m_pColor != nullptr)
			m_pColor->Release();

		if (m_pRenderTarget != nullptr)
			m_pRenderTarget->Release();

		if (m_pTextFormat != nullptr)
			m_pTextFormat->Release();
	}

	optFunction(void) SetDrawColor(float r, float g, float b, float a)
	{
		if (m_pColor != nullptr)
			m_pColor->Release();

		m_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(Color(r, g, b, a)),
			&m_pColor
			);
	}

	optFunction(void) FilledRectangle(float x, float y, float w, float h)
	{
		m_pRenderTarget->FillRectangle(&D2D1::RectF(x, y, x + w, y + h), m_pColor);
	}

	optFunction(void) Rectangle(float x, float y, float w, float h)
	{
		m_pRenderTarget->DrawRectangle(&D2D1::RectF(x, y, x + w, y + h), m_pColor);
	}

	optFunction(void) FilledRectangleRelative(float x, float y, float w, float h)
	{
		m_pRenderTarget->FillRectangle(&D2D1::RectF(x, y, w, h), m_pColor);
	}

	optFunction(void) RectangleRelative(float x, float y, float w, float h)
	{
		m_pRenderTarget->DrawRectangle(&D2D1::RectF(x, y, w, h), m_pColor);
	}

	optFunction(void) String(WCHAR* szString, float x, float y, float w, float h)
	{
		if (m_pTextFormat == nullptr)
			return;

		m_pRenderTarget->DrawText(
			szString,
			wcslen(szString),
			m_pTextFormat,
			D2D1::RectF(x, y, w, h),
			m_pColor
			);
	}

	optFunction(void) Line(float x, float y, float w, float h, float strokeWidth = 1)
	{
		m_pRenderTarget->DrawLine(
			D2D1::Point2F(x, y),
			D2D1::Point2F(w, h),
			m_pColor,
			strokeWidth
			);
	}

	optFunction(void) SetFont(Font* pTextFormat)
	{
		if (pTextFormat == nullptr || pTextFormat == m_pTextFormat)
			return;

		m_pTextFormat = pTextFormat;
	}

	Font* CreateFont(WCHAR* szFontName, float flFontSize = 20)
	{
		if (m_pDWriteFactory == nullptr)
		{
			// Create a DirectWrite factory.
			DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(m_pDWriteFactory),
				reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
				);
		}

		if (m_pDWriteFactory == nullptr)
			return nullptr;

		Font* pTextFormat;
		// Create a DirectWrite text format object.
		m_pDWriteFactory->CreateTextFormat(
			szFontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			flFontSize,
			L"", // Locale
			&pTextFormat
			);

		return pTextFormat;
	}
};


namespace Saori
{
	Draw* pDraw = nullptr;

	void Render(ID2D1HwndRenderTarget* pRenderTarget)
	{
		pDraw = new Draw(pRenderTarget);
		if (pDraw == nullptr)
			return;

		static Font* pSegoeUI = pDraw->CreateFont(L"Segoe UI", 20);
		pDraw->SetFont(pSegoeUI);
		D2D1_SIZE_F rtSize = pRenderTarget->GetSize();

		pDraw->SetDrawColor(119, 136, 153, 255);
		for (unsigned int x = 0; x < rtSize.width; x += 10)
			pDraw->Line(x, 0, x, rtSize.height);
		for (unsigned int y = 0; y < rtSize.height; y += 10)
			pDraw->Line(0, y, rtSize.width, y);

		pDraw->SetDrawColor(50, 150, 255, 255);
		pDraw->FilledRectangleRelative(30, 20, rtSize.width - 30, rtSize.height - 20);

		pDraw->SetDrawColor(255, 0, 0, 255);
		pDraw->String(L"Test", 30, 20, rtSize.width - 30, 20 + 20);
	}
}
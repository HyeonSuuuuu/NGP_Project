#ifndef COLOR_KEY_CODE
#define COLOR_KEY_CODE

struct COLOR {
	BYTE r, g, b;
	COLORREF getRGB()
	{
		return ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)));
	}
};

static COLOR blackColor = COLOR{0, 0, 0};
static COLOR WhiteColor = COLOR{255, 255, 255};

#endif // COLOR_KEY_CODE
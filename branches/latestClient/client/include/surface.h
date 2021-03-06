
#if !defined(__SURFACE_H__)
#define __SURFACE_H__

#include <qglobal.h>

/*
	This class represents an image in memory
*/
class cSurface {
protected:
	int realWidth_, realHeight_; // Original width and height
	int width_, height_; // Real texture width and height
	unsigned char *data; // RGBA pixel data
	unsigned int pitch; // Length of one scanline in byte

public:
	/*
		Create a new memory surface. If texture is true,
		the system assures, that the surface size will be
		OpenGL compatible.
	*/
	cSurface(int width, int height, bool texture = true);
	~cSurface();

	// Clear the entire surface and set every pixel to 0
	void clear();
	
	// Fill with the given color
	void fill(unsigned int color);

	// Create a color value
	static unsigned int color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	static unsigned int color(unsigned short pixel, unsigned char a);
	static unsigned int color(unsigned short pixel);

	// Set a single pixel. Must be locked first.
	void setPixel(unsigned int x, unsigned int y, unsigned int color);

	// Get a single pixel value. Must be locked first.
	unsigned int getPixel(unsigned int x, unsigned int y) const;
	static unsigned char red(unsigned int color);
	static unsigned char blue(unsigned int color);
	static unsigned char alpha(unsigned int color);
	static unsigned char green(unsigned int color);

	// Getters
	int width() const;
	int height() const;
	int realHeight() const;
	int realWidth() const;
	unsigned char *bits() const;
	unsigned char *scanline(int y) const;
};

inline unsigned int cSurface::color(unsigned short pixel, unsigned char a) {
	// Color format is ABGR
	return ((uint)(pixel & 0x7C00) >> 7) | ((uint)(pixel & 0x3E0) << 6) | ((uint)(pixel & 0x1F) << 19) | ((uint)a << 24);
	
	// This is for RGBA
	// return ((uint)(pixel & 0x7C00) << 17) | ((uint)(pixel & 0x3E0) << 14) | ((uint)(pixel & 0x1F) << 11) | (uint)a;
}

inline unsigned int cSurface::color(unsigned short pixel) {
	// This is for RGBA
	// return ((uint)(pixel & 0x7C00) << 17) | ((uint)(pixel & 0x3E0) << 14) | ((uint)(pixel & 0x1F) << 11) | (uint)255;

	// Color format is ABGR
	return ((uint)(pixel & 0x7C00) >> 7) | ((uint)(pixel & 0x3E0) << 6) | ((uint)(pixel & 0x1F) << 19) | 0xFF000000;
}

inline unsigned int cSurface::color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	return ((a << 24) | (b << 16) | (g << 8) | r);
}

inline unsigned char cSurface::red(unsigned int color) {
	return color & 0xFF;
}

inline unsigned char cSurface::blue(unsigned int color) {
	return (color >> 16) & 0xFF;
}

inline unsigned char cSurface::alpha(unsigned int color) {
	return (color >> 24) & 0xFF;
}

inline unsigned char cSurface::green(unsigned int color) {
	return (color >> 8) & 0xFF;
}

inline int cSurface::width() const {
	return width_;
}

inline int cSurface::height() const {
	return height_;
}

inline int cSurface::realHeight() const {
	return realHeight_;
}

inline int cSurface::realWidth() const {
	return realWidth_;
}

inline unsigned char *cSurface::bits() const {
	return data;
}

inline unsigned char *cSurface::scanline(int y) const {
	return data + (y * pitch);
}

inline void cSurface::setPixel(unsigned int x, unsigned int y, unsigned int color) {
	// x << 2 == x * 4
	*(unsigned int*)(data + y * pitch + (x << 2)) = color;
}

inline unsigned int cSurface::getPixel(unsigned int x, unsigned int y) const {
	// x << 2 == x * 4
	return *(unsigned int*)(data + y * pitch + (x << 2));
}

#endif

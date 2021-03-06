
#if !defined(__TEXTFIELD_H__)
#define __TEXTFIELD_H__

//Added by qt3to4:
#include <QMouseEvent>
#include <QKeyEvent>

#include "control.h"
#include "bordergump.h"
#include "texture.h"

/*
	A textfield is just another container for now
*/
class cTextField : public cControl {
Q_OBJECT
Q_PROPERTY(QString text READ text WRITE setText)
Q_PROPERTY(ushort font READ font WRITE setFont)
Q_PROPERTY(bool password READ password WRITE setPassword)

protected:
	cBorderGump *background_;
	unsigned short backgroundId_;

	bool dirty;
	bool hueAll_;
	unsigned char font_;
	unsigned short hue_;
	QString text_;
	short mouseOverHue_;
	short focusHue_;
	bool password_;
	bool unicodeMode_;
	bool textBorder_;

	cTexture *surfaces[3]; // Three States

	bool mouseOver_; // False by default (is the mouse over this control)

	void update();
	void draw(int xoffset, int yoffset);
	unsigned int caret_; // The offset in characters from the left to the text input positioner.
	unsigned int leftOffset_; // Offset in characters from the left border of the text field to the beginning of the text.
	unsigned int maxLength_; // Maximum length in characters of the text fields contents.
	unsigned int caretXOffset_; // The offset to the left border of the text field in pixels.
	int selection_; // The number of characters that are selected in relation to the current caret.

	void drawSelection(cSurface *surface);
	void replaceSelection(const QString &replacement);
	QString getSelection();
	inline QChar translateChar(QChar c) {
		if (password_) {
			return QChar('*');
		} else {
			return c;
		}
	}
	bool getCharacterWidth(uint i, uchar &charWidth);

public:
	cTextField(int x, int y, int width, int height, unsigned char font, unsigned short hue = 0, unsigned short background = 0xbb8, bool hueAll = false, bool unicodeMode = false);
	cTextField();
	virtual ~cTextField();

	void processDefinitionAttribute(QString name, QString value);
	void processDefinitionElement(QDomElement element);

public slots:

	// Getters
	inline bool hueAll() const { return hueAll_; }
	inline unsigned char font() const { return font_; }
	inline unsigned short hue() const { return hue_; }
	inline const QString &text() const { return text_; }
	inline unsigned short background() const { return backgroundId_; }
	inline unsigned int leftOffset() const { return leftOffset_; }
	inline bool password() const { return password_; }
	bool textBorder() const;
	void setTextBorder(bool border);
	bool unicodeMode() const;
	void setUnicodeMode(bool data);

	inline void setPassword(bool data) {
		password_ = data;
		invalidateText();
	}

	unsigned int getOffset(int x); // Get the character index for the given x coordinate

	// Invalidate the Text
	inline void invalidateText() {
		for (int i = 0; i < 3; ++i) {
			if (surfaces[i]) {
				surfaces[i]->decref();
				surfaces[i] = 0;
			}
		}
		dirty = true;
	}

	// Setters
	inline void setHueAll(bool data) {
		if (hueAll_ != data) {
			hueAll_ = data;
			invalidateText();
		}
	}

	inline void setFont(unsigned char data) {
		if (font_ != data) {
			font_ = data;
			invalidateText();
		}
	}

	inline void setHue(unsigned short data) {
		if (hue_ != data) {
			hue_ = data;
			invalidateText();
		}
	}

	inline void setMouseOverHue(short data) {
		if (mouseOverHue_ != data) {
			mouseOverHue_ = data;
			invalidateText();
		}
	}

	inline void setFocusHue(short data) {
		if (focusHue_ != data) {
			focusHue_ = data;
			invalidateText();
		}
	}

	inline void setText(const QString &data) {
		if (text_ != data) {
			text_ = data;
			selection_ = 0;
			caret_ = 0;
			caretXOffset_ = 0;
			setCaret(text_.length());
			invalidateText();
		}
	}

	void setBackground(unsigned short data);
	void setCaret(unsigned int pos);

	inline void setMaxLength(unsigned int data) {
		if (maxLength_ != data) {
			maxLength_ = data;

			if (text_.length() > (int)maxLength_) {
				int toremove = text_.length() - maxLength_;
				text_.remove(text_.length() - toremove, toremove);
				invalidateText();
			}
		}
	}

	void onChangeBounds(int oldx, int oldy, int oldwidth, int oldheight);
	void onMouseLeave(); // Toggle the mouseOver state
	void onMouseEnter(); // Toggle the mouseOver state
	void onMouseDown(QMouseEvent *e);
	void onMouseUp(QMouseEvent *e);
	void onKeyDown(QKeyEvent *e);
	void onKeyUp(QKeyEvent *e);

	// This is called if enter is pressed
	void onEnter();

signals:
	void enterPressed(cTextField *textfield);
};

inline bool cTextField::unicodeMode() const {
	return unicodeMode_;
}

inline void cTextField::setUnicodeMode(bool data) {
	unicodeMode_ = data;
	invalidateText();
}

inline bool cTextField::textBorder() const {
	return textBorder_;
}

inline void cTextField::setTextBorder(bool border) {
	textBorder_ = border;
	invalidateText();
}

#endif

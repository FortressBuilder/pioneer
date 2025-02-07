// Copyright © 2008-2016 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef UI_LABEL_H
#define UI_LABEL_H

#include "Widget.h"
#include "SmartPtr.h"
#include "text/TextureFont.h"
#include "graphics/VertexBuffer.h"

// single line of text

namespace UI {

class Label: public Widget {
public:
	virtual Point PreferredSize();
	virtual void Layout();
	virtual void Draw();

	Label *SetText(const std::string &text);
	const std::string &GetText() const { return m_text; }

	Label *SetColor(const Color &c) { m_color = c; return this; }

protected:
	friend class Context;
	Label(Context *context, const std::string &text);

private:
	void BindText(PropertyMap &p, const std::string &k);

	bool m_bNeedsUpdating;
	bool m_bPrevDisabled;
	float m_prevOpacity;
	std::string m_text;
	Color m_color;
	Point m_preferredSize;
	RefCountedPtr<Text::TextureFont> m_font;
	RefCountedPtr<Graphics::VertexBuffer> m_vbuffer;
};

}

#endif

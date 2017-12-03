
void fontDrawText(Spritefont* font, wplRenderGroup* group, f32 x, f32 y, string txt, isize len, f32 scale, u32 color)
{
	if(len == -1) len = strlen(txt);
	Vec2 initialPos = v2(x, y);
	Vec2 pos = v2(0, 0);
	for(isize i = 0; i < len; ++i) {
		char c = txt[i];
		if(c == '\n') {
			pos.x = 0;
			pos.y += font->glyphs[1].h + 2;
		}

		if(c < 32 || c > 128) continue;
		Rect2i glyph = font->glyphs[c-32];
		Vec2 fpos = v2Add(pos, initialPos);

		wplGroupAdd(group, Anchor_TopLeft | Sprite_NoAA,
				fpos.x, fpos.y, glyph.w * scale, glyph.h * scale, 
				glyph.x, glyph.y, glyph.w, glyph.h);
		pos.x += glyph.w * scale;
	}
}

f32 fontDrawTextWrapped(Spritefont* font, wplRenderGroup* group, f32 x, f32 y, string txt, isize len, f32 scale, u32 color, f32 width)
{
	if(len == -1) len = strlen(txt);
	Vec2 initialPos = v2(x, y);
	Vec2 pos = v2(0, 0);
	for(isize i = 0; i < len; ++i) {
		char c = txt[i];
		if(c == '\n') {
			pos.x = 0;
			pos.y += font->glyphs[1].h + 2;
		}

		if((c == ' ' || c == '-') && pos.x > width) {
			pos.x = 0;
			pos.y += font->glyphs[1].h + 2;
		}

		if(c < 32 || c > 128) continue;
		Rect2i glyph = font->glyphs[c-32];
		Vec2 fpos = v2Add(pos, initialPos);

		wplGroupAdd(group, Anchor_TopLeft | Sprite_NoAA,
				fpos.x, fpos.y, glyph.w * scale, glyph.h * scale, 
				glyph.x, glyph.y, glyph.w, glyph.h);
		pos.x += glyph.w * scale;
	}
	pos.y += font->glyphs[1].h;
	return pos.y;
}

wplRenderGroup* textGroup;

isize sizeText(string s)
{
	return strlen(s) * gameData.font.glyphs[1].w / 2;
}

void drawText(f32 x, f32 y, string s)
{
	fontDrawText(&gameData.font, textGroup, x, y, s, -1, 0.5, 0xFFFFFFFF);
}

f32 drawTextSW(f32 x, f32 y, string s, f32 scale, f32 w)
{
	return fontDrawTextWrapped(&gameData.font, textGroup, x, y, s, -1, scale, 0xFFFFFFFF, w);
}

void drawTextS(f32 x, f32 y, string s, f32 scale)
{
	fontDrawText(&gameData.font, textGroup, x, y, s, -1, scale, 0xFFFFFFFF);
}

void drawTextR(f32 x, f32 y, string s)
{
	drawText(x - sizeText(s), y, s);
}

void getMouse(wplRenderGroup* group, f32* mx, f32* my)
{
	*mx = mouseX / group->scale;
	*my = mouseY / group->scale;
}

int uiButton(f32 x, f32 y, string msg)
{
	f32 c = strlen(msg) * gameData.font.glyphs[1].w;
	c *= 0.5;
	c += 8;
	f32 width = c < 48 ? 48 : c;
	f32 height = 10;

	int mouseIn = 0, mouseState = 0;
	f32 mx, my;
	getMouse(textGroup, &mx, &my);
	if(mx > x && my > y && mx < (x + width) && my < (y + height)) {
		mouseIn = 1;
		mouseState = wplMouseIsDown(1);
		if(!mouseState) {
			mouseState = wplMouseIsJustUp(1) * 2;
		}
	}

	wplSprite* s = wplGetSprite(textGroup);
	s->x = x;
	s->y = y;
	s->w = width;
	s->h = height;
	s->flags = Sprite_NoTexture | Anchor_TopLeft;

	if(!mouseState && !mouseIn) {
		s->color = 0x00000066;
	} else if(mouseIn) {
		s->color = 0x00000099;
		if(mouseState == 1) {
			s->color = 0x000000BB;
		} 
	}

	drawText(x + 4, y + 2, msg);
	
	return mouseIn && mouseState == 2;
}


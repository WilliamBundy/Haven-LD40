static
void initDefaultShader(wplWindow* window, wplShader* shader)
{
	u32 vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &window->vertShader, NULL);
	glCompileShader(vert);

	{
		i32 success = 1;
		glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
		if(!success) {
			char log[4096];
			i32 logSize = 0;
			glGetShaderInfoLog(vert, 4096, &logSize, log);
			fprintf(stderr, "\nVertex Shader Compile Log\n%s\n\n", log);
		}
	}
	
	u32 frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &window->fragShader, NULL);
	glCompileShader(frag);

	{
		i32 success = 1;
		glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
		if(!success) {
			char log[4096];
			i32 logSize = 0;
			glGetShaderInfoLog(frag, 4096, &logSize, log);
			fprintf(stderr, "\nFragment Shader Compile Log\n%s\n\n", log);
		}
	}

	shader->vert = vert;
	shader->frag = frag;

	shader->program = glCreateProgram();
	
	if(window->glVersion < 33) {
		glAttachShader(shader->program, vert);
		glBindAttribLocation(shader->program, 0, "xy");
		glBindAttribLocation(shader->program, 1, "uv");
		glBindAttribLocation(shader->program, 2, "color");
		glAttachShader(shader->program, frag);
		glLinkProgram(shader->program);

		{
			i32 success = 1;
			glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
			if(!success) {
				char log[4096];
				i32 logSize = 0;
				glGetProgramInfoLog(shader->program, 4096, &logSize, log);
				fprintf(stderr, "\nShader Program Link Log\n%s\n\n", log);

			}
		}

		glUseProgram(shader->program);
		shader->uInvTextureSize = glGetUniformLocation(
				shader->program, "invTextureSize");
		return;
	}

	glAttachShader(shader->program, vert);
	glAttachShader(shader->program, frag);
	glLinkProgram(shader->program);

	{
		i32 success = 1;
		glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
		if(!success) {
			char log[4096];
			i32 logSize = 0;
			glGetProgramInfoLog(shader->program, 4096, &logSize, log);
			fprintf(stderr, "\nShader Program Link Log\n%s\n\n", log);

		}
	}

	//TODO(will) Do shader program error checking
	glUseProgram(shader->program);
	shader->uOrtho = glGetUniformLocation(shader->program, "uOrtho");
	shader->uTint = glGetUniformLocation(shader->program, "uTint");
	shader->uShadow = glGetUniformLocation(shader->program, "uShadow");
	shader->uInvTextureSize = glGetUniformLocation(shader->program, "uInvTextureSize");
	shader->uScale = glGetUniformLocation(shader->program, "uScale");
	shader->uOffset = glGetUniformLocation(shader->program, "uOffset");
	shader->uViewport = glGetUniformLocation(shader->program, "uViewport");
}

void wplGroupAdd(
		wplRenderGroup* group,
		i32 flags,
		f32 x, f32 y,
		f32 w, f32 h,
		i16 tx, i16 ty, i16 tw, i16 th)
{
	wplSprite s;
	s.flags = flags;
	s.color = 0xFFFFFFFF;
	s.x = x;
	s.y = y;
	s.w = w;
	s.h = h;
	s.cx = 0;
	s.cy = 0;
	s.tx = tx;
	s.ty = ty;
	s.tw = tw;
	s.th = th;
	s.angle = 0;
	group->sprites[group->count++] = s;
}

wplSprite* wplGetSprite(wplRenderGroup* group)
{
	return group->sprites + group->count++;
}

void wplGroupInit(
		wplWindow* window,
		wplRenderGroup* group, 
		i64 cap, 
		wplShader* shader, wplTexture* texture, 
		MemoryArena* arena)
{
	group->scale = 1;
	group->clearOnDraw = 1;
	group->offsetX = 0;
	group->offsetY = 0;
	group->tint = 0;

	group->texture = texture;
	group->shader = shader;

	if(!texture->glIndex) {
		wplUploadTexture(texture);
	}

	if(!shader->vert || !shader->frag || !shader->program) {
		initDefaultShader(window, shader);
	}

	group->capacity = cap;
	group->sprites = arenaPush(arena, sizeof(wplSprite) * group->capacity);
	group->verts = arenaPush(arena, sizeof(wplVertex) * 4 * group->capacity);
	group->indices = arenaPush(arena, sizeof(i32) * group->capacity);
	group->vertCounts = arenaPush(arena, sizeof(i32) * group->capacity);
	group->count = 0;
	group->lastFilled = 0;

	if(window->glVersion > 21) {
		glGenVertexArrays(1, &group->vao);
		glBindVertexArray(group->vao);
	}

	glGenBuffers(1, &group->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, group->vbo);

	if(window->glVersion < 33) {
		i32 i = 0;
		i32 stride = sizeof(wplVertex);
#define primMember(name) (void*)offsetof(wplVertex, name)
		glVertexAttribPointer(i, 2, GL_FLOAT, 0, stride, primMember(x));
		glEnableVertexAttribArray(i);
		i++;

		glVertexAttribPointer(i, 2, GL_FLOAT, 0, stride, primMember(u));
		glEnableVertexAttribArray(i);
		i++;

		glVertexAttribPointer(i, 4, GL_UNSIGNED_BYTE, 1, stride, primMember(color));
		glEnableVertexAttribArray(i);
		i++;
	} else {
		i32 i = 0;
		i32 stride = sizeof(wplSprite);
#define spriteMember(name) (void*)offsetof(wplSprite, name)
		glVertexAttribIPointer(i, 1, GL_INT, stride, spriteMember(flags));
		glEnableVertexAttribArray(i);
		glVertexAttribDivisor(i++, 1);

		glVertexAttribPointer(i, 4, GL_UNSIGNED_BYTE, 1, stride, spriteMember(color));
		glEnableVertexAttribArray(i);
		glVertexAttribDivisor(i++, 1);

		glVertexAttribPointer(i, 2, GL_FLOAT, 0, stride, spriteMember(x));
		glEnableVertexAttribArray(i);
		glVertexAttribDivisor(i++, 1);

		glVertexAttribPointer(i, 2, GL_FLOAT, 0, stride, spriteMember(w));
		glEnableVertexAttribArray(i);
		glVertexAttribDivisor(i++, 1);

		glVertexAttribPointer(i, 2, GL_FLOAT, 0, stride, spriteMember(cx));
		glEnableVertexAttribArray(i);
		glVertexAttribDivisor(i++, 1);

		glVertexAttribPointer(i, 4, GL_SHORT, 0, stride, spriteMember(tx));
		glEnableVertexAttribArray(i);
		glVertexAttribDivisor(i++, 1);

		glVertexAttribPointer(i, 1, GL_FLOAT, 0, stride, spriteMember(angle));
		glEnableVertexAttribArray(i);
		glVertexAttribDivisor(i++, 1);
		glBindVertexArray(0);
	}
}

static
void groupProcessSprites(wplState* state, wplRenderGroup* group)
{
	vf128 groupScale = _mm_set_ps1(group->scale);
	vf128 offsetXs = _mm_set_ps1(group->offsetX);
	vf128 offsetYs = _mm_set_ps1(group->offsetY);
	vf128 viewportXs = _mm_set_ps1((f32)state->width);
	vf128 viewportYs = _mm_set_ps1((f32)state->height);
	for(isize i = 0; i < group->count; ++i) {
		wplSprite* s = group->sprites + i;
		wplVertex* p = group->verts + (i*4);
		group->indices[i] = i * 4;	
		group->vertCounts[i] = 4;

		f32 uvrect[4];
		uvrect[0] = (f32)s->tx;
		uvrect[1] = (f32)s->ty;
		uvrect[2] = (f32)(s->tx + s->tw); 
		uvrect[3] = (f32)(s->ty + s->th);

		vf128 xs = _mm_set_ps(-0.5, -0.5, 0.5, 0.5);
		vf128 ys = _mm_set_ps(0.5, -0.5, 0.5, -0.5);
		vf128 uvxs = _mm_set_ps(uvrect[2], uvrect[2], uvrect[0], uvrect[0]);
		vf128 uvys = _mm_set_ps(uvrect[3], uvrect[1], uvrect[3], uvrect[1]);


		f32 scaleX = s->w;
		f32 scaleY = s->h;
		if(s->flags & Sprite_RotateCW) {
			uvxs = vfShuffle(uvxs, 3, 1, 2, 0);
			uvys = vfShuffle(uvys, 3, 1, 2, 0);
			scaleX = s->h;
			scaleY = s->w;
		}

		if(s->flags & Sprite_RotateCCW) {
			uvxs = vfShuffle(uvxs, 2, 0, 3, 1);
			uvys = vfShuffle(uvys, 2, 0, 3, 1);
			scaleX = s->h;
			scaleY = s->w;
		}

		{
			vf128 scaleXs = _mm_set_ps1(scaleX * group->scale);
			vf128 scaleYs = _mm_set_ps1(scaleY * group->scale);
			xs = _mm_mul_ps(xs, scaleXs);
			ys = _mm_mul_ps(ys, scaleYs);
		}

		/* Rotations 
		 * -angle yields the correct rotation for inverted Y, matching
		 *  the shader version.
		 * */
		if(s->angle != 0) {
			vf128 lsin, lcos;
			wpl_sse2_sincos_ps(_mm_set_ps1(-s->angle), &lsin, &lcos);

			vf128 centerX = _mm_set_ps1(s->cx);
			vf128 centerY = _mm_set_ps1(s->cy);

			xs = _mm_sub_ps(xs, centerX);
			ys = _mm_sub_ps(ys, centerY);
			
			vf128 cxs = _mm_mul_ps(lcos, xs);
			vf128 sxs = _mm_mul_ps(lsin, xs);

			vf128 cys = _mm_mul_ps(lcos, ys);
			vf128 sys = _mm_mul_ps(lsin, ys);

			xs = _mm_add_ps(cxs, sys);
			ys = _mm_sub_ps(cys, sxs);

			xs = _mm_add_ps(xs, centerX);
			ys = _mm_add_ps(ys, centerY);
		}

		{
			vf128 pxs = _mm_set_ps1(s->x);
			pxs = _mm_mul_ps(pxs, groupScale);
			xs = _mm_add_ps(xs, pxs);
			xs = _mm_sub_ps(xs, offsetXs);

			vf128 pys = _mm_set_ps1(s->y);
			pys = _mm_mul_ps(pys, groupScale);
			ys = _mm_add_ps(ys, pys);
			ys = _mm_sub_ps(ys, offsetYs);
		}

		/* Normalize the position to -1, 1 based on the window size
		 * pos * vec2(2, -2) / viewportWH - vec2(1, -1)
		 */
		{
			vf128 number = _mm_set_ps1(2);
			xs = _mm_mul_ps(xs, number);
			xs = _mm_div_ps(xs, viewportXs);
			number = _mm_set_ps1(1);
			xs = _mm_sub_ps(xs, number);

			number = _mm_set_ps1(-2);
			ys = _mm_mul_ps(ys, number);
			ys = _mm_div_ps(ys, viewportYs);
			number = _mm_set_ps1(-1);
			ys = _mm_sub_ps(ys, number);
		}

		vf32x4 x = {xs}, y = {ys}, uvx = {uvxs}, uvy = {uvys};
		for(isize j = 0; j < 4; ++j) {
			p[j].x = x.f[j];
			p[j].y = y.f[j];
			p[j].u = uvx.f[j];
			p[j].v = uvy.f[j];
			p[j].color = s->color;
		}
	}
}

void wplGroupDrawBasic(wplState* state, wplRenderGroup* group)
{
	wplShader* shader = group->shader;

	glUniform2f(
			shader->uInvTextureSize,
			1.0f / (f32)group->texture->w, 
			1.0f / (f32)group->texture->h);

	glBindTexture(GL_TEXTURE_2D, group->texture->glIndex);

	groupProcessSprites(state, group);

	glBindBuffer(GL_ARRAY_BUFFER, group->vbo);
	glBufferData(GL_ARRAY_BUFFER,
			sizeof(wplSprite) * group->count,
			NULL,
			GL_STREAM_DRAW);

	glBufferData(GL_ARRAY_BUFFER,
			sizeof(wplVertex) * 4 * group->count,
			group->verts,
			GL_STREAM_DRAW);

	glMultiDrawArrays(GL_TRIANGLE_STRIP, 
			group->indices,
			group->vertCounts, 
			group->count);

	if(group->clearOnDraw) {
		group->count = 0;
	}
}

void wplGroupDraw(wplWindow* window, wplState* state, wplRenderGroup* group)
{
	if(group->count == 0) return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	wplShader* shader = group->shader;
	glUseProgram(shader->program);

	if(window->glVersion < 33) {
		wplGroupDrawBasic(state, group);
		return;
	}
	glUniform2f(shader->uViewport, 
			state->width, 
			state->height);
	glUniform4f(shader->uTint, 
			(f32)(group->tint & 0xFF) / 255.0f, 
			(f32)((group->tint >> 8) & 0xFF) / 255.0f,
			(f32)((group->tint >> 16) & 0xFF) / 255.0f,
			(f32)((group->tint >> 24) & 0xFF) / 255.0f);

	glUniform2f(
			shader->uInvTextureSize,
			1.0f / (f32)group->texture->w, 
			1.0f / (f32)group->texture->h);

	glUniform1f(shader->uScale, group->scale);
	glUniform2f(shader->uOffset, 
			group->offsetX / group->scale,
			group->offsetY / group->scale);

	glBindTexture(GL_TEXTURE_2D, group->texture->glIndex);
	glBindVertexArray(group->vao);
	glBindBuffer(GL_ARRAY_BUFFER, group->vbo);
	glBufferData(GL_ARRAY_BUFFER,
			sizeof(wplSprite) * group->count,
			group->sprites,
			GL_STREAM_DRAW);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, group->count);

	glBindVertexArray(0);

	if(group->clearOnDraw) {
		group->count = 0;
	}
}

void wplUploadTexture(wplTexture* texture)
{
	glGenTextures(1, &texture->glIndex);
	glBindTexture(GL_TEXTURE_2D, texture->glIndex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
			texture->w, texture->h, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, 
			texture->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

wplTexture* wplLoadTexture(wplWindow* window, string filename, MemoryArena* arena)
{
	int t = 0;
	wplTexture texture = {0};
	//string lfilename = filename;
	stringValidate(&filename);
	char buf[2048];
	snprintf(buf, 2048, "%s%s", window->basePath, wplTempString);
	i32 w, h, bpp;
	u8* data = stbi_load(buf, &w, &h, &bpp, STBI_rgb_alpha);
	if(w == 0 || h == 0) {
		//fprintf(stderr, "Error: Unable to load image %s\n", buf);
		return NULL;
	}

	texture.w = w;
	texture.h = h;
	texture.pixels = data;

	wplTexture* ptex = arenaPush(arena, sizeof(wplTexture));
	*ptex = texture;
	return ptex;
}


/***************************************************************************
    Open GL Video Rendering.

    Useful References:
    http://www.sdltutorials.com/sdl-opengl-tutorial-basics
    http://www.opengl.org/wiki/Common_Mistakes
    http://open.gl/textures

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

#include <iostream>
#include <algorithm> 

#include "rendergl.hpp"
#include "frontend/config.hpp"

const static uint32_t SCANLINE_TEXTURE[] = { 0x00000000, 0xff000000 }; // BGRA 8-8-8-8-REV

RenderGL::RenderGL()
{
}

bool RenderGL::init(int src_width, int src_height,
                    int scale,
                    int video_mode,
                    int scanlines)
{
    this->src_width  = src_width;
    this->src_height = src_height;
    this->video_mode = video_mode;
    this->scanlines  = scanlines;

    // Setup SDL Screen size
    if (!RenderBase::sdl_screen_size())
        return false;

    int flags = SDL_OPENGL;

    // Full Screen Mode
    if (video_mode == video_settings_t::MODE_FULL)
    {
        // Calculate how much to scale screen from its original resolution
        uint32_t w = (scn_width  << 16)  / src_width;
        uint32_t h = (scn_height << 16)  / src_height;
        dst_width  = (src_width  * std::min(w, h)) >> 16;
        dst_height = (src_height * std::min(w, h)) >> 16;
        flags |= SDL_FULLSCREEN; // Set SDL flag
        SDL_ShowCursor(false);   // Don't show mouse cursor in full-screen mode
    }
    // Stretch screen. Lose original proportions
    else if (video_mode == video_settings_t::MODE_STRETCH)
    {
        dst_width  = scn_width;
        dst_height = scn_height;
        flags |= SDL_FULLSCREEN; // Set SDL flag
        SDL_ShowCursor(false);   // Don't show mouse cursor in full-screen mode
    }
    // Window Mode
    else
    {
        scn_width  = dst_width  = src_width  * scale;
        scn_height = dst_height = src_height * scale;
        SDL_ShowCursor(true);
    }

    // If we're not stretching the screen, centre the image
    if (video_mode != video_settings_t::MODE_STRETCH)
    {
        screen_xoff = scn_width - dst_width;
        if (screen_xoff)
            screen_xoff = (screen_xoff / 2);

        screen_yoff = scn_height - dst_height;
        if (screen_yoff)
            screen_yoff = (screen_yoff / 2);
    }
    // Otherwise set to the top-left corner
    else
    {
        screen_xoff = 0;
        screen_yoff = 0;
    }

    //int bpp = info->vfmt->BitsPerPixel;
	const int bpp = SDL_BPP;
    const int available = SDL_VideoModeOK(scn_width, scn_height, bpp, flags);

    // Frees (Deletes) existing surface
    if (surface)
        SDL_FreeSurface(surface);

    // Set the video mode
    surface = SDL_SetVideoMode(scn_width, scn_height, bpp, flags);

    if (!surface || !available)
    {
        std::cerr << "Video mode set failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (screen_pixels)
        delete[] screen_pixels;
    screen_pixels = new uint32_t[src_width * src_height];

    // SDL Pixel Format Information
    Rshift = surface->format->Rshift;
    Gshift = surface->format->Gshift;
    Bshift = surface->format->Bshift;

    // This hack is necessary to fix an Apple OpenGL with SDL issue
    #ifdef __APPLE__
      #if SDL_BYTEORDER == SDL_LIL_ENDIAN
        Rmask = 0x000000FF;
        Gmask = 0x0000FF00;
        Bmask = 0x00FF0000;
        Rshift += 8;
        Gshift -= 8;
        Bshift += 8;
      #else
        Rmask = 0xFF000000;
        Gmask = 0x00FF0000;
        Bmask = 0x0000FF00;
      #endif
    #else
        Rmask  = surface->format->Rmask;
        Gmask  = surface->format->Gmask;
        Bmask  = surface->format->Bmask;
    #endif

	// --------------------------------------------------------------------------------------------
	// Initalize Screen Quads
	// --------------------------------------------------------------------------------------------

	ASSIGN_VERTEX(screen[0].vertices[0], screen_xoff, screen_yoff + dst_height, 0, 1)
	ASSIGN_VERTEX(screen[0].vertices[1], screen_xoff, screen_yoff, 0, 0)
	ASSIGN_VERTEX(screen[0].vertices[2], screen_xoff + dst_width, screen_yoff + dst_height, 1, 1)
	ASSIGN_VERTEX(screen[0].vertices[3], screen_xoff + dst_width, screen_yoff, 1, 0)

	ASSIGN_VERTEX(screen[1].vertices[0], screen_xoff, screen_yoff + dst_height, 0, S16_HEIGHT)
	ASSIGN_VERTEX(screen[1].vertices[1], screen_xoff, screen_yoff, 0, 0)
	ASSIGN_VERTEX(screen[1].vertices[2], screen_xoff + dst_width, screen_yoff + dst_height, src_width, S16_HEIGHT)
	ASSIGN_VERTEX(screen[1].vertices[3], screen_xoff + dst_width, screen_yoff, src_width, 0)

    // --------------------------------------------------------------------------------------------
    // Initalize Open GL
    // --------------------------------------------------------------------------------------------

    // Disable dithering
    glDisable(GL_DITHER);
    // Disable anti-aliasing
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    // Disable depth buffer
    glDisable(GL_DEPTH_TEST);

    // V-Sync
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

    glClearColor(0, 0, 0, 0); // Black background
    glShadeModel(GL_FLAT);

    glViewport(0, 0, scn_width, scn_height);

    // Initalize Texture ID
    glGenTextures(scanlines ? 2 : 1, textures);

    // Screen Texture Setup
    const GLint param = config.video.filtering ? GL_LINEAR : GL_NEAREST;
    glBindTexture(GL_TEXTURE_2D, textures[SCREEN]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                src_width, src_height, 0,                // texture width, texture height
				GL_RGBA, GL_UNSIGNED_BYTE,    // Data format in pixel array
                NULL);

    // Scanline Texture Setup
    if (scanlines)
    {
        glBindTexture(GL_TEXTURE_2D, textures[SCANLN]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     1, 2, 0,
					 GL_RGBA, GL_UNSIGNED_BYTE,
                     SCANLINE_TEXTURE);
    }

    return true;
}

void RenderGL::disable()
{
    glDeleteTextures(scanlines ? 2 : 1, textures);
}

bool RenderGL::start_frame()
{
    return !(SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) < 0);
}

bool RenderGL::finalize_frame()
{
	SDL_GL_SwapBuffers();

    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);

    return true;
}

void RenderGL::draw_frame(uint16_t* pixels)
{
    uint32_t* spix = screen_pixels;

    // Lookup real RGB value from rgb array for backbuffer
	uint32_t p = 0;
	for (int i = 0; i < (src_width * src_height); ++i)
	{
		p = rgb[*(pixels++) & ((S16_PALETTE_ENTRIES * 3) - 1)];
		*(spix++) = (p & 0xFF00FF00) | ((p & 0x00FF0000) >> 16) | ((p & 0x000000FF) << 16);
	}

    glBindTexture(GL_TEXTURE_2D, textures[SCREEN]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,            // target, LOD, xoff, yoff
            src_width, src_height,                     // texture width, texture height
            GL_RGBA,                                   // format of pixel data
			GL_UNSIGNED_BYTE,               // data type of pixel data
            screen_pixels);                            // pointer in image memory

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, scn_width, scn_height, 0, 0, 1);         // left, right, bottom, top, near, far

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen and depth buffer

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[SCREEN]);     // Select Screen Texture

	glVertexPointer(2, GL_FLOAT, sizeof(vertex_t), screen[0].vertices[0].pos);
	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex_t), screen[0].vertices[0].texcoord);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (scanlines)
	{
		glEnable(GL_BLEND);
		glColor4ub(255, 255, 255, ((scanlines - 1) << 8) / 100);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, textures[SCANLN]);

		glVertexPointer(2, GL_FLOAT, sizeof(vertex_t), screen[1].vertices[0].pos);
		glTexCoordPointer(2, GL_FLOAT, sizeof(vertex_t), screen[1].vertices[0].texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisable(GL_BLEND);
	}

	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopMatrix();
}
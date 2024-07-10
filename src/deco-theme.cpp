#include "deco-theme.hpp"
#include <iostream>
#include <string>
#include <wayfire/core.hpp>
#include <wayfire/opengl.hpp>
#include <wayfire/config/types.hpp>

namespace wb
{
namespace decor
{
/** Create a new theme with the default parameters */
decoration_theme_t::decoration_theme_t()
{
}

/** @return The available border for resizing */
int decoration_theme_t::get_border_size() const
{
	return border_size;
}

/**
 * Fill the given rectangle with the background color(s).
 *
 * @param fb The target framebuffer, must have been bound already
 * @param rectangle The rectangle to redraw.
 * @param scissor The GL scissor rectangle to use.
 * @param active Whether to use active or inactive colors
 */
void decoration_theme_t::render_background(const wf::render_target_t &fb,
					   wf::geometry_t rectangle,
					   const wf::geometry_t &scissor,
					   bool active) const
{
	wf::color_t color = active ? active_color : inactive_color;
	OpenGL::render_begin(fb);
	fb.logic_scissor(scissor);
	OpenGL::render_rectangle(rectangle, color,
				 fb.get_orthographic_projection());
	OpenGL::render_end();
}
}
}

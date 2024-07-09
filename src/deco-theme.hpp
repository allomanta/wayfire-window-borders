#pragma once
#include <wayfire/render-manager.hpp>

namespace wb
{
namespace decor
{
/**
 * A  class which manages the outlook of decorations.
 * It is responsible for determining the background colors, sizes, etc.
 */
class decoration_theme_t {
    public:
	/** Create a new theme with the default parameters */
	decoration_theme_t();

	/** @return The available border for resizing */
	int get_border_size() const;

	/**
     * Fill the given rectangle with the background color(s).
     *
     * @param fb The target framebuffer, must have been bound already.
     * @param rectangle The rectangle to redraw.
     * @param scissor The GL scissor rectangle to use.
     * @param active Whether to use active or inactive colors
     */
	void render_background(const wf::render_target_t &fb,
			       wf::geometry_t rectangle,
			       const wf::geometry_t &scissor,
			       bool active) const;

    private:
	wf::option_wrapper_t<int> border_size{ "borders/border_size" };
	wf::option_wrapper_t<wf::color_t> active_color{ "borders/active_color" };
	wf::option_wrapper_t<wf::color_t> inactive_color{
		"borders/inactive_color"
	};
};
}
}

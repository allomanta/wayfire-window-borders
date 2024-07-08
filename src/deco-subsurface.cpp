#include "wayfire/geometry.hpp"
#include "wayfire/scene-input.hpp"
#include "wayfire/scene-operations.hpp"
#include "wayfire/scene-render.hpp"
#include "wayfire/scene.hpp"
#include "wayfire/signal-provider.hpp"
#include "wayfire/toplevel.hpp"
#include <memory>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

#include <linux/input-event-codes.h>

#include <wayfire/nonstd/wlroots.hpp>
#include <wayfire/output.hpp>
#include <wayfire/opengl.hpp>
#include <wayfire/core.hpp>
#include <wayfire/view-transform.hpp>
#include <wayfire/signal-definitions.hpp>
#include <wayfire/toplevel-view.hpp>
#include "deco-subsurface.hpp"
#include "deco-theme.hpp"
#include <wayfire/window-manager.hpp>

#include <wayfire/plugins/common/cairo-util.hpp>

#include <cairo.h>

namespace wb
{
class simple_decoration_node_t : public wf::scene::node_t {
	std::weak_ptr<wf::toplevel_view_interface_t> _view;
	wf::signal::connection_t<wf::view_title_changed_signal> title_set =
		[=](wf::view_title_changed_signal *ev) {
			if (auto view = _view.lock()) {
				view->damage();
			}
		};

	struct {
		wf::simple_texture_t tex;
		std::string current_text = "";
	} title_texture;

    public:
	wb::decor::decoration_theme_t theme;

	wf::dimensions_t size;

	int current_thickness;
	int current_titlebar;

	simple_decoration_node_t(wayfire_toplevel_view view)
		: node_t(false)
		, theme{}
	{
		this->_view = view->weak_from_this();
		view->connect(&title_set);

		// make sure to hide frame if the view is fullscreen
		update_decoration_size();
	}

	wf::point_t get_offset()
	{
		return { -current_thickness, -current_thickness };
	}

	void render_scissor_box(const wf::render_target_t &fb,
				wf::point_t origin, const wlr_box &scissor)
	{
		/* Clear background */
		wlr_box geometry{ origin.x, origin.y, size.width, size.height };

		bool activated = false;
		if (auto view = _view.lock()) {
			activated = view->activated;
		}

		theme.render_background(fb, geometry, scissor, activated);
	}

	class decoration_render_instance_t
		: public wf::scene::render_instance_t {
		std::shared_ptr<simple_decoration_node_t> self;
		wf::scene::damage_callback push_damage;

		wf::signal::connection_t<wf::scene::node_damage_signal>
			on_surface_damage =
				[=](wf::scene::node_damage_signal *data) {
					push_damage(data->region);
				};

	    public:
		decoration_render_instance_t(
			simple_decoration_node_t *self,
			wf::scene::damage_callback push_damage)
		{
			this->self = std::dynamic_pointer_cast<
				simple_decoration_node_t>(
				self->shared_from_this());
			this->push_damage = push_damage;
			self->connect(&on_surface_damage);
		}

		void schedule_instructions(
			std::vector<wf::scene::render_instruction_t>
				&instructions,
			const wf::render_target_t &target,
			wf::region_t &damage) override
		{
		}

		void render(const wf::render_target_t &target,
			    const wf::region_t &region) override
		{
			for (const auto &box : region) {
				self->render_scissor_box(
					target, self->get_offset(),
					wlr_box_from_pixman_box(box));
			}
		}
	};

	void gen_render_instances(
		std::vector<wf::scene::render_instance_uptr> &instances,
		wf::scene::damage_callback push_damage,
		wf::output_t *output = nullptr) override
	{
		instances.push_back(
			std::make_unique<decoration_render_instance_t>(
				this, push_damage));
	}

	wf::geometry_t get_bounding_box() override
	{
		return wf::construct_box(get_offset(), size);
	}

	void resize(wf::dimensions_t dims)
	{
		if (auto view = _view.lock()) {
			view->damage();
			size = dims;
			view->damage();
		}
	}

	void update_decoration_size()
	{
		bool fullscreen =
			_view.lock()->toplevel()->current().fullscreen;
		if (fullscreen) {
			current_thickness = 0;
		} else {
			current_thickness = theme.get_border_size();
		}
	}
};

wb::simple_decorator_t::simple_decorator_t(wayfire_toplevel_view view)
{
	this->view = view;
	deco = std::make_shared<simple_decoration_node_t>(view);
	deco->resize(wf::dimensions(view->get_pending_geometry()));
	wf::scene::add_back(view->get_surface_root_node(), deco);

	view->connect(&on_view_activated);
	view->connect(&on_view_geometry_changed);
	view->connect(&on_view_fullscreen);

	on_view_activated = [this](auto) {
		wf::scene::damage_node(deco, deco->get_bounding_box());
	};

	on_view_geometry_changed = [this](auto) {
		deco->resize(wf::dimensions(this->view->get_geometry()));
	};

	on_view_fullscreen = [this](auto) {
		deco->update_decoration_size();
		if (!this->view->toplevel()->current().fullscreen) {
			deco->resize(
				wf::dimensions(this->view->get_geometry()));
		}
	};
}

wb::simple_decorator_t::~simple_decorator_t()
{
	wf::scene::remove_child(deco);
}

wf::decoration_margins_t
wb::simple_decorator_t::get_margins(const wf::toplevel_state_t &state)
{
	if (state.fullscreen) {
		return { 0, 0, 0, 0 };
	}

	const int thickness = deco->theme.get_border_size();
	return wf::decoration_margins_t{
		.left = thickness,
		.right = thickness,
		.bottom = thickness,
		.top = thickness,
	};
}
}

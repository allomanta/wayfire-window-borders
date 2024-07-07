 project(
	'wayfire-window-borders',
    'c',
	'cpp',
	version: '0.1',
	license: 'MIT',
	meson_version: '>=0.51.0',
	default_options: [
		'cpp_std=c++17',
        'c_std=c11',
		'warning_level=2',
		'werror=false',
	],
)

wayfire = dependency('wayfire')
wlroots = dependency('wlroots')
wfconfig = dependency('wf-config')

wayland_client = dependency('wayland-client')
cairo          = dependency('cairo')
pango          = dependency('pango')
pangocairo     = dependency('pangocairo')
pixman         = dependency('pixman-1')


 
add_project_arguments(['-DWLR_USE_UNSTABLE'], language: ['cpp', 'c'])
add_project_arguments(['-DWAYFIRE_PLUGIN'], language: ['cpp', 'c'])
add_project_link_arguments(['-rdynamic'], language:'cpp')

wl_protos_src = []
wl_protos_headers = []

lib_wl_protos = static_library('wl_protos', wl_protos_src + wl_protos_headers,
	dependencies: [wayland_client]) # for the include directory

wf_protos = declare_dependency(
	link_with: lib_wl_protos,
	sources: wl_protos_headers,
)

subdir('src')
subdir('metadata')

summary = [
	'',
	'----------------',
	'wayfire-window-borders @0@'.format(meson.project_version()),
	'----------------',
	''
]
message('\n'.join(summary))

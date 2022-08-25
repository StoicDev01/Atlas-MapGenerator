inc = include_directories('.')

sources = files(
    'map_generator_gui.cpp',
    'map_generator.cpp',
    'map_viewer.cpp',
    'map_generator_scene.cpp',
    'main.cpp',
    'gui.cpp'
)

dependencies = [core_dep, fastnoiselite_dep, json_dep, magic_enum_dep]

cpp_args = []

if (host_machine.system() == 'windows')
    cpp_args += '-Wl,-subsystem,windows'
endif

executable(
    'Map Generator',
    sources : sources,
    include_directories: inc,
    dependencies: dependencies,
    cpp_args: cpp_args
)
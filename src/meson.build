inc = include_directories('.')

sources = files(
    'map_generator_gui.cpp',
    'map_generator.cpp',
    'map_viewer.cpp',
    'main.cpp',
    'gui.cpp',
    'world_template_creator.cpp'
)

dependencies = [core_dep, fastnoiselite_dep]

executable(
    'Map Generator',
    sources : sources,
    include_directories: inc,
    dependencies: dependencies
)
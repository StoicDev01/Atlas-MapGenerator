project(
    'Map Generator',
    'cpp',
    version: '1.0',
    default_options : ['cpp_std=c++20', 'default_library=static']
)

host_system = host_machine.system()

if (host_system == 'windows')
    add_global_arguments('-Wl,-subsystem,windows', language : 'cpp')
endif

core_dep = dependency('core', default_options: 'default_library=static')
fastnoiselite_dep = dependency('fastnoiselite', default_options:  'default_library=static')
json_dep = dependency('nlohmann_json', default_options: 'default_library=static')
magic_enum_dep = dependency('magic_enum', default_options:'default_library=static')

subdir('src')
#pragma once
#include "core/core.h"
#include "gui.h"

#include "nlohmann/json.hpp"
#include "FastNoiseLite.h"

namespace core{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Range<float>, min, max)

    namespace graphics{
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Color, r,g,b,a)
    }
}

class Biome{
    public:
    std::string name;
    core::Range<float> height_range;
    core::Range<float> temperature_range;
    core::Range<float> humidity_range;

    int index;
    int surface_block_id;
    int interior_block_id;

    core::graphics::Color color;

    Biome();
    Biome(
        std::string name, core::Range<float> height_range, core::Range<float> temperature_range, core::Range<float> humidity_range, core::graphics::Color color
    );

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Biome, name, height_range, temperature_range, humidity_range, index, surface_block_id, interior_block_id, color)
};


float inverseLerp(float x, float y, float value);


typedef std::vector<std::vector<float>> FloatMatrix;
typedef std::vector<std::vector<Biome*>> BiomeMatrix;
typedef std::vector<Biome> Biomes;
typedef std::vector<std::vector<bool>> BoolMatrix;

class MapGeneratorSettings{
    public:    
    uint32_t m_size_x;
    uint32_t m_size_y;

    int m_seed_input;
    int m_octaves_slider;
    float m_gain_slider;
    float m_lacunatiry_slider;
    float m_weightedStr_slider;
    float m_pingPongStr_slider;
    float m_frequency_slider;
    int m_current_fallof_type;

    /* Fallof */
    bool m_use_fallof_map;
    std::string m_fallof_type;
    float m_fallof_a;
    float m_fallof_b;

    /* latitude fallof */
    float m_latitude_a;
    float m_latitude_b;

    /* Temperature */
    float m_max_temperature; // max world surface temperature
    float m_min_temperature; // min world surface temperature
    float m_temperature_a;
    float m_temperature_b;

    float m_min_temperature_latitude; // temperature at poles
    float m_max_temperature_latitude;  // temperature at equator and tropics

    float m_temperature_altitude_increment;
    float m_temperature_noise_scale;
    float m_max_temperature_at_sea;

    /* Humidity */
    float m_humidity_noise_scale;
    float m_humidity_temperature_increment;

    /* Elevation */
    float m_min_elevation;
    float m_max_elevation;
    float m_sea_level;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        MapGeneratorSettings, m_seed_input, m_octaves_slider, m_gain_slider, m_lacunatiry_slider, m_weightedStr_slider,
        m_pingPongStr_slider, m_frequency_slider, m_current_fallof_type, m_use_fallof_map, m_fallof_a, m_fallof_b,
        m_latitude_a, m_latitude_b, m_max_temperature, m_min_temperature, m_temperature_a, m_temperature_b, m_min_temperature_latitude,
        m_max_temperature_latitude, m_temperature_altitude_increment, m_temperature_noise_scale, m_max_temperature_at_sea,
        m_humidity_noise_scale, m_humidity_temperature_increment, m_min_elevation, m_max_elevation, m_sea_level, m_size_x, m_size_y,
        m_fallof_type
    )
};


// Main Map generator class
class MapGenerator{
    public:

    enum class draw_modes {
        COLOR_MAP = 0,
        NOISE_MAP,
        TEMPERATURE_MAP,
        HUMIDITY_MAP,
        FALLOF_MAP,
    };

    // list of map biomes
    Biomes m_biomes;
    
    /* Maps */
    FloatMatrix m_height_map;
    FloatMatrix m_temperature_map;
    FloatMatrix m_fallof_map;
    BiomeMatrix m_biome_map;
    FloatMatrix m_humidity_map;

    // generation settings
    MapGeneratorSettings m_settings;

    // 
    core::graphics::Sprite m_sprite;          // sprite of the map
    core::graphics::Sprite m_chunk_sprite;    // sprite of the generated chunk
    core::graphics::Texture m_chunk_texture;
    core::graphics::Texture m_texture;

    FastNoiseLite m_noise;

    MapGenerator(uint32_t size_x = 1500, uint32_t size_y = 1000);

    // load a mapgenerator from a world template
    bool load_from_template(std::filesystem::path& path);
    // save a map generator to a world template
    bool save_to_template(std::string name);

    void initialize();
    void generate(draw_modes draw_mode);

    // functions to get data from x and y
    float   get_height_from(float x, float y);
    float   get_fallof_map_from(float x, float y);
    float   get_temperature_from(float x, float y, bool use_map_data);
    Biome*  get_biome_from(float x, float y, bool use_map_data);
    float   get_humidity_from(float x, float y, bool use_map_data);
    core::graphics::Color get_color_from(float x, float y, draw_modes draw_mode, bool use_map_data);

    // functions to generate maps
    void generate_heigh_map();
    void generate_sprite(draw_modes draw_mode);
    void generate_fallof_map();
    void generate_temperature_map();
    void generate_biome_map();
    void generate_humidity_map();
    //void generate_chunk(core::Vector2i chunk_position);

    // functions to evaluate values
    float evaluate_fallof_map(float value);
    float evaluate_latitude(float value);
    float temperature_to_celcius(float temperature);
    float height_to_elevation(float height);

    // utils functions
    int get_last_biome_index();
    Biome* get_biome_by_index(int index);
    void add_biome(Biome biome);
};
/* Some Utils functions */
bool compareBiomesIndex(Biome b1, Biome b2);


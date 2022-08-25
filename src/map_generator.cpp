#include "map_generator.h"
#include <algorithm>

float smoothstep(float edge0, float edge1, float x) {
    // Scale, bias and saturate x to 0..1 range
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

/* Biome */
Biome::Biome(std::string name, core::Range<float> height_range, core::Range<float> temperature_range, core::Range<float> humidity_range, core::graphics::Color color){
    this->name = name;
    this->height_range = height_range;
    this->temperature_range = temperature_range;
    this->humidity_range = humidity_range;
    this->color = color;
}

Biome::Biome(){}

float inverseLerp(float x, float y, float value){
    return (value - x)/ (y - x);
}

/*
Object WorldObjects::get_by_id(uint id){
    id -= 1;

    if (id < 0 or id + 1 > objects.size()){
        return (Object());
    }
    else{
        return objects.at(id);
    }
}
*/

MapGenerator::MapGenerator(uint32_t size_x, uint32_t size_y){
    m_settings.m_size_x = size_x;
    m_settings.m_size_y = size_y;
    m_settings.m_seed_input = 1;
    m_settings.m_octaves_slider = 5;
    m_settings.m_gain_slider = 0.5f;
    m_settings.m_lacunatiry_slider = 0.3f;
    m_settings.m_weightedStr_slider = 0.0f;
    m_settings.m_pingPongStr_slider = 0.0f;
    m_settings.m_frequency_slider = 0.34f;
    m_settings.m_current_fallof_type = 0;

    /* Fallof */
    m_settings.m_use_fallof_map = true;
    m_settings.m_fallof_type = "Square";
    m_settings.m_fallof_a = 3.0f;
    m_settings.m_fallof_b = 2.2f;

    /* latitude fallof */
    m_settings.m_latitude_a = 4.0f;
    m_settings.m_latitude_b = 0.8f;

    /* Temperature */
    m_settings.m_max_temperature = 40; // max world surface temperature
    m_settings.m_min_temperature = -50; // min world surface temperature
    m_settings.m_temperature_a = 0.232;
    m_settings.m_temperature_b = 1.072;

    m_settings.m_min_temperature_latitude = -50; // temperature at poles
    m_settings.m_max_temperature_latitude = 25;  // temperature at equator and tropics

    m_settings.m_temperature_altitude_increment = 0.95f;
    m_settings.m_temperature_noise_scale = 1.381f;
    m_settings.m_max_temperature_at_sea = 25.0f;

    /* Humidity */
    m_settings.m_humidity_noise_scale = 1.289f;
    m_settings.m_humidity_temperature_increment = 0.95f;

    /* Elevation */
    m_settings.m_min_elevation = 0;
    m_settings.m_max_elevation = 100;
    m_settings.m_sea_level = 40.0f;

    initialize();
}

// load from world template
bool MapGenerator::load_from_template(std::filesystem::path& path){
    std::ifstream fs;

    std::filesystem::path map_generator_json = path / "map_generator.json";
    std::filesystem::path biomes_json = path / "biomes.json";

    // load map generator settings
    fs.open(map_generator_json.string(), std::ios::in);

    if (!fs){
        std::cout << "[ Error ] could not load template map_generator.json" << std::endl;
    }
    else{
        nlohmann::json map_json_data;
        fs >> map_json_data;
        m_settings = map_json_data.get<MapGeneratorSettings>();
    }

    fs.close();
    fs.clear();

    // load biomes json settings
    fs.open(biomes_json.string(), std::ios::in);

    if (!fs){
        std::cout << "[ Error ] could not load template biomes.json" << std::endl;
    }
    else{
        nlohmann::json biomes_json_data;
        fs >> biomes_json_data;
        m_biomes = biomes_json_data.get<Biomes>();
    }
    fs.close();
    fs.clear();

    return true;
}

// save to a world template
bool MapGenerator::save_to_template(std::string name){
    std::filesystem::path current_path = std::filesystem::path("./data/world templates") / name;

    std::filesystem::path map_generator_json     = current_path / "map_generator.json";
    std::filesystem::path resource_packs_json    = current_path / "resourcepacks.json";
    std::filesystem::path objects_json           = current_path / "objects.json";
    std::filesystem::path biomes_json            = current_path / "biomes.json";

    // verifiy if folder exists
    if (!std::filesystem::is_directory(current_path)){
        // create presets directory
        if (!std::filesystem::create_directories(current_path)){
            std::cout << "[ Error ] could not create preset directory" << std::endl;
            return false;
        }
    }

    // open map generator file
    std::ofstream file;
    file.open(map_generator_json.string(), std::ios::out);

    if (!file){
        std::cout << "[ Error ] could not save map_generator" << std::endl;
    }
    else{
        // save map generator settings
        nlohmann::json json_data = (m_settings);
        file << std::setw(2) << json_data << std::endl;
    }

    file.close();
    file.clear();

    // open biome file
    file.open(biomes_json.string(), std::ios::out);

    if (!file){
        std::cout << "[ Error ] could save biomes.json to preset folder" << std::endl;
    }
    else{        
        // save biomes settings
        nlohmann::json biome_data = m_biomes;
        file << std::setw(2) << biome_data << std::endl;
    }
    file.close();
    file.clear();

    return true;
}


void MapGenerator::initialize(){
    // initialize matrizes
    m_height_map      = FloatMatrix(m_settings.m_size_x, std::vector<float>(m_settings.m_size_y, 0.0f));
    m_temperature_map = FloatMatrix(m_settings.m_size_x, std::vector<float>(m_settings.m_size_y, 0.0f));
    m_humidity_map    = FloatMatrix(m_settings.m_size_x, std::vector<float>(m_settings.m_size_y, 0.0f));
    m_fallof_map      = FloatMatrix(m_settings.m_size_x, std::vector<float>(m_settings.m_size_y, 0.0f));
    m_biome_map       = BiomeMatrix(m_settings.m_size_x, std::vector<Biome*>(m_settings.m_size_y, nullptr));

    // initialize noise
    m_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_noise.SetSeed(1);
}

void MapGenerator::generate(draw_modes draw_mode){
    m_noise.SetSeed(m_settings.m_seed_input);
    m_noise.SetFractalOctaves(m_settings.m_octaves_slider);
    m_noise.SetFrequency(m_settings.m_frequency_slider * 0.01);
    m_noise.SetFractalGain(m_settings.m_gain_slider);
    m_noise.SetFractalLacunarity(m_settings.m_lacunatiry_slider);
    m_noise.SetFractalWeightedStrength(m_settings.m_weightedStr_slider);
    m_noise.SetFractalPingPongStrength(m_settings.m_pingPongStr_slider);

    generate_fallof_map();
    generate_heigh_map();
    generate_temperature_map();
    generate_humidity_map();
    generate_biome_map();
    generate_sprite(draw_mode);
}

float MapGenerator::get_height_from(float x, float y){
    float noise_value;
    noise_value = m_noise.GetNoise(x,y);

    if (m_settings.m_use_fallof_map){
        return std::clamp(noise_value - m_fallof_map[x][y], -1.0f, 1.0f);
    }

    return noise_value;
}

float MapGenerator::get_fallof_map_from(float x, float y){
    if (m_settings.m_fallof_type == "Circle"){
        core::Vector2i center = core::Vector2i(m_settings.m_size_x/2, m_settings.m_size_y/2);
        core::Vector2i current_pos(x, y);

        float distance = glm::distance(core::Vector2f(center), core::Vector2f(current_pos));

        // make distance range between 0 and 1
        float distance_normalized = std::clamp(0.0f, 1.0f, distance / (m_settings.m_size_y/ 2.0f) );

        return smoothstep(m_settings.m_fallof_a, m_settings.m_fallof_b, distance_normalized);
    }
    else if (m_settings.m_fallof_type == "Square"){
        float nx = std::abs(x *2.0f -  m_settings.m_size_x) / m_settings.m_size_x;
        float ny = std::abs(y *2.0f -  m_settings.m_size_y) / m_settings.m_size_y;

        float value = std::max(nx, ny);
        float value2 = smoothstep(m_settings.m_fallof_a, m_settings.m_fallof_b, value);
        return value2;
    }
    return 0.0f;
}

float MapGenerator::get_temperature_from(float x, float y, bool use_map_data = true){
    /// Generate Temperature latitude variation
    float temperature_latitude;
    float temperature_noise = m_noise.GetNoise(
        ((float)x) * m_settings.m_temperature_noise_scale, ((float)y) * m_settings.m_temperature_noise_scale
    );

    bool at_poles = false;

    float elevation;
    if (use_map_data){
        elevation = height_to_elevation(m_height_map[x][y]);
    }
    else{
        elevation = height_to_elevation(get_height_from(x, y));
    }
    
    bool at_sea_level = false;

    // normalize temperature m_noise
    temperature_noise = ((temperature_noise + 1) / 2);

    // poles to equator temperature
    if (y <= m_settings.m_size_y / 2){
        // generate temperature latitude from north pole to equator
        temperature_latitude = inverseLerp(0.0, m_settings.m_size_y/2, y);
    }
    else if (y > m_settings.m_size_y / 2){
        // generate temperature latitude from south pole to equator
        temperature_latitude = inverseLerp(m_settings.m_size_y, m_settings.m_size_y/2, y);
    }
    temperature_latitude = std::clamp(temperature_latitude, 0.0f, 1.0f);
    temperature_latitude = evaluate_latitude(temperature_latitude);

    /// booleans
    // verify if is at poles
    if (temperature_latitude <= 0.4){
        at_poles = true;
    }

    // verifiy if is at sea level
    if (elevation <= m_settings.m_sea_level){
        at_sea_level = true;
    }

    // transform in a range from min_temperature_latitude to max_temperature latitude
    temperature_latitude = (temperature_latitude * (m_settings.m_max_temperature_latitude+ abs(m_settings.m_min_temperature_latitude))+m_settings.m_min_temperature_latitude);
    
    /// generate temperature altitude with latitude and height map
    float temperature_altitude_subtract = (m_settings.m_temperature_a * pow(m_settings.m_temperature_b, elevation) + 0.0);

    // combine altitude with latitude temperature
    float temperature_altitude = (temperature_latitude - temperature_altitude_subtract);

    // combine altitude and latitude temperature with random temperature
    temperature_altitude += temperature_noise * 20;

    // clamp sea temperature
    if (!at_sea_level)
        temperature_altitude = std::clamp(temperature_altitude, m_settings.m_min_temperature, m_settings.m_max_temperature);
    else
        temperature_altitude = std::clamp(temperature_altitude, m_settings.m_min_temperature, m_settings.m_max_temperature_at_sea);

    /// transform temperature in a range from 0 to 1 which represents -100C and 100C
    temperature_altitude = (temperature_altitude * 0.01) / 2 + 0.5;

    float temperature = temperature_altitude;
    temperature = std::clamp(temperature, 0.0f, 1.0f);

    return temperature;
}

Biome* MapGenerator::get_biome_from(float x, float y, bool use_map_data = true){
    float elevation, temperature, humidity;

    if (use_map_data){
        elevation = height_to_elevation(m_height_map[x][y]);
        temperature = temperature_to_celcius(m_temperature_map[x][y]);
        humidity = m_humidity_map[x][y];
    }
    else{
        elevation = height_to_elevation(get_height_from(x, y));
        temperature = temperature_to_celcius(get_temperature_from(x, y, false));
        humidity = get_humidity_from(x, y, false);
    }

    for (Biome& biome : m_biomes){
        if (
            biome.height_range.fits(elevation) && 
            biome.temperature_range.fits(temperature) &&
            biome.humidity_range.fits(humidity)
        ){
            return &biome;
            break;
        }
    }
    return nullptr;
}

float MapGenerator::get_humidity_from(float x, float y, bool use_map_data = true){
    float humidity_noise = m_noise.GetNoise(x * m_settings.m_humidity_noise_scale, y * m_settings.m_humidity_noise_scale);
    humidity_noise = ((humidity_noise + 1) / 2) * 100;
    float temperature;

    if (use_map_data){
        temperature = temperature_to_celcius(m_temperature_map[x][y]);
    }
    else{
        temperature = temperature_to_celcius(get_temperature_from(x,y));
    }

    // how much more temperature, more water the air can hold, thus the relative humidity decreases
    float relative_humidity;

    relative_humidity = humidity_noise - (temperature * m_settings.m_humidity_temperature_increment);

    // if at sea level
    if (height_to_elevation(get_height_from(x, y)) <= m_settings.m_sea_level){
        // humidity is 100%
        relative_humidity = 100;
    }

    relative_humidity = std::clamp(relative_humidity, 2.0f, 100.0f);

    return relative_humidity;
}

core::graphics::Color MapGenerator::get_color_from(float x, float y, draw_modes draw_mode, bool use_map_data = true){
    core::graphics::Color color;

    if (draw_mode == draw_modes::COLOR_MAP){
        Biome* biome_xy;

        if (use_map_data){
            biome_xy = m_biome_map[x][y];
        }
        else{
            biome_xy = get_biome_from(x, y);
        }
        
        if (biome_xy){
            color = biome_xy->color;
        }
        else{
            color = core::graphics::Color::red();
        }
    }

    else if (draw_mode == draw_modes::NOISE_MAP){
        float noise_value;

        if (use_map_data){
            noise_value = m_height_map[x][y];
        }
        else{
            noise_value = get_height_from(x, y);
        }

        if (m_settings.m_use_fallof_map){
            noise_value -=  get_fallof_map_from(x, y);
        }

        float normalized_value = (noise_value + 1) / 2;

        if (normalized_value < 0 ){
            normalized_value = 0;
        }

        color = core::graphics::Color(255 * normalized_value, 255 * normalized_value, 255 * normalized_value, 255 );
    }
    
    else if (draw_mode == draw_modes::TEMPERATURE_MAP){
        float temperature_value;

        if (use_map_data){
            temperature_value = m_temperature_map[x][y];
        }
        else{
            temperature_value = get_temperature_from(x, y);
        }

        // invert so the coldest parts turn white
        temperature_value = (temperature_value - 1) * -1;
        color = core::graphics::Color(255 * temperature_value, 255 * temperature_value, 255 * temperature_value, 255 );
    }

    else if (draw_mode == draw_modes::HUMIDITY_MAP){
        float humidity_value;

        if (use_map_data){
            humidity_value = m_humidity_map[x][y];
        }
        else{
            humidity_value = get_humidity_from(x, y);
        }

        // transform in a range of 0 to 1
        humidity_value = humidity_value * 0.01;
        color = core::graphics::Color(255 *humidity_value , 255 * humidity_value, 255 * humidity_value, 255);

    }

    else if (draw_mode == draw_modes::FALLOF_MAP){
        float noise_value = get_fallof_map_from(x, y);
        float normalized_value = noise_value;

        if (normalized_value < 0 ){
            normalized_value = 0;
        }

        if (normalized_value > 1){
            normalized_value = 1;
        }

        color = core::graphics::Color(255 * normalized_value, 255 * normalized_value, 255 * normalized_value, 255 );
    }

    return color;
}

void MapGenerator::generate_heigh_map(){
    for (int y = 0; y < m_settings.m_size_y; y++){
        for (int x = 0; x < m_settings.m_size_x; x++){
            m_height_map[x][y] = get_height_from((float)x, (float)y);
        }
    }
}

void MapGenerator::generate_sprite(draw_modes draw_mode){
    core::graphics::Image image(
        core::Vector2u(m_settings.m_size_x, m_settings.m_size_y), core::graphics::Color::red()
    );

    for (int y = 0; y < m_settings.m_size_y; y++){
        for (int x = 0; x < m_settings.m_size_x; x++){
            core::graphics::Color color = get_color_from(x, y, draw_mode, true);
            image.set_pixel(core::Vector2u(x,y), color);
        }
    }

    m_texture.load(image);
    m_sprite.set_texture(m_texture);
}

void MapGenerator::generate_fallof_map(){
    for (int x= 0; x < m_settings.m_size_x; x++){
        for (int y = 0; y < m_settings.m_size_y; y++){
            m_fallof_map[x][y] = get_fallof_map_from(x, y);
        }
    }
}

float MapGenerator::evaluate_fallof_map(float value){
    value = (value * 2) -1;
    return (
        pow(value, m_settings.m_fallof_a) / 
        (
            pow(value, m_settings.m_fallof_a) + 
            pow(m_settings.m_fallof_b - m_settings.m_fallof_b*value, m_settings.m_fallof_a)
        )
    );
}

float MapGenerator::evaluate_latitude(float value){
    return (
        pow(value, m_settings.m_latitude_a) / 
        (
            pow(value, m_settings.m_latitude_a) + 
            pow(m_settings.m_latitude_b-m_settings.m_latitude_b*value, m_settings.m_latitude_a)
        )
    );
}

void MapGenerator::generate_temperature_map(){
    // temperature variation from poles to equator
    for (auto x = 0; x < m_settings.m_size_x; x++){
        for (auto y = 0; y < m_settings.m_size_y; y++){
            m_temperature_map[x][y] = get_temperature_from(x, y, true);
        }
    }
}

float MapGenerator::temperature_to_celcius(float temperature){
    return (temperature * 200.0) - 100.0;
}

float MapGenerator::height_to_elevation(float height){
    float elevation = (((height + 1) / 2) * m_settings.m_max_elevation) - m_settings.m_min_elevation;
    elevation = std::clamp(elevation, m_settings.m_min_elevation, m_settings.m_max_elevation);
    return elevation;
}

void MapGenerator::generate_biome_map(){
    for (int x = 0; x < m_settings.m_size_x; x++){
        for (int y = 0; y < m_settings.m_size_y; y++){
            m_biome_map[x][y] = get_biome_from(x, y);
        }
    }
}

int MapGenerator::get_last_biome_index(){
    int last_index = 0;

    for (Biome& biome : m_biomes){
        if (biome.index > last_index){
            last_index = biome.index;
        }
    }
    return last_index;
}

Biome* MapGenerator::get_biome_by_index(int index){
    return &m_biomes[index];
}

void MapGenerator::add_biome(Biome biome){
    biome.index = get_last_biome_index() + 1;
    m_biomes.push_back(biome);
}

void MapGenerator::generate_humidity_map(){
    for (int x = 0; x < m_settings.m_size_x; x++){
        for (int y = 0; y < m_settings.m_size_y; y++){
            m_humidity_map[x][y] = get_humidity_from(x,y);
        }
    }
}

/* Json conversions */
bool compareBiomesIndex(Biome b1, Biome b2){
    return (b1.index < b2.index);
}



#include <Eruptor/resource_manager.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <Eruptor/resource/stb_image.h>
#include <Eruptor/hardware/resources/resource_manager.hpp>
#include <Eruptor/event/event_manager.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <print>

eruptor::resource::Resource_manager::Resource_manager(): event_manager{ event::event_manager }
{

}

void eruptor::resource::Resource_manager::Init(hardware::Resource_manager & hw_resource_manager)
{
    this->hw_resource_manager = &hw_resource_manager;

    hardware::Texture_data tex_data{};
    tex_data.pixels = stbi_load("../../textures/nothing.png", &tex_data.width, &tex_data.height, &tex_data.tex_chanels, STBI_rgb_alpha);

    if(!tex_data.pixels)
    {
        throw std::runtime_error{"failed to load nothing texture!"};
    }

    tex_data.format = vk::Format::eR8G8B8A8Srgb;

    textures_handles.push_back( Texture_handle{ hw_resource_manager.Stage_texture_data( tex_data ) } );
    stbi_image_free( tex_data.pixels );

    if(FT_Init_FreeType(&free_type))
    {
        throw std::runtime_error{"Failed to init FreeType."};
    }

    event_manager.Add_listener( *this );
}

eruptor::resource::Model & eruptor::resource::Resource_manager::Get_model(Model_handle & model_handle)
{
    return models[ model_handle.Get_id() ];
}

eruptor::physic::AABB eruptor::resource::Resource_manager::Get_model_aabb(Model_handle & model_handle)
{
    return models_AABB[ model_handle.Get_id() ];
}

eruptor::physic::Hitbox eruptor::resource::Resource_manager::Get_model_hitbox(Model_handle & model_handle)
{
    return models_hitboxes[ model_handle.Get_id() ];
}


eruptor::resource::Material eruptor::resource::Resource_manager::Get_material(Material_handle & material_handle)
{
    return materials[ material_handle.Get_id() ];
}

void eruptor::resource::Resource_manager::Add_model_alias(uint32_t model_id, const std::string & model_alias)
{
    models_aliases[ model_id ] = model_alias;
}

std::string_view eruptor::resource::Resource_manager::Get_model_alias(uint32_t model_id)
{
    return models_aliases[ model_id ];
}

eruptor::resource::Font_atlas & eruptor::resource::Resource_manager::Get_font_atlas(Font_handle & font_handle)
{
    return fonts_atlases[ font_handle.Get_id() ];
}

eruptor::resource::Model_handle eruptor::resource::Resource_manager::Get_model_handle(std::string_view model_alias)
{
    for(auto & [id, alias] : models_aliases)
    {
        if(alias == model_alias)
        {
            return Model_handle{id};
        }
    }

    throw std::runtime_error{ std::string{"No model with alias: "}.append(model_alias) };
}

eruptor::resource::Font_handle eruptor::resource::Resource_manager::Add_font_atlas(const std::filesystem::path & path, float font_size)
{
    for(auto i{0UZ}; i < fonts_atlases.size(); i++)
    {
        if(fonts_atlases[i].path == path)
        {
            Font_handle font_handle{ static_cast<uint32_t>(i) };
            return font_handle;
        }
    }

    Font_atlas atlas{};
    atlas.path = path;
    atlas.size = font_size;
    atlas.width = 1024;
    atlas.height = 1024;
    atlas.bitmap.resize(atlas.width * atlas.height);
    atlas.starus = Status::PENDING;

    fonts_atlases.push_back( atlas );

    Font_handle font_handle{ static_cast<uint32_t>(fonts_atlases.size() - 1) };

    return font_handle;
}

void eruptor::resource::Resource_manager::Load_font_atlases()
{
    for(auto & font_atlas : fonts_atlases)
    {
        Load_font( font_atlas );
        font_atlas.starus = Status::LODADED;
    }

    hw_resource_manager->Upload_data_to_GPU();
}

void eruptor::resource::Resource_manager::Load_font(Font_atlas & font_atlas)
{
    FT_Face face{};

    if(FT_New_Face(free_type, font_atlas.path.c_str(), 0, &face))
    {
        throw std::runtime_error{"Failed to load font"};
    }

    FT_Set_Pixel_Sizes(face, 0, static_cast<uint32_t>(font_atlas.size));

    std::fill(font_atlas.bitmap.begin(), font_atlas.bitmap.end(), 0);

    font_atlas.glyphs.clear();


    uint32_t x{};
    uint32_t y{};
    uint32_t row_height{};


    for(char32_t c{32}; c < 127; c++)
    {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            continue;
        }


        FT_GlyphSlot glyph = face->glyph;

        Glyph info{};

        info.size = {static_cast<int>(glyph->bitmap.width), static_cast<int>(glyph->bitmap.rows)};

        info.bearing = {glyph->bitmap_left, glyph->bitmap_top};

        info.advance = glyph->advance.x >> 6;


        if(glyph->bitmap.width == 0 || glyph->bitmap.rows == 0)
        {
            font_atlas.glyphs[c] = info;
            continue;
        }


        if(x + glyph->bitmap.width >= font_atlas.width)
        {
            x = 0;
            y += row_height + 2;
            row_height = 0;
        }



        if(y + glyph->bitmap.rows >= font_atlas.height)
        {
            FT_Done_Face(face);
            throw std::runtime_error{"Font atlas is too small"};
        }

        for(uint32_t row{}; row < glyph->bitmap.rows; row++)
        {
            for(uint32_t col{}; col < glyph->bitmap.width; col++)
            {
                font_atlas.bitmap[(y + row) * font_atlas.width + x + col] = glyph->bitmap.buffer[ row * glyph->bitmap.pitch + col ];
            }
        }

        info.uv_min = {static_cast<float>(x) / font_atlas.width, static_cast<float>(y) / font_atlas.height};

        info.uv_max = {static_cast<float>(x + glyph->bitmap.width) / font_atlas.width, static_cast<float>(y + glyph->bitmap.rows) / font_atlas.height};


        font_atlas.glyphs[c] = info;

        row_height = std::max(row_height, glyph->bitmap.rows);

        x += glyph->bitmap.width + 2;
    }

    FT_Done_Face(face);

    hardware::Texture_data texture_data{};

    texture_data.width = font_atlas.width;
    texture_data.height = font_atlas.height;
    texture_data.tex_chanels = 1;
    texture_data.pixels = font_atlas.bitmap.data();
    texture_data.format = vk::Format::eR8Unorm;

    Texture_handle tex_handle{hw_resource_manager->Stage_texture_data(texture_data)};

    font_atlas.texture_handle = tex_handle;
}

std::vector<eruptor::resource::Text_vertex_data> eruptor::resource::Resource_manager::Generate_text_vertices_data(std::string_view text, float start_x, float start_y, Font_handle font_handle, glm::u8vec4 color)
{
    auto & font_atlas = fonts_atlases[font_handle.Get_id()];

    std::vector<Text_vertex_data> vertices{};
    vertices.reserve(text.size() * 6);

    float x = start_x;
    float y = start_y;

    for(char c : text)
    {
        if(c < 32) continue;

        auto it = font_atlas.glyphs.find(c);

        if(it == font_atlas.glyphs.end()) continue;

        const Glyph & glyph = it->second;

        float x_pos = x + glyph.bearing.x;
        float y_pos = y - glyph.bearing.y;

        float w = glyph.size.x;
        float h = glyph.size.y;

        float x0 = x_pos;
        float y0 = y_pos;

        float x1 = x_pos + w;
        float y1 = y_pos + h;

        float s0 = glyph.uv_min.x;
        float t0 = glyph.uv_min.y;

        float s1 = glyph.uv_max.x;
        float t1 = glyph.uv_max.y;

        vertices.push_back({ {x0, y0}, {s0, t0}, color });
        vertices.push_back({ {x1, y0}, {s1, t0}, color });
        vertices.push_back({ {x1, y1}, {s1, t1}, color });

        vertices.push_back({ {x0, y0}, {s0, t0}, color });
        vertices.push_back({ {x1, y1}, {s1, t1}, color });
        vertices.push_back({ {x0, y1}, {s0, t1}, color });

        x += glyph.advance;
    }

    return vertices;
}

eruptor::resource::Model_handle eruptor::resource::Resource_manager::Add_model(const std::filesystem::path & path)
{
    auto it = std::ranges::find_if(models,
              [&path](const Model & model)
              {
                return model.path == path;
              });

    if(it != models.end())
    {
        return Model_handle{ static_cast<uint32_t>( it - models.begin() ) };
    }

    models.push_back({Status::PENDING, path});
    return Model_handle{ static_cast<uint32_t>(models.size() - 1) };
}

void eruptor::resource::Resource_manager::Load_models()
{
    size_t models_to_load_count{};
    for(auto & model : models)
    {
        if(model.status != Status::PENDING) continue;

        Load_model(model);
        models_to_load_count++;
    }

    if(models_to_load_count)
    {
        hw_resource_manager->Upload_data_to_GPU();
    }
}

void eruptor::resource::Resource_manager::Load_model(Model & model)
{
    Assimp::Importer importer{};
    const aiScene * scene = importer.ReadFile(model.path, aiProcess_Triangulate | aiProcess_PreTransformVertices);

    std::vector<glm::vec3> all_vertecies{};

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        model.status = Status::ERROR;
        throw std::runtime_error{" ERROR::REOUURCE::RESOURCE_MANAGER::Failed to load model."};
    }
    auto directory = model.path.parent_path();

    Process_node(scene->mRootNode, scene, model, directory, all_vertecies);

    Calculate_model_hitbox(model, all_vertecies);
    model.status = Status::LODADED;
}

void eruptor::resource::Resource_manager::Process_node(aiNode* node, const aiScene* scene, Model& model, const std::filesystem::path & directory, std::vector<glm::vec3> & all_vertecies)
{
    for(auto i{0u}; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        Process_mesh(mesh, scene, model, directory, all_vertecies);
    }

    for(auto i{0u}; i < node->mNumChildren; i++)
    {
        Process_node(node->mChildren[i], scene, model, directory, all_vertecies);
    }
}

void eruptor::resource::Resource_manager::Process_mesh(aiMesh* mesh, const aiScene* scene, Model& model, const std::filesystem::path & directory, std::vector<glm::vec3> & all_vertecies)
{
    hardware::Mesh_data mesh_data{};
    if(mesh->mMaterialIndex >= 0)
    {
        Material material{};
        aiMaterial * ai_material = scene->mMaterials[mesh->mMaterialIndex];

        material.diffuse_texture_handle = Load_material_texture(ai_material, aiTextureType_DIFFUSE, Texture_type::DIFFUSE, directory);
        material.specular_texture_handle = Load_material_texture(ai_material, aiTextureType_SPECULAR, Texture_type::SPECULAR, directory);

        this->materials.push_back(material);
        model.materials_handles.emplace_back( static_cast<uint32_t>(materials.size() - 1) ) ;
        mesh_data.material_id = materials.size() - 1;
    }
    else
    {
        mesh_data.material_id = 0;
    }

    for(auto i{0u}; i < mesh->mNumVertices; i++)
    {
        hardware::Opaque_vertex vertex{};

        glm::vec3 vector{};

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.pos = vector;

        all_vertecies.push_back(vertex.pos);

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normals = vector;

        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec{};
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_cord = vec;
        }
        else
        {
            vertex.texture_cord = glm::vec2{0.0f, 0.0f};
        }

        mesh_data.vertecies.push_back( vertex );
    }


    for(auto i{0u}; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(auto j{0u}; j < face.mNumIndices; j++)
        {
            mesh_data.indices.push_back( face.mIndices[j] );
        }
    }

    Mesh_handle mesh_handle{ hw_resource_manager->Stage_mesh_data( mesh_data ) };
    mesh_handles.push_back(mesh_handle);
    model.Meshes_handles.push_back( mesh_handle );
}

eruptor::resource::Texture_handle eruptor::resource::Resource_manager::Load_material_texture(aiMaterial* mat, aiTextureType ai_type, Texture_type type, const std::filesystem::path & directory)
{
    hardware::Texture_data tex_data{};

    aiString str{};
    if(mat->GetTexture(ai_type, 0, &str) != AI_SUCCESS)
    {
        return Texture_handle{0};
    }
    tex_data.format = (type == Texture_type::DIFFUSE)? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8Unorm;

    if(str.Empty())
    {
        return Texture_handle{0};
    }

    int loaded_chanels = (type == Texture_type::DIFFUSE)? STBI_rgb_alpha : STBI_grey;

    tex_data.pixels = stbi_load( (directory / std::filesystem::path{str.C_Str()}.filename()).string().c_str()  , &tex_data.width, &tex_data.height, &tex_data.tex_chanels, loaded_chanels);

    if(!tex_data.pixels)
    {
        throw std::runtime_error{"failed to load texture image!"};
    }

    tex_data.tex_chanels = (type == Texture_type::DIFFUSE)? 4 : 1;

    Texture_handle tex_handle{ hw_resource_manager->Stage_texture_data(tex_data)};
    textures_handles.push_back(tex_handle);

    stbi_image_free(tex_data.pixels);

    return tex_handle;
}

void eruptor::resource::Resource_manager::Calculate_model_hitbox(Model & model, std::vector<glm::vec3> & all_vertecies)
{
    physic::AABB aabb{glm::vec3{std::numeric_limits<float>::max()}, glm::vec3{std::numeric_limits<float>::lowest()}};
    physic::Hitbox hitbox{};

    for(const auto & vert : all_vertecies)
    {
        aabb.min = glm::min(vert, aabb.min);
        aabb.max = glm::max(vert, aabb.max);
    }

    if(model.hitbox_type == Hitbox_type::OBB)
    {
        physic::OBB_hitbox oob_hitbox{};

        Calculate_obb_hitbox(oob_hitbox, all_vertecies);

        hitbox = oob_hitbox;
    }
    else if(model.hitbox_type == Hitbox_type::SPHERE)
    {
        physic::Sphere_hitbox sphere_hitbox{};

        Calculate_sphere_hitbox(sphere_hitbox, all_vertecies);

        hitbox = sphere_hitbox;
    }
    else if(model.hitbox_type ==  Hitbox_type::CAPSULE)
    {
        physic::Capsule_hitbox capsule_hitbox{};
        Calculate_capsule_hitbox(capsule_hitbox, all_vertecies);
        hitbox = capsule_hitbox;
    }

    models_AABB.push_back(aabb);
    models_hitboxes.push_back(hitbox);
}

void eruptor::resource::Resource_manager::Calculate_sphere_hitbox(physic::Sphere_hitbox & sphere, std::vector<glm::vec3> & all_vertecies)
{
    if (all_vertecies.empty()) return;

    glm::vec3 min_v = all_vertecies[0];
    glm::vec3 max_v = all_vertecies[0];

    for(const auto& v : all_vertecies)
    {
        min_v = glm::min(min_v, v);
        max_v = glm::max(max_v, v);
    }

    sphere.center = (min_v + max_v) * 0.5f;

    float max_dist_sq = 0.0f;
    for(const auto& v : all_vertecies)
    {
        glm::vec3 dis = v - sphere.center;
        float dist_sq = glm::dot(dis, dis);
        if(dist_sq > max_dist_sq)
        {
            max_dist_sq = dist_sq;
        }
    }

    sphere.radius = std::sqrt(max_dist_sq);
}

void eruptor::resource::Resource_manager::Calculate_obb_hitbox(physic::OBB_hitbox & obb, std::vector<glm::vec3> & all_vertecies)
{
    glm::vec3 centroid{};
    glm::mat3 cov = Compute_covariance(all_vertecies, centroid);
    glm::mat3 eigen_vectors = Jacobi_eigenvectors(cov);

    glm::vec3 axis_x = glm::normalize(glm::vec3{eigen_vectors[0]});
    glm::vec3 axis_y = glm::normalize(glm::vec3{eigen_vectors[1]});
    glm::vec3 axis_z = glm::normalize(glm::vec3{eigen_vectors[2]});

    glm::vec3 min_p{ std::numeric_limits<float>::max() };
    glm::vec3 max_p{ std::numeric_limits<float>::lowest() };

    for(const auto vert : all_vertecies)
    {
        glm::vec3 dis = vert - centroid;
        glm::vec3 local{ glm::dot(dis, axis_x), glm::dot(dis, axis_y), glm::dot(dis, axis_z) };

        min_p = glm::min(min_p, local);
        max_p = glm::max(max_p, local);
    }

    obb.half_width = (max_p - min_p) * 0.5f;
    glm::vec3 local_center = (max_p + min_p) * 0.5f;

    obb.center = centroid + axis_x * local_center.x + axis_y * local_center.y + axis_z * local_center.z;
    obb.axies[0] = axis_x;
    obb.axies[1] = axis_y;
    obb.axies[2] = axis_z;
}

void eruptor::resource::Resource_manager::Calculate_capsule_hitbox(physic::Capsule_hitbox & capsule, std::vector<glm::vec3> & all_vertecies)
{
    if (all_vertecies.empty()) return;

    glm::vec3 centroid{};
    glm::mat3 cov = Compute_covariance(all_vertecies, centroid);
    glm::mat3 eigen_vectors = Jacobi_eigenvectors(cov);

    // 1. Sprawdź wszystkie 3 osie i wybierz tę, która daje największą rozpiętość (najdłuższy wymiar modelu)
    glm::vec3 best_axis{1.0f, 0.0f, 0.0f};
    float max_span = -1.0f;
    float best_min_proj = 0.0f;
    float best_max_proj = 0.0f;

    for (int i = 0; i < 3; ++i)
    {
        glm::vec3 axis = glm::normalize(glm::vec3{eigen_vectors[i]});
        if (glm::dot(axis, axis) < 1e-4f) continue;

        float min_p = std::numeric_limits<float>::max();
        float max_p = std::numeric_limits<float>::lowest();

        for (const auto & vert : all_vertecies)
        {
            float proj = glm::dot(vert - centroid, axis);
            min_p = std::min(min_p, proj);
            max_p = std::max(max_p, proj);
        }

        float span = max_p - min_p;
        if (span > max_span)
        {
            max_span = span;
            best_axis = axis;
            best_min_proj = min_p;
            best_max_proj = max_p;
        }
    }

    // 2. Ustaw faktyczne punkty początkowe i końcowe kapsuły na pełnym zakresie modelu (bez sztucznego obcinania paddingiem)
    capsule.start = centroid + best_axis * best_min_proj;
    capsule.end = centroid + best_axis * best_max_proj;

    // 3. Oblicz promień jako maksymalną odległość wierzchołka od odcinka [start, end]
    float max_dist_sq = 0.0f;
    glm::vec3 ba = capsule.end - capsule.start;
    float ba_len_sq = glm::dot(ba, ba);

    for (const auto & vert : all_vertecies)
    {
        glm::vec3 pa = vert - capsule.start;
        float t = (ba_len_sq > 1e-6f) ? glm::dot(pa, ba) / ba_len_sq : 0.0f;
        t = glm::clamp(t, 0.0f, 1.0f);

        glm::vec3 closest_point = capsule.start + t * ba;
        float dist_sq = glm::dot(vert - closest_point, vert - closest_point);

        if (dist_sq > max_dist_sq)
        {
            max_dist_sq = dist_sq;
        }
    }

    capsule.radius = std::sqrt(max_dist_sq);
}

glm::mat3 eruptor::resource::Resource_manager::Compute_covariance(const std::vector<glm::vec3>& all_vertecies, glm::vec3& centroid)
{
    centroid = glm::vec3{};
    for(const auto & vert : all_vertecies)
    {
        centroid += vert;
    }
    centroid /= static_cast<float>(all_vertecies.size());

    glm::mat3 cov{};
    for(const auto & vert : all_vertecies)
    {
        glm::vec3 d = vert - centroid;
        cov[0][0] += d.x * d.x;
        cov[0][1] += d.x * d.y;
        cov[0][2] += d.x * d.z;
        cov[1][1] += d.y * d.y;
        cov[1][2] += d.y * d.z;
        cov[2][2] += d.z * d.z;
    }
    cov[1][0] = cov[0][1];
    cov[2][0] = cov[0][2];
    cov[2][1] = cov[1][2];

    cov /= static_cast<float>(all_vertecies.size());
    return cov;
}

glm::mat3 eruptor::resource::Resource_manager::Jacobi_eigenvectors(glm::mat3 & cov, size_t iterations)
{
    glm::mat3 v{1.0f};

    for(auto iter{0UZ}; iter < iterations; iter++)
    {
        int p{0}, q{1};
        float max_val = std::abs(cov[0][1]);
        if(std::abs(cov[0][2]) > max_val) {max_val = std::abs(cov[0][2]); p = 0; q = 2;}
        if(std::abs(cov[1][2]) > max_val) {max_val = std::abs(cov[1][2]); p = 1; q = 2;}

        if(max_val < 1e-8f) break;

        float theta = (cov[q][q] - cov[p][p]) / (2.0f * cov[p][q]);
        float t = glm::sign(theta) / (std::abs(theta) + std::sqrt(theta * theta + 1.0f));
        float c = 1.0f / std::sqrt(t * t + 1.0f);
        float s = t * c;

        glm::mat3 rot{1.0f};
        rot[p][p] = c;
        rot[q][q] = c;
        rot[q][p] = s;
        rot[p][q] = -s;

        cov = glm::transpose(rot) * cov * rot;
        v = v * rot;
    }

    return v;
}

void eruptor::resource::Resource_manager::On_event([[maybe_unused]] const event::Event & event)
{

}

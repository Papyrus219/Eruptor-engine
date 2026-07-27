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
#include <print>

namespace
{
    glm::mat4 Convert_matrix(const aiMatrix4x4 & m)
    {
        return glm::transpose(glm::make_mat4(&m.a1));
    }
}

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
        hardware::Vertex vertex{};

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

    // int min_id{}, max_id{}, min_x_id{}, min_y_id{}, min_z_id{}, max_x_id{}, max_y_id{}, max_z_id{};
    // for(auto i{0UZ}; i < all_vertecies.size(); i++)
    // {
    //     if(all_vertecies[i].x < all_vertecies[min_x_id].x) min_x_id = i;
    //     if(all_vertecies[i].y < all_vertecies[min_y_id].y) min_y_id = i;
    //     if(all_vertecies[i].z < all_vertecies[min_z_id].z) min_z_id = i;
    //     if(all_vertecies[i].x > all_vertecies[max_x_id].x) max_x_id = i;
    //     if(all_vertecies[i].y > all_vertecies[max_y_id].y) max_y_id = i;
    //     if(all_vertecies[i].z > all_vertecies[max_z_id].z) max_z_id = i;
    // }
    //
    // float dist_x_squared = glm::dot(all_vertecies[max_x_id] - all_vertecies[min_x_id], all_vertecies[max_x_id] - all_vertecies[min_x_id]);
    // float dist_y_squared = glm::dot(all_vertecies[max_y_id] - all_vertecies[min_y_id], all_vertecies[max_y_id] - all_vertecies[min_y_id]);
    // float dist_z_squared = glm::dot(all_vertecies[max_z_id] - all_vertecies[min_z_id], all_vertecies[max_z_id] - all_vertecies[min_z_id]);
    //
    // min_id = min_x_id;
    // max_id = max_x_id;
    // if(dist_y_squared > dist_x_squared && dist_y_squared > dist_z_squared)
    // {
    //     min_id = min_y_id;
    //     max_id = max_y_id;
    // }
    // if(dist_z_squared > dist_x_squared && dist_z_squared > dist_y_squared)
    // {
    //     min_id = min_z_id;
    //     max_id = max_z_id;
    // }
    //
    // sphere.center = (all_vertecies[min_id] + all_vertecies[max_id]) * 0.5f;
    // sphere.radius = std::sqrt( glm::dot(all_vertecies[max_id] - sphere.center, all_vertecies[max_id] - sphere.center) );
    //
    // for(auto i{0UZ}; i < all_vertecies.size(); i++)
    // {
    //     glm::vec3 dis = all_vertecies[i] - sphere.center;
    //     float distance_squared = glm::dot(dis, dis);
    //
    //     if(distance_squared > sphere.radius * sphere.radius)
    //     {
    //         float distance = std::sqrt( distance_squared );
    //         float new_radius = (sphere.radius + distance) * 0.5f;
    //         float k = (new_radius - sphere.radius) / distance;
    //
    //         sphere.radius = new_radius;
    //         sphere.center += dis * k;
    //     }
    // }
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


void eruptor::resource::Resource_manager::On_event(const event::Event & event)
{

}

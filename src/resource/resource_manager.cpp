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

eruptor::physic::AABB eruptor::resource::Resource_manager::Get_model_aabb(Model_handle& model_handle)
{
    return models_AABB[ model_handle.Get_id() ];
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
    for(auto & model : models)
    {
        Load_model(model);
    }

    hw_resource_manager->Upload_data_to_GPU();
}

void eruptor::resource::Resource_manager::Load_model(Model & model)
{
    Assimp::Importer importer{};
    const aiScene * scene = importer.ReadFile(model.path, aiProcess_Triangulate | aiProcess_PreTransformVertices);

    physic::AABB aabb{};

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        model.status = Status::ERROR;
        throw std::runtime_error{" ERROR::REOUURCE::RESOURCE_MANAGER::Failed to load model."};
    }
    auto directory = model.path.parent_path();

    Process_node(scene->mRootNode, scene, model, directory, glm::mat4(1.0f), aabb);
    model.status = Status::LODADED;
    models_AABB.push_back(aabb);

    std::print(std::clog, "Model: {}\n AABB.min: {} {} {} AABB.max: {} {} {}\n", model.path.string(), aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
}

void eruptor::resource::Resource_manager::Process_node(aiNode* node, const aiScene* scene, Model& model, const std::filesystem::path & directory, const glm::mat4 & parent_transform, physic::AABB & aabb)
{
    glm::mat4 global_transform = parent_transform * Convert_matrix(node->mTransformation);

    for(auto i{0u}; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        Process_mesh(mesh, scene, model, directory, global_transform, aabb);
    }

    for(auto i{0u}; i < node->mNumChildren; i++)
    {
        Process_node(node->mChildren[i], scene, model, directory, global_transform, aabb);
    }
}

void eruptor::resource::Resource_manager::Process_mesh(aiMesh* mesh, const aiScene* scene, Model& model, const std::filesystem::path & directory, const glm::mat4 & transform, physic::AABB & aabb)
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
        //vertex.pos = glm::vec3(transform * glm::vec4(vector, 1.0f));
        vertex.pos = vector;

        aabb.min = glm::min(aabb.min, vertex.pos);
        aabb.max = glm::max(aabb.max, vertex.pos);

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

void eruptor::resource::Resource_manager::On_event(const event::Event & event)
{

}

#include <Eruptor/physic/hitbox_calculator.hpp>

using namespace eruptor::physic;

void eruptor::physic::Hitbox_calculator::Calculate_sphere_hitbox(physic::Sphere_hitbox & sphere, std::vector<glm::vec3> & all_vertecies)
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

void eruptor::physic::Hitbox_calculator::Calculate_obb_hitbox(physic::OBB_hitbox & obb, std::vector<glm::vec3> & all_vertecies)
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

void eruptor::physic::Hitbox_calculator::Calculate_capsule_hitbox(physic::Capsule_hitbox & capsule, std::vector<glm::vec3> & all_vertecies)
{
    if (all_vertecies.empty()) return;

    glm::vec3 centroid{};
    glm::mat3 cov = Compute_covariance(all_vertecies, centroid);
    glm::mat3 eigen_vectors = Jacobi_eigenvectors(cov);

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

    capsule.start = centroid + best_axis * best_min_proj;
    capsule.end = centroid + best_axis * best_max_proj;

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

glm::mat3 eruptor::physic::Hitbox_calculator::Compute_covariance(const std::vector<glm::vec3>& all_vertecies, glm::vec3& centroid)
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

glm::mat3 eruptor::physic::Hitbox_calculator::Jacobi_eigenvectors(glm::mat3 & cov, size_t iterations)
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

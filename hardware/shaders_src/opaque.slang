struct VSInput
{
    [[vk::location(0)]] float3 in_position;
    [[vk::location(1)]] float3 normals;
    [[vk::location(2)]] float2 in_tex_coord;
};

struct VertexOutput
{
    float4 pos: SV_Position;
    [[vk::location(0)]] float3 world_pos;
    [[vk::location(1)]] float3 normals;
    [[vk::location(2)]] float2 frag_tex_coord;
};

struct Unifrom_buffer_vp
{
    float4x4 view;
    float4x4 projection;
    float3 view_position;
};
[[vk::binding(0, 0)]]
ConstantBuffer<Unifrom_buffer_vp> ub_vp;

struct Direction_caster
{
    float4 dirr;

    float4 ambient;
    float4 diffuse;
    float4 specular;
};

struct Point_caster
{
    float3 position;

    float3 ambient;
    float constant;
    float3 diffuse;
    float linear;
    float3 specular;
    float quadratic;
};

struct Uniform_buffer_light
{
    Direction_caster direction_caster;
    Point_caster point_casters[16];
    uint active_point_casters_count;
};
[[vk::binding(0, 1)]]
ConstantBuffer<Uniform_buffer_light> ub_light;

struct Push_constants
{
    float4x4 model;
    float4 skin_color;
};
[[vk::push_constant]]
Push_constants pc;

[shader("vertex")]
VertexOutput Vert_main(VSInput input)
{
    VertexOutput output;

    float4 world_pos = mul(pc.model, float4(input.in_position, 1.0));
    output.world_pos = world_pos.xyz;

    output.pos = mul(ub_vp.projection, mul(ub_vp.view, world_pos));
    output.frag_tex_coord = input.in_tex_coord;
    output.normals = input.normals;
    return output;
}

[[vk::binding(0, 2)]]
Sampler2D diffuse_texture;

[shader("fragment")]
float4 Frag_main(VertexOutput invVert): SV_Target
{
    float3 norm = normalize( invVert.normals );
    float3 view_dirr = normalize(ub_vp.view_position - invVert.world_pos);

    float3 result = float3(0.0, 0.0, 0.0);;
    result += Calculate_dirrection_light(ub_light.direction_caster, norm, view_dirr, invVert.frag_tex_coord);

    for(int i = 0; i < ub_light.active_point_casters_count; i++)
    {
        result += Calculate_point_light(ub_light.point_casters[i], norm, invVert.world_pos, view_dirr, invVert.frag_tex_coord);
    }

    return float4( result, 1.0 );
}

float3 Calculate_dirrection_light(Direction_caster dirr_caster, float3 normal, float3 view_dirr, float2 tex_cords)
{
    float3 light_dirr = normalize(-dirr_caster.dirr.xyz);

    float diff = max( dot(normal, light_dirr), 0.0 );

    float3 reflect_dirr = reflect(-light_dirr, normal);
    float spec = pow( max( dot(view_dirr, reflect_dirr), 0.0 ), 64 );

    float3 diffuse_tex = diffuse_texture.Sample(tex_cords).rgb * pc.skin_color.rgb;

    float3 ambient = dirr_caster.ambient.xyz * diffuse_tex;
    float3 diffuse = dirr_caster.diffuse.xyz * diff * diffuse_tex;
    float3 specular = dirr_caster.specular.xyz * spec;

    return ambient + diffuse + specular;
}

float3 Calculate_point_light(Point_caster point_caster, float3 normal, float3 frag_pos, float3 view_dirr, float2 tex_cords)
{
    float3 light_dirr = normalize(point_caster.position - frag_pos);
    float3 halfway_dirr = normalize(light_dirr + view_dirr);

    float diff = max(dot(normal, light_dirr), 0.0);

    float3 reflect_dirr = reflect(-light_dirr, normal);
    float spec = pow( max(dot(normal, halfway_dirr), 0.0), 64 );

    float distance = length(point_caster.position - frag_pos);
    float attenuation = 1.0 / (point_caster.constant + point_caster.linear * distance + point_caster.quadratic * (distance * distance));

    float3 diffuse_tex = diffuse_texture.Sample(tex_cords).rgb * pc.skin_color.rgb;

    float3 ambient = point_caster.ambient.xyz * diffuse_tex;
    float3 diffuse = point_caster.diffuse * diff * diffuse_tex;
    float3 specular = point_caster.specular.xyz * spec;

    return (ambient + diffuse + specular) * attenuation;
}






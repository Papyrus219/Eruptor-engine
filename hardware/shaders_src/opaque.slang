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
    float3 pos;

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

    return float4( Calculate_dirrection_light(ub_light.direction_caster, norm, view_dirr, invVert.frag_tex_coord), 1.0 );
}

float3 Calculate_dirrection_light(Direction_caster dirr_caster, float3 normal, float3 view_dirr, float2 tex_cords)
{
    float3 light_dirr = normalize(-dirr_caster.dirr.xyz);

    float diff = max( dot(normal, light_dirr), 0.0 );

    float3 reflect_dirr = reflect(-light_dirr, normal);
    float spec = pow( max( dot(view_dirr, reflect_dirr), 0.0 ), 32 );

    float3 diffuse_tex = diffuse_texture.Sample(tex_cords).rgb * pc.skin_color.rgb;

    float3 ambient = dirr_caster.ambient.xyz * diffuse_tex;
    float3 diffuse = dirr_caster.diffuse.xyz * diff * diffuse_tex;
    float3 specular = dirr_caster.specular.xyz * spec;

    return ambient + diffuse + specular;
}

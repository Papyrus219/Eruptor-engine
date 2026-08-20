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

struct Push_constants
{
    float4x4 model;
    float4 light_color;
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

[[vk::binding(0, 1)]]
Sampler2D diffuse_texture;

[shader("fragment")]
float4 Frag_main(VertexOutput invVert): SV_Target
{
    return float4( diffuse_texture.Sample(invVert.frag_tex_coord).rgb * pc.light_color.rgb, 1.0);
}

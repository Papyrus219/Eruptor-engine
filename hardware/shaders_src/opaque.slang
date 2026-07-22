struct VSInput
{
    float3 in_position;
    float3 normals;
    float2 in_tex_coord;
};

struct VertexOutput
{
    float4 pos: SV_Position;
    float3 normals;
    float2 frag_tex_coord;
};

struct Unifrom_buffer_vp
{
    float4x4 view;
    float4x4 projection;
};
[[vk::binding(0, 0)]]
ConstantBuffer<Unifrom_buffer_vp> ub_vp;

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
    output.pos = mul(ub_vp.projection, mul(ub_vp.view, mul(pc.model, float4(input.in_position, 1.0))));
    output.frag_tex_coord = input.in_tex_coord;
    output.normals = input.normals;
    return output;
}

[[vk::binding(0, 1)]]
Sampler2D texture;

[shader("fragment")]
float4 Frag_main(VertexOutput invVert): SV_Target
{
    float4 tex_color = texture.Sample(invVert.frag_tex_coord);

    return tex_color * pc.skin_color;
}

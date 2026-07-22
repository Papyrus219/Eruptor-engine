static const float3 edge_verts[24] =
{
    float3(0,0,0), float3(1,0,0),
    float3(1,0,0), float3(1,1,0),
    float3(1,1,0), float3(0,1,0),
    float3(0,1,0), float3(0,0,0),

    float3(0,0,1), float3(1,0,1),
    float3(1,0,1), float3(1,1,1),
    float3(1,1,1), float3(0,1,1),
    float3(0,1,1), float3(0,0,1),

    float3(0,0,0), float3(0,0,1),
    float3(1,0,0), float3(1,0,1),
    float3(1,1,0), float3(1,1,1),
    float3(0,1,0), float3(0,1,1),
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
    float3 aabb_min;
    float3 aabb_max;
    float4 color;
};
[[vk::push_constant]]
Push_constants pc;

struct VS_out
{
    float4 pos : SV_Position;
};

[shader("vertex")]
VS_out Vert_main(uint vid : SV_VertexID)
{
    VS_out out;

    float3 world_pos = lerp(pc.aabb_min, pc.aabb_max, edge_verts[vid]);
    float4 view_pos = mul(ub_vp.view, float4(world_pos, 1.0f));
    out.pos = mul(ub_vp.projection, view_pos);

    return out;
}

[shader("fragment")]
float4 Frag_main(): SV_Target
{
    return pc.color;
}

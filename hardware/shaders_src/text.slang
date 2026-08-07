struct VS_input
{
    float2 pos;
    float2 uv;
    float4 color;
};

struct VS_output
{
    float4 pos : SV_Position;
    float2 uv;
    float4 color;
};

struct Push_constants
{
    float2 screen_size;
};

[[vk::push_constant]]
Push_constants pc;

[shader("vertex")]
VS_output Vert_main(VS_input input)
{
    VS_output output;

    float2 ndc_pos = (input.pos / pc.screen_size) * 2.0f - float2(1.0, 1.0);

    output.pos = float4(ndc_pos, 0.0, 1.0);
    output.uv = input.uv;
    output.color = input.color;

    return output;
}

[[vk::binding(0, 0)]]
Sampler2D font_texture;

[shader("fragment")]
float4 Frag_main(VS_output input): SV_Target
{
    float alpha = font_texture.Sample(input.uv).r;

    if(alpha < 0.001) discard;

    return float4(input.color.rgb, input.color.a * alpha);
}

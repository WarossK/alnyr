struct VSIn
{
    float4 position : POSITION;
    float4 color : COLOR;

    float3 position_per_instance : RELATIVE_POS;
};

struct VSOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOut main(VSIn vsin)
{
    VSOut result;
 
    result.position = float4(vsin.position.xyz + vsin.position_per_instance, 1.0f);
    result.color = vsin.color;
 
    return result;
}
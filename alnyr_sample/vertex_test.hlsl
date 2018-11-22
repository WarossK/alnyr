struct VSOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOut main(float4 position : SV_POSITION, float4 color : COLOR, uint index : SV_InstanceId)
{
    VSOut result;
    
    result.position = position + float4(0.1f * index, 0.0f, 0.0f, 1.0f);
    result.color = color;
 
    return result;
}
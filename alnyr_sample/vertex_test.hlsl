struct VSOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOut main(float4 position : POSITION, float4 color : COLOR)
{
    VSOut result;
 
    result.position = position;
    result.color = color;
 
    return result;
}
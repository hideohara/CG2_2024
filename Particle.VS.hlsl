
#include "Particle.hlsli"

struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
};

struct ParticleForGPU
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4 color;
};

//ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
//StructuredBuffer<TransformationMatrix> gTransformationMatrices : register(t0);
StructuredBuffer<ParticleForGPU> gParticle : register(t0);


struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};

//VertexShaderOutput main(VertexShaderInput input)
VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    //output.position = mul(input.position, gTransformationMatrix.WVP);
    //output.position = mul(input.position, gTransformationMatrices[instanceId].WVP);
    output.position = mul(input.position, gParticle[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.color = gParticle[instanceId].color;
    return output;
}



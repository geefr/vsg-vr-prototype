#include <vsg/io/VSG.h>
#include <vsg/io/mem_stream.h>
static auto cube_red = []() {
std::istringstream str(
R"(#vsga 1.0.0
Root id=1 vsg::MatrixTransform
{
  userObjects 0
  children 1
  vsg::Node id=2 vsg::Group
  {
    userObjects 0
    children 2
    vsg::Node id=3 vsg::StateGroup
    {
      userObjects 0
      children 1
      vsg::Node id=4 vsg::VertexIndexDraw
      {
        userObjects 0
        firstBinding 0
        NumArrays 4
        Array id=5 vsg::vec3Array
        {
          userObjects 0
          properties 0 12 0 1 1 1 0 -1 0
          size 24
          storage id=0
          data 1 1 -1 1 1 -1 1 1 -1 1 -1 -1
           1 -1 -1 1 -1 -1 1 1 1 1 1 1
           1 1 1 1 -1 1 1 -1 1 1 -1 1
           -1 1 -1 -1 1 -1 -1 1 -1 -1 -1 -1
           -1 -1 -1 -1 -1 -1 -1 1 1 -1 1 1
           -1 1 1 -1 -1 1 -1 -1 1 -1 -1 1
        }
        Array id=6 vsg::vec3Array
        {
          userObjects 0
          properties 0 12 0 1 1 1 0 -1 0
          size 24
          storage id=0
          data 0 0 -1 0 1 -0 1 0 -0 0 -1 -0
           0 0 -1 1 0 -0 0 0 1 0 1 -0
           1 0 -0 0 -1 -0 0 0 1 1 0 -0
           -1 0 -0 0 0 -1 0 1 -0 -1 0 -0
           0 -1 -0 0 0 -1 -1 0 -0 0 0 1
           0 1 -0 -1 0 -0 0 -1 -0 0 0 1
        }
        Array id=7 vsg::vec2Array
        {
          userObjects 0
          properties 0 8 0 1 1 1 0 -1 0
          size 24
          storage id=0
          data 0.625 0.5 0.625 0.5 0.625 0.5 0.375 0.5 0.375 0.5 0.375 0.5
           0.625 0.25 0.625 0.25 0.625 0.25 0.375 0.25 0.375 0.25 0.375 0.25
           0.625 0.75 0.625 0.75 0.875 0.5 0.375 0.75 0.125 0.5 0.375 0.75
           0.625 1 0.625 0 0.875 0.25 0.375 1 0.125 0.25 0.375 0
        }
        Array id=8 vsg::vec4Value
        {
          userObjects 0
          properties 0 0 0 1 1 1 0 -1 0
          value 1 1 1 1
        }
        Indices id=9 vsg::ushortArray
        {
          userObjects 0
          properties 0 2 0 1 1 1 0 -1 0
          size 36
          storage id=0
          data 17 13 0 17 0 4 5 2 8 5 8 11
           16 3 9 16 9 22 10 6 19 10 19 23
           1 20 7 1 14 20 21 18 12 21 12 15
        }
        indexCount 36
        instanceCount 1
        firstIndex 0
        vertexOffset 0
        firstInstance 0
      }
      stateCommands 3
      vsg::StateCommand id=10 vsg::BindGraphicsPipeline
      {
        userObjects 0
        slot 0
        pipeline id=11 vsg::GraphicsPipeline
        {
          userObjects 0
          layout id=12 vsg::PipelineLayout
          {
            userObjects 0
            flags 0
            setLayouts 2
            descriptorLayout id=13 vsg::DescriptorSetLayout
            {
              userObjects 0
              bindings 1
              binding 10
              descriptorType 6
              descriptorCount 1
              stageFlags 16
            }
            descriptorLayout id=14 vsg::ViewDescriptorSetLayout
            {
              userObjects 0
            }
            pushConstantRanges 1
            stageFlags 1
            offset 0
            size 128
          }
          stages 2
          vsg::ShaderStage id=15 vsg::ShaderStage
          {
            userObjects 0
            stage 1
            entryPointName "main"
            module id=16 vsg::ShaderModule
            {
              userObjects 0
              hints id=17 vsg::ShaderCompileSettings
              {
                vulkanVersion 4194304
                clientInputVersion 100
                language 0
                defaultVersion 450
                target 65536
                forwardCompatible 0
                defines 0
              }
              source "#version 450
#extension GL_ARB_separate_shader_objects : enable

#pragma import_defines (VSG_INSTANCE_POSITIONS, VSG_DISPLACEMENT_MAP)

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 modelView;
} pc;

#ifdef VSG_DISPLACEMENT_MAP
layout(binding = 6) uniform sampler2D displacementMap;
#endif

layout(location = 0) in vec3 vsg_Vertex;
layout(location = 1) in vec3 vsg_Normal;
layout(location = 2) in vec2 vsg_TexCoord0;
layout(location = 3) in vec4 vsg_Color;

#ifdef VSG_INSTANCE_POSITIONS
layout(location = 4) in vec3 vsg_position;
#endif

layout(location = 0) out vec3 eyePos;
layout(location = 1) out vec3 normalDir;
layout(location = 2) out vec4 vertexColor;
layout(location = 3) out vec2 texCoord0;

layout(location = 5) out vec3 viewDir;

out gl_PerVertex{ vec4 gl_Position; };

void main()
{
    vec4 vertex = vec4(vsg_Vertex, 1.0);
    vec4 normal = vec4(vsg_Normal, 0.0);

#ifdef VSG_DISPLACEMENT_MAP
    // TODO need to pass as as uniform or per instance attributes
    vec3 scale = vec3(1.0, 1.0, 1.0);

    vertex.xyz = vertex.xyz + vsg_Normal * (texture(displacementMap, vsg_TexCoord0.st).s * scale.z);

    float s_delta = 0.01;
    float width = 0.0;

    float s_left = max(vsg_TexCoord0.s - s_delta, 0.0);
    float s_right = min(vsg_TexCoord0.s + s_delta, 1.0);
    float t_center = vsg_TexCoord0.t;
    float delta_left_right = (s_right - s_left) * scale.x;
    float dz_left_right = (texture(displacementMap, vec2(s_right, t_center)).s - texture(displacementMap, vec2(s_left, t_center)).s) * scale.z;

    // TODO need to handle different origins of displacementMap vs diffuseMap etc,
    float t_delta = s_delta;
    float t_bottom = max(vsg_TexCoord0.t - t_delta, 0.0);
    float t_top = min(vsg_TexCoord0.t + t_delta, 1.0);
    float s_center = vsg_TexCoord0.s;
    float delta_bottom_top = (t_top - t_bottom) * scale.y;
    float dz_bottom_top = (texture(displacementMap, vec2(s_center, t_top)).s - texture(displacementMap, vec2(s_center, t_bottom)).s) * scale.z;

    vec3 dx = normalize(vec3(delta_left_right, 0.0, dz_left_right));
    vec3 dy = normalize(vec3(0.0, delta_bottom_top, -dz_bottom_top));
    vec3 dz = normalize(cross(dx, dy));

    normal.xyz = normalize(dx * vsg_Normal.x + dy * vsg_Normal.y + dz * vsg_Normal.z);
#endif


#ifdef VSG_INSTANCE_POSITIONS
   vertex.xyz = vertex.xyz + vsg_position;
#endif

    gl_Position = (pc.projection * pc.modelView) * vertex;

    eyePos = (pc.modelView * vertex).xyz;

    vec4 lpos = /*vsg_LightSource.position*/ vec4(0.0, 0.0, 1.0, 0.0);
    viewDir = - (pc.modelView * vertex).xyz;
    normalDir = (pc.modelView * normal).xyz;

    vertexColor = vsg_Color;
    texCoord0 = vsg_TexCoord0;
}
"
              code 583
               119734787 65536 524298 79 0 131089 1 393227 1 1280527431 1685353262 808793134
               0 196622 0 1 983055 0 4 1852399981 0 12 20 29
               48 56 63 69 71 75 77 196611 2 450 589828 1096764487
               1935622738 1918988389 1600484449 1684105331 1868526181 1667590754 29556 262149 4 1852399981 0 262149
               9 1953654134 30821 327685 12 1600615286 1953654102 30821 262149 19 1836216174 27745
               327685 20 1600615286 1836216142 27745 393221 27 1348430951 1700164197 2019914866 0 393222
               27 0 1348430951 1953067887 7237481 196613 29 0 393221 33 1752397136 1936617283
               1953390964 115 393222 33 0 1785688688 1769235301 28271 393222 33 1 1701080941
               1701402220 119 196613 35 25456 262149 48 1348827493 29551 262149 54 1936683116
               0 262149 56 2003134838 7498052 327685 63 1836216174 1766091873 114 327685 69
               1953654134 1866692709 7499628 327685 71 1600615286 1869377347 114 327685 75 1131963764 1685221231
               48 393221 77 1600615286 1131963732 1685221231 48 262215 12 30 0 262215
               20 30 1 327752 27 0 11 0 196679 27 2 262216
               33 0 5 327752 33 0 35 0 327752 33 0 7
               16 262216 33 1 5 327752 33 1 35 64 327752 33
               1 7 16 196679 33 2 262215 48 30 0 262215 56
               30 5 262215 63 30 1 262215 69 30 2 262215 71
               30 3 262215 75 30 3 262215 77 30 2 131091 2
               196641 3 2 196630 6 32 262167 7 6 4 262176 8
               7 7 262167 10 6 3 262176 11 1 10 262203 11
               12 1 262187 6 14 1065353216 262203 11 20 1 262187 6
               22 0 196638 27 7 262176 28 3 27 262203 28 29
               3 262165 30 32 1 262187 30 31 0 262168 32 7
               4 262174 33 32 32 262176 34 9 33 262203 34 35
               9 262176 36 9 32 262187 30 39 1 262176 45 3
               7 262176 47 3 10 262203 47 48 3 458796 7 55
               22 22 14 22 262203 47 56 3 262203 47 63 3
               262203 45 69 3 262176 70 1 7 262203 70 71 1
               262167 73 6 2 262176 74 3 73 262203 74 75 3
               262176 76 1 73 262203 76 77 1 327734 2 4 0
               3 131320 5 262203 8 9 7 262203 8 19 7 262203
               8 54 7 262205 10 13 12 327761 6 15 13 0
               327761 6 16 13 1 327761 6 17 13 2 458832 7
               18 15 16 17 14 196670 9 18 262205 10 21 20
               327761 6 23 21 0 327761 6 24 21 1 327761 6
               25 21 2 458832 7 26 23 24 25 22 196670 19
               26 327745 36 37 35 31 262205 32 38 37 327745 36
               40 35 39 262205 32 41 40 327826 32 42 38 41
               262205 7 43 9 327825 7 44 42 43 327745 45 46
               29 31 196670 46 44 327745 36 49 35 39 262205 32
               50 49 262205 7 51 9 327825 7 52 50 51 524367
               10 53 52 52 0 1 2 196670 48 53 196670 54
               55 327745 36 57 35 39 262205 32 58 57 262205 7
               59 9 327825 7 60 58 59 524367 10 61 60 60
               0 1 2 262271 10 62 61 196670 56 62 327745 36
               64 35 39 262205 32 65 64 262205 7 66 19 327825
               7 67 65 66 524367 10 68 67 67 0 1 2
               196670 63 68 262205 7 72 71 196670 69 72 262205 73
               78 77 196670 75 78 65789 65592
            }
            NumSpecializationConstants 0
          }
          vsg::ShaderStage id=18 vsg::ShaderStage
          {
            userObjects 0
            stage 16
            entryPointName "main"
            module id=19 vsg::ShaderModule
            {
              userObjects 0
              hints id=17
              source "#version 450
#extension GL_ARB_separate_shader_objects : enable
#pragma import_defines (VSG_DIFFUSE_MAP, VSG_GREYSACLE_DIFFUSE_MAP, VSG_EMISSIVE_MAP, VSG_LIGHTMAP_MAP, VSG_NORMAL_MAP, VSG_METALLROUGHNESS_MAP, VSG_SPECULAR_MAP, VSG_TWO_SIDED_LIGHTING, VSG_WORKFLOW_SPECGLOSS, VSG_VIEW_LIGHT_DATA)

const float PI = 3.14159265359;
const float RECIPROCAL_PI = 0.31830988618;
const float RECIPROCAL_PI2 = 0.15915494;
const float EPSILON = 1e-6;
const float c_MinRoughness = 0.04;

#ifdef VSG_DIFFUSE_MAP
layout(binding = 0) uniform sampler2D diffuseMap;
#endif

#ifdef VSG_METALLROUGHNESS_MAP
layout(binding = 1) uniform sampler2D mrMap;
#endif

#ifdef VSG_NORMAL_MAP
layout(binding = 2) uniform sampler2D normalMap;
#endif

#ifdef VSG_LIGHTMAP_MAP
layout(binding = 3) uniform sampler2D aoMap;
#endif

#ifdef VSG_EMISSIVE_MAP
layout(binding = 4) uniform sampler2D emissiveMap;
#endif

#ifdef VSG_SPECULAR_MAP
layout(binding = 5) uniform sampler2D specularMap;
#endif

layout(binding = 10) uniform PbrData
{
    vec4 baseColorFactor;
    vec4 emissiveFactor;
    vec4 diffuseFactor;
    vec4 specularFactor;
    float metallicFactor;
    float roughnessFactor;
    float alphaMask;
    float alphaMaskCutoff;
} pbr;

layout(set = 1, binding = 0) uniform LightData
{
    vec4 values[64];
} lightData;

layout(location = 0) in vec3 eyePos;
layout(location = 1) in vec3 normalDir;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec2 texCoord0;
layout(location = 5) in vec3 viewDir;

layout(location = 0) out vec4 outColor;


// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct PBRInfo
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
    float VdotL;                  // cos angle between view direction and light direction
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};


vec4 SRGBtoLINEAR(vec4 srgbIn)
{
    vec3 linOut = pow(srgbIn.xyz, vec3(2.2));
    return vec4(linOut,srgbIn.w);
}

vec4 LINEARtoSRGB(vec4 srgbIn)
{
    vec3 linOut = pow(srgbIn.xyz, vec3(1.0 / 2.2));
    return vec4(linOut, srgbIn.w);
}

float rcp(const in float value)
{
    return 1.0 / value;
}

float pow5(const in float value)
{
    return value * value * value * value * value;
}

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 getNormal()
{
#ifdef VSG_NORMAL_MAP
    // Perturb normal, see http://www.thetenthplanet.de/archives/1180
    vec3 tangentNormal = texture(normalMap, texCoord0).xyz * 2.0 - 1.0;

    //tangentNormal *= vec3(2,2,1);

    vec3 q1 = dFdx(eyePos);
    vec3 q2 = dFdy(eyePos);
    vec2 st1 = dFdx(texCoord0);
    vec2 st2 = dFdy(texCoord0);

    vec3 N = normalize(normalDir);
    vec3 T = normalize(q1 * st2.t - q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
#else
    return normalize(normalDir);
#endif
}

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 BRDF_Diffuse_Lambert(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor * RECIPROCAL_PI;
}

vec3 BRDF_Diffuse_Custom_Lambert(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor * RECIPROCAL_PI * pow(pbrInputs.NdotV, 0.5 + 0.3 * pbrInputs.perceptualRoughness);
}

// [Gotanda 2012, \"Beyond a Simple Physically Based Blinn-Phong Model in Real-Time\"]
vec3 BRDF_Diffuse_OrenNayar(PBRInfo pbrInputs)
{
    float a = pbrInputs.alphaRoughness;
    float s = a;// / ( 1.29 + 0.5 * a );
    float s2 = s * s;
    float VoL = 2 * pbrInputs.VdotH * pbrInputs.VdotH - 1;		// double angle identity
    float Cosri = pbrInputs.VdotL - pbrInputs.NdotV * pbrInputs.NdotL;
    float C1 = 1 - 0.5 * s2 / (s2 + 0.33);
    float C2 = 0.45 * s2 / (s2 + 0.09) * Cosri * ( Cosri >= 0 ? 1.0 / max(pbrInputs.NdotL, pbrInputs.NdotV) : 1 );
    return pbrInputs.diffuseColor / PI * ( C1 + C2 ) * ( 1 + pbrInputs.perceptualRoughness * 0.5 );
}

// [Gotanda 2014, \"Designing Reflectance Models for New Consoles\"]
vec3 BRDF_Diffuse_Gotanda(PBRInfo pbrInputs)
{
    float a = pbrInputs.alphaRoughness;
    float a2 = a * a;
    float F0 = 0.04;
    float VoL = 2 * pbrInputs.VdotH * pbrInputs.VdotH - 1;		// double angle identity
    float Cosri = VoL - pbrInputs.NdotV * pbrInputs.NdotL;
    float a2_13 = a2 + 1.36053;
    float Fr = ( 1 - ( 0.542026*a2 + 0.303573*a ) / a2_13 ) * ( 1 - pow( 1 - pbrInputs.NdotV, 5 - 4*a2 ) / a2_13 ) * ( ( -0.733996*a2*a + 1.50912*a2 - 1.16402*a ) * pow( 1 - pbrInputs.NdotV, 1 + rcp(39*a2*a2+1) ) + 1 );
    //float Fr = ( 1 - 0.36 * a ) * ( 1 - pow( 1 - NoV, 5 - 4*a2 ) / a2_13 ) * ( -2.5 * Roughness * ( 1 - NoV ) + 1 );
    float Lm = ( max( 1 - 2*a, 0 ) * ( 1 - pow5( 1 - pbrInputs.NdotL ) ) + min( 2*a, 1 ) ) * ( 1 - 0.5*a * (pbrInputs.NdotL - 1) ) * pbrInputs.NdotL;
    float Vd = ( a2 / ( (a2 + 0.09) * (1.31072 + 0.995584 * pbrInputs.NdotV) ) ) * ( 1 - pow( 1 - pbrInputs.NdotL, ( 1 - 0.3726732 * pbrInputs.NdotV * pbrInputs.NdotV ) / ( 0.188566 + 0.38841 * pbrInputs.NdotV ) ) );
    float Bp = Cosri < 0 ? 1.4 * pbrInputs.NdotV * pbrInputs.NdotL * Cosri : Cosri;
    float Lr = (21.0 / 20.0) * (1 - F0) * ( Fr * Lm + Vd + Bp );
    return pbrInputs.diffuseColor * RECIPROCAL_PI * Lr;
}

vec3 BRDF_Diffuse_Burley(PBRInfo pbrInputs)
{
)"
R"(    float energyBias = mix(pbrInputs.perceptualRoughness, 0.0, 0.5);
    float energyFactor = mix(pbrInputs.perceptualRoughness, 1.0, 1.0 / 1.51);
    float fd90 = energyBias + 2.0 * pbrInputs.VdotH * pbrInputs.VdotH * pbrInputs.perceptualRoughness;
    float f0 = 1.0;
    float lightScatter = f0 + (fd90 - f0) * pow(1.0 - pbrInputs.NdotL, 5.0);
    float viewScatter = f0 + (fd90 - f0) * pow(1.0 - pbrInputs.NdotV, 5.0);

    return pbrInputs.diffuseColor * lightScatter * viewScatter * energyFactor;
}

vec3 BRDF_Diffuse_Disney(PBRInfo pbrInputs)
{
	float Fd90 = 0.5 + 2.0 * pbrInputs.perceptualRoughness * pbrInputs.VdotH * pbrInputs.VdotH;
    vec3 f0 = vec3(0.1);
	vec3 invF0 = vec3(1.0, 1.0, 1.0) - f0;
	float dim = min(invF0.r, min(invF0.g, invF0.b));
	float result = ((1.0 + (Fd90 - 1.0) * pow(1.0 - pbrInputs.NdotL, 5.0 )) * (1.0 + (Fd90 - 1.0) * pow(1.0 - pbrInputs.NdotV, 5.0 ))) * dim;
	return pbrInputs.diffuseColor * result;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 specularReflection(PBRInfo pbrInputs)
{
    //return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance90*pbrInputs.reflectance0) * exp2((-5.55473 * pbrInputs.VdotH - 6.98316) * pbrInputs.VdotH);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs)
{
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float r = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r + (1.0 - r) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r + (1.0 - r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from \"Average Irregularity Representation of a Roughened Surface for Ray Reflection\" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (PI * f * f);
}

vec3 BRDF(vec3 u_LightColor, vec3 v, vec3 n, vec3 l, vec3 h, float perceptualRoughness, float metallic, vec3 specularEnvironmentR0, vec3 specularEnvironmentR90, float alphaRoughness, vec3 diffuseColor, vec3 specularColor, float ao)
{
    float unclmapped_NdotL = dot(n, l);

    #ifdef VSG_TWO_SIDED_LIGHTING
    if (unclmapped_NdotL < 0.0)
    {
        n = -n;
        unclmapped_NdotL = -unclmapped_NdotL;
    }
    #endif

    vec3 reflection = -normalize(reflect(v, n));
    reflection.y *= -1.0f;

    float NdotL = clamp(unclmapped_NdotL, 0.001, 1.0);
    float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);
    float VdotL = clamp(dot(v, l), 0.0, 1.0);

    PBRInfo pbrInputs = PBRInfo(NdotL,
                                NdotV,
                                NdotH,
                                LdotH,
                                VdotH,
                                VdotL,
                                perceptualRoughness,
                                metallic,
                                specularEnvironmentR0,
                                specularEnvironmentR90,
                                alphaRoughness,
                                diffuseColor,
                                specularColor);

    // Calculate the shading terms for the microfacet specular shading model
    vec3 F = specularReflection(pbrInputs);
    float G = geometricOcclusion(pbrInputs);
    float D = microfacetDistribution(pbrInputs);

    // Calculation of analytical lighting contribution
    vec3 diffuseContrib = (1.0 - F) * BRDF_Diffuse_Disney(pbrInputs);
    vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
    // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
    vec3 color = NdotL * u_LightColor * (diffuseContrib + specContrib);

    color *= ao;

#ifdef VSG_EMISSIVE_MAP
    vec3 emissive = SRGBtoLINEAR(texture(emissiveMap, texCoord0)).rgb * pbr.emissiveFactor.rgb;
#else
    vec3 emissive = pbr.emissiveFactor.rgb;
#endif
    color += emissive;

    return color;
}

float convertMetallic(vec3 diffuse, vec3 specular, float maxSpecular)
{
    float perceivedDiffuse = sqrt(0.299 * diffuse.r * diffuse.r + 0.587 * diffuse.g * diffuse.g + 0.114 * diffuse.b * diffuse.b);
    float perceivedSpecular = sqrt(0.299 * specular.r * specular.r + 0.587 * specular.g * specular.g + 0.114 * specular.b * specular.b);

    if (perceivedSpecular < c_MinRoughness)
    {
        return 0.0;
    }

    float a = c_MinRoughness;
    float b = perceivedDiffuse * (1.0 - maxSpecular) / (1.0 - c_MinRoughness) + perceivedSpecular - 2.0 * c_MinRoughness;
    float c = c_MinRoughness - perceivedSpecular;
    float D = max(b * b - 4.0 * a * c, 0.0);
    return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}

void main()
{
    float perceptualRoughness = 0.0;
    float metallic;
    vec3 diffuseColor;
    vec4 baseColor;

    float ambientOcclusion = 1.0;

    vec3 f0 = vec3(0.04);

#ifdef VSG_DIFFUSE_MAP
    #ifdef VSG_GREYSACLE_DIFFUSE_MAP
        float v = texture(diffuseMap, texCoord0.st).s * pbr.baseColorFactor;
        baseColor = vertexColor * vec4(v, v, v, 1.0);
    #else
        baseColor = vertexColor * SRGBtoLINEAR(texture(diffuseMap, texCoord0)) * pbr.baseColorFactor;
    #endif
#else
    baseColor = vertexColor * pbr.baseColorFactor;
#endif

    if (pbr.alphaMask == 1.0f)
    {
        if (baseColor.a < pbr.alphaMaskCutoff)
            discard;
    }

#ifdef VSG_WORKFLOW_SPECGLOSS
    #ifdef VSG_DIFFUSE_MAP
        vec4 diffuse = SRGBtoLINEAR(texture(diffuseMap, texCoord0));
    #else
        vec4 diffuse = vec4(1.0);
    #endif

    #ifdef VSG_SPECULAR_MAP
        vec3 specular = SRGBtoLINEAR(texture(specularMap, texCoord0)).rgb;
        perceptualRoughness = 1.0 - texture(specularMap, texCoord0).a;
    #else
        vec3 specular = vec3(0.0);
        perceptualRoughness = 0.0;
    #endif

        float maxSpecular = max(max(specular.r, specular.g), specular.b);

        // Convert metallic value from specular glossiness inputs
        metallic = convertMetallic(diffuse.rgb, specular, maxSpecular);

        const float epsilon = 1e-6;
        vec3 baseColorDiffusePart = diffuse.rgb * ((1.0 - maxSpecular) / (1 - c_MinRoughness) / max(1 - metallic, epsilon)) * pbr.diffuseFactor.rgb;
        vec3 baseColorSpecularPart = specular - (vec3(c_MinRoughness) * (1 - metallic) * (1 / max(metallic, epsilon))) * pbr.specularFactor.rgb;
        baseColor = vec4(mix(baseColorDiffusePart, baseColorSpecularPart, metallic * metallic), diffuse.a);
#else
        perceptualRoughness = pbr.roughnessFactor;
        metallic = pbr.metallicFactor;

    #ifdef VSG_METALLROUGHNESS_MAP
        vec4 mrSample = texture(mrMap, texCoord0);
        perceptualRoughness = mrSample.g * perceptualRoughness;
        metallic = mrSample.b * metallic;
    #endif
#endif

#ifdef VSG_LIGHTMAP_MAP
    ambientOcclusion = texture(aoMap, texCoord0).r;
#endif

    diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
    diffuseColor *= 1.0 - metallic;

    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    vec3 specularColor = mix(f0, baseColor.rgb, metallic);

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 specularEnvironmentR0 = specularColor.rgb;
    vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

    vec3 n = getNormal();
    vec3 v = normalize(viewDir);    // Vector from surface point to camera

    float shininess = 100.0f;

    vec3 color = vec3(0.0, 0.0, 0.0);

    vec4 lightNums = lightData.values[0];
    int numAmbientLights = int(lightNums[0]);
    int numDirectionalLights = int(lightNums[1]);
    int numPointLights = int(lightNums[2]);
    int numSpotLights = int(lightNums[3]);
    int index = 1;
    if (numAmbientLights>0)
    {
        // ambient lights
        for(int i = 0; i<numAmbientLights; ++i)
        {
            vec4 ambient_color = lightData.values[index++];
            color += (baseColor.rgb * ambient_color.rgb) * (ambient_color.a * ambientOcclusion);
        }
    }

    if (numDirectionalLights>0)
    {
        // directional lights
        for(int i = 0; i<numDirectionalLights; ++i)
        {
            vec4 lightColor = lightData.values[index++];
            vec3 direction = -lightData.values[index++].xyz;

            vec3 l = direction;         // Vector from surface point to light
            vec3 h = normalize(l+v);    // Half vector between both l and v
            float scale = lightColor.a;

            color.rgb += BRDF(lightColor.rgb * scale, v, n, l, h, perceptualRoughness, metallic, specularEnvironmentR0, specularEnvironmentR90, alphaRoughness, diffuseColor, specularColor, ambientOcclusion);
        }
    }

    if (numPointLights>0)
    {
        // point light
        for(int i = 0; i<numPointLights; ++i)
        {
            vec4 lightColor = lightData.values[index++];
            vec3 position = lightData.values[index++].xyz;
            vec3 delta = position - eyePos;
            float distance2 = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            vec3 direction = delta / sqrt(distance2);

            vec3 l = direction;         // Vector from surface point to light
            vec3 h = normalize(l+v);    // Half vector between both l and v
            float scale = lightColor.a / distance2;

            color.rgb += BRDF(lightColor.rgb * scale, v, n, l, h, perceptualRoughness, metallic, specularEnvironmentR0, specularEnvironmentR90, alphaRoughness, diffuseColor, specularColor, ambientOcclusion);
        }
    }

    if (numSpotLights>0)
    {
        // spot light
        for(int i = 0; i<numSpotLights; ++i)
        {
            vec4 lightColor = lightData.values[index++];
            vec4 position_cosInnerAngle = lightData.values[index++];
            vec4 lightDirection_cosOuterAngle = lightData.values[index++];

            vec3 delta = position_cosInnerAngle.xyz - eyePos;
            float distance2 = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            vec3 direction = delta / sqrt(distance2);
            float dot_lightdirection = -dot(lightDirection_cosOuterAngle.xyz, direction);

            vec3 l = direction;        // Vector from surface point to light
            vec3 h = normalize(l+v);    // Half vector between both l and v
            float scale = (lightColor.a * smoothstep(lightDirection_cosOuterAngle.w, position_cosInnerAngle.w, dot_lightdirection)) / distance2;

            color.rgb += BRDF(lightColor.rgb * scale, v, n, l, h, perceptualRoughness, metallic, specularEnvironmentR0, specularEnvironmentR90, alphaRoughness, diffuseColor, specularColor, ambientOcclusion);
        }
    }

    outColor = LINEARtoSRGB(vec4(color, baseColor.a));
}
"
              code 5168
               119734787 65536 524298 804 0 131089 1 393227 1 1280527431 1685353262 808793134
               0 196622 0 1 720911 4 4 1852399981 0 69 368 438
               598 788 803 196624 4 7 196611 2 450 589828 1096764487 1935622738
               1918988389 1600484449 1684105331 1868526181 1667590754 29556 262149 4 1852399981 0 458757 11
               1162758476 1869894209 1111970387 879130152 59 262149 10 1650946675 28233 327685 15 1316250983
               1634562671 10348 262149 17 1230127696 7300718 327686 17 0 1953457230 76 327686
               17 1 1953457230 86 327686 17 2 1953457230 72 327686 17 3
               1953457228 72 327686 17 4 1953457238 72 327686 17 5 1953457238 76
               524294 17 6 1668441456 1970565221 1867672673 1852335989 7566181 393222 17 7 1635018093
               1936027244 115 458758 17 8 1818649970 1635017573 811950958 0 458758 17 9
               1818649970 1635017573 962945902 48 458758 17 10 1752198241 1970229857 1701734503 29555 458758
               17 11 1717987684 1130722165 1919904879 0 458758 17 12 1667592307 1918987381 1869377347
               114 1441797 21 1178882626 1718174815 1702065510 1936278623 679044462 1970435187 1345156195 1850298946 1714253670
               828779825 758212141 1714237798 828779825 758212141 1982673254 1982673766 1714238310 1719020849 1719020851 3879219 327685
               20 1232233072 1953853550 115 1441797 24 1667592307 1918987381 1818649938 1769235301 1932029551 1668641396
               1112550772 1718503762 828779887 758212141 1714237798 828779825 758212141 1714237798 1719020849 1719020851 828779827 862352941
               862352941 15153 327685 23 1232233072 1953853550 115 1441797 28 1836016999 1769108581 1667452771
               1769174380 1932029551 1668641396 1112550772 1718503762 828779887 758212141 1714237798 828779825 758212141 1714237798 1719020849
               1719020851 828779827 862352941 862352941 15153 327685 27 1232233072 1953853550 115 1507333 31
               1919117677 1667327599 1766093925 1769108595 1769239906 1932029551 1668641396 1112550772 1718503762 828779887 758212141 1714237798
               828779825 758212141 1714237798 1719020849 1719020851 828779827 862352941 862352941 15153 327685 30 1232233072
               1953853550 115 1048581 49 1178882626 862352936 862352955 862352955 862352955 862352955 993093179 1983590758
               1983591270 1715155814 1719024433 1719024435 828783411 59 393221 36 1766612853 1131702375 1919904879 0
               196613 37 118 196613 38 110 196613 39 108 196613 40 104
               458757 41 1668441456 1970565221 1867672673 1852335989 7566181 327685 42 1635018093 1667853420 0
               524293 43 1667592307 1918987381 1769369157 1835954034 1383362149 48 524293 44 1667592307 1918987381
               1769369157 1835954034 1383362149 12345 393221 45 1752198241 1970229857 1701734503 29555 393221 46
               1717987684 1130722165 1919904879 0 393221 47 1667592307 1918987381 1869377347 114 196613 48
               28513 262149 51 1332636012 29813 327685 69 1836216174 1766091873 114 262149 74
               809067590 0 196613 90 12390 262149 93 1182166633 48 196613 98 7170404
               262149 110 1970496882 29804 262149 166 1953457230 76 262149 169 1953457230 86
               196613 172 114 393221 179 1702130785 1952544110 1282305897 0 393221 194 1702130785
               1952544110 1450078057 0 327685 214 1735749490 1936027240 7426931 196613 220 102 458757
               242 1818455669 1886413165 1314874469 1282699108 0 327685 246 1818649970 1769235301 28271 262149
               257 1953457230 76 262149 261 1953457230 86 262149 267 1953457230 72 262149
               273 1953457228 72 262149 278 1953457238 72 262149 283 1953457238 76 327685
               288 1232233072 1953853550 115 196613 303 70 262149 304 1634886000 109 196613
               307 71 262149 308 1634886000 109 196613 311 68 262149 312 1634886000
               109 393221 315 1717987684 1130722165 1920233071 25193 262149 319 1634886000 109 327685
               323 1667592307 1953394499 6449522 262149 336 1869377379 114 327685 347 1936289125 1702259059
               0 262149 348 1148346960 6386785 458758 348 0 1702060386 1869377347 1667319410 7499636
)"
R"(               458758 348 1 1936289125 1702259059 1952670022 29295 458758 348 2 1717987684 1181053813
               1869898593 114 458758 348 3 1667592307 1918987381 1952670022 29295 458758 348 4
               1635018093 1667853420 1952670022 29295 458758 348 5 1735749490 1936027240 1667319411 7499636 393222
               348 6 1752198241 1935756641 107 458758 348 7 1752198241 1935756641 1953842027 6710895
               196613 350 7496304 458757 361 1668441456 1970565221 1867672673 1852335989 7566181 458757 362
               1768058209 1333030501 1970037603 1852795251 0 196613 363 12390 327685 366 1702060386 1869377347
               114 327685 368 1953654134 1866692709 7499628 327685 392 1635018093 1667853420 0 393221
               395 1717987684 1130722165 1919904879 0 393221 405 1752198241 1970229857 1701734503 29555 393221
               409 1667592307 1918987381 1869377347 114 327685 416 1818649970 1635017573 6644590 393221 425
               1818649970 1635017573 962945902 48 524293 430 1667592307 1918987381 1769369157 1835954034 1383362149 48
               524293 432 1667592307 1918987381 1769369157 1835954034 1383362149 12345 196613 435 110 196613
               437 118 262149 438 2003134838 7498052 327685 441 1852401779 1936027241 115 262149
               443 1869377379 114 327685 445 1751607660 1836404340 115 327685 448 1751607628 1952531572
               97 327686 448 0 1970037110 29541 327685 450 1751607660 1952531572 97 458757
               454 1097692526 1701405293 1766618222 1937008743 0 524293 458 1148024174 1667592809 1852795252 1766616161
               1937008743 0 393221 462 1349350766 1953393007 1751607628 29556 393221 466 1399682414 1282699120
               1952999273 115 262149 470 1701080681 120 196613 475 105 393221 484 1768058209
               1601465957 1869377379 114 196613 507 105 327685 516 1751607660 1819231092 29295 327685
               521 1701996900 1869182051 110 196613 528 108 196613 530 104 262149 535
               1818321779 101 262149 542 1634886000 109 262149 543 1634886000 109 262149 545
               1634886000 109 262149 547 1634886000 109 262149 549 1634886000 109 262149 551
               1634886000 109 262149 553 1634886000 109 262149 555 1634886000 109 262149 557
               1634886000 109 262149 559 1634886000 109 262149 561 1634886000 109 262149 563
               1634886000 109 262149 565 1634886000 109 196613 576 105 327685 585 1751607660
               1819231092 29295 327685 590 1769172848 1852795252 0 262149 596 1953260900 97 262149
               598 1348827493 29551 327685 601 1953720676 1701015137 50 327685 619 1701996900 1869182051
               110 196613 625 108 196613 627 104 262149 632 1818321779 101 262149
               641 1634886000 109 262149 642 1634886000 109 262149 644 1634886000 109 262149
               646 1634886000 109 262149 648 1634886000 109 262149 650 1634886000 109 262149
               652 1634886000 109 262149 654 1634886000 109 262149 656 1634886000 109 262149
               658 1634886000 109 262149 660 1634886000 109 262149 662 1634886000 109 262149
               664 1634886000 109 196613 675 105 327685 684 1751607660 1819231092 29295 524293
               689 1769172848 1852795252 1936679775 1701736009 1735278962 25964 655365 694 1751607660 1919501428 1769235301
               1667198575 1968141167 1098016116 1701603182 0 262149 699 1953260900 97 327685 704 1953720676
               1701015137 50 327685 722 1701996900 1869182051 110 458757 728 1601466212 1751607660 1919509620
               1769235301 28271 196613 734 108 196613 736 104 262149 741 1818321779 101
               262149 757 1634886000 109 262149 758 1634886000 109 262149 760 1634886000 109
               262149 762 1634886000 109 262149 764 1634886000 109 262149 766 1634886000 109
               262149 768 1634886000 109 262149 770 1634886000 109 262149 772 1634886000 109
               262149 774 1634886000 109 262149 776 1634886000 109 262149 778 1634886000 109
               262149 780 1634886000 109 327685 788 1131705711 1919904879 0 262149 796 1634886000
               109 327685 803 1131963764 1685221231 48 262215 69 30 1 327752 348
               0 35 0 327752 348 1 35 16 327752 348 2 35
               32 327752 348 3 35 48 327752 348 4 35 64 327752
               348 5 35 68 327752 348 6 35 72 327752 348 7
               35 76 196679 348 2 262215 350 34 0 262215 350 33
               10 262215 368 30 2 262215 438 30 5 262215 447 6
               16 327752 448 0 35 0 196679 448 2 262215 450 34
               1 262215 450 33 0 262215 598 30 0 262215 788 30
               0 262215 803 30 3 131091 2 196641 3 2 196630 6
               32 262167 7 6 4 262176 8 7 7 262177 9 7
               8 262167 13 6 3 196641 14 13 983070 17 6 6
               6 6 6 6 6 6 13 13 6 13 13 262176
               18 7 17 262177 19 13 18 262177 26 6 18 262176
               33 7 13 262176 34 7 6 1048609 35 13 33 33
               33 33 33 34 34 33 33 34 33 33 34 262187
               6 54 1055439407 393260 13 55 54 54 54 262165 58 32
               0 262187 58 59 3 262176 68 1 13 262203 68 69
               1 262187 6 75 1056964608 262187 6 76 1073741824 262165 77 32
               1 262187 77 78 6 262187 77 82 4 262187 6 91
               1036831949 393260 13 92 91 91 91 262187 6 94 1065353216 393260
               13 95 94 94 94 262187 58 99 0 262187 58 102
               1 262187 58 105 2 262187 77 113 0 262187 6 117
               1084227584 262187 77 123 1 262187 77 133 11 262187 77 140
               8 262187 77 143 9 262187 6 152 3232874585 262187 6 156
               1088386572 262187 77 173 10 262187 77 221 2 262187 6 234
               1078530011 262187 6 252 3212836864 262187 6 259 981668463 262187 6 271
               0 262187 6 329 1082130432 655390 348 7 7 7 7 6
               6 6 6 262176 349 2 348 262203 349 350 2 262176
               351 2 7 262187 6 364 1025758986 393260 13 365 364 364
               364 262176 367 1 7 262203 367 368 1 262176 373 2
               6 131092 376 262187 77 382 7 262187 77 389 5 262187
               6 427 1103626240 262203 68 438 1 262187 6 442 1120403456 393260
               13 444 271 271 271 262187 58 446 64 262172 447 7
               446 196638 448 447 262176 449 2 448 262203 449 450 2
               262176 453 7 77 262203 68 598 1 262176 787 3 7
               262203 787 788 3 262187 6 798 1050868099 262187 6 799 1042479491
               262187 6 800 897988541 262167 801 6 2 262176 802 1 801
               262203 802 803 1 327734 2 4 0 3 131320 5 262203
               34 361 7 262203 34 362 7 262203 33 363 7 262203
               8 366 7 262203 34 392 7 262203 33 395 7 262203
               34 405 7 262203 33 409 7 262203 34 416 7 262203
               34 425 7 262203 33 430 7 262203 33 432 7 262203
               33 435 7 262203 33 437 7 262203 34 441 7 262203
               33 443 7 262203 8 445 7 262203 453 454 7 262203
               453 458 7 262203 453 462 7 262203 453 466 7 262203
               453 470 7 262203 453 475 7 262203 8 484 7 262203
               453 507 7 262203 8 516 7 262203 33 521 7 262203
               33 528 7 262203 33 530 7 262203 34 535 7 262203
               33 542 7 262203 33 543 7 262203 33 545 7 262203
               33 547 7 262203 33 549 7 262203 34 551 7 262203
               34 553 7 262203 33 555 7 262203 33 557 7 262203
               34 559 7 262203 33 561 7 262203 33 563 7 262203
               34 565 7 262203 453 576 7 262203 8 585 7 262203
               33 590 7 262203 33 596 7 262203 34 601 7 262203
               33 619 7 262203 33 625 7 262203 33 627 7 262203
               34 632 7 262203 33 641 7 262203 33 642 7 262203
               33 644 7 262203 33 646 7 262203 33 648 7 262203
               34 650 7 262203 34 652 7 262203 33 654 7 262203
               33 656 7 262203 34 658 7 262203 33 660 7 262203
               33 662 7 262203 34 664 7 262203 453 675 7 262203
               8 684 7 262203 8 689 7 262203 8 694 7 262203
               33 699 7 262203 34 704 7 262203 33 722 7 262203
               34 728 7 262203 33 734 7 262203 33 736 7 262203
               34 741 7 262203 33 757 7 262203 33 758 7 262203
               33 760 7 262203 33 762 7 262203 33 764 7 262203
               34 766 7 262203 34 768 7 262203 33 770 7 262203
               33 772 7 262203 34 774 7 262203 33 776 7 262203
               33 778 7 262203 34 780 7 262203 8 796 7 196670
               361 271 196670 362 94 196670 363 365 262205 7 369 368
               327745 351 370 350 113 262205 7 371 370 327813 7 372
               369 371 196670 366 372 327745 373 374 350 78 262205 6
               375 374 327860 376 377 375 94 196855 379 0 262394 377
               378 379 131320 378 327745 34 380 366 59 262205 6 381
               380 327745 373 383 350 382 262205 6 384 383 327864 376
               385 381 384 196855 387 0 262394 385 386 387 131320 386
               65788 131320 387 131321 379 131320 379 327745 373 390 350 389
               262205 6 391 390 196670 361 391 327745 373 393 350 82
               262205 6 394 393 196670 392 394 262205 7 396 366 524367
               13 397 396 396 0 1 2 262205 13 398 363 327811
               13 399 95 398 327813 13 400 397 399 196670 395 400
               262205 6 401 392 327811 6 402 94 401 262205 13 403
               395 327822 13 404 403 402 196670 395 404 262205 6 406
               361 262205 6 407 361 327813 6 408 406 407 196670 405
               408 262205 13 410 363 262205 7 411 366 524367 13 412
               411 411 0 1 2 262205 6 413 392 393296 13 414
               413 413 413 524300 13 415 1 46 410 412 414 196670
               409 415 327745 34 417 409 99 262205 6 418 417 327745
               34 419 409 102 262205 6 420 419 458764 6 421 1
               40 418 420 327745 34 422 409 105 262205 6 423 422
               458764 6 424 1 40 421 423 196670 416 424 262205 6
               426 416 327813 6 428 426 427 524300 6 429 1 43
               428 271 94 196670 425 429 262205 13 431 409 196670 430
               431 262205 6 433 425 327822 13 434 95 433 196670 432
               434 262201 13 436 15 196670 435 436 262205 13 439 438
               393228 13 440 1 69 439 196670 437 440 196670 441 442
               196670 443 444 393281 351 451 450 113 113 262205 7 452
               451 196670 445 452 327745 34 455 445 99 262205 6 456
               455 262254 77 457 456 196670 454 457 327745 34 459 445
               102 262205 6 460 459 262254 77 461 460 196670 458 461
               327745 34 463 445 105 262205 6 464 463 262254 77 465
               464 196670 462 465 327745 34 467 445 59 262205 6 468
               467 262254 77 469 468 196670 466 469 196670 470 123 262205
               77 471 454 327853 376 472 471 113 196855 474 0 262394
               472 473 474 131320 473 196670 475 113 131321 476 131320 476
               262390 478 479 0 131321 480 131320 480 262205 77 481 475
               262205 77 482 454 327857 376 483 481 482 262394 483 477
               478 131320 477 262205 77 485 470 327808 77 486 485 123
               196670 470 486 393281 351 487 450 113 485 262205 7 488
               487 196670 484 488 262205 7 489 366 524367 13 490 489
               489 0 1 2 262205 7 491 484 524367 13 492 491
               491 0 1 2 327813 13 493 490 492 327745 34 494
               484 59 262205 6 495 494 262205 6 496 362 327813 6
               497 495 496 327822 13 498 493 497 262205 13 499 443
               327809 13 500 499 498 196670 443 500 131321 479 131320 479
               262205 77 501 475 327808 77 502 501 123 196670 475 502
               131321 476 131320 478 131321 474 131320 474 262205 77 503 458
               327853 376 504 503 113 196855 506 0 262394 504 505 506
               131320 505 196670 507 113 131321 508 131320 508 262390 510 511
               0 131321 512 131320 512 262205 77 513 507 262205 77 514
               458 327857 376 515 513 514 262394 515 509 510 131320 509
               262205 77 517 470 327808 77 518 517 123 196670 470 518
               393281 351 519 450 113 517 262205 7 520 519 196670 516
               520 262205 77 522 470 327808 77 523 522 123 196670 470
               523 393281 351 524 450 113 522 262205 7 525 524 524367
               13 526 525 525 0 1 2 262271 13 527 526 196670
               521 527 262205 13 529 521 196670 528 529 262205 13 531
               528 262205 13 532 437 327809 13 533 531 532 393228 13
               534 1 69 533 196670 530 534 327745 34 536 516 59
               262205 6 537 536 196670 535 537 262205 7 538 516 524367
               13 539 538 538 0 1 2 262205 6 540 535 327822
               13 541 539 540 196670 542 541 262205 13 544 437 196670
               543 544 262205 13 546 435 196670 545 546 262205 13 548
               528 196670 547 548 262205 13 550 530 196670 549 550 262205
               6 552 361 196670 551 552 262205 6 554 392 196670 553
               554 262205 13 556 430 196670 555 556 262205 13 558 432
               196670 557 558 262205 6 560 405 196670 559 560 262205 13
               562 395 196670 561 562 262205 13 564 409 196670 563 564
               262205 6 566 362 196670 565 566 1114169 13 567 49 542
               543 545 547 549 551 553 555 557 559 561 563 565
               262205 13 568 443 327809 13 569 568 567 196670 443 569
               131321 511 131320 511 262205 77 570 507 327808 77 571 570
               123 196670 507 571 131321 508 131320 510 131321 506 131320 506
               262205 77 572 462 327853 376 573 572 113 196855 575 0
               262394 573 574 575 131320 574 196670 576 113 131321 577 131320
               577 262390 579 580 0 131321 581 131320 581 262205 77 582
               576 262205 77 583 462 327857 376 584 582 583 262394 584
               578 579 131320 578 262205 77 586 470 327808 77 587 586
               123 196670 470 587 393281 351 588 450 113 586 262205 7
               589 588 196670 585 589 262205 77 591 470 327808 77 592
               591 123 196670 470 592 393281 351 593 450 113 591 262205
               7 594 593 524367 13 595 594 594 0 1 2 196670
               590 595 262205 13 597 590 262205 13 599 598 327811 13
               600 597 599 196670 596 600 327745 34 602 596 99 262205
               6 603 602 327745 34 604 596 99 262205 6 605 604
               327813 6 606 603 605 327745 34 607 596 102 262205 6
               608 607 327745 34 609 596 102 262205 6 610 609 327813
               6 611 608 610 327809 6 612 606 611 327745 34 613
               596 105 262205 6 614 613 327745 34 615 596 105 262205
               6 616 615 327813 6 617 614 616 327809 6 618 612
               617 196670 601 618 262205 13 620 596 262205 6 621 601
               393228 6 622 1 31 621 393296 13 623 622 622 622
               327816 13 624 620 623 196670 619 624 262205 13 626 619
               196670 625 626 262205 13 628 625 262205 13 629 437 327809
               13 630 628 629 393228 13 631 1 69 630 196670 627
               631 327745 34 633 585 59 262205 6 634 633 262205 6
               635 601 327816 6 636 634 635 196670 632 636 262205 7
               637 585 524367 13 638 637 637 0 1 2 262205 6
               639 632 327822 13 640 638 639 196670 641 640 262205 13
               643 437 196670 642 643 262205 13 645 435 196670 644 645
               262205 13 647 625 196670 646 647 262205 13 649 627 196670
               648 649 262205 6 651 361 196670 650 651 262205 6 653
               392 196670 652 653 262205 13 655 430 196670 654 655 262205
               13 657 432 196670 656 657 262205 6 659 405 196670 658
               659 262205 13 661 395 196670 660 661 262205 13 663 409
               196670 662 663 262205 6 665 362 196670 664 665 1114169 13
               666 49 641 642 644 646 648 650 652 654 656 658
               660 662 664 262205 13 667 443 327809 13 668 667 666
               196670 443 668 131321 580 131320 580 262205 77 669 576 327808
               77 670 669 123 196670 576 670 131321 577 131320 579 131321
               575 131320 575 262205 77 671 466 327853 376 672 671 113
               196855 674 0 262394 672 673 674 131320 673 196670 675 113
               131321 676 131320 676 262390 678 679 0 131321 680 131320 680
)"
R"(               262205 77 681 675 262205 77 682 466 327857 376 683 681
               682 262394 683 677 678 131320 677 262205 77 685 470 327808
               77 686 685 123 196670 470 686 393281 351 687 450 113
               685 262205 7 688 687 196670 684 688 262205 77 690 470
               327808 77 691 690 123 196670 470 691 393281 351 692 450
               113 690 262205 7 693 692 196670 689 693 262205 77 695
               470 327808 77 696 695 123 196670 470 696 393281 351 697
               450 113 695 262205 7 698 697 196670 694 698 262205 7
               700 689 524367 13 701 700 700 0 1 2 262205 13
               702 598 327811 13 703 701 702 196670 699 703 327745 34
               705 699 99 262205 6 706 705 327745 34 707 699 99
               262205 6 708 707 327813 6 709 706 708 327745 34 710
               699 102 262205 6 711 710 327745 34 712 699 102 262205
               6 713 712 327813 6 714 711 713 327809 6 715 709
               714 327745 34 716 699 105 262205 6 717 716 327745 34
               718 699 105 262205 6 719 718 327813 6 720 717 719
               327809 6 721 715 720 196670 704 721 262205 13 723 699
               262205 6 724 704 393228 6 725 1 31 724 393296 13
               726 725 725 725 327816 13 727 723 726 196670 722 727
               262205 7 729 694 524367 13 730 729 729 0 1 2
               262205 13 731 722 327828 6 732 730 731 262271 6 733
               732 196670 728 733 262205 13 735 722 196670 734 735 262205
               13 737 734 262205 13 738 437 327809 13 739 737 738
               393228 13 740 1 69 739 196670 736 740 327745 34 742
               684 59 262205 6 743 742 327745 34 744 694 59 262205
               6 745 744 327745 34 746 689 59 262205 6 747 746
               262205 6 748 728 524300 6 749 1 49 745 747 748
               327813 6 750 743 749 262205 6 751 704 327816 6 752
               750 751 196670 741 752 262205 7 753 684 524367 13 754
               753 753 0 1 2 262205 6 755 741 327822 13 756
               754 755 196670 757 756 262205 13 759 437 196670 758 759
               262205 13 761 435 196670 760 761 262205 13 763 734 196670
               762 763 262205 13 765 736 196670 764 765 262205 6 767
               361 196670 766 767 262205 6 769 392 196670 768 769 262205
               13 771 430 196670 770 771 262205 13 773 432 196670 772
               773 262205 6 775 405 196670 774 775 262205 13 777 395
               196670 776 777 262205 13 779 409 196670 778 779 262205 6
               781 362 196670 780 781 1114169 13 782 49 757 758 760
               762 764 766 768 770 772 774 776 778 780 262205 13
               783 443 327809 13 784 783 782 196670 443 784 131321 679
               131320 679 262205 77 785 675 327808 77 786 785 123 196670
               675 786 131321 676 131320 678 131321 674 131320 674 262205 13
               789 443 327745 34 790 366 59 262205 6 791 790 327761
               6 792 789 0 327761 6 793 789 1 327761 6 794
               789 2 458832 7 795 792 793 794 791 196670 796 795
               327737 7 797 11 796 196670 788 797 65789 65592 327734 7
               11 0 9 196663 8 10 131320 12 262203 33 51 7
               262205 7 52 10 524367 13 53 52 52 0 1 2
               458764 13 56 1 26 53 55 196670 51 56 262205 13
               57 51 327745 34 60 10 59 262205 6 61 60 327761
               6 62 57 0 327761 6 63 57 1 327761 6 64
               57 2 458832 7 65 62 63 64 61 131326 65 65592
               327734 13 15 0 14 131320 16 262205 13 70 69 393228
               13 71 1 69 70 131326 71 65592 327734 13 21 0
               19 196663 18 20 131320 22 262203 34 74 7 262203 33
               90 7 262203 33 93 7 262203 34 98 7 262203 34
               110 7 327745 34 79 20 78 262205 6 80 79 327813
               6 81 76 80 327745 34 83 20 82 262205 6 84
               83 327813 6 85 81 84 327745 34 86 20 82 262205
               6 87 86 327813 6 88 85 87 327809 6 89 75
               88 196670 74 89 196670 90 92 262205 13 96 90 327811
               13 97 95 96 196670 93 97 327745 34 100 93 99
               262205 6 101 100 327745 34 103 93 102 262205 6 104
               103 327745 34 106 93 105 262205 6 107 106 458764 6
               108 1 37 104 107 458764 6 109 1 37 101 108
               196670 98 109 262205 6 111 74 327811 6 112 111 94
               327745 34 114 20 113 262205 6 115 114 327811 6 116
               94 115 458764 6 118 1 26 116 117 327813 6 119
               112 118 327809 6 120 94 119 262205 6 121 74 327811
               6 122 121 94 327745 34 124 20 123 262205 6 125
               124 327811 6 126 94 125 458764 6 127 1 26 126
               117 327813 6 128 122 127 327809 6 129 94 128 327813
               6 130 120 129 262205 6 131 98 327813 6 132 130
               131 196670 110 132 327745 33 134 20 133 262205 13 135
               134 262205 6 136 110 327822 13 137 135 136 131326 137
               65592 327734 13 24 0 19 196663 18 23 131320 25 327745
               33 141 23 140 262205 13 142 141 327745 33 144 23
               143 262205 13 145 144 327745 33 146 23 143 262205 13
               147 146 327745 33 148 23 140 262205 13 149 148 327813
               13 150 147 149 327811 13 151 145 150 327745 34 153
               23 82 262205 6 154 153 327813 6 155 152 154 327811
               6 157 155 156 327745 34 158 23 82 262205 6 159
               158 327813 6 160 157 159 393228 6 161 1 29 160
               327822 13 162 151 161 327809 13 163 142 162 131326 163
               65592 327734 6 28 0 26 196663 18 27 131320 29 262203
               34 166 7 262203 34 169 7 262203 34 172 7 262203
               34 179 7 262203 34 194 7 327745 34 167 27 113
               262205 6 168 167 196670 166 168 327745 34 170 27 123
               262205 6 171 170 196670 169 171 327745 34 174 27 173
               262205 6 175 174 327745 34 176 27 173 262205 6 177
               176 327813 6 178 175 177 196670 172 178 262205 6 180
               166 327813 6 181 76 180 262205 6 182 166 262205 6
               183 172 262205 6 184 172 327811 6 185 94 184 262205
               6 186 166 262205 6 187 166 327813 6 188 186 187
               327813 6 189 185 188 327809 6 190 183 189 393228 6
               191 1 31 190 327809 6 192 182 191 327816 6 193
               181 192 196670 179 193 262205 6 195 169 327813 6 196
               76 195 262205 6 197 169 262205 6 198 172 262205 6
               199 172 327811 6 200 94 199 262205 6 201 169 262205
               6 202 169 327813 6 203 201 202 327813 6 204 200
               203 327809 6 205 198 204 393228 6 206 1 31 205
               327809 6 207 197 206 327816 6 208 196 207 196670 194
               208 262205 6 209 179 262205 6 210 194 327813 6 211
               209 210 131326 211 65592 327734 6 31 0 26 196663 18
               30 131320 32 262203 34 214 7 262203 34 220 7 327745
               34 215 30 173 262205 6 216 215 327745 34 217 30
               173 262205 6 218 217 327813 6 219 216 218 196670 214
               219 327745 34 222 30 221 262205 6 223 222 262205 6
               224 214 327813 6 225 223 224 327745 34 226 30 221
               262205 6 227 226 327811 6 228 225 227 327745 34 229
               30 221 262205 6 230 229 327813 6 231 228 230 327809
               6 232 231 94 196670 220 232 262205 6 233 214 262205
               6 235 220 327813 6 236 234 235 262205 6 237 220
               327813 6 238 236 237 327816 6 239 233 238 131326 239
               65592 327734 13 49 0 35 196663 33 36 196663 33 37
               196663 33 38 196663 33 39 196663 33 40 196663 34 41
               196663 34 42 196663 33 43 196663 33 44 196663 34 45
               196663 33 46 196663 33 47 196663 34 48 131320 50 262203
               34 242 7 262203 33 246 7 262203 34 257 7 262203
               34 261 7 262203 34 267 7 262203 34 273 7 262203
               34 278 7 262203 34 283 7 262203 18 288 7 262203
               33 303 7 262203 18 304 7 262203 34 307 7 262203
               18 308 7 262203 34 311 7 262203 18 312 7 262203
               33 315 7 262203 18 319 7 262203 33 323 7 262203
               33 336 7 262203 33 347 7 262205 13 243 38 262205
               13 244 39 327828 6 245 243 244 196670 242 245 262205
               13 247 37 262205 13 248 38 458764 13 249 1 71
               247 248 393228 13 250 1 69 249 262271 13 251 250
               196670 246 251 327745 34 253 246 102 262205 6 254 253
               327813 6 255 254 252 327745 34 256 246 102 196670 256
               255 262205 6 258 242 524300 6 260 1 43 258 259
               94 196670 257 260 262205 13 262 38 262205 13 263 37
               327828 6 264 262 263 393228 6 265 1 4 264 524300
               6 266 1 43 265 259 94 196670 261 266 262205 13
               268 38 262205 13 269 40 327828 6 270 268 269 524300
               6 272 1 43 270 271 94 196670 267 272 262205 13
               274 39 262205 13 275 40 327828 6 276 274 275 524300
               6 277 1 43 276 271 94 196670 273 277 262205 13
               279 37 262205 13 280 40 327828 6 281 279 280 524300
               6 282 1 43 281 271 94 196670 278 282 262205 13
               284 37 262205 13 285 39 327828 6 286 284 285 524300
               6 287 1 43 286 271 94 196670 283 287 262205 6
               289 257 262205 6 290 261 262205 6 291 267 262205 6
               292 273 262205 6 293 278 262205 6 294 283 262205 6
               295 41 262205 6 296 42 262205 13 297 43 262205 13
               298 44 262205 6 299 45 262205 13 300 46 262205 13
               301 47 1048656 17 302 289 290 291 292 293 294 295
               296 297 298 299 300 301 196670 288 302 262205 17 305
               288 196670 304 305 327737 13 306 24 304 196670 303 306
               262205 17 309 288 196670 308 309 327737 6 310 28 308
               196670 307 310 262205 17 313 288 196670 312 313 327737 6
               314 31 312 196670 311 314 262205 13 316 303 393296 13
               317 94 94 94 327811 13 318 317 316 262205 17 320
               288 196670 319 320 327737 13 321 21 319 327813 13 322
               318 321 196670 315 322 262205 13 324 303 262205 6 325
               307 327822 13 326 324 325 262205 6 327 311 327822 13
               328 326 327 262205 6 330 257 327813 6 331 329 330
               262205 6 332 261 327813 6 333 331 332 393296 13 334
               333 333 333 327816 13 335 328 334 196670 323 335 262205
               6 337 257 262205 13 338 36 327822 13 339 338 337
               262205 13 340 315 262205 13 341 323 327809 13 342 340
               341 327813 13 343 339 342 196670 336 343 262205 6 344
               48 262205 13 345 336 327822 13 346 345 344 196670 336
               346 327745 351 352 350 123 262205 7 353 352 524367 13
               354 353 353 0 1 2 196670 347 354 262205 13 355
               347 262205 13 356 336 327809 13 357 356 355 196670 336
               357 262205 13 358 336 131326 358 65592
            }
            NumSpecializationConstants 0
          }
          pipelineStates 6
          vsg::GraphicsPipelineState id=20 vsg::ColorBlendState
          {
            userObjects 0
            logicOp 3
            logicOpEnable 0
            attachments 1
            blendEnable 0
            srcColorBlendFactor 0
            dstColorBlendFactor 0
            colorBlendOp 0
            srcAlphaBlendFactor 0
            dstAlphaBlendFactor 0
            alphaBlendOp 0
            colorWriteMask 15
            blendConstants 0 0 0 0
          }
          vsg::GraphicsPipelineState id=21 vsg::DepthStencilState
          {
            userObjects 0
            depthTestEnable 1
            depthWriteEnable 1
            depthCompareOp 4
            depthBoundsTestEnable 0
            stencilTestEnable 0
            front.failOp 0
            front.passOp 0
            front.depthFailOp 0
            front.compareOp 0
            front.compareMask 0
            front.writeMask 0
            front.reference 0
            back.failOp 0
            back.passOp 0
            back.depthFailOp 0
            back.compareOp 0
            back.compareMask 0
            back.writeMask 0
            back.reference 0
            minDepthBounds 0
            maxDepthBounds 1
          }
          vsg::GraphicsPipelineState id=22 vsg::InputAssemblyState
          {
            userObjects 0
            topology 3
            primitiveRestartEnable 0
          }
          vsg::GraphicsPipelineState id=23 vsg::MultisampleState
          {
            userObjects 0
            rasterizationSamples 1
            sampleShadingEnable 0
            minSampleShading 0
            sampleMasks 0
            alphaToCoverageEnable 0
            alphaToOneEnable 0
          }
          vsg::GraphicsPipelineState id=24 vsg::RasterizationState
          {
            userObjects 0
            depthClampEnable 0
            rasterizerDiscardEnable 0
            polygonMode 0
            cullMode 2
            frontFace 0
            depthBiasEnable 0
            depthBiasConstantFactor 1
            depthBiasClamp 0
            depthBiasSlopeFactor 1
            lineWidth 1
          }
          vsg::GraphicsPipelineState id=25 vsg::VertexInputState
          {
            userObjects 0
            NumBindings 4
            binding 0
            stride 12
            inputRate 0
            binding 1
            stride 12
            inputRate 0
            binding 2
            stride 8
            inputRate 0
            binding 3
            stride 0
            inputRate 1
            NumAttributes 4
            location 0
            binding 0
            format 106
            offset 0
            location 1
            binding 1
            format 106
            offset 0
            location 2
            binding 2
            format 103
            offset 0
            location 3
            binding 3
            format 109
            offset 0
          }
          subpass 0
        }
      }
      vsg::StateCommand id=26 vsg::BindDescriptorSet
      {
        userObjects 0
        slot 1
        pipelineBindPoint 0
        layout id=12
        firstSet 0
        descriptorSet id=27 vsg::DescriptorSet
        {
          userObjects 0
          setLayout id=13
          descriptors 1
          vsg::Descriptor id=28 vsg::DescriptorBuffer
          {
            userObjects 0
            dstBinding 10
            dstArrayElement 0
            dataList 1
            data id=29 vsg::PbrMaterialValue
            {
              userObjects 0
              properties 0 0 0 1 1 1 0 -1 0
              value              baseColorFactor 1 0 0 1
              emissiveFactor 0 0 0 1
              diffuseFactor 0.9 0.9 0.9 1
              specularFactor 0.2 0.2 0.2 1
              metallicFactor 0
              roughnessFactor 0.5
              alphaMask 1
              alphaMaskCutoff 0.5

            }
          }
        }
        dynamicOffsets 0
      }
      vsg::StateCommand id=30 vsg::BindViewDescriptorSets
      {
        userObjects 0
        slot 2
        pipelineBindPoint 0
        layout id=12
        firstSet 1
      }
      prototypeArrayState id=0
    }
    vsg::Node id=31 vsg::MatrixTransform
    {
      userObjects 0
      children 1
      vsg::Node id=32 vsg::PointLight
      {
        name "Light"
        color 1000 1000 1000
        intensity 1
        position 0 0 0
      }
      matrix -0.290865 -0.0551892 -0.955171 0 -0.771101 0.604525 0.199883 0 0.566393 0.794672 -0.218391 0
       4.07625 5.90386 -1.00545 1
      subgraphRequiresLocalFrustum 1
    }
  }
  matrix 1 0 0 0 0 0 1 0 0 -1 0 0
   0 0 0 1
  subgraphRequiresLocalFrustum 1
}
)");
vsg::VSG io;
return io.read_cast<vsg::MatrixTransform>(str);
};

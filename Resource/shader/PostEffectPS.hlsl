#include "PostEffect.hlsli"

Texture2D<float4> mainTex : register(t0);//0番スロットに設定されたテクスチャ
Texture2D<float4> subTex : register(t1);//1番スロットに設定されたテクスチャ

SamplerState smp : register(s0);//0番スロットに設定されたサンプラー

float Gaussian(float2 drawUV, float2 pickUV, float sigma) {
	float d = distance(drawUV, pickUV);
	return exp(-(d * d) / (2 * sigma * sigma));
}

float4 HighLumi(float4 texColor, VSOutput input) : SV_TARGET{
	float4 grayScale =
	texColor.r * 0.299 + texColor.g * 0.587 + texColor.b * 0.114;
	float4 extract = smoothstep(0.6,0.9,grayScale);

	return texColor * extract;
}

float4 Blur(float4 texColor, VSOutput input) : SV_TARGET{
	float totalWeight = 0, _Sigma = 0.005, _StepWidth = 0.001;
	float4 col = float4(0, 0, 0, 0);

	for (float py = -_Sigma * 2; py <= _Sigma * 2; py += _StepWidth) {
		for (float px = -_Sigma * 2; px <= _Sigma * 2; px += _StepWidth) {
			float2 pickUV = input.uv + float2(px, py);
			float weight = Gaussian(input.uv, pickUV, _Sigma);
			col += texColor * weight;
			totalWeight += weight;
		}
	}

	col.rgb = col.rgb / totalWeight;
	return col;
}

float4 main(VSOutput input) : SV_TARGET{
	float4 colorMainTex = float4(0,0,0,0);
	float4 colorSubTex = float4(0,0,0,0);

	float u = 1.0f / 1280.0f;
	float v = 1.0f / 780.0f;

	float4 color = float4(0,0,0,0);

	colorMainTex = mainTex.Sample(smp, input.uv);

	float4 bloomColor = HighLumi(colorMainTex, input);
	//bloomColor = Blur(bloomColor, input);

	colorMainTex = colorMainTex + bloomColor;

	color = colorMainTex + colorSubTex;

	return float4(color.rgb, 1.0f);
}
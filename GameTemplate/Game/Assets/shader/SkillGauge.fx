// --- 定数バッファ (b0: エンジン標準) ---
cbuffer SpriteCB : register(b0)
{
    float4x4 matWorld;
};

// --- 定数バッファ (b1: ユーザー拡張) ---
cbuffer HPGaugeCB : register(b1)
{
    float hpRate; // SPの割合 (0.0~1.0)
    float startAngle; // C++から渡される開始角 (3.14159)
    float maxAngle; // C++から渡される終了角 (0.0)
    float innerRadius; // 内径
    float outerRadius; // 外径
    float damageFlash; // 光らせる演出用//
    float2 centerPos; // パディング込み
};

Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct VSInput
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// 頂点シェーダー
PSInput VSMain(VSInput input)
{
    PSInput output;
    output.pos = mul(matWorld, input.pos);
    output.uv = input.uv;
    return output;
}

// 定数バッファ等の定義は省略（既存のものを使用）

float4 PSMain(PSInput input) : SV_Target
{
    float4 texColor = g_texture.Sample(g_sampler, input.uv);

    // 1. 中心(0.5, 0.5)基準の座標
    float2 d = input.uv - 0.5;
    float dist = length(d);

    // 2. 角度計算（12時方向から時計回り）
    float angle = atan2(-d.x, d.y);
    float normalizedAngle = (angle + 3.14159265f) / 6.28318530f;

    // 3. バウンティ再現ロジック
    if (hpRate >= 1.0f)
    {
        // 100%溜まったら本来の色で表示
        texColor.rgb *= 1.0f;
    }
    else
    {
        // チャージ中の判定
        if (normalizedAngle > hpRate)
        {
            // まだ溜まっていない場所は「真っ暗」
            texColor.rgb *= 0.2f;
        }
        else
        {
            // 溜まってきている場所は「少し明るい青」などを乗算するとそれっぽくなります
            texColor.rgb *= float3(0.4f, 0.6f, 1.0f);
        }
    }

    // 4. 円形に切り抜く
    if (dist > outerRadius || dist < innerRadius)
    {
        discard;
    }

    // アルファ値が0の場所（画像の透明部分）は捨てる
    if (texColor.a <= 0.0f)
        discard;

    return texColor;
}
//グローバル変数
texture  g_texDecal;	//メッシュのテクスチャ
float4x4 g_matWVP;		//ワールド、ビュー、射影行列
float4x4 g_matInvW;		//ワールド逆行列
float4	 g_vecLightDir;	//ライトの方向
float4   g_vecEye;		//視線ベクトル(カメラの位置）

//サンプラー（貼り方）
sampler Sampler = sampler_state	
{
	Texture = <g_texDecal>;
};

//頂点シェーダーの出力構造体
struct VS_OUT
{
	float4	Position : SV_POSITION;
	float2	Texture : TEXCOORD0; //TEXCOORD0...UV座標
	
	//指定するものがないときはセマンティクスはTEXCOORD
	float3 Light : TEXCOORD1; //ライトの方向(情報）をピクセルシェーダーで受け取り計算する
    float3 Normal : TEXCOORD2; //頂点法線の情報をピクセルシェーダに渡す
    float3 View : TEXCOORD3; //各ピクセルからカメラに向かうベクトル
};

//頂点シェーダー
VS_OUT VS(float4 Position : POSITION, float2 Texture : TEXCOORD, float4 Normal:NORMAL)	//法線はマヤで作った時点で求まる
{
	//ピクセルシェーダで必要な頂点の情報を渡す
	VS_OUT Out = (VS_OUT)0;	//0に初期化
	Out.Position = mul(Position, g_matWVP);
	Out.Light = g_vecLightDir; //ライトの情報をピクセルシェーダに渡す 
	float3 PosWorld = normalize(mul(Position, g_matInvW)); //ワールド変換後の頂点の位置
	Out.View = g_vecEye - PosWorld; //今処理しようとしている頂点からカメラへのベクトル
	Out.Normal = mul(Normal, g_matInvW); //今の状態での法線の情報を渡す

	Out.Texture = Texture;

	return Out;	//ピクセルシェーダーに渡す
}

//ピクセルシェーダー
float4 PS(VS_OUT In) : COLOR
{
	//正規化（長さを1に）
	float3 Normal = normalize(In.Normal);	
    float3 LightDir = normalize(In.Light);
    float3 ViewDir = normalize(In.View); 

	float4 NL = saturate(dot(Normal, LightDir));				//saturate（-になったら0にする）（法線とライトベクトルの内積を求める（面（ピクセルの明るさを求める))

	float3 Reflect = normalize(2 * NL * Normal - LightDir);		//反射ベクトルの公式通り

	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 4);  //specular(ハイライト) powの最後の数字を変えるとハイライトも変わる

	float4 Color = tex2D(Sampler, In.Texture) * NL + specular; //通常、照明モデルと言えば環境光の項を含めますが、簡単のため除きました。

	Color.a = tex2D(Sampler, In.Texture).a;
	
	return Color;
}

//テクニック
//ひとつの,頂点・ピクセルシェーダーのセット
technique tec
{
	pass Test
	{
		//VertexShaderは、VS()という関数を、vs_3_0というバージョンで コンパイルする
		VertexShader = compile vs_3_0 VS();
		PixelShader  = compile ps_3_0 PS();
	}
}
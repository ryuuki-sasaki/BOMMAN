//�O���[�o���ϐ�
texture  g_texDecal;	//���b�V���̃e�N�X�`��
float4x4 g_matWVP;		//���[���h�A�r���[�A�ˉe�s��
float4x4 g_matInvW;		//���[���h�t�s��
float4	 g_vecLightDir;	//���C�g�̕���
float4   g_vecEye;		//�����x�N�g��(�J�����̈ʒu�j

//�T���v���[�i�\����j
sampler Sampler = sampler_state	
{
	Texture = <g_texDecal>;
};

//���_�V�F�[�_�[�̏o�͍\����
struct VS_OUT
{
	float4	Position : SV_POSITION;
	float2	Texture : TEXCOORD0; //TEXCOORD0...UV���W
	
	//�w�肷����̂��Ȃ��Ƃ��̓Z�}���e�B�N�X��TEXCOORD
	float3 Light : TEXCOORD1; //���C�g�̕���(���j���s�N�Z���V�F�[�_�[�Ŏ󂯎��v�Z����
    float3 Normal : TEXCOORD2; //���_�@���̏����s�N�Z���V�F�[�_�ɓn��
    float3 View : TEXCOORD3; //�e�s�N�Z������J�����Ɍ������x�N�g��
};

//���_�V�F�[�_�[
VS_OUT VS(float4 Position : POSITION, float2 Texture : TEXCOORD, float4 Normal:NORMAL)	//�@���̓}���ō�������_�ŋ��܂�
{
	//�s�N�Z���V�F�[�_�ŕK�v�Ȓ��_�̏���n��
	VS_OUT Out = (VS_OUT)0;	//0�ɏ�����
	Out.Position = mul(Position, g_matWVP);
	Out.Light = g_vecLightDir; //���C�g�̏����s�N�Z���V�F�[�_�ɓn�� 
	float3 PosWorld = normalize(mul(Position, g_matInvW)); //���[���h�ϊ���̒��_�̈ʒu
	Out.View = g_vecEye - PosWorld; //���������悤�Ƃ��Ă��钸�_����J�����ւ̃x�N�g��
	Out.Normal = mul(Normal, g_matInvW); //���̏�Ԃł̖@���̏���n��

	Out.Texture = Texture;

	return Out;	//�s�N�Z���V�F�[�_�[�ɓn��
}

//�s�N�Z���V�F�[�_�[
float4 PS(VS_OUT In) : COLOR
{
	//���K���i������1�Ɂj
	float3 Normal = normalize(In.Normal);	
    float3 LightDir = normalize(In.Light);
    float3 ViewDir = normalize(In.View); 

	float4 NL = saturate(dot(Normal, LightDir));				//saturate�i-�ɂȂ�����0�ɂ���j�i�@���ƃ��C�g�x�N�g���̓��ς����߂�i�ʁi�s�N�Z���̖��邳�����߂�))

	float3 Reflect = normalize(2 * NL * Normal - LightDir);		//���˃x�N�g���̌����ʂ�

	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 4);  //specular(�n�C���C�g) pow�̍Ō�̐�����ς���ƃn�C���C�g���ς��

	float4 Color = tex2D(Sampler, In.Texture) * NL + specular; //�ʏ�A�Ɩ����f���ƌ����Ί����̍����܂߂܂����A�ȒP�̂��ߏ����܂����B

	Color.a = tex2D(Sampler, In.Texture).a;
	
	return Color;
}

//�e�N�j�b�N
//�ЂƂ�,���_�E�s�N�Z���V�F�[�_�[�̃Z�b�g
technique tec
{
	pass Test
	{
		//VertexShader�́AVS()�Ƃ����֐����Avs_3_0�Ƃ����o�[�W������ �R���p�C������
		VertexShader = compile vs_3_0 VS();
		PixelShader  = compile ps_3_0 PS();
	}
}
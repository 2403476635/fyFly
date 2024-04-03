#include "fyFilter.h"
#define numStages  1                /* 2��IIR�˲��ĸ��� */
#define TEST_LENGTH_SAMPLES  400    /* �������� */
#define BLOCK_SIZE           1    	/* ����һ��arm_biquad_cascade_df1_f32����Ĳ�������� */

float accelAndGyroDataInBuffer[6];
float accelAndGyroOutBuffer[6];
uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;      /* ��Ҫ����arm_biquad_cascade_df1_f32�Ĵ��� */


static float32_t testInput_f32_50Hz_200Hz[TEST_LENGTH_SAMPLES]; /* ������ */
static float32_t testOutput[TEST_LENGTH_SAMPLES];               /* �˲������� */
static float32_t IIRStateF32[4*numStages];                      /* ״̬���� */

/* ������˹��ͨ�˲���ϵ�� 40Hz*/        
//1  2  1  1  -0.369527377351241470559983781640767119825  0.195815712655833140676264747526147402823
const float32_t IIRCoeffs32LP[5*numStages] = {                                                                                 
    1.0f,  2.0f,  1.0f,  1.475480443592646073724949928873684257269f,  
-0.586919508061190309256005548377288505435f                                                                                            
};   

arm_biquad_casd_df1_inst_f32 S;
float32_t ScaleValue;

void armIirFilterInit(void)
{
	arm_biquad_cascade_df1_init_f32(&S, numStages, (float32_t *)&IIRCoeffs32LP[0], (float32_t *)&IIRStateF32[0]);
}

void armIirFilter(float32_t *inputF32, float32_t *outputF32)
{
	arm_biquad_cascade_df1_f32(&S, inputF32, outputF32, blockSize);
	/*����ϵ�� */
	ScaleValue = 0.027859766117136031127188289246987551451f;
	*outputF32 = *outputF32 * ScaleValue;
}

/* ���ٶȼƺ������ǵ��˲������ */
#define accelAndGyroFilter_numStages  1        										/* 2��IIR�˲��ĸ��� */
static arm_biquad_casd_df1_inst_f32 accelAndGyroFilter_S[6];
const float32_t accelAndGyroFilter_IIRCoeffs32LP[5*numStages] = { /* ������˹��ͨ�˲���ϵ�� fs 500Hz fc 40Hz*/                                                                          
    1.0f,  2.0f,  1.0f,  
		1.475480443592646073724949928873684257269f,  
		-0.586919508061190309256005548377288505435f                                                                                            
};       
static float32_t accelAndGyroFilter_IIRStateF32[6][4*numStages];     /* ״̬���� */
void accelAndGyroFilterInit(void)
{
	for(uint8_t i=0;i<6;i++)
	{
		arm_biquad_cascade_df1_init_f32(&accelAndGyroFilter_S[i], accelAndGyroFilter_numStages, (float32_t *)&accelAndGyroFilter_IIRCoeffs32LP[0], (float32_t *)&accelAndGyroFilter_IIRStateF32[i][0]);
	}
}
static float32_t accelAndGyroScaleValue = 0.027859766117136031127188289246987551451f;
void accelAndGyroFilter(void)
{
	for(uint8_t i=0;i<6;i++)
	{
		arm_biquad_cascade_df1_f32(&accelAndGyroFilter_S[i], &accelAndGyroDataInBuffer[i], &accelAndGyroOutBuffer[i], blockSize);
		accelAndGyroOutBuffer[i] *= accelAndGyroScaleValue;
	}
}

/* �����Ƶ������˲�����ʼ�� */
void magFilterInit(void)
{
	
}
/*
*********************************************************************************************************
*    �� �� ��: arm_iir_f32_lp
*    ����˵��: ���ú���arm_iir_f32_lpʵ�ֵ�ͨ�˲���
*    ��    �Σ���
*    �� �� ֵ: ��
*********************************************************************************************************
*/
static void arm_iir_f32_lp(void)
{
    uint32_t i;
    arm_biquad_casd_df1_inst_f32 S;
    float32_t ScaleValue;
    float32_t  *inputF32, *outputF32;
    
    /* ��ʼ�������������ָ�� */
    inputF32 = &testInput_f32_50Hz_200Hz[0];
    outputF32 = &testOutput[0];
    
    
    /* ��ʼ�� */
    arm_biquad_cascade_df1_init_f32(&S, numStages, (float32_t *)&IIRCoeffs32LP[0], 
(float32_t *)&IIRStateF32[0]);
    
    /* ʵ��IIR�˲�������ÿ�δ���1���� */
    for(i=0; i < numBlocks; i++)
    {
        arm_biquad_cascade_df1_f32(&S, inputF32 + (i * blockSize),  outputF32 + (i * blockSize),
  blockSize);
    }
            
    /*����ϵ�� */
    ScaleValue = 0.052219514664161220673932461977528873831f * 0.042798017416583809813257488485760404728f ; 
    
    /* ��ӡ�˲����� */
    for(i=0; i<TEST_LENGTH_SAMPLES; i++)
    {
//        printf("%f, %f\r\n", testInput_f32_50Hz_200Hz[i], testOutput[i]*ScaleValue);
    }
}



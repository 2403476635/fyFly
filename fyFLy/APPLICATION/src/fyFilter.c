#include "fyFilter.h"
#define numStages  2                /* 2��IIR�˲��ĸ��� */
#define TEST_LENGTH_SAMPLES  400    /* �������� */
#define BLOCK_SIZE           1    	/* ����һ��arm_biquad_cascade_df1_f32����Ĳ�������� */


uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;      /* ��Ҫ����arm_biquad_cascade_df1_f32�Ĵ��� */


static float32_t testInput_f32_50Hz_200Hz[TEST_LENGTH_SAMPLES]; /* ������ */
static float32_t testOutput[TEST_LENGTH_SAMPLES];               /* �˲������� */
static float32_t IIRStateF32[4*numStages];                      /* ״̬���� */

/* ������˹��ͨ�˲���ϵ�� 80Hz*/                                                                                                                                         
const float32_t IIRCoeffs32LP[5*numStages] = {                                                                                 
    1.0f,  2.0f,  1.0f,  0.897657940036644674819399369880557060242f,  
-0.527186904631597452741686993249459192157f,

    1.0f,  2.0f,  1.0f,  0.672740911191527235502007897594012320042f,  
-0.144535199833120991952384315482049714774f                                                                                              
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
	ScaleValue = 0.15738224114873822223614752147113904357f * 0.117948572160398459929275816193694481626f ;
	*outputF32 = *outputF32 * ScaleValue;
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
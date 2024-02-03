#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#define  RINGBUFF_LEN          (1024)     //定义最大接收字节数 1k
#define  RINGBUFF_OK           1
#define  RINGBUFF_ERR          0


typedef struct
{
    unsigned short Head;
    unsigned short Tail;
    unsigned short Lenght;
    unsigned char  RingBufferData[RINGBUFF_LEN];
}__attribute__((packed)) _RingBuffer;

void RingBuffer_Init(_RingBuffer *ringbuffer);
unsigned char WriteOneByte_RingBuffer(_RingBuffer *ringbuffer, unsigned char OneByteData);
unsigned char WriteBytes(_RingBuffer *ringbuffer, unsigned char *Bytes,unsigned short DataLen);
unsigned char ReadOneByte_RingBuffer(_RingBuffer *ringbuffer,unsigned char *OneByteData);
unsigned char ReadBytes(_RingBuffer *ringbuffer,unsigned char *ReadBuffer, unsigned short Len);
unsigned short GetDataLength(unsigned char *DataBuffer);
unsigned char FrameDataCheck(unsigned char* OneFrameData);


/* 缓存内存地址在SRAM内存上，大小:16Kbytes */
extern _RingBuffer VCP_RxRingBuffer __attribute__((at(0x2001C000))); 

#endif /* __RINGBUFFER_H__ */





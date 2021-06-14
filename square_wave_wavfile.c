/* square_wave_wavfile.c
 * 
 * create a square wave wav audio file
 * 
 * author: Yangsheng Wang
 * wang_yangsheng@163.com
 * 
 * coding in 2021/6/14
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

struct wav_file_head_info {
	uint32_t chunk_id; /* "RIFF" */
	uint32_t chunk_size; /* 36 + subchunk2_size */
	uint32_t format; /* "WAVE" */

	/* sub-chunk "fmt" */
	uint32_t subchunk1_id; /* "fmt " */
	uint32_t subchunk1_size; /* 16:PCM */
	uint16_t audio_format; /* PCM:1 */
	uint16_t num_channels; /* mono:1, stereo:2 */
	uint32_t sample_rate; /* 8000 16000 ... 44100 48000 */
	uint32_t byte_rate; /* equal sample_rate * num_channels * bits_persample/8 */
	uint16_t block_align; /* equal num_channels * bits_persample/8 */
	uint16_t bits_persample; /* 8bits, 16bits, etc. */

	/* sub-chunk "data" */
	uint32_t subchunk2_id; /* "data" */
	uint32_t subchunk2_size; /* data size */
};


int main(int argc, char *argv[])
{
	int fd_des; 
	struct wav_file_head_info wav;
	char *buf_des;
	int i;
	unsigned char wave;
	int ret;
	char file_name[128] = {0};
	short *value;
	int sample_rate;
	int play_time;
	int SAMPLE_RATE;
	int PLAY_TIME;

/* #define SAMPLE_RATE 48000 */
#define SAMPLE_BIT 16
#define SAMPLE_BYTE 2
#define NUMBER_OF_CHANNEL 2

/* #define PLAY_TIME 3 */
#define AUDIO_FREQUENCY 1000


	if (argc < 2) {
		printf("%s [sample rate] [play time second]\n", argv[0]);
		return -1;
	}

	sample_rate = atoi(argv[1]);
	printf("sample rate:%d\n", sample_rate);
	if (sample_rate < 8000) {
		printf("sample rate should biger 8k\n");
		return -1;
	}

	play_time = atoi(argv[2]);
	printf("play time:%d\n", play_time);

	SAMPLE_RATE = sample_rate;
	PLAY_TIME = play_time;

	wav.chunk_id = 0x46464952;
	wav.chunk_size = SAMPLE_RATE * NUMBER_OF_CHANNEL * SAMPLE_BYTE * (PLAY_TIME) + 36;
	wav.format = 0x45564157;
	wav.subchunk1_id = 0x20746d66;
	wav.subchunk1_size = 16;
	wav.audio_format = 1;
	wav.num_channels = NUMBER_OF_CHANNEL;
	wav.sample_rate = SAMPLE_RATE;
	wav.byte_rate = SAMPLE_RATE * NUMBER_OF_CHANNEL * SAMPLE_BYTE;
	wav.block_align = NUMBER_OF_CHANNEL * SAMPLE_BYTE;
	wav.bits_persample = SAMPLE_BIT;
	wav.subchunk2_id = 0x61746164;
	wav.subchunk2_size = SAMPLE_RATE * NUMBER_OF_CHANNEL * SAMPLE_BYTE * (PLAY_TIME);

	printf("chunk_id \t%x\n", wav.chunk_id);
	printf("chunk_size \t%d\n", wav.chunk_size);
	printf("format \t\t%x\n", wav.format);
	printf("subchunk1_id \t%x\n", wav.subchunk1_id);
	printf("subchunk1_size \t%d\n", wav.subchunk1_size);
	printf("audio_format \t%d\n", wav.audio_format);
	printf("num_channels \t%d\n", wav.num_channels);
	printf("sample_rate \t%d\n", wav.sample_rate);
	printf("byte_rate \t%d\n", wav.byte_rate);
	printf("block_align \t%d\n", wav.block_align);
	printf("bits_persample \t%d\n", wav.bits_persample);
	printf("subchunk2_id \t%x\n", wav.subchunk2_id);
	printf("Subchunk2_size \t%d\n", wav.subchunk2_size);

	buf_des = (char *)malloc(wav.subchunk2_size);
	memset(buf_des, 0, wav.subchunk2_size);

	wave = 0x00;
	i = 0;
	do {
		if (i % (SAMPLE_RATE * NUMBER_OF_CHANNEL * SAMPLE_BYTE / AUDIO_FREQUENCY / NUMBER_OF_CHANNEL) == 0)
			wave = ~wave;

		if (wave == 0xff) {
			value = (short *)(buf_des + i);
			*value = (65536 / 8);
		} else {
			value = (short *)(buf_des + i);
			*value = -(65536 / 8);
		}

		i += 2;
	} while (i < wav.subchunk2_size);

	sprintf(file_name, "square_wave-s%d-%dhz-%ds.wav", SAMPLE_RATE, AUDIO_FREQUENCY, PLAY_TIME);
	fd_des = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0666);

	ret = write(fd_des, &wav, sizeof(struct wav_file_head_info));
	printf("%s() %d->ret:%d.\n", __FUNCTION__, __LINE__, ret);
	ret = write(fd_des, buf_des, wav.subchunk2_size);
	printf("%s() %d->ret:%d.\n", __FUNCTION__, __LINE__, ret);

	free(buf_des);

	ret = close(fd_des);
	printf("%s() %d->ret:%d.\n", __FUNCTION__, __LINE__, ret);

	return 0;  
}

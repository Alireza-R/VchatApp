#include "SendClientThread.h"
#include "ServerConnectionThread.h"

#define SAMPLE_RATE (44100)
#define FRAMES_BUFFER (1024)
#define OVERFLOW (0)
#define SAMPLE_SIZE (4)
#define CLEAR(a) memset((a), 0, FRAMES_BUFFER * SAMPLE_SIZE * 2)
 
 

void* SendClientThread(void* data) // send udp packets containing message/voice to the other client
{
	struct thread* new_data = data;	
	printf("you are sending to %s IP address\n", (char*)new_data->IPaddr);

	PaStreamParameters inputParam;
	PaStream *stream = NULL;
	char *sampleBlock;
	int i, sockfd, numBytes;
	PaError error;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); // make udp socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(new_data->IPaddr);
	servaddr.sin_port = htons(new_data->Port);

	fflush(stdout); 
	numBytes = FRAMES_BUFFER * SAMPLE_SIZE * 2;
	sampleBlock = (char *) malloc(numBytes);
	if(sampleBlock == NULL){
		printf("Could not allocate record array.\n");
		exit(1);
	}
	CLEAR(sampleBlock);
 
	printf("Initializing the input devices...\n"); 
	error = Pa_Initialize();
	if(error != paNoError) 
		goto errorState;
 
	inputParam.device = Pa_GetDefaultInputDevice(); 
	printf("Input device # %d.\n", inputParam.device);
	inputParam.channelCount = 2;
	inputParam.sampleFormat = paFloat32;
	inputParam.suggestedLatency = Pa_GetDeviceInfo( inputParam.device )->defaultHighInputLatency;
	inputParam.hostApiSpecificStreamInfo = NULL;
 
	error = Pa_OpenStream(&stream, &inputParam, NULL, SAMPLE_RATE, FRAMES_BUFFER, paClipOff, NULL, NULL );
  
	if( error != paNoError ) 
		goto errorState;
 
	error = Pa_StartStream( stream );
	if( error != paNoError ) 
		goto errorState;

	printf("you can now talk to the mic\n"); 
	fflush(stdout);
	
	while(1){
		error = Pa_ReadStream(stream, sampleBlock, FRAMES_BUFFER);
		if(error && OVERFLOW) 
			goto exitState;
		sleep(0.01);
		sendto(sockfd, sampleBlock, 4096, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); // send packet containing voice data to the other client
	}

	error = Pa_StopStream(stream);
	if(error != paNoError) 
		goto errorState;
 
	CLEAR(sampleBlock);
	free(sampleBlock);
	Pa_Terminate();
	return 0;
 
exitState:
	if(stream){
		Pa_AbortStream(stream);
		Pa_CloseStream(stream);
	}
	close(sockfd);
	free(sampleBlock);
	Pa_Terminate();
	if(error & paInputOverflow)
		fprintf(stderr, "Input Overflow.\n");
	if(error & paOutputUnderflow)
		fprintf(stderr, "Output Underflow.\n");
	return (void*)-2;
 
errorState:
	if(stream){
		Pa_AbortStream(stream);
		Pa_CloseStream(stream);
	}
	close(sockfd);	
	free(sampleBlock);
	Pa_Terminate();
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", error);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(error));
	return (void*)-1;
}

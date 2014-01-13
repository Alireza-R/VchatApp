#include "RecieveClientThread.h"
#include "ServerConnectionThread.h"

#define SAMPLE_RATE (44100)
#define FRAMES_BUFFER (1024)
#define UNDERFLOW (0)
#define SAMPLE_SIZE (4)
#define CLEAR(a) memset((a), 0, FRAMES_BUFFER * SAMPLE_SIZE * 2)

void* RecieveClientThread(void* data) // recieve udp packets containing message/voice to the other client
{
	struct thread* new_data = data;	

	printf("you are recieving from %s IP address\n", new_data->IPaddr);
	PaStreamParameters outputParam;
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

	bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	fflush(stdout); 
	numBytes = FRAMES_BUFFER * SAMPLE_SIZE * 2;
	sampleBlock = (char *) malloc(numBytes);
	if(sampleBlock == NULL){
		printf("Could not allocate record array.\n");
		exit(1);
	}
	CLEAR(sampleBlock);
 
	printf("Initializing the devices...\n"); 
	error = Pa_Initialize();
	if(error != paNoError) 
		goto errorState;
 
	outputParam.device = Pa_GetDefaultOutputDevice();
  	printf( "Output device # %d.\n", outputParam.device );
	outputParam.channelCount = 2;
	outputParam.sampleFormat = paFloat32;
	outputParam.suggestedLatency = Pa_GetDeviceInfo( outputParam.device )->defaultHighOutputLatency;
	outputParam.hostApiSpecificStreamInfo = NULL;	
 
	error = Pa_OpenStream(&stream, NULL, &outputParam, SAMPLE_RATE, FRAMES_BUFFER, paClipOff, NULL, NULL );
  
	if( error != paNoError ) 
		goto errorState;
 
	error = Pa_StartStream( stream );
	if( error != paNoError ) 
		goto errorState;

	printf("The client is talking to you\n"); 
	fflush(stdout);

	while(1){
		recvfrom(sockfd, sampleBlock, 4096, 0, NULL, NULL); // recieve packet containing voice data from the other client
		error = Pa_WriteStream(stream, sampleBlock, FRAMES_BUFFER);
 		if(error && UNDERFLOW) 
			goto exitState;
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
	free(sampleBlock);
	Pa_Terminate();
	if(error & paOutputUnderflow)
		fprintf(stderr, "Output Underflow.\n");
	return (void*)-2;
 
errorState:
	if(stream) {
		Pa_AbortStream(stream);
		Pa_CloseStream(stream);
	}
	free(sampleBlock);
	Pa_Terminate();
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", error);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(error));
	return (void*)-1;

}

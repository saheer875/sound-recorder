#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

#define SAMPLE_RATE 58000
#define NUM_SECONDS 4.5
#define FRAMES_PER_BUFFER 256
#define NUM_CHANNELS 1
#define FORMAT paFloat32

static int recordCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData) {
    FILE *fp = (FILE *)userData;
    float *in = (float *)inputBuffer;
    
    if(inputBuffer == NULL) {
        for(unsigned long i = 0; i < framesPerBuffer; i++) {
            // Write silent data if input buffer is NULL
            float silent = 0;
            fwrite(&silent, sizeof(float), 1, fp);
        }
    } else {
        // Write the actual buffer to the file
        fwrite(in, sizeof(float), framesPerBuffer, fp);
    }

    return paContinue;
}

int main(void) {
    PaStream *stream;
    PaError err;
    FILE *fp;

    fp = fopen("output.raw", "wb");
    if (!fp) {
        fprintf(stderr, "Could not open file for writing\n");
        return 1;
    }

    err = Pa_Initialize();
    if (err != paNoError) goto error;

    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default input device.\n");
        goto error;
    }

    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = FORMAT;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, &inputParameters, NULL, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, recordCallback, fp);
    if (err != paNoError) goto error;

    err = Pa_StartStream(stream);
    if (err != paNoError) goto error;

    printf("Recording...\n");
    Pa_Sleep(NUM_SECONDS * 1000);

    err = Pa_StopStream(stream);
    if (err != paNoError) goto error;

    err = Pa_CloseStream(stream);
    if (err != paNoError) goto error;

    Pa_Terminate();
    fclose(fp);
    printf("Finished recording.\n");

    return 0;

error:
    Pa_Terminate();
    fprintf(stderr, "An error occurred while using the portaudio stream\n");
    fprintf(stderr, "Error number: %d\n", err);
    fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
    if (fp) fclose(fp);
    return -1;
}

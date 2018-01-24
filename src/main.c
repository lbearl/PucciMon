#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "twilio.h"
#include "pi_2_dht_read.h"

float cToF(float c){
	return (9.0/5.0 * c) + 32.0;
}

void sendMessage(char* message, char* toNumber){
	char* twilio_sid = getenv("TWIL_WEATHER_SID");
	char* twilio_auth_token = getenv("TWIL_WEATHER_AUTH");
	char* twilio_from_number = getenv("TWIL_WEATHER_NUMBER");
	twilio_send_message(twilio_sid,
			twilio_auth_token,
			message,
			twilio_from_number,
			toNumber,
			NULL,
			false);
}

int main(int argc, char *argv[])
{
	puts("starting application\n");
	bool forceSend = false;
	bool output = false;
	int opt;       
	while((opt = getopt(argc, argv, "f")) != -1)
		switch(opt){
			case 'f':
				forceSend = true; break;
			default:
				break;
		}

	printf("ForceSend: %d; Output %d\n", forceSend, output);
	// This should be a comma seperated list of phone numbers in E.164 format (+1XXXYYYZZZZ for USA).
	char* to_numbers = getenv("TWIL_WEATHER_TONUMS");
	float humidity = 0;
	float temperature = 0;

	//The read operation can fail. Let's add some some retry logic
	for(int i =0; i< 10; i++)
	{
		// 22 = AM2302, 4= GPIO 4 (physical pin 7)
		pi_2_dht_read(22, 4, &humidity, &temperature);
		if(temperature > 0.0 && humidity > 0.0)
			break;
		// Per documentation, AM2302 should only be read once every 2 seconds.
		sleep(2);
	}

	char message[500];
	sprintf(message, "PUCCIMON:\nCurrent temperature is %2.2f°F\nCurrent humidity is %2.2f%%\nThreshold is 86°F", cToF(temperature), humidity);



	puts("Writing file for output mode");
	FILE *f = fopen("/opt/lbearl/temp.txt", "w");
	if(f == NULL){
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf(f, message);
	fclose(f);
	puts(message);

	// Now process SMS if necessary. Default threshold is 30C/86F
	printf("Parsing %s", to_numbers);
	if((strlen(to_numbers) > 1) && (forceSend || temperature > 30.0)){
		char *token = strtok(to_numbers, ",");

		while(token != NULL)
		{
			printf("sending message to %s", token);
			sendMessage(message, token);		        
			token = strtok(NULL, ",");                            
		}
	}


	return 0;
}

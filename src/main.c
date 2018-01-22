#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rrd.h>

#include "twilio.h"
#include "pi_2_dht_read.h"

float cToF(float c){
        return (9.0/5.0 * c) + 32.0;
}

int main(int argc, char *argv[])
{
	bool forceSend = false;
        char* toNumber;
        int opt;       
	while((opt = getopt(argc, argv, "ft:")) != -1)
        	switch(opt){
			case 'f':
				forceSend = true; break;
			case 't':
				toNumber = optarg; break;
			default:
				exit(1);
		}

	printf("Starting the Weather Monitor service...\n");
	char* twilio_sid = getenv("TWIL_WEATHER_SID");
	char* twilio_auth_token = getenv("TWIL_WEATHER_AUTH");
	char* twilio_from_number = getenv("TWIL_WEATHER_NUMBER");
        float humidity = 0;
        float temperature = 0;
	// 22 = AM2302, 4= GPIO 4 (physical pin 7)
 	pi_2_dht_read(22, 4, &humidity, &temperature);

 	printf("Humidity: %2.2f, Temperature: %2.2f\n", humidity, temperature);

	// Now process SMS if necessary.
	if(forceSend || true){
	char message[140];
	sprintf(message, "Current temperature is %2.2f°F\nCurrent humidity is %2.2f%%", cToF(temperature), humidity);
	printf("Sending sms...\n");
	twilio_send_message(twilio_sid, 
			    twilio_auth_token,
			    message,
                            twilio_from_number,
                            toNumber,
                            NULL,
                            true);                            
	}

	
	return 0;
}

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

#include <env.h>

#include <addons/RTDBHelper.h>

FirebaseData fbdo;

FirebaseAuth auth;

FirebaseConfig config;

int pushupTarget = -1;
int fetchedValue = 0;
int reset = 0;


void setup()
{

    Serial.begin(9600);
    Serial2.begin(115200); // comms 

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the certificate file (optional) */
    // config.cert.file = "/cert.cer";
    // config.cert.file_storage = StorageType::FLASH;

    config.database_url = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_SECRET;

    Firebase.reconnectNetwork(true);

    fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

    Firebase.begin(&config, &auth);


    // Serial.printf("Get int ref... %s\r\n", Firebase.getInt(fbdo, F("/pushups/target"), &pushupTarget) ? "ok" : fbdo.errorReason().c_str());

    // Serial.printf("Initial target: %d\n", pushupTarget);
    // Serial2.printf("%d\n", pushupTarget);

}

void loop()
{

    Firebase.getInt(fbdo, F("/pushups/target"), &fetchedValue);
    Firebase.getInt(fbdo, F("/pushups/reset"), &reset);

    if(reset){
        Serial2.printf("%c", 'J');
        pushupTarget = -1;
        fetchedValue = -1;
        Firebase.setInt(fbdo, F("/pushups/target"), -1);
        Firebase.setInt(fbdo, F("/pushups/reset"), 0);
        Serial.print("Resetting from Firebase\n");
    }

    if((fetchedValue > 0 || pushupTarget > 0) && fetchedValue != pushupTarget){
        pushupTarget = fetchedValue;
        Serial2.printf("%d\n", pushupTarget);
        Serial.printf("New target: %d\n", pushupTarget);
    }
}

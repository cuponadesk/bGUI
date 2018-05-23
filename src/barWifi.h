#ifndef _BARWIFI_H_
#define _BARWIFI_H_
#include <memory>
#include "ESP8266WiFi.h"

#define MAX_SSID_LENGTH          32
#define MAX_PASSWORD_LENGTH      32
#define SCAN_LIST_MAX             6
#define RETRY_WAIT            45000
#define ON                        1
#define OFF                       0



static const char SAVED[] = "SAVED";

class barWifi {
public:


struct WiFiStation {
        std::string ssid, pswd="";
        int rssi;
        int8_t enc_type;
};



barWifi();
bool * conn, * strAP;
wl_status_t begin(uint16_t rom_size); //reads EEPROM flags and sets up wifi. Needs size of eeprom for reading saved wifi crds
bool save(const char * name, const char * pass ); //save passed network in eeprom if space available
void saveAP(); //saved current ap configuration to eeprom
void maintain();  //monitors connection and maintains desired settings
const char * saved(uint8_t id = 0); // returns ssid at save location id
void keepAlive(bool keepAlive = true); //use saved wifi cred as backup if connection fails
void connectOnBoot(bool connect_on_startup = true); //connect on startup flag
wl_status_t wifi(bool make_connection); //starts or stops wifi connection
wl_status_t wifi(const char * ssid = SAVED, const char * password = SAVED); //set ssid and password for connection
wl_status_t accessPoint(bool access_point);//starts or stops access point
wl_status_t accessPoint(char * access_point_name, char * access_point_password = "", IPAddress ip = IPAddress(192, 168, 4, 1), IPAddress netmask = IPAddress(255, 255, 255, 0) ); //configures access point
wl_status_t status(); //returns status of wifi connection
uint8_t getScanned(); //return number of scanned units
const char * getScanned(uint8_t slot);//returns name of wifi station in list
std::string active();//return network currently connected to


private:

bool b_keep_alive, b_connect, b_access_point, b_connect_on_boot, b_ap_started, b_part_of_mesh, b_ap_on_boot;
wl_status_t m_wifi_status;
uint16_t ii_rom_size=512;
uint32_t iii_next_connect_attempt=0;
IPAddress ip_ap_ip;
IPAddress ip_ap_netmask;
std::string s_password, s_ssid, s_ap_name, s_ap_pswd;
WiFiStation ws_list[SCAN_LIST_MAX],ws_saved[4+1];


void loadEEPROM(char * linked_var, uint16_t start_location, uint8_t read_size);
uint8_t loadEEPROM(uint16_t start_location);
void writeEEPROM(const char * linked_var, uint16_t start_location, uint8_t write_size);
void writeEEPROM(uint8_t value, uint16_t start_location);
void scanNetworks();
void load();
void saveHelper();
uint8_t findEmptySlot();
uint8_t alreadySaved(const char * name);
wl_status_t connect();
uint8_t findSavedNetwork();
void setFromSaved(uint8_t slot);
barWifi::WiFiStation findSaveSlot(uint8_t slot);




};
#endif

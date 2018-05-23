#include "barWifi.h"
#include "EEPROM.h"

barWifi::barWifi()
{
        ;
}

wl_status_t barWifi::begin(uint16_t rom_size)
{
        WiFi.disconnect();
        WiFi.softAPdisconnect();
        ii_rom_size = rom_size;
        b_ap_started=false;
        uint8_t temp;
        temp=loadEEPROM(ii_rom_size-1);
        b_connect_on_boot = (temp%2)>0;
        b_connect = (temp%2)>0;
        b_keep_alive = (temp%4)>1;
        b_ap_on_boot = (temp%8)>3;
        b_part_of_mesh = (temp%16)>7;

        Serial.printf("Setup. Read: %i  \nConnect on Boot? %i  Keep? %i AP? %i\n", temp, b_connect_on_boot, b_keep_alive, b_ap_on_boot);
        this->load();
        Serial.println("Load complete.");

        s_ssid = "";
        s_ap_name=ws_saved[4].ssid;
        s_ap_pswd=ws_saved[4].pswd;

        if(b_ap_on_boot)
        {
                accessPoint(ON);
        }
        if(b_connect_on_boot)
        {
                Serial.println("Connect on boot.");
                this->scanNetworks();
                Serial.println("Scan complete");

                uint8_t found_network = findSavedNetwork();
                setFromSaved(found_network);

                return this->connect();

        }
        return WL_DISCONNECTED;
}

uint8_t barWifi::findSavedNetwork()
{
        for(int x = 0; ws_saved[x].ssid!="" && s_ssid == "" && x < 4; x++)
        {
                for(int y = 0; ws_list[y].ssid != "" && y < SCAN_LIST_MAX; y++)
                {
                        Serial.printf("Comparing %s and %s\n",ws_saved[x].ssid.c_str(),ws_list[y].ssid.c_str());
                        if(ws_list[y].ssid.length()>1 && ws_saved[x].ssid==ws_list[y].ssid)
                        {
                                Serial.println("Match found.");
                                s_ssid = ws_saved[x].ssid;
                                s_password = ws_saved[x].pswd;
                                return x;
                        }
                }
        }
        return 255;
}


uint8_t barWifi::loadEEPROM(uint16_t start_location)
{
        EEPROM.begin(ii_rom_size);
        uint8_t temp = EEPROM.read(start_location);
        EEPROM.end();
        Serial.printf("Read char %c at %i\n", char(temp), start_location);
        return temp;
}

void barWifi::loadEEPROM(char * linked_var, uint16_t start_location, uint8_t read_size)
{

        for (int x = 0; x < read_size; x++)
        {
                linked_var[x] = loadEEPROM(start_location+x);
        }
}

void barWifi::writeEEPROM(uint8_t value, uint16_t start_location)
{
        EEPROM.begin(ii_rom_size);
        EEPROM.write(start_location,value);
        Serial.printf("Write char %c at %i\n", char(value), start_location);
        EEPROM.commit();
        EEPROM.end();
}

void barWifi::writeEEPROM(const char * linked_var, uint16_t start_location, uint8_t write_size)
{
        for (int x = 0; x < write_size; x++)
        {
                writeEEPROM(linked_var[x], start_location+x);
        }

}



void barWifi::scanNetworks()
{
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        uint8_t numNetworks = WiFi.scanNetworks();
        for(uint8_t x = 0; x<numNetworks-1 && x < SCAN_LIST_MAX; x++)
        {
                Serial.printf("Name: %s Strength: %i \n", WiFi.SSID(x).c_str(), WiFi.RSSI(x) );
                ws_list[x].ssid=std::string(WiFi.SSID(x).c_str());
                ws_list[x].rssi = WiFi.RSSI(x);
                ws_list[x].enc_type = WiFi.encryptionType(x);

        }
}

void barWifi::load()
{
        for(int x = 0; x < 4; x++)
        {
                ws_saved[x].ssid = "";
                ws_saved[x].pswd = "";
        }
        uint8_t c;
        uint16_t eeprom_loc = ii_rom_size-2, eeprom_end=eeprom_loc;
        for(int x = 0; x < 5; x++)
        {
                EEPROM.read(eeprom_loc);
                //  Serial.print('\n');
                do {
                        if( eeprom_loc < eeprom_end - MAX_SSID_LENGTH )
                        {
                                Serial.printf("No station at %i\n", x);
                                ws_saved[x].ssid="";
                                return;
                        }
                        c = loadEEPROM(eeprom_loc);
                        eeprom_loc--;

                        if ( c == '|')
                                break;
                        //  Serial.print(char(c));
                        ws_saved[x].ssid += char(c);


                } while(1);
                Serial.printf("\nFound %s location %i\n", ws_saved[x].ssid.c_str(), x);
                eeprom_end = eeprom_loc;

                do {
                        if( eeprom_loc < eeprom_end - MAX_PASSWORD_LENGTH )
                        {
                                Serial.printf("No password at %i\n", x);
                                ws_saved[x].pswd="";
                                return;
                        }
                        c = loadEEPROM(eeprom_loc);
                        eeprom_loc--;
                        if ( c == ':')
                        {
                                break;
                        }
                        Serial.print(char(c));
                        ws_saved[x].pswd += char(c);
                } while(1);

        }

}

wl_status_t barWifi::connect()
{
        if(millis()<iii_next_connect_attempt)
                return WL_CONNECT_FAILED;
        Serial.printf("Connecting as wifi client to %s Password: %s", s_ssid.c_str(), s_password.c_str());
        WiFi.begin ( s_ssid.c_str(), s_password.c_str() );
        WiFi.waitForConnectResult();
        m_wifi_status = WiFi.status();
        iii_next_connect_attempt=millis()+RETRY_WAIT;
        Serial.printf("Status: %i\n", WiFi.status());
        return m_wifi_status;
}

uint8_t barWifi::findEmptySlot()
{
        for(int x = 0; x< 4; x++)
        {
                if(ws_saved[x].ssid=="")
                        return x+1;
        }

        return 0;
}

uint8_t barWifi::alreadySaved(const char * name)
{
        for(int x = 0; x < 4; x++)
        {
                if(!strcmp(name, ws_saved[x].ssid.c_str()))
                {
                        Serial.printf("%s exists at %i\n", name, x);
                        return x+1;
                }
        }
        Serial.printf("%s doesn't exists\n", name);
        return 0;
}

bool barWifi::save(const char * name, const char * pass)
{
        Serial.printf("Saving %s\n", s_ssid.c_str());
        Serial.printf("Slot 1 %s %s\n", ws_saved[0].ssid.c_str(), ws_saved[0].pswd.c_str());
        Serial.printf("Slot 2 %s %s\n", ws_saved[1].ssid.c_str(), ws_saved[1].pswd.c_str());
        Serial.printf("Slot 3 %s %s\n", ws_saved[2].ssid.c_str(), ws_saved[2].pswd.c_str());
        Serial.printf("Slot 4 %s %s\n", ws_saved[3].ssid.c_str(), ws_saved[3].pswd.c_str());
        Serial.printf("Slot 5 %s %s\n", ws_saved[4].ssid.c_str(), ws_saved[4].pswd.c_str());
        uint8_t location = alreadySaved(name);
        if(location && ws_saved[location-1].pswd == pass)
        {
                return true;
        }
        else if(location)
        {
                ws_saved[location-1].pswd = pass;
                saveHelper();
                return true;
        }

        location = findEmptySlot();
        if(location)
        {
                ws_saved[location-1].ssid=name;
                ws_saved[location-1].pswd=pass;
                saveHelper();
                return true;
        }
        else
        {
                return false;
        }
}

void barWifi::saveAP()
{
        ws_saved[4].ssid=s_ap_name;
        ws_saved[4].pswd=s_ap_pswd;
        saveHelper();
}

void barWifi::saveHelper()
{
        Serial.println("here");
        uint16_t write_location=ii_rom_size-1;
        uint8_t firstByte = b_connect_on_boot;
        firstByte += (b_keep_alive)*2;
        firstByte += (b_access_point)*4;
        writeEEPROM(firstByte, write_location);
        write_location--;
        for(int x = 0; x< 5; x++)
        {
                for(int y = 0; y < ws_saved[x].ssid.length(); y++)
                {
                        writeEEPROM(ws_saved[x].ssid[y], write_location-y);
                }
                write_location-=ws_saved[x].ssid.length();
                writeEEPROM('|', write_location);
                write_location--;
                for(int y = 0; y < ws_saved[x].pswd.length(); y++)
                {
                        writeEEPROM(ws_saved[x].pswd[y], write_location-y);
                }
                write_location-=ws_saved[x].pswd.length();
                writeEEPROM(':', write_location);
                write_location--;
        }
}

void barWifi::setFromSaved(uint8_t slot)
{
        if(slot < 4)
        {
                s_ssid = ws_saved[slot].ssid;
                s_password = ws_saved[slot].pswd;
        }
}

void barWifi::maintain()
{
        if(!*conn && b_connect)//disconnect
        {
                Serial.println("discp");
                WiFi.disconnect();
                iii_next_connect_attempt=millis();
                b_connect = *conn;
        }
        else if(*conn && !b_connect && s_ssid !="")
        {
                b_connect =*conn;
        }

        if(b_connect && WiFi.status() != WL_CONNECTED && s_ssid != "")
        {
                connect();
        }
        else if(b_connect && WiFi.status() != WL_CONNECTED && b_keep_alive)
        {
                this->scanNetworks();
                uint8_t newNetwork = findSavedNetwork();
                setFromSaved(newNetwork);
                connect();
        }

        if(b_access_point != b_ap_started && (b_access_point && s_ap_name!=""))
        {
                accessPoint(b_access_point);
        }


}

const char * barWifi::saved(uint8_t id){
        if(id<4)
                return ws_saved[id].ssid.c_str();
        else
                return NULL;
}


void barWifi::keepAlive(bool keepAlive) {
        b_keep_alive = keepAlive;
};

void barWifi::connectOnBoot(bool connect_on_startup ) {
        b_connect_on_boot = connect_on_startup;
};
wl_status_t barWifi::wifi(bool make_connection) {
        b_connect = make_connection;
        if(b_connect)
                return this->connect();
        else
                WiFi.disconnect();
};
wl_status_t barWifi::wifi(const char * ssid, const char * password ) {
        s_ssid = ssid; s_password = password;
};
wl_status_t barWifi::accessPoint(bool access_point) {
        b_access_point = access_point;
        if(b_access_point && s_ap_name != "" )
        {
                Serial.printf("AP started: %i\n",WiFi.softAP(s_ap_name.c_str(), s_ap_pswd.c_str()));
                delay(500);
                b_ap_started=true;
        }
        else
        {
                Serial.println("AP stopped.");
                WiFi.softAPdisconnect();
                b_ap_started=false;
        }


};

wl_status_t barWifi::accessPoint(char * access_point_name, char * access_point_password, IPAddress ip, IPAddress netmask)
{
        s_ap_name = access_point_name;
        s_ap_pswd = access_point_password;
        ip_ap_ip = ip;
        ip_ap_netmask = netmask;
};


wl_status_t barWifi::status(){
        return m_wifi_status;
};

uint8_t barWifi::getScanned()
{
        for(int x = 0; x < SCAN_LIST_MAX; x++)
        {
                if(ws_list[x].ssid == "")
                        return x;
        }
}

const char * barWifi::getScanned(uint8_t slot)
{
        if(slot < SCAN_LIST_MAX )
        {
                return ws_list[slot].ssid.c_str();
        }
        return "";
}

std::string barWifi::active()
{
        return s_ssid;

}

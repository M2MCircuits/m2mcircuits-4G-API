#include "MakerModem.h"


bool MakerModem::isGPRSAttached() {
	string resp;
	resp = sendCommand("AT+UPSND=0,8");
	//cout << resp << endl;
	int tmp;
	if ((tmp = resp.find("SND: 0,8,")) >= 0) {
		return resp[tmp + 9] == '1';
	}
	return false;
}
bool MakerModem::attachGPRS() {
	if (isGPRSAttached()) return true;
	string resp;
	resp = sendCommand("AT+UPSDA=0,2");
	resp = sendCommand("AT+UPSDA=0,3");
	int tmp;
	if ((tmp = resp.find("SND: 0,8,")) >= 0) {
		return resp[tmp + 9] == '1';
	}
	return false;
}
int MakerModem::socketOpen(bool tcp) {
	string cmd = "AT+USOCR=";
	if (tcp) cmd += "6";
	else cmd += "17";
	string ret = sendCommand(cmd.c_str());//6 for tcp
	int tmp;
	cout << "Open socket: ";
	cout << ret << endl;
	if ((tmp = ret.find("USOCR: ")) >= 0) {
		return atoi(&ret[tmp + 7]);
	}
	return -1;
}
bool MakerModem::socketConnect(int sid, const char *dest, int port) {
	string resp;
	string cmd = "AT+USOCO=";
	cmd += std::to_string(sid);
	cmd += ",";
	cmd += dest;
	cmd += ",";
	cmd += std::to_string(port);
	resp = sendCommand(cmd.c_str());
	if (resp.find("OK") >= 0) return true;
	return false;

}
int MakerModem::socketWrite(int sid, const char *buff, int len) {
	string resp;
	string cmd = "AT+USOWR=";
	cmd += std::to_string(sid);
	cmd += ",";
	string cmd2 = cmd;
	cmd += std::to_string(len&1023);
	cmd2 += "1024";
	string findStr = "USOWR: ";
	findStr += to_string(sid);
	findStr += ",";
	int wrote = 0;
	int index;
	while(len > 0){
		resp = sendCommand(cmd2.c_str());
		if (resp[0] == '@') {
			SP->writeData(buff, 1024);
			resp = sendCommand("");
			if ((index = resp.find(findStr)) >= 0) {
				index = atoi(&resp.c_str()[index + findStr.length()]);
				wrote += index;
				len -= index;
			}
			else {
				return wrote;
			}
		}
		else {
			return wrote;
		}
	}
	return -1;

}
int MakerModem::socketRead(int sid, char *buff, int len) {
	if (len == 0) return 0;
	string cmd = "AT+USORD=";
	cmd += to_string(sid);
	cmd += ",";
	if (len > 1024) len = 1024;
	cmd += to_string(len);
	cmd += "\r\n";
	string resp = sendCommand(cmd.c_str());

	SP->readData(buffer, BUFFER_SIZE);
	resp = buffer;
	int read = 0;
	int index = resp.find("USORD: ");
	if (index >= 0) {
		index += 7;
		read = atoi(&resp.c_str()[index+2]);
		index = resp.find("\"");
		memcpy(buff, &resp.c_str()[index + 1], read);
		return read;
	}

	return -1;

}
int MakerModem::socketAvail(int sid) {
	string cmd = "AT+USORD=";
	cmd += to_string(sid);
	cmd += ",0";
	string resp = sendCommand(cmd.c_str());
	int index;
	if ((index = resp.find("USORD: "))>=0) {
		return atoi(&resp.c_str()[index + 9]);
	}
	return -1;

}

bool MakerModem::enterDirectLink(int sid) {
	string cmd = "";
	cmd = "AT+USODL=";
	cmd += to_string(sid);
	string resp = sendCommand(cmd.c_str());
	int index;
	if ((index = resp.find("CONNECT") >= 0)) {
		return true;
	}
	return false;

}
bool MakerModem::exitDirectLink() {
	Sleep(2000); //3 seconds
	SP->writeData("+++", 3);
	Sleep(1000);
	SP->writeData("\r\n", 2);
	string resp;
	do {
		resp = sendCommand("AT");
	} while (resp.find("OK") < 0);
	return true;
}
/*int MakerModem::directLinkAvailable() {
	return SP->availableData();
}*/
int MakerModem::directLinkRead(char * buff, int count) {
	return SP->readData(buff, count);
}
int MakerModem::directLinkWrite(const char * buff, int count) {
	return SP->writeData(buff, count);
}
void MakerModem::socketClose(int sid) {
	if (sid < 0 || sid > 6) return;
	string cmd = "AT+USOCL=";
	cmd += to_string(sid); // sid is 0-6
	//cout << cmd << endl;
	cout << sendCommand(cmd.c_str()) << endl;

}
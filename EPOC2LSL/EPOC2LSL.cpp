// EPOC2LSL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "lsl_cpp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstdlib>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#endif
#ifdef __linux__
#include <unistd.h>
#endif

#include "IEmoStateDLL.h"
#include "Iedk.h"
#include "IEegData.h"
#include "IedkErrorCode.h"

void logEmoState(std::ostream& os, unsigned int userID,
	EmoStateHandle eState, bool withHeader = false);

#ifdef __linux__
int _kbhit(void);
#endif

lsl::stream_info info("EMOTIVStream", "EEG", 14, 128, lsl::cf_float32, "test");
int main()
{
	IEE_DataChannel_t EpocChannelList[] = {

		IED_COUNTER, IED_INTERPOLATED,
		IED_AF3, IED_F7, IED_F3, IED_FC5, IED_T7,
		IED_P7, IED_O1, IED_O2, IED_P8, IED_T8,
		IED_FC6, IED_F4, IED_F8, IED_AF4, IED_RAW_CQ,
		IED_GYROX, IED_GYROY, IED_MARKER, IED_TIMESTAMP
	};

	IEE_DataChannel_t InsightChannelList[] = {

		IED_COUNTER,
		IED_INTERPOLATED,
		IED_RAW_CQ,
		IED_AF3,
		IED_T7,
		IED_Pz,
		IED_T8,
		IED_AF4,
		IED_TIMESTAMP,
		IED_MARKER,
		IED_SYNC_SIGNAL
	};

	const char header[] = "	IED_COUNTER, IED_INTERPOLATED, IED_AF3, IED_F7, IED_F3, IED_FC5, IED_T7, "
		"IED_P7, IED_O1, IED_O2, IED_P8, IED_T8, IED_FC6, IED_F4, IED_F8, IED_AF4, IED_RAW_CQ, "
		"IED_GYROX, IED_GYROY, IED_MARKER, IED_TIMESTAMP ";
	//const char* channz[] = { "COUNTER", "INTERPOLATED", "AF3", "F7", "F3", "FC5", "T7", "P7", "O1", "O2", "P8", "T8", "FC6", "F4", "F8", "AF4", "RAW_CQ", "GYROX", "GYROY", "MARKER", "TIMESTAMP"};
	const char* channz[] = { "AF3", "F7", "F3", "FC5", "T7", "P7", "O1", "O2", "P8", "T8", "FC6", "F4", "F8", "AF4" };
	const int channzids[14] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	EmoEngineEventHandle eEvent = IEE_EmoEngineEventCreate();
	EmoStateHandle eState = IEE_EmoStateCreate();
	unsigned int userID = 0;
	bool readytocollect = false;
	float secs = 1;
	int option = 0;
	int state = 0;


	try {
		std::cout << "==================================================================="
			<< std::endl;
		std::cout << "Example to show how to log the EmoState from EmoEngine."
			<< std::endl;
		std::cout << "==================================================================="
			<< std::endl;

		if (IEE_EngineConnect() != EDK_OK) {
			throw std::runtime_error("Emotiv Driver start up failed.");
		}

		DataHandle hData = IEE_DataCreate();
		IEE_DataSetBufferSizeInSec(secs);
		info.desc().append_child_value("manufacturer", "Emotiv");
		lsl::xml_element chns = info.desc().append_child("channels");
		std::vector<std::string> channVec(channz, channz + 14);
		for (int k = 0; k < 14; k++) {
			chns.append_child("channel")
				.append_child_value("label", channVec[k])
				.append_child_value("unit", "microvolts")
				.append_child_value("type", "EEG");
		}
		lsl::stream_outlet outlet(info);
		while (!_kbhit()) {
			state = IEE_EngineGetNextEvent(eEvent);

			if (state == EDK_OK) {
				IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);
				IEE_EmoEngineEventGetUserId(eEvent, &userID);

				if (eventType == IEE_UserAdded) {
					std::cout << "User added" << std::endl;
					IEE_DataAcquisitionEnable(userID, true);
					readytocollect = true;
				}
			}

			if (readytocollect) {
				IEE_DataUpdateHandle(0, hData);
				unsigned int nSamplesTaken = 0;
				IEE_DataGetNumberOfSample(hData, &nSamplesTaken);
				std::cout << "Updated " << nSamplesTaken << std::endl;

				if (nSamplesTaken != 0) {
					double* data = new double[nSamplesTaken];
					for (int sampleIdx = 0; sampleIdx < (int)nSamplesTaken; ++sampleIdx) {
						std::vector<double> samplevec;
						/*for (int i = 0; i < sizeof(EpocChannelList) / sizeof(IEE_DataChannel_t); i++) {
						IEE_DataGet(hData, EpocChannelList[i], data, nSamplesTaken);
						samplevec.push_back(data[sampleIdx]);
						}*/
						for (int i = 0; i < 14; i++) {
							//						std::cout << "id = " << EpocChannelList[channzids[i]] << std::endl;
							IEE_DataGet(hData, EpocChannelList[channzids[i]], data, nSamplesTaken);
							samplevec.push_back(data[sampleIdx]);
						}
						//						system("pause");
						//						std::cout << "test" << std::endl;
						outlet.push_sample(samplevec);
					}

					delete[] data;
				}
#ifdef _WIN32
				Sleep(1);
#endif
#ifdef __linux__
				sleep(1);
#endif
			}
		}

	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		std::cout << "Press any key to exit..." << std::endl;
		getchar();
	}

	IEE_EngineDisconnect();
	IEE_EmoStateFree(eState);
	IEE_EmoEngineEventFree(eEvent);

	return 0;
}


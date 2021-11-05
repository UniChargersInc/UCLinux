#pragma once

enum StatusNotificationType
{
	SN_NONE,
	SN_STATUS,
	SN_FIRMWARE
};

struct StopTrData_T {
	int HRowId, transactionId, meterStop;
	int64_t TS;

	string idTag = "", reason = "", transactionData = "";
};
struct MeterValuesData_T {
	std::string CSSerN;
	int HRowId, 
		connectorId,
		transactionId=0;  // optional
	
	string meterValueData = "";  // this is a serialized field of TransactionData
};
struct CS_History_T {
	uint64_t ts;
	std::string IPAddr, Action, CSSts, ErrCode, RTValues;
};
struct CS_Statistics_T {
	int x0,x1;  // HRowId for StartTransacton and StopTransaction
	int transactionId, connectorId;
	std::string meterValues;
};


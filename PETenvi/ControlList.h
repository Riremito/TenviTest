#ifndef __CONTROL_H__
#define __CONTROL_H__

enum SubControl {
	STATIC_INFO = 100,
	BUTTON_OPEN_FORMATVIEW,
	BUTTON_OPEN_FILTER,
	CHECK_HEADER_SIZE,
	LISTVIEW_LOGGER,
	BUTTON_CLEAR,
	CHECK_SEND,
	CHECK_RECV,
	EDIT_PACKET_SEND,
	BUTTON_SEND,
	EDIT_PACKET_RECV,
	BUTTON_RECV,
	CHECK_LOCK,
	BUTTON_INC_SEND,
	BUTTON_INC_RECV,
	EDIT_PACKET_FORMAT,
	BUTTON_RECV_FORMAT,
};

enum ListViewIndex {
	LV_TYPE,
	LV_ID,
	LV_LENGTH,
	LV_STATUS,
	LV_PACKET_FORMAT,
	LV_PACKET,
};

enum HeaderViewIndex {
	HV_TYPE,
	HV_FILTER,
	HV_HEADER
};

#endif
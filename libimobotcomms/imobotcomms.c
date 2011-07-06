
#include <stdio.h>
#include <stdlib.h>
#include "imobotcomms.h"

#ifdef _WIN32
typedef struct bdaddr_s {
  UINT8 b[6];
} bdaddr_t;
int str2ba(const char *str, bdaddr_t *ba);
#define write(sock, buf, len) send(sock, buf, len, 0)
#define read(sock, buf, len) \
  recvfrom(sock, buf, len, 0, (struct sockaddr*)0, 0)
#endif

int BRComms_init(br_comms_t* comms)
{
  memset(&comms->addr, 0, sizeof(sockaddr_t));
  comms->connected = 0;
#ifdef _WIN32
  WSADATA wsd;
  WSAStartup (MAKEWORD(1,1), &wsd);
#endif
}

int BRComms_connect(br_comms_t* comms, const char* address, int channel)
{
  int status;
  comms->socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  // set the connection parameters (who to connect to)
#ifndef _WIN32
  comms->addr.rc_family = AF_BLUETOOTH;
  comms->addr.rc_channel = (uint8_t) channel;
  str2ba( address, &comms->addr.rc_bdaddr );
#else
  comms->addr.addressFamily = AF_BTH;
  str2ba( address, (bdaddr_t*)&comms->addr.btAddr);
  comms->addr.port = channel;
#endif

  // connect to server
  status = connect(comms->socket, (const struct sockaddr *)&comms->addr, sizeof(comms->addr));
  if(status == 0) {
    comms->connected = 1;
  }
  return status;
}

int BRComms_disconnect(br_comms_t* comms)
{
#ifndef _WIN32
  close(comms->socket);
#else
  closesocket(comms->socket);
  CloseHandle((LPVOID)comms->socket);
#endif
  comms->connected = 0;
  return 0;
}

int BRComms_isConnected(br_comms_t* comms)
{
  return comms->connected;
}

int BRComms_setMotorDirection(br_comms_t* comms, int id, int dir)
{
  char buf[160];
  int status;
  int bytes_read;
  sprintf(buf, "SET_MOTOR_DIRECTION %d %d", id, dir);
  status = write(comms->socket, buf, strlen(buf)+1);
  if(status < 0) return status;
  bytes_read = read(comms->socket, buf, sizeof(buf));
  if(strcmp(buf, "OK")) return -1;
  return 0;
}

int BRComms_getMotorDirection(br_comms_t* comms, int id, int *dir)
{
  char buf[160];
  int status;
  int bytes_read;
  sprintf(buf, "GET_MOTOR_DIRECTION %d", id);
  status = write(comms->socket, buf, strlen(buf)+1);
  if(status < 0) return status;
  bytes_read = read(comms->socket, buf, sizeof(buf));
  if(!strcmp(buf, "ERROR")) return -1;
  sscanf(buf, "%d", dir);
  return 0;
}

int BRComms_setMotorSpeed(br_comms_t* comms, int id, int speed)
{
  char buf[160];
  int status;
  int bytes_read;
  sprintf(buf, "SET_MOTOR_SPEED %d %d", id, speed);
  status = write(comms->socket, buf, strlen(buf)+1);
  if(status < 0) return status;
  bytes_read = read(comms->socket, buf, sizeof(buf));
  if(strcmp(buf, "OK")) return -1;
  return 0;
}

int BRComms_getMotorSpeed(br_comms_t* comms, int id, int *speed)
{
  char buf[160];
  int status;
  int bytes_read;
  sprintf(buf, "GET_MOTOR_SPEED %d", id);
  status = write(comms->socket, buf, strlen(buf)+1);
  if(status < 0) return status;
  bytes_read = read(comms->socket, buf, sizeof(buf));
  if(!strcmp(buf, "ERROR")) return -1;
  sscanf(buf, "%d", speed);
  return 0;
}

int BRComms_setMotorPosition(br_comms_t* comms, int id, double position)
{
  char buf[160];
  int status;
  int bytes_read;
  sprintf(buf, "SET_MOTOR_POSITION %d %lf", id, position);
  status = write(comms->socket, buf, strlen(buf)+1);
  if(status < 0) return status;
  bytes_read = read(comms->socket, buf, sizeof(buf));
  if(strcmp(buf, "OK")) return -1;
  return 0;
}

int BRComms_getMotorPosition(br_comms_t* comms, int id, double *position)
{
  char buf[160];
  int status;
  int bytes_read;
  sprintf(buf, "GET_MOTOR_POSITION %d", id);
  status = write(comms->socket, buf, strlen(buf)+1);
  if(status < 0) return status;
  bytes_read = read(comms->socket, buf, sizeof(buf));
  if(!strcmp(buf, "ERROR")) return -1;
  sscanf(buf, "%lf", &position);
  return 0;
}

int BRComms_getMotorState(br_comms_t* comms, int id, int *state)
{
  char buf[160];
  int status;
  int bytes_read;
  sprintf(buf, "GET_MOTOR_STATE %d", id);
  status = write(comms->socket, buf, strlen(buf)+1);
  if(status < 0) return status;
  bytes_read = read(comms->socket, buf, sizeof(buf));
  if(!strcmp(buf, "ERROR")) return -1;
  sscanf(buf, "%d", state);
  return 0;
}

int BRComms_waitMotor(br_comms_t* comms, int id)
{
  int state;
  while(1)
  {
    if(BRComms_getMotorState(comms, id, &state)) {
      return -1;
    }
    if(state == 0) {
      return 0;
    } else {
      usleep(200000);
    }
  }
}

int BRComms_stop(br_comms_t* comms)
{
  char buf[160];
  int status;
  int bytes_read;
  status = write(comms->socket, "STOP", 5);
  if(status < 0) return status;
  bytes_read = read(comms->socket, buf, sizeof(buf));
  if(!strcmp(buf, "ERROR")) return -1;
  return 0;
}

#ifdef _WIN32
void baswap(bdaddr_t *dst, const bdaddr_t *src)
{
	register unsigned char *d = (unsigned char *) dst;
	register const unsigned char *s = (const unsigned char *) src;
	register int i;

	for (i = 0; i < 6; i++)
		d[i] = s[5-i];
}

int str2ba(const char *str, bdaddr_t *ba)
{
	UINT8 b[6];
	const char *ptr = str;
	int i;

	for (i = 0; i < 6; i++) {
		b[i] = (UINT8) strtol(ptr, NULL, 16);
		if (i != 5 && !(ptr = strchr(ptr, ':')))
			ptr = ":00:00:00:00:00";
		ptr++;
	}

	baswap(ba, (bdaddr_t *) b);

	return 0;
}
#endif

/*
 Module:        socket_.h
 Creation date: 13-jul-99
 Author:        Marc Savary
 Company:       Lyre Technologies Inc.
 Project:       MTY3 - H.323 Stack
 Revision:      1

 Description:   Debug layer for socket interface.
*/

#ifndef _SOCKET_
#define _SOCKET_

int Sock_Init();
#if DBG
#define sock_socket( fm, ty, fl) Debug_socket( fm, ty, fl, _HERE_)
#define sock_accept( a, b, c) Debug_accept( a, b, c, _HERE_)
#define sock_closesocket( so) Debug_closesocket( so)
#define sock_bind( a, b, c) Debug_bind( a, b, c)
#define sock_connect( a, b, c) Debug_connect( a, b, c)
#define sock_send( a, b, c, d) Debug_send( a, b, c, d)
#define sock_sendto( a, b, c, d, e, f) Debug_sendto( a, b, c, d, e, f)
#define sock_recv( a, b, c, d) Debug_recv( a, b, c, d)
#define sock_recvfrom( a, b, c, d, e, f) Debug_recvfrom( a, b, c, d, e, f)
#else
#define sock_socket( fm, ty, fl) socket( fm, ty, fl)
#define sock_accept( a, b, c) accept( a, b, c)
#define sock_closesocket( so) closesocket( so)
#define sock_bind( a, b, c) bind( a, b, c)
#define sock_connect( a, b, c) connect( a, b, c)
#define sock_send( a, b, c, d) send( a, b, c, d)
#define sock_sendto( a, b, c, d, e, f) sendto( a, b, c, d, e, f)
#define sock_recv( a, b, c, d) recv( a, b, c, d)
#define sock_recvfrom( a, b, c, d, e, f) recvfrom( a, b, c, d, e, f)
#endif


#if DBG
SOCKET Debug_socket( int family, int type, int flags, char * comment);
SOCKET Debug_accept( SOCKET so, struct sockaddr * addr, int * addrLen, char * comment);
int Debug_closesocket( SOCKET so);
int Debug_bind( SOCKET so, const struct sockaddr * name, int namelen);
int Debug_connect( SOCKET so, const struct sockaddr * name, int namelen);
int Debug_recv( SOCKET so, char * buffer, int buffLen, int flags);
int Debug_recvfrom( SOCKET so, char * buffer, int buffLen, int flags, struct sockaddr * from, int * fromLen);
int Debug_send( SOCKET so, char * buffer, int buffLen, int flags);
int Debug_sendto( SOCKET so, char * buffer, int buffLen, int flags, const struct sockaddr * to, int toLen);
int Debug_GetUdpSockCount();
int Debug_GetTcpSockCount();
int Debug_SetMaxUdpSockCount( int count);
int Debug_SetMaxTcpSockCount( int count);
int Debug_Sock_DisplayList();
#endif

#endif // _SOCKET_

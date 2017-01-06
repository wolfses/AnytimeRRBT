// SerialPort.cpp (original name: CSerial.cpp)
// Original by Tom Archer and Rick Leinecker as found on CODEGURU:
// http://www.codeguru.com/cpp/i-n/network/serialcommunications/article.php/c2503/CSerial--A-C-Class-for-Serial-Communications.htm
// Slightly modified/adapted by Marc Freese

#include "stdafx.h"
#include "SerialPort.h"

int CSerialPort::_nextPortHandle=0;

CSerialPort::CSerialPort()
{
	memset(&m_OverlappedRead,0,sizeof(OVERLAPPED));
 	memset(&m_OverlappedWrite,0,sizeof(OVERLAPPED));
	m_hIDComDev=NULL;
	m_bOpened=FALSE;
	_portHandle=_nextPortHandle++;
}

CSerialPort::~CSerialPort()
{
	Close();
}

int CSerialPort::getPortHandle()
{
	return(_portHandle);
}

BOOL CSerialPort::Open(char* portString,int nBaud)
{
	if (m_bOpened)
		return(TRUE);
	DCB dcb;
	m_hIDComDev=CreateFile(portString,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);

	if (m_hIDComDev==NULL)
		return(FALSE);
	memset(&m_OverlappedRead,0,sizeof(OVERLAPPED));
 	memset(&m_OverlappedWrite,0,sizeof(OVERLAPPED));

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout=0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier=0;
	CommTimeOuts.ReadTotalTimeoutConstant=0;
	CommTimeOuts.WriteTotalTimeoutMultiplier=0;
	CommTimeOuts.WriteTotalTimeoutConstant=5000;
	SetCommTimeouts(m_hIDComDev,&CommTimeOuts);

	m_OverlappedRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	m_OverlappedWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	dcb.DCBlength=sizeof(DCB);
	GetCommState(m_hIDComDev,&dcb);
	dcb.BaudRate=nBaud;
	dcb.ByteSize=8;
	unsigned char ucSet;
	ucSet=(unsigned char)((FC_RTSCTS&FC_DTRDSR)!=0);
	ucSet=(unsigned char)((FC_RTSCTS&FC_RTSCTS)!=0);
	ucSet=(unsigned char)((FC_RTSCTS&FC_XONXOFF)!=0);
	if( !SetCommState( m_hIDComDev, &dcb ) ||
		!SetupComm( m_hIDComDev, 10000, 10000 ) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL ){
		DWORD dwError = GetLastError();
		if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
		if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
		CloseHandle( m_hIDComDev );
		return( FALSE );
		}

	m_bOpened = TRUE;

	return( m_bOpened );
}

BOOL CSerialPort::Close( void )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( TRUE );

	if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
	if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
	CloseHandle( m_hIDComDev );
	m_bOpened = FALSE;
	m_hIDComDev = NULL;

	return( TRUE );
}

BOOL CSerialPort::WriteCommByte(unsigned char ucByte)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile( m_hIDComDev, (LPSTR) &ucByte, 1, &dwBytesWritten, &m_OverlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) ){
		if( WaitForSingleObject( m_OverlappedWrite.hEvent, 1000 ) ) dwBytesWritten = 0;
		else{
			GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
			m_OverlappedWrite.Offset += dwBytesWritten;
			}
		}

	return( TRUE );
}

int CSerialPort::SendData(const char *buffer,int size)
{
  if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

  BOOL bWriteStat;
  DWORD dwBytesWritten;

  bWriteStat = WriteFile( m_hIDComDev, buffer, size, &dwBytesWritten, &m_OverlappedWrite );
  if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) ){
    if( WaitForSingleObject( m_OverlappedWrite.hEvent, 1000 ) ) dwBytesWritten = 0;
    else{
      GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
      m_OverlappedWrite.Offset += dwBytesWritten;
      }
    }

  return( dwBytesWritten );
}

int CSerialPort::ReadDataWaiting( void )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	return( (int) ComStat.cbInQue );
}

int CSerialPort::ReadData(char *buffer,int limit)
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	if( !ComStat.cbInQue ) return( 0 );

	dwBytesRead = (DWORD) ComStat.cbInQue;
	if( limit < (int) dwBytesRead ) dwBytesRead = (DWORD) limit;

	bReadStatus = ReadFile( m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead );
	if( !bReadStatus ){
		if( GetLastError() == ERROR_IO_PENDING ){
			WaitForSingleObject( m_OverlappedRead.hEvent, 2000 );
			return( (int) dwBytesRead );
			}
		return( 0 );
		}

	return( (int) dwBytesRead );
}


#ifndef _Ftp_FtpServer_h_
#define _Ftp_FtpServer_h_

#include <Core/Core.h>

NAMESPACE_UPP

struct FTPClientConnection {
	
	One<TcpSocket> data_socket;
	String command, arg, arg2, cwd;
	TcpSocket* sock;
	int state;
	bool passive, running, stopped;
	
	String Scan();
	String GetSysDir(String cwd);
	String AppendCwd(String s);
	String AppendCwdSys(String s);
	String ChangeDirectory(String s);
	String ChangeDirectorySys(String s);
	void Response(String s);
	void CheckPassive();
public:
	FTPClientConnection();
	
	void Stop();
	void Process(TcpSocket& sock);
};

class FTPServer {
	
protected:
	TcpSocket listen_socket;
	ArrayMap<int, Thread> thrd_list;
	ArrayMap<int, FTPClientConnection> client_list;
	ArrayMap<int, TcpSocket> sock_list;
	Vector<int> rm_queue;
	Mutex lock;
	int port, client_counter;
	bool running, stopped;
	
	void ServeClient(int id);
	void Listener();
public:
	typedef FTPServer CLASSNAME;
	FTPServer();
	~FTPServer();
	void Start();
	void Stop();
	
	void SetPort(uint16 p) {port = p;}
	int  GetPort() const   {return port;}
	bool IsRunning() const {return running;}
};

typedef FTPServer FtpServer;

END_UPP_NAMESPACE

#endif

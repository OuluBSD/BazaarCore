#include <Core/Core.h>
#include <Ftp/Ftp.h>
#include <Ftp/FtpServer.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
	StdLogSetup(LOG_COUT | LOG_FILE);
	Cout() << "=== Running FtpCheck Unit Tests ===\n";

	// 1. Test Unix directory parsing
	{
		String unix_ls = "-rwxr-xr-x 1 owner group 12345 Nov 10 12:34 testfile.txt\r\n";
		Ftp::DirList list;
		bool parsed = ParseFtpDirEntry(unix_ls, list);
		ASSERT(parsed);
		ASSERT(list.GetCount() == 1);
		ASSERT(list[0].GetName() == "testfile.txt");
		ASSERT(list[0].GetSize() == 12345);
		ASSERT(list[0].IsFile());
		ASSERT(list[0].GetOwner() == "owner");
		ASSERT(list[0].GetGroup() == "group");
		Cout() << "✓ Unix DirEntry parsing passed\n";
	}

	// 2. Test DOS directory parsing
	{
		String dos_ls = "11-10-26 12:34PM 54321 document.pdf\r\n";
		Ftp::DirList list;
		bool parsed = ParseFtpDirEntry(dos_ls, list);
		ASSERT(parsed);
		ASSERT(list.GetCount() == 1);
		ASSERT(list[0].GetName() == "document.pdf");
		ASSERT(list[0].GetSize() == 54321);
		ASSERT(list[0].IsFile());
		Cout() << "✓ DOS DirEntry parsing passed\n";
	}

	// 3. Test FTPServer configuration
	{
		FTPServer server;
		server.SetPort(19821);
		ASSERT(server.GetPort() == 19821);
		ASSERT(!server.IsRunning());
		Cout() << "✓ FTPServer configuration passed\n";
	}

	// 4. Test Local Loopback Client-Server Connection
	{
		FTPServer server;
		int test_port = 19821;
		server.SetPort(test_port);
		server.Start();
		Sleep(150);
		ASSERT(server.IsRunning());

		Ftp client;
		client.Timeout(3000);
		client.Passive();
		client.User("funk", "123123");

		bool connected = client.Connect("127.0.0.1", test_port);
		ASSERT(connected);
		ASSERT(client.GetCode() == 230); // 230 User logged in, proceed.

		String dir = client.GetDir();
		Cout() << "✓ Client connected and authenticated, current dir: " << dir << "\n";

		client.Disconnect();
		server.Stop();
		Sleep(100);
		ASSERT(!server.IsRunning());
		Cout() << "✓ FTPServer loopback test cleanly disconnected and stopped\n";
	}

	Cout() << "\n>>> ALL FTP TESTS PASSED SUCCESSFULLY! <<<\n";
}

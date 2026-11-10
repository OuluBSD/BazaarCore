#ifndef _FtpBrowser_FtpBrowser_h_
#define _FtpBrowser_FtpBrowser_h_

#include <CtrlLib/CtrlLib.h>
#include <Ftp/Ftp.h>

NAMESPACE_UPP

#define LAYOUTFILE <FtpBrowser/FtpBrowser.lay>
#include <CtrlCore/lay.h>

class FtpBrowser : public WithFtpBrowserLayout<TopWindow> {

    WithAccountLayout<ParentCtrl>   account;
    WithServerLayout<ParentCtrl>    server;

    TabDlg          settings;
    Ftp             browser;
    String          workdir, rootdir;
    MenuBar         mainmenu;
    Progress        progress;

    void            Connect();
    void            Disconnect();
    void            Browse();
    void            Action();
    void            Information(const FileList::File& entry);
    void            Download(const String& file, const String& path);
    bool            DownloadProgress(int64 total, int64 done);

    void            About();
    void            Settings();

    void            MainMenu(Bar& bar);
    void            FileMenu(Bar& bar);
    void            HelpMenu(Bar& bar);

    void            ContextMenu(Bar& bar);
    
    void            UpdateGui()             { ProcessEvents(); }

public:
    typedef FtpBrowser CLASSNAME;

    void            Serialize(Stream& s);
    FtpBrowser();
   ~FtpBrowser();
};

END_UPP_NAMESPACE

#endif

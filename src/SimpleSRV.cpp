// -------- Minimum includes for this module --------------------------------------

    #include <FS.h>

	#include "externIO.h"       // global externals of cpu,...,server
    #include "SimpleSRV.h"
    
       
// ------ Expected definitions/allocations in main() ------------------------------

    #include "externIO.h"       // global externals of cpu,...,server
	
// --------- Forward References needed by the editor ------------------------------

    void handleCLItxt();
	void handleCLIhtm();
	void handleFileList();
    bool handleFileRead(String path);
    bool handleFileRead1(String path);
    void handleFileDelete();
    void handleFileCreate();
    void handleFileUpload1();                 
    void handleFileUpload(); 

// ----------------------- Default Landing Pages -------------------------------------
char *navigate 			= 	"<br/><h2 align='center'>Click for <a href=\"index.htm\">INDEX</a> | <a href=\"help\">HELP</a> | <a href='/'>ROOT</a></h2>";

char *Landing_STA_Page 	=	"<h1 align='center'>STA Mode</h1>"\
							"<h2 align='center'>Click for <a href=\"index.htm\">INDEX</a> | <a href=\"help\">HELP</a> | <a href='/'>ROOT</a></h2>";

char *Landing_AP_Page 	= 	"<h1 align='center'>AP Mode</h1>"\
							"<h2 align='center'>Click for <a href=\"index.htm\">INDEX</a> | <a href=\"help\">HELP</a> | <a href='/'>ROOT</a></h2>";
								
char *Landing_Help_Page = 	"<h2 align='center'><b>END POINTS (hardcoded)</b><br/><br/>"\
							"<a href=\"status\">Show WiFi Status</a> (/status)<br/>"\
							"<a href=\"trace\">Show/Change trace</a> (/trace )<br/>"\
							"<a href=\"files\">Hyperlinked list of files</a> (/files, /dir)<br/>"\
							"<a href=\"cli?cmd=h\">Simple text CLI</a> (/cli?cmd=...)<br/>"\
							"<a href=\"reset\">Reset CPU</a> (/reset)<br/>"\
							"<a href=\"upload\">Upload file</a> (/upload)<br/>"\
							"<a href=\"ed\">Delete file</a> (/ed?del=file)<br/>"\
							"<a href=\"ed\">Create file</a> (/ed?new=file)<br/>"\
							"<a href=\"help\">Display this page</a> (/help, /h)<br/><br/></h2>"\
							"<h2 align='center'><b>END POINTS (.htm files required)</b><br/><br/>"\
							"<a href=\"webcli.htm\">Web Command Line Interface (CLI)</a><br/>"\
							"<a href=\"edit\">Interactive file edit</a>  (/edit)<br/><br/>"\
							"Click for <a href=\"index.htm\">INDEX</a> | <a href='/'>ROOT</a></h1></h2>";

// ----------------------- CLI Callbacks() -------------------------------------

#define SERVER_ON 			servptr->on
#define SERVER_SEND 		servptr->send
#define SERVER_URI  		servptr->uri
#define SERVER_ARGS 		servptr->args
#define SERVER_ARG  		servptr->arg
#define SERVER_ARGNAME		servptr->argName
#define SERVER_UPLOAD 		servptr->upload
#define SERVER_STREAMFILE   servptr->streamFile
#define SERVER_NOTFOUND 	servptr->onNotFound


static BUF *cliresp;				// initialized by cliCallbacks() before used.
static int g_trace=0;  				// used only in this file. Elsewhere, use setTrace() and getTrace()
static ESP8266WebServer *servptr;

void cliCallbacks( ESP8266WebServer &myserver, BUF &mybuffer )
{
    cliresp = &mybuffer;								// save pointer to buffer area
	servptr = &myserver;								// save pointer to server
	
	SERVER_ON("/cli", 		 HTTP_GET,	handleCLItxt );			// simple CLI without .htm file
    SERVER_ON("/webcli.htm", HTTP_GET,	handleCLIhtm );			// simple CLI without .htm file
	
	// [](){
        // showArgs();
		// cliresp.set("Use /cli?cmd=<args>");
        // if( server.args() )                                 // if command given
        // {
            // cliresp->init();                              	// initialize the RESPONSE buffer
			// B80 cmd;
			// cmd.copy( server.arg(0).c_str() );
			// exe.dispatchBuf( !cmd, cliresp );  				// command is executed here and RESPONSE is saved in 'resp' buffer
        // }        
		// showJson( !cliresp );                           		// show the same on the console
        // server.send(200, "text/plain", !cliresp );
    // });
	// SERVER_ON("/webcli.htm", HTTP_GET,              // when command is submitted, webcli.htm is called  
    // [](){
        // showArgs();
        // if( server.args() )                                     // if command given
        // {
            // cliresp->init();                                     // initialize the response buffer
            // B80 cmd;
            // cmd.copy( server.arg(0).c_str() );
            // exe.dispatchBufPtr( !cmd, cliresp );                  // command is executed here and response is saved in 'cliresp' buffer
            
            // cliresp->add("(Used:%d of %d bytes)\r\n", cliresp->length(), cliresp->size());
        // }        
        // showJson( cliresp->c_str() );                                   // show the same on the console
        // handleFileRead("/webcli.htm" );                         // reprint same html
    // });
    SERVER_ON("/clirsp", HTTP_GET,      
    [](){
        showArgs();
        showJson( cliresp->c_str() );
        SERVER_SEND(200, "text/html", cliresp->c_str() );
    });
}

void handleCLI( bool mode )
{
	showArgs();

	cliresp->copy("Use /cli?cmd=<args>");

	if( server.args() )                                 // if command given
	{
		cliresp->init();                              	// initialize the RESPONSE buffer
		B80 cmd;
		cmd.copy( server.arg(0).c_str() );
		exe.dispatchBufPtr( !cmd, cliresp );  				// command is executed here and RESPONSE is saved in 'resp' buffer
		cliresp->add("(Used:%d of %d bytes)\r\n", 
				cliresp->length(), cliresp->size());
	}        
	showJson( cliresp->c_str() );                           		// show the same on the console
	if( mode )
		handleFileRead("/webcli.htm" );
	else
		server.send(200, "text/plain", cliresp->c_str() );	
}
void handleCLItxt() {handleCLI( false ); }
void handleCLIhtm() {handleCLI( true ); }

// ----------------------- Server Callbacks() -------------------------------------

static char *Landing_User_Page;
void srvCallbacks( ESP8266WebServer &myserver, char *landingpage )
{
	Landing_User_Page = landingpage;
	servptr = &myserver;								// save pointer to server
	
	SERVER_ON("/",
    [](){
        SERVER_SEND(200, "text/html", Landing_User_Page );
    });
	SERVER_ON("/help", HTTP_GET, 
    [](){
      showArgs();
      SERVER_SEND(200, "text/html", Landing_Help_Page);
    }); 
    SERVER_ON("/h", HTTP_GET, 
    [](){
      showArgs();
      SERVER_SEND(200, "text/html", Landing_Help_Page);
    }); 
    
    SERVER_ON("/restart", HTTP_GET, 
    [](){
      showArgs();
      String resp("<h3 align='center'>RESTARTED! (control is lost)</h3>");
      SERVER_SEND(200, "text/html", resp);
      delay( 1000 );
      ESP.restart();
    }); 
    SERVER_ON("/reset", HTTP_GET, 
    [](){
        showArgs();
        String resp( "<h3 align='center'>RESET! (control is lost)</h3>");
        SERVER_SEND(200, "text/html", resp);
      delay( 1000 );
      ESP.reset();
    }); 
    SERVER_ON("/status", HTTP_GET, 
    [](){
      showArgs();
      BUF tmp( 256 );
	  getWiFiStatus( &tmp, true/*Use HTML*/ );	// fill buffer with status
	  tmp.add( navigate );						// add navigation
      showJson( !tmp );  
      SERVER_SEND(200, "text/html", !tmp);		// serve this page
    }); 
	
	SERVER_ON("/wifi", HTTP_GET, 
    [](){
      showArgs();
      BUF json("",100);
      json.set( "{" );
      json.add( "'ssid': '%s',", eep.wifi.ssid );
      json.add( " 'pwd': '%s'", eep.wifi.pwd );
      json.add( "}" );
	  json.quotes();
      showJson( !json );  
      SERVER_SEND(200, "text/json", !json);
    }); 
    SERVER_ON("/jtrace", HTTP_GET, 
    [](){
        showArgs();
        BUF json("",80);
        json.set("{'json':'%d', ",  g_trace & T_JSON    ? 1 : 0);
        json.add(  "'req':'%d', ",  g_trace & T_REQUEST ? 1 : 0);
        json.add( "'args':'%d', ",  g_trace & T_ARGS    ? 1 : 0);
        json.add("'fileio':'%d', ", g_trace & T_FILEIO  ? 1 : 0);
        json.add("'actions':'%d'}", g_trace & T_ACTIONS ? 1 : 0);
        showJson( !json );  
        SERVER_SEND(200, "text/json", !json);
    }); 
    SERVER_ON("/trace", HTTP_GET, 
    [](){
        showArgs();
        BUF s(256);

		s.init();
		s.add( "<h2 align='center'>Web trace=%d<br/>\r\n", g_trace );
		s.add( "(Requests=%d, Responses=%d, Arguments=%d)<br/>", T_REQUEST, T_JSON, T_ARGS);
		s.add( "Use '/trace?set=value' to modify</h2>" );
		
        if( (server.args() > 0) && (server.argName(0) == "set") )
        {
			g_trace = atoi( server.arg(0).c_str() );
			s.set( "<h2 align='center'>Trace set to %d</h2>\r\n", g_trace );
        }
		s.add( navigate );
        showJson( !s );  
        SERVER_SEND(200, "text/html", !s);
    }); 
    SERVER_ON("/dir", HTTP_GET,
    [](){
        showArgs();
        BUF resp("",400);
        resp.set(   "<h3 align='center'>%s<br/>"
                    "<a href='/'>Click to navigate</a></h3>", fileList( true ).c_str()+1 ); 
                          
        SERVER_SEND(200, "text/html", !resp );     
    });
	SERVER_ON("/files", HTTP_GET,
    [](){
        showArgs();
		BUF s(1024);
		s.set("<h2 align='center'><br/>");
		
		Dir dir = SPIFFS.openDir("/");
		bool empty = true;
		while (dir.next()) 
		{    
			empty=false;
			String fileName = dir.fileName();
			size_t fileSize = dir.fileSize();
			s.add( "<a href='%s'>%s</a> of %dkB\r\n<br/>", fileName.c_str(), fileName.c_str()+1, fileSize/1024 );
		}  
		if( empty) s.add("No files found!<br/>\r\n");
		s.add( "</h2>%s", navigate);
		showJson( !s );  
        SERVER_SEND(200, "text/html", !s);
    });


    // --------------- editor files ------------------------
    SERVER_ON("/list", HTTP_GET, handleFileList );
    SERVER_ON("/edit", HTTP_GET,[](){if(!handleFileRead("/edit.htm")) SERVER_SEND(404, "text/plain", "FileNotFound");});
    SERVER_ON("/edit", HTTP_PUT, handleFileCreate);
    SERVER_ON("/edit", HTTP_DELETE, handleFileDelete);
    SERVER_ON("/edit", HTTP_POST, [](){ SERVER_SEND(200, "text/plain", ""); }, handleFileUpload);
    // -----------------------------------------------------

    SERVER_ON("/ed", HTTP_GET,          
    [](){
        showArgs();
        if(server.args() == 0) 
            return SERVER_SEND(500, "text/plain", "BAD ARGS");
        String func = server.argName(0);  
        String path = "/"+server.arg(0);

        if( func=="del" )                   // use ed?del=filename
        {
            PRN("handleFileDelete: " + path);
            if(!SPIFFS.exists( path ) )
                return SERVER_SEND(404, "text/plain", "FileNotFound");
            SPIFFS.remove(path);
        }
        else if( func=="new" )              // use ed?new=filename
        {
            PRN("handleFileCreate: " + path);
            if(SPIFFS.exists(path))
                return SERVER_SEND(500, "text/plain", "FILE EXISTS");
            
            File file = SPIFFS.open(path, "w");
            if(file)
                file.close();
            else
                return SERVER_SEND(500, "text/plain", "CREATE FAILED");
        }
        SERVER_SEND(200, "text/plain", fileList() );     
    });

//    SERVER_ON("/upload", HTTP_GET, 
//    []() {                                                      // if the client requests the upload page
//        showArgs();
//        if (!handleFileRead1("/upload.htm"))                     // send it if it exists
//            SERVER_SEND(404, "text/plain", "404: Not Found");   // otherwise, respond with a 404 (Not Found) error
//    });

    SERVER_ON("/upload", HTTP_GET, 
    []() {                                                      // if the client requests the upload page
        showArgs();
        char *upto = "<html><head><title>File Upload</title><style>body {text-align: center;}input {text-align:center;}</style>"
        "</head><body><h2>UPLOAD FILE</h2><form method=\"post\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"name\">"
        "<input class=\"button\" type=\"submit\" value=\"OK\"></form>"
		"<h2 align='center'>Click for <a href=\"index.htm\">INDEX</a> | <a href=\"help\">HELP</a> | <a href='/'>ROOT</a></h2></body></html>";
		
        SERVER_SEND(200, "text/html", upto);  
    });    
    SERVER_ON("/upload", HTTP_POST,                         // if the client posts to the upload page
        [](){ showArgs(); SERVER_SEND(200); },                                                  // Send status 200 (OK) to tell the client we are ready to receive
        handleFileUpload);                                    // Receive and save the file

    SERVER_ON("/favicon.ico", HTTP_GET, 
    [](){
        showArgs();
        //PRN( "request: "+ server.uri() );
        SERVER_SEND(200, "image/x-icon", "");
    });
	
	SERVER_ON("/currentsetting.htm", HTTP_GET, 
    [](){
        IPAddress ip = WiFi.localIP();
        char r[200];
        sprintf( r, "Model=GeorgeESP\r\nName=%d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3] );
        SERVER_SEND( 200, "text/html", r );
        PF("Responded to /currentsetting %s\r\n", r );
    });
	
    SERVER_NOTFOUND( 
    [](){
        showArgs();
        String path = SERVER_URI();
        //PRN( "request: "+ path );
        if(SPIFFS.exists(path))
        {
            File file = SPIFFS.open(path, "r");
            SERVER_STREAMFILE(file, "text/html");
            file.close();
        }
        else
            SERVER_SEND(404, "text/plain", "Serves only .htm files");   
    });
}
// -------------------------------- handlers -----------------------------------------

void setTrace( int value ) {g_trace = value;}
int getTrace() {return g_trace;}

String fileList( bool br )
{
    Dir dir = SPIFFS.openDir("/");
    BUF output("",500);
    while (dir.next()) 
    {    
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        output.add( "\t%s (%s)", fileName.c_str(), formatBytes(fileSize).c_str() );
        if( br )
            output.add( "<br/>" );
        output.add("\r\n");
    }  
    return String( !output );
}
// see: https://tttapa.github.io/ESP8266/Chap12%20-%20Uploading%20to%20Server.html

String getContentType(String filename) // convert the file extension to the MIME type
{ 
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".txt")) return "text/plain";
  else if (filename.endsWith(".pdf")) return "image/x-icon";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
void showArgs( )
{
  cpu.heapUpdate();
  if( g_trace & T_REQUEST )
    PRN( "*** request: "+ SERVER_URI() );
  if( g_trace & T_ARGS )
  {
    int N=SERVER_ARGS();
    PRN("=== arg: "); PRN( N );
    if( N>0 )
    {
      for( int i=0; i<N; i++ )
        PF( "Name:%s, Value:%s\r\n", (SERVER_ARGNAME(i)).c_str(), (SERVER_ARG(i)).c_str() );
    }
  }
}
void showJson( String &json )
{
  cpu.heapUpdate();
  if( g_trace & T_JSON )
  {
    PR( "--- " ); PRN( json );
  }
}
void showJson( char *s )
{
  cpu.heapUpdate();
  if( g_trace & T_JSON )
    PF( "--- %s\r\n", s );
}

//format bytes
String formatBytes(size_t bytes)
{
  if (bytes < 1024)
  {
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024))
  {
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024))
  {
    return String(bytes/1024.0/1024.0)+"MB";
  } else 
  {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}
// --------------------------------- Editor Handlers ---------------------------------------
void handleFileList() 
{
  if(!server.hasArg("dir")) 
    {
        SERVER_SEND(500, "text/plain", "BAD ARGS"); 
        return;
    }
  String path = server.arg("dir");
  Serial.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next())
    {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  output += "]";
  SERVER_SEND(200, "text/json", output);
}
bool handleFileRead(String path)
{
    if(path.endsWith("/")) 
        path += "index.htm";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    
    if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
    {
        if(SPIFFS.exists(pathWithGz))
            path += ".gz";
    
        File file = SPIFFS.open(path, "r");
        SERVER_STREAMFILE(file, contentType);
        file.close();
        return true;
    }
    return false;
}
bool handleFileRead1(String path)  // send the right file to the client (if it exists)
{
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
    String contentType = getContentType(path);             // Get the MIME type
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = SERVER_STREAMFILE(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}
void handleFileDelete()
{
  if(SERVER_ARGS() == 0) 
    return SERVER_SEND(500, "text/plain", "BAD ARGS");
  String path = SERVER_ARG(0);
  Serial.println("handleFileDelete: " + path);
  if(path == "/")
    return SERVER_SEND(500, "text/plain", "BAD PATH");
  
  if(!SPIFFS.exists(path))
    return SERVER_SEND(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  SERVER_SEND(200, "text/plain", "");
  path = String();
}
void handleFileCreate()
{
  if(server.args() == 0)
    return SERVER_SEND(500, "text/plain", "BAD ARGS");
  
  String path = server.arg(0);
  Serial.println("handleFileCreate: " + path);
  if(path == "/")
    return SERVER_SEND(500, "text/plain", "BAD PATH");
  if(SPIFFS.exists(path))
    return SERVER_SEND(500, "text/plain", "FILE EXISTS");
  
  File file = SPIFFS.open(path, "w");
  if(file)
    file.close();
  else
    return SERVER_SEND(500, "text/plain", "CREATE FAILED");
  
  SERVER_SEND(200, "text/plain", "");
  path = String();
}
File fsUploadFile;
void handleFileUpload()
{                                                       // upload a new file to the SPIFFS
    HTTPUpload& upload = server.upload();
    
    if(upload.status == UPLOAD_FILE_START)
    {
        String filename = upload.filename;
        if(!filename.startsWith("/")) 
            filename = "/"+filename;        
        Serial.print("handleFileUpload Name: "); Serial.println(filename);
        fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
        filename = String();
    } 
    else if(upload.status == UPLOAD_FILE_WRITE)
    {
        if(fsUploadFile)
        {
            fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
            PF("Current Size = %d\r\n", upload.currentSize );
        }
    } 
    else if(upload.status == UPLOAD_FILE_END)
    {
        if(fsUploadFile) 
        {                                                       // If the file was successfully created
          fsUploadFile.close();                                 // Close the file again
          Serial.print("handleFileUpload Size: "); 
          Serial.println(upload.totalSize);
          char *ok = "<html><body><h2 align='center'>OK!<br/><br/><a href='/'>Goto ROOT</a></h2></body></html>";
//          SERVER_SENDHeader("Location","/success.htm");        // Redirect the client to the success page
//          SERVER_SEND(303);
            SERVER_SEND(200, "text/html", ok );
        } 
        else 
        {
          SERVER_SEND(500, "text/plain", "500: couldn't create file");
        }
    }
}    
void handleFileUploadOLD()
{
  if(server.uri() != "/edit") return;
  HTTPUpload& upload = SERVER_UPLOAD();
  if(upload.status == UPLOAD_FILE_START)
    {
    String filename = upload.filename;
    if(!filename.startsWith("/")) 
      filename = "/"+filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } 
  else if(upload.status == UPLOAD_FILE_WRITE)
    {
    //Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } 
  else if(upload.status == UPLOAD_FILE_END)
    {
    if(fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}
// ------------------------ WiFi Status ---------------------------------
void bufWiFiStatus( BUF *s, bool usehtml )
{
	char *termT = "<br/>\r\n";		
	char *termF = "\r\n\t";
	char *term = usehtml ? termT: termF;
	

	if( usehtml )
		s->add( "<h3 align='center'>" );
	else
		s->add("\t");
	
	s->add(" CPU: %04x%s", ESP.getChipId(), term );
	s->add("Core: %s%s",   ESP.getCoreVersion().c_str(), term);
		
	uint8_t m[6];
	WiFi.macAddress( m );
	s->add(" MAC: %02x:%02x:%02x:%02x:%02x:%02x%s", m[0],m[1],m[2],m[3],m[4],m[5], term );

	IPAddress ipa; 
	ipa = WiFi.localIP();        
	s->add("  IP: %s%s",  		ipa.toString().c_str(), term );
	
	s->add("DHCP: %s%s",     	WiFi.hostname().c_str(), term );
	s->add("SSID: %s%s",  		WiFi.SSID().c_str(), term );
	s->add(" PWD: %s%s",   		WiFi.psk().c_str(), term );
	s->add("Chan: %d%s",   		WiFi.channel(), term );
	s->add("RSSI: %d dBm%s",		WiFi.RSSI(), term );
	s->add("HeapUse: %d KB%s", 	cpu.heapUsedNow()/1024, term );
	s->add("HeapMax: %d KB%s", 	cpu.heapUsedMax()/1024, term );	
	
	if( usehtml )
	{
		s->add( "</h3>");
		s->add( navigate );
	}
}
void getWiFiStatus( BUF *s, bool usehtml )
{
	s->init();
	bufWiFiStatus( s, usehtml );
}
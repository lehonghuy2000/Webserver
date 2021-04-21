#include "WebServer.h"

// Hàm lấy thời gian của file
string CreatTimeFile(SYSTEMTIME time)
{
	ostringstream ossMessage;

	ossMessage << time.wYear << "-"
		<< setw(2) << setfill('0') << time.wMonth << "-"
		<< setw(2) << setfill('0') << time.wDay << " "
		<< setw(2) << setfill('0') << time.wHour << ":"
		<< setw(2) << setfill('0') << time.wMinute << ":"
		<< setw(2) << setfill('0') << time.wSecond;
	string strMessage = ossMessage.str();
	return strMessage;
}
// Hàm lấy thời gian của tất cả các file
void GetTime(vector<string> files, vector<string> &date)
{

	HANDLE hFile1;
	FILETIME ftCreate;
	SYSTEMTIME stUTC, stLocal;

	for (int i = 0; i < files.size(); i++)
	{
		string temp;
		temp = ".\\webstored\\DownloadFolder/" + files[i];
		hFile1 = CreateFile(temp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		GetFileTime(hFile1, &ftCreate, NULL, NULL);
		FileTimeToSystemTime(&ftCreate, &stUTC);
		string time = CreatTimeFile(stUTC);
		date.push_back(time);
	}
}
// Hàm lấy kích thước của tất cả các file
void GetSize(vector<string> files, vector<string>& size)
{
	for (int i = 0; i < files.size(); i++)
	{
		string temp;
		temp = ".\\webstored\\DownloadFolder/" + files[i];
		ifstream in_file(temp, ios::binary);
		in_file.seekg(0, ios::end);
		int file_size = in_file.tellg();
		string temp1 = to_string(file_size);
		size.push_back(temp1);
		in_file.close();
	}
}
// Hàm thay thế một chuỗi con trong chuỗi bằng một chuỗi khác
void replaceAll(string& str, const string& from, const string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); 
	}
}
// Hàm duyệt các file trong một thư mục
void list_dir(const char* path, vector<string> &files) {
	struct dirent* entry;
	DIR* dir = opendir(path);
	if (dir == NULL) {
		return;
	}
	while ((entry = readdir(dir)) != NULL) {
		files.push_back(entry->d_name);
	}
	closedir(dir);
}
// Hàm khởi tạo mặc định 
WebServer::WebServer(){}
// Hàm khởi tạo có tham số đầu vào
WebServer::WebServer(const char* ipAddress, int port)
{
	m_ipAddress = ipAddress;
	m_port = port;
}
// Hàm xử lý khi nhận được Request từ trình duyệt
void WebServer::MessageReceived(int CSocket, const char* message, int length)
{
	// Lấy ra các cụm từ từ message và lưu vào parsed
	istringstream iss(message);
	vector<string> parsed((istream_iterator<string>(iss)), istream_iterator<string>());

	// Thiết lập các trường hợp mặc định
	string content = "<h1>404 Not Found</h1>";
	string htmlFile = "/index.html";
	int errorCode = 404;

	// Nếu là GET thì bắt đầu lấy địa chỉ
	if (parsed.size() >= 3 && parsed[0] == "GET")
	{
		htmlFile = parsed[1];
		if (htmlFile == "/")
		{
			htmlFile = "/index.html";
		}
		if (htmlFile.find("%20") != -1)
		{
			replaceAll(htmlFile, "%20", " ");
		}
	}
	else
	{
		if (parsed.size() >= 3 && parsed[0] == "POST")
		{
			string temp = parsed.back();
			int pos = temp.find('&', 0);
			int pos1 = temp.find('&', pos + 1);
			string temp1 = temp.substr(6, pos - 6);
			string temp2 = temp.substr(pos + 5, pos1 - pos - 5);
			if (temp1 == "admin" && temp2 == "admin")
			{
				htmlFile = "/RedirectInfor.html";
			}
			else
			{
				htmlFile = "/Redirect404.html";
			}
		}
	}
	if (htmlFile == "/files.html")
	{
		ifstream f(".\\webstored/htmltemp.html", ios::binary); 
		if (f) {
			ostringstream temp;
			temp << f.rdbuf();
			content = temp.str();
			errorCode = 200;
		}
		f.close();
		vector<string> files;
		vector<string> date;
		vector<string> size;
		list_dir(".\\webstored\\DownloadFolder\\", files);
		files.erase(files.begin());
		files.erase(files.begin());
		GetTime(files, date);
		GetSize(files, size);
		string temp1;
		while (!files.empty())
		{
			
			temp1 += "<tr><td data-value=";
			temp1.push_back('\"');
			temp1 += files.back();
			temp1.push_back('\"');
			temp1 += "><a class =";
			temp1.push_back('\"');
			temp1 += "icon file";
			temp1.push_back('\"');
			temp1 += " href=";
			temp1.push_back('\"');
			temp1 += "http://192.168.137.1/DownloadFolder/" + files.back();
			temp1.push_back('\"');
			temp1 += " download>"+files.back()+"</a></td><td class=";
			temp1.push_back('\"');
			temp1 += "detailsColumn";
			temp1.push_back('\"');
			temp1 += " data-value=";
			temp1.push_back('\"');
			temp1 += "1426116";
			temp1.push_back('\"');
			temp1 += ">"+size.back()+" B</td><td class = ";
			temp1.push_back('\"');
			temp1 += "detailsColumn";
			temp1.push_back('\"');
			temp1 += " data-value = ";
			temp1.push_back('\"');
			temp1 += "1592905434";
			temp1.push_back('\"');
			temp1 += ">"+date.back() +"</td></tr>\n";
			files.pop_back();
			size.pop_back();
			date.pop_back();
		}
		temp1 += "</tbody>\n</table>\n</body>\n</html>";
		content += temp1;
	}
	else
	{
		ifstream f(".\\webstored" + htmlFile, ios::binary); //taking file as inputstream
		if (f) {
			ostringstream ss;
			ss << f.rdbuf(); // reading data
			content = ss.str();
			errorCode = 200;
		}
		f.close();
	}
	ostringstream oss;
	if(htmlFile.find("DownloadFolder")==-1)
	{
		oss << "HTTP/1.1 " << errorCode << " OK\r\n";
		oss << "Cache-Control: no-cache, private\r\n";
		oss << "Content-Type: text/html; charset=utf-8\r\n";
		oss << "Content-Type: multipart\r\n";
		//oss << "Content-Type: application/pdf\r\n";
		oss << "Content - Disposition: attackment\n";
		oss << "Content-Length: " << content.size() << "\r\n";
		oss << "\r\n";
		oss << content;
	}
	else
	{
		if (htmlFile.find("pdf") != -1)
		{
			oss << "HTTP/1.1 " << errorCode << " OK\r\n";
			oss << "Cache-Control: no-cache, private\r\n";
			//oss << "Content-Type: text/html; charset=utf-8\r\n";
			oss << "Content-Type: multipart\r\n";
			oss << "Content-Type: application/pdf\r\n";
			oss << "Content - Disposition: attackment\n";
			oss << "Content-Length: " << content.size() << "\r\n";
			oss << "\r\n";
			oss << content;
		}
		else
		{
			oss << "HTTP/1.1 " << errorCode << " OK\r\n";
			oss << "Cache-Control: no-cache, private\r\n";
			//oss << "Content-Type: text/html; charset=utf-8\r\n";
			oss << "Content-Type: multipart\r\n";
			//oss << "Content-Type: application/pdf\r\n";
			oss << "Content - Disposition: attackment\n";
			oss << "Content-Length: " << content.size() << "\r\n";
			oss << "\r\n";
			oss << content;
		}
	}
	string output = oss.str();
	int size = output.size() + 1;
	 // Gửi lại cho Client
	sendToClient(CSocket, output.c_str(), size);
}

void WebServer::ClientConnected(int clientSocket)
{

}
void WebServer::ClientDisconnected(int clientSocket)
{

}
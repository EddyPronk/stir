#include <editline/readline.h>
#include <curl/curl.h>
#include <string>
#include <iostream>

using namespace std;

struct Buffer
{
	std::string data;
	size_t callback(void* ptr, size_t size, size_t nmemb)
	{
		data = std::string(reinterpret_cast<char*>(ptr), size*nmemb);
	}
};

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	Buffer& buf(*reinterpret_cast<Buffer*>(data));
	buf.callback(ptr, size, nmemb);
	return 0;
}

struct http_client
{
	CURL* curl;
	http_client()
		: curl(curl_easy_init()) {}
	~http_client()
	{
		curl_easy_cleanup(curl);
	}
	std::string get(const std::string& url)
	{
		Buffer buf;
		std::cout << "fetching " << url << std::endl;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

		CURLcode res = curl_easy_perform(curl);

		return buf.data;
	}
	std::string post(const std::string& url, const std::string& arg)
	{ 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, arg.c_str());
		return get(url);
	}

};

bool ends_with(const std::string& s, const std::string t)
{
	string::size_type len = t.length();
	if (s.length() >= len)
		if (s.substr(s.length() - len, len) == t)
			return true;
	return false;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cerr << "Usage: progname [URL]" << std::endl;
		return 0;
	}

	std::string base_url = argv[1];
	std::string line;

	curl_global_init(CURL_GLOBAL_ALL);

	std::string url = base_url + "/start_session";
	http_client http;
	std::string session_id = http.get(url);

	std::string lines;
	while (true)
	{
		char* raw_line = readline ("~ ");
		if (!raw_line)
			break;
		string line(raw_line);
		if (ends_with(line, ".."))
		{
			lines += line.substr(0, line.length() - 2) + "\n";
			string url = base_url + "/evaluate?session=" + session_id;
			string response = http.post(url, lines);
			cout << response << endl;
			lines = "";
		}
		else
			lines += line + "\n";

		add_history(raw_line);
	}

	http.get(base_url + "/end_session?session=" + session_id);
	std::cerr << std::endl;
	std::cerr << "exiting" << std::endl;

	return 0;
}

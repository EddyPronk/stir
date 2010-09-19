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
	std::string base_url;
	std::string session_id;
	http_client(const std::string& url)
		: curl(curl_easy_init())
		, base_url(url) {}
	~http_client()
	{
		curl_easy_cleanup(curl);
	}
	void start_session()
	{
		session_id = get(base_url + "/start_session");
	}
	void end_session()
	{
		get(base_url + "/end_session");
	}
	std::string get(const std::string& url)
	{
		std::cout << "fetching " << url << std::endl;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		Buffer buf;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

		curl_slist* chunk = NULL;
		chunk = curl_slist_append(chunk, ("User-Agent: stir/0.1"));
		if(session_id.length())
			chunk = curl_slist_append(chunk, ("Session-Id: " + session_id).c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

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

	http_client http(base_url);
	http.start_session();

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
			string url = base_url + "/evaluate";
			string response = http.post(url, lines);
			cout << response << endl;
			lines = "";
		}
		else
			lines += line + "\n";

		add_history(raw_line);
	}

	http.end_session();
	std::cerr << std::endl;
	std::cerr << "exiting" << std::endl;

	return 0;
}

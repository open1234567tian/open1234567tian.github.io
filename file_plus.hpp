#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <codecvt>
#include <vector>
#if defined(_WIN32)
#include <Windows.h>
#endif
#include <iterator>
#include <algorithm>
#include <sys/types.h>
#include <sstream>
#include <locale>
#if defined(__linux__)
#include <unistd.h>
#include <pwd.h>
#include <limits.h>
#endif

using namespace std;
using namespace filesystem;

namespace file_plus {
	const string string_null = "";
	struct PATH {
		string parent;
		string name;
	};

	string Path_to_string(PATH p)
	{
#if defined(_WIN32)
		if (p.parent == string_null)
		{
			return p.name;
		}
		if (p.name == string_null)
		{
			return p.parent;
		}
		return p.parent + "//" + p.name;
#endif
#if defined(__linux__)
		if (p.parent == string_null)
		{
			return p.name;
		}
		if (p.name == string_null)
		{
			return p.parent;
		}
		return p.parent + "/" + p.name;
#endif
	}
	wstring To_wstring(string a)
	{
		wstring b(a.begin(), a.end());
		return b;
	}
	PATH String_to_path(string p)
	{
		int l = p.length();
		int where = l;
		string li, we;
		bool c = false;
		for (int i = l - 1; i >= 0; i--)
		{
			if (p[i] == '/')
			{
				c = true;
				where = i;
				break;
			}
			li = p[i] + li;
		}
		where--;
		if (!c)
		{
			return { string_null,p };
		}
#if defined(_WIN32)
		where--;
#endif
		for (int i = 0; i <= where; i++)
		{
			we += p[i];
		}
		return { we,li };
	}
	string Username()
	{
#if defined(_WIN32)
		DWORD userNameSize = 256 + 1;
		vector<wchar_t> userName(userNameSize);
		if (GetUserNameW(&userName[0], &userNameSize)) {
			wstring ws(&userName[0]);
			string str(ws.begin(), ws.end());
			return str;
		}
#endif
#if defined(__linux__)
		uid_t euid = geteuid();
		passwd* pwd = getpwuid(euid);
		return pwd->pw_name;
#endif
	}
	bool File_exist(PATH u)
	{
		string url = Path_to_string(u);
		ifstream filess(url);
		return filess.good();
	}
	string File_read(PATH pathh, int noline)
	{
		string url = Path_to_string(pathh);
		ifstream file(url);
		if (!file.is_open())
		{
			return "fpE1";
		}
		string line;
		int crl = 0;
		while (getline(file, line))
		{
			crl++;
			if (crl == noline)
			{
				break;
			}
		}
		if (crl < noline)
		{
			return "fpE2";
		}
		return line;
	}
	void File_change(PATH printt, string what, int doing)
	{
		string print = Path_to_string(printt);
		if (doing == 1)
		{
			fstream file;
			file.open(print, ios::in | ios::out | ios::trunc);
			if (file.is_open())
			{
				file << what << endl;
				file.close();
			}
			else {
				cout << "fpE1" << endl;
			}
		}
		else {
			ofstream file;
			file.open(print, ios::app);
			if (file.is_open())
			{
				file << what;
				if (doing == 3)
				{
					file << endl;
				}
				file.close();
			}
			else {
				cout << "fpE1" << endl;
			}
		}
		return;
	}
	void File_new(PATH inn, bool wjf)
	{
		string in = Path_to_string(inn);
		if (!File_exist({ inn.parent,string_null }))
		{
			cout << "fpE1" << endl;
			return;
		}
		if (wjf)
		{
			path p = in;
			if (!create_directory(p))
			{
				cout << "fpE1" << endl;
			}
		}
		else {
			ofstream file(in);
			if (file.is_open())
			{
				file.close();
			}
			else {
				cout << "fpE1" << endl;
			}
		}
	}
	void File_delete(PATH urll)
	{
		string url = Path_to_string(urll);
		if (remove(url))
		{
			return;
		}
		cout << "fpE1" << endl;
	}
	void Clone(PATH onn, PATH too)
	{
		string on = Path_to_string(onn);
		string to = Path_to_string(too);
		ifstream src(on, ios::binary);
		if (!src.is_open()) {
			cout << "fpE1" << endl;
			return;
		}
		ofstream dst(to, ios::binary);
		if (!dst.is_open()) {
			cout << "fpE1" << endl;
			src.close();
			return;
		}
		dst << src.rdbuf();
		src.close();
		dst.close();
	}
	int File_line(PATH filenamee) {
		string filename = Path_to_string(filenamee);
		ifstream file(filename);
		stringstream buffer;
		buffer << file.rdbuf();
		string content = buffer.str();
		return count(content.begin(), content.end(), '\n');
	}
	int Folder_files_number(PATH pathh) {
		string path = Path_to_string(pathh);
		int count = 0;
		if (exists(path) && is_directory(path)) {
			for (const auto& entry : directory_iterator(path)) {
				if (entry.is_regular_file()) {
					++count;
				}
			}
		}
		else {
			cout << "fpE1" << endl;
		}
		return count;
	}
	string Folder_files_numth(PATH pathss, int n) {
		path directoryPath = Path_to_string(pathss);
		vector<string> files;
		for (const auto& entry : directory_iterator(directoryPath)) {
			if (entry.is_regular_file()) {
				files.push_back(entry.path().string());
			}
		}
		if (n < 1 || n > files.size()) {
			throw out_of_range("fpE2");
		}
		return files[n - 1];
	}
	class file_system {
	private:
		string os;
		string x86s;
		string usr;
		PATH paths;
	public:
		void init_file_system()
		{
#ifdef _WIN32
			os = "Windows";
			wchar_t pat[MAX_PATH] = { 0 };
			DWORD result = GetModuleFileNameW(NULL, pat, MAX_PATH);
			if (result > 0 && result < MAX_PATH) {
				wstring_convert<codecvt_utf8<wchar_t>> converter;
				string str = converter.to_bytes(pat);
				paths = String_to_path(str);
			}
#else
			os = "Other";
#endif
#ifdef __APPLE__
			os = "MacOS";
#endif		
#ifdef __linux__
			os = "Linux";
			char pat[PATH_MAX];
			ssize_t count = readlink("/proc/self/exe", pat, PATH_MAX - 1);	
			if (count != -1) {
				pat[count] = '\0';
				string str(pat);
				paths=String_to_path(pat);
			} else {
				perror("readlink");
			}
#endif
#if defined(__x86_64__) || defined(_M_X64)
			x86s = "x86_64";
#elif defined(i386) || defined(__i386__) || defined(_M_IX86)
			x86s = "x86";
#elif defined(__arm__) || defined(_M_ARM)
			x86s = "ARM";
#elif defined(__aarch64__) || defined(_M_ARM64)
			x86s = "ARM64";
#else
			x86s = "Other";
#endif
			usr = Username();
		}
		string print(string name)
		{
			if (name == "OS")
			{
				return os;
			}
			else if (name == "SA")
			{
				return x86s;
			}
			else if (name == "USR")
			{
				return usr;
			}
			else if (name == "PATH")
			{
				return Path_to_string(paths);
			}
			else {
				return string_null;
			}
		}
	};
}

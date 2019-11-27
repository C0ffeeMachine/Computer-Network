#pragma once
#include<fstream>
#include<strstream>
#include<sstream>
#include<string>

constexpr auto MAXLINE = 128;

class ExportFile {
private:
	std::ifstream in;
	std::ofstream out;

public:
	ExportFile(std::string outname) {
		in.open("kernal.dat");
		out.open(outname + ".cc");

	}
	ExportFile() {
		in.open("kernal.dat");
		out.open("defaultoutputfile.cc");

	}
	~ExportFile() {
		in.close();
		out.close();
	}
	ExportFile(const ExportFile&) = delete;

	void CopyToOut(std::string tag) {
		std::string line;
		bool start = false, end=false;
		while (!in.eof()) {
			std::getline(in, line);
			if (line == '_'+tag) {
				if (start) end=true;
				start ^=1;
				if(start) continue;
				if (end) break;
			}
			if (start) {
				if (line[0] == '_' || line[0] == '-') continue;
				else out << line << std::endl;
			}
		}
		in.clear();
		in.seekg(0, std::ios::beg);
	}

	void FormatToOut(std::string tag,const std::vector<std::string>& container) {
		std::string line,temp;
		bool found=false,numin=false;
		int n,i=0;
		while (!in.eof()) {
			std::getline(in, line);
			std::stringstream ss(line);
			if (line == '-' + tag) {
				found = true;
				numin = true;
				continue;
			}
			if (found) {
				if (numin) {
					numin = false;
					ss >> n;
					continue;
				}
				if (line[0] == '_') continue;
				else if (line[0] == '-') break;
				else {
					int lvc = std::count(line.begin(),line.end(),'#');
					if (lvc>0) {
						while (std::getline(ss, temp, '#')) {
							out << temp;
							if (i < n && lvc) {
								out << container[i];
								i++;
								lvc--;
							}
						}
						out << std::endl;
					}else out << line << std::endl;
				}
			}
		}
		in.clear();
		in.seekg(0, std::ios::beg);
	}
	void WriteLine(std::string output) {
		out << output << std::endl;
	}
};
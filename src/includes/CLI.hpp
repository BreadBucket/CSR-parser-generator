#include <string>


namespace CLI {
	extern std::string programName;
	extern std::string inputFilePath;
	extern std::string outputFilePath;
	
	extern std::string graph_outputFilePath;
	extern std::string graph_outputFormat;
	
	extern bool unicode;
	extern bool ansi;
	extern int tabSize;
	extern bool verifyReduction;
}


namespace CLI {
	void parse(int argc, char const* const* argv);
	void clear();
}

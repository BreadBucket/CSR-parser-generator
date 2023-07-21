#include <string>


namespace CLI {
	extern std::string programName;
	extern std::string inputFilePath;
	extern std::string outputFilePath;
	extern int tabSize;
	extern bool verifyReduction;
}


namespace CLI {
	void parse(int argc, char const* const* argv);
	void clear();
}

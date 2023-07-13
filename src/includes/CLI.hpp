#include <string>


namespace CLI {
	extern const char* programName;
	extern const char* inputFilePath;
	extern const char* outputFilePath;
	extern int tabSize;
}


namespace CLI {
	void parse(int argc, char const* const* argv);
	void clear();
}

#include <string>


namespace CLI {
	extern const char* inputFilePath;
	extern const char* outputFilePath;
	extern int tabSize;
}


namespace CLI {
	void parse(int argc, char* const* argv);
	void clear();
}

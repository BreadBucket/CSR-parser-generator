#include <string>
#include <optional>


// ------------------------------------[ Variables ] ---------------------------------------- //


namespace CLI {
	extern std::string programName;
	
	extern std::optional<std::string> inputFilePath;				// If empty and not tty, open '0'
	
	extern std::optional<std::string> outputFilePath;				// If all outputs are null, open '1'
	extern std::optional<std::string> outputHeaderFilePath;
	extern std::optional<std::string> outputHeaderFilePath_token;
	
	extern std::optional<std::string> graph_outputFilePath;
	extern std::optional<std::string> graph_outputFormat;
	
	extern bool verifyReduction;
	
	extern bool unicode;
	extern bool ansi;
	extern int tabSize;
	
	extern bool help;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


namespace CLI {
	void parse(int argc, char const* const* argv);
	void clear();
}


// ------------------------------------------------------------------------------------------ //
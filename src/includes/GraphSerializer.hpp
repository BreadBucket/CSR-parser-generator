#pragma once
#include <string>
#include <ostream>

#include "Document.hpp"


namespace csr {
class GraphSerializer {
// ---------------------------------- [ Structures ] ---------------------------------------- //
public:
	enum class Format {
		UNKNOWN,
		TXT
	};
	
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Format format = Format::TXT;
	bool unicode = true;
	bool ansi = true;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void serialize(std::ostream& stream, const Document& doc);
	void serialize_txt(std::ostream& stream, const Document& doc);

public:
	static Format getFormat(const std::string& filePath);
	
// ------------------------------------------------------------------------------------------ //
};
}
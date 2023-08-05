#pragma once
#include <vector>
#include <memory>


namespace csr {
	class ParsedReduction;
	class Reduction;
	class SymbolEnum;
}


namespace csr {
// ----------------------------------- [ Functions ] ---------------------------------------- //
	
	/**
	 * @brief Convert list of parsed reductions into DFA reductions.
	 *        ID's of reductions correspond to their index in the list.
	 * @param v List of parsed reductions.
	 */
	void convertReductions(
		const std::vector<std::shared_ptr<ParsedReduction>>& in_parsedReductions,
		std::vector<std::shared_ptr<Reduction>>& out_reductions,
		SymbolEnum& symEnum
	);
	
// ------------------------------------------------------------------------------------------ //
}
#pragma once
#include <vector>

#include "ParsedSymbol.hpp"
#include "ParsedReduction.hpp"
#include "Reduction.hpp"
#include "SymbolEnum.hpp"


namespace csr {
// ----------------------------------- [ Functions ] ---------------------------------------- //
	
	/**
	 * @brief Insert all symbols into an enum.
	 * @param v List of symbols.
	 * @param out_enum Refrence of the destination enum.
	 */
	void enumerate(const std::vector<ParsedSymbol>& v, SymbolEnum& out_symEnum);
	
	/**
	 * @brief Insert all symbols from all reductions into an enum.
	 * @param v List of reductions.
	 * @param out_enum Refrence of the destination enum.
	 */
	void enumerate(const std::vector<ParsedReduction>& v, SymbolEnum& out_symEnum);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
	
	/**
	 * @brief Converd parsed reduction info into a DFA reduction.
	 * @param reduction Reference to the parsed reduction info.
	 * @param symEnum Enum of symbols to use when converting symbol names into DFA symbols.
	 * @return Reduction with converted left and right side. ID is left unchanged.
	 */
	Reduction* createReduction(const ParsedReduction& reduction, const SymbolEnum& symEnum);
	
	/**
	 * @brief Convert list of parsed reductions into DFA reductions.
	 *        ID's of reductions correspond to their index in the list.
	 * @param v List of parsed reductions.
	 * @param symEnum Enum of symbols to use when converting symbol names into DFA symbols.
	 * @tparam T Pointer type of converted reductions:
	 *            [const] T*,
	 *            uniq_ptr<[const] T>,
	 *            shared_ptr<[const] T>.
	 * @return List of converted reductions.
	 */
	template<typename T>
	std::vector<T> createReductions(const std::vector<ParsedReduction>& v, const SymbolEnum& symEnum);
	
// ------------------------------------------------------------------------------------------ //
}
/*
 * EndomorphsimSLP.h
 *
 *  Created on: Jan 29, 2013
 *      Author: pmorar
 */

#ifndef CRAG_FREE_GROUPS_ENDOMORPHISM_SLP_H_
#define CRAG_FREE_GROUPS_ENDOMORPHISM_SLP_H_

#include <map>
#include <random>
#include <assert.h>
#include "slp.h"

namespace crag {

/**
 * Represents a free group endomorphism using straight-line programs.
 * @tparam TerminalSymbol terminal symbols class
 */
template<typename TerminalSymbol = int>
class EndomorphismSLP {
public:

  //use default copy/move constructors/assignments

	//! Returns the identity automorphism
	static EndomorphismSLP identity() {
		return EndomorphismSLP();
	}

	//! Returns the automorphism inverting the specified terminal symbbol.
	/**
	 * @param symbol inverted terminal symbol, must be > 0
	 */
	static EndomorphismSLP inverter(const TerminalSymbol& symbol) {
	  assert(is_positive_terminal_symbol(symbol));
		return EndomorphismSLP(symbol);
	}

	//! Returns the automorphism mapping #symbol to the product #symbol #right_multiplier.
	/**
	 * @param symbol mapped terminal symbol, must be > 0
	 * @param	right_multiplier	right multiplier
	 */
	static EndomorphismSLP right_multiplier(const TerminalSymbol& symbol, const TerminalSymbol& right_multiplier) {
	  assert(is_positive_terminal_symbol(symbol));
	  EndomorphismSLP tmp;
	  tmp.images_.insert(symbol,
	      slp::NonterminalVertex(terminal_vertex(symbol), terminal_vertex(right_multiplier)));
		return tmp;
	}

	//! Returns the automorphism mapping #symbol to the product #left_multiplier #symbol.
	/**
	 * @param  left_multiplier  left multiplier
	 * @param symbol mapped terminal symbol, must be > 0
	 */
	static EndomorphismSLP left_multiplier(const TerminalSymbol& left_multiplier, const TerminalSymbol& symbol) {
	  assert(is_positive_terminal_symbol(symbol));
	  EndomorphismSLP tmp;
	  tmp.images_.insert(symbol,
	          slp::NonterminalVertex(terminal_vertex(left_multiplier), terminal_vertex(symbol)));
		return tmp;
	}

	//! Returns the composition of endomorphisms specified by the range
	template<typename Iterator>
	static EndomorphismSLP composition(Iterator begin, Iterator end) {
	  return identity().compose_with(begin, end);
	}

	//! Returns the composition of #num endomorphisms produced by #producer
	/**
	 * @param num      the number of endomorphisms to compose
	 * @param producer endomorphisms generator (using operator())
	 */
  template<typename Generator>
  static EndomorphismSLP composition(unsigned int num, Generator& generator) {
    return identity().compose_with(num, generator);
  }

	//! Compose with the given endomorphism.
	EndomorphismSLP& operator*=(const EndomorphismSLP& a);

	//! Compose with the given endomorphism.
	EndomorphismSLP& operator*(const EndomorphismSLP& a) const {
		EndomorphismSLP result(*this);
		return result *= a;
	}

	//! Compose with endomorphisms specified by the range.
	template<typename Iterator>
  EndomorphismSLP& compose_with(Iterator begin, Iterator end) {
  for(;begin != end; ++begin)
    (*this) *= *begin;
  return *this;
	}

	//! Compose with #num automorphism produced by #producer
	/**
   * @param num the number of endomorphisms to compose with
   * @param producer endomorphisms generator (using operator())
   */
  template<typename Generator>
  EndomorphismSLP& compose_with(unsigned int num, Generator& generator) {
    for (unsigned int i = 0; i < num; ++i)
      (*this) *= generator();
    return *this;
  }


	//! Returns the image of the terminal.
	SLPProducedWord image(const TerminalSymbol& t) const {
	  bool is_positive = is_positive_terminal_symbol(t);
		return SLPProducedWord(is_positive ? slp(t) : slp(-t).negate());
	}

	//! Returns the root of the straight-line program representing the positive terminal symbol.
	/**
	 * @param t positive terminal symbol
	 */
	slp::Vertex slp(const TerminalSymbol& t) const {
	  assert(is_positive_terminal_symbol(t));
		auto result = images_.find(t);
		if (result == images_.end()) //if it is not in images_, then it is the identity map.
			return terminal_vertex(t);
		else
			return result->second;
	}

private:
  typedef typename slp::TerminalVertexTemplate<TerminalSymbol> terminal_vertex;

  //! Checks whether the symbol is not inverse.
  static bool is_positive_terminal_symbol(const TerminalSymbol& symbol) {
    static TerminalSymbol null_symbol();
    return symbol > null_symbol;
  }

	//! The default constructor.
	EndomorphismSLP() {}

	EndomorphismSLP(const TerminalSymbol& inverted) {
		images_.insert(inverted,
		  terminal_vertex(inverted).negate());
	}

	//! The representation of images of terminal symbols by straight-line programs
	/**
	 * If there is no entry for a given terminal symbol, then its image is the terminal itself.
	 */
	std::map<TerminalSymbol, slp::Vertex> images_;
};



//! Automorphisms generator
/**
 * @tparam TerminalSymbol          terminal symbol representation. We suppose it has a constructor taking index
 * @tparam RandomEngine            engine generating uniformly random non-negative numbers. See std::random library documentation.
 * @tparam TerminalSymbolIndexType terminal symbol index type
 */
template <typename TerminalSymbol = int,
  typename RandomEngine = std::default_random_engine,
  typename TerminalSymbolIndexType = int>
class UniformAutomorphismSLPGenerator {
public:
	typedef TerminalSymbolIndexType index_type;

	//! Constructs a generator of automorphisms of the free group of the given rank.
	/**
	 * @param rank free group rank > 0
	 */
	UniformAutomorphismSLPGenerator(index_type rank)
		: UniformAutomorphismSLPGenerator(rank, RandomEngine())
	{}

	//! Constructs a generator of automorphisms of the free group of the given rank.
	/**
	 * @param rank free group rank > 0
	 * @param seed random engine seed for creation of a new one
	 */
  UniformAutomorphismSLPGenerator(index_type rank, typename RandomEngine::result_type seed)
    : UniformAutomorphismSLPGenerator(rank, RandomEngine(seed))
  {}

	//! Constructs a generator of automorphisms of the free group of the given rank.
	/**
	 * @param rank free group rank > 0
	 * @param random_engine random engine
	 */
	UniformAutomorphismSLPGenerator(index_type rank, RandomEngine& random_engine)
    : RANK(rank),
      RIGHT_MULTIPLIERS_COUNT(2 * rank * (rank - 1)),
      INVERTERS_COUNT(rank),
      random_engine_(random_engine),
      random_distr_(0, COUNT - 1) {
	  assert(rank > 0);
  }

	//! Generates a random automorphism
	EndomorphismSLP<TerminalSymbol> operator()() {
	  index_type r_val = random_distr_(random_engine_);
	  if (r_val < MULTIPLIERS_COUNT) {
      const bool inverted = r_val % 2;
      r_val >>= 1;
      const index_type symbol_index = 1 + (r_val % RANK);
      const TerminalSymbol symbol(symbol_index);
      index_type multiplier_index = 1 + r_val / RANK;
      if (multiplier_index >= symbol_index)
        ++multiplier_index;//correction for the skipping of the symbol when pick the multiplier
      const TerminalSymbol multiplier(inverted ? - multiplier_index : multiplier_index);
      if (r_val < RIGHT_MULTIPLIERS_COUNT)
        return EndomorphismSLP<TerminalSymbol>::right_multiplier(symbol, multiplier);
      else
        return EndomorphismSLP<TerminalSymbol>::left_multiplier(multiplier, symbol);
    } else {
      return EndomorphismSLP<TerminalSymbol>::inverter(TerminalSymbol(r_val));
    }
	}


private:
	const index_type RANK;
	const index_type RIGHT_MULTIPLIERS_COUNT;
  const index_type LEFT_MULTIPLIERS_COUNT = RIGHT_MULTIPLIERS_COUNT;
  const index_type MULTIPLIERS_COUNT = RIGHT_MULTIPLIERS_COUNT + LEFT_MULTIPLIERS_COUNT;
  const index_type INVERTERS_COUNT;
  const index_type COUNT = MULTIPLIERS_COUNT + INVERTERS_COUNT;
  //! Random generator, which is a binding of provided distribution and uniform random engine.
  RandomEngine random_engine_;
  std::uniform_int_distribution<index_type> random_distr_;
};

namespace internal {
  class EndomorphismSLPToCopyInspector: public slp::Inspector<slp::inspector::Postorder> {
    const std::unordered_map<slp::Vertex, slp::Vertex>& copied_vertices_;

    EndomorphismSLPToCopyInspector(const std::unordered_map<slp::Vertex, slp::Vertex>& new_vertices)
      : copied_vertices_(new_vertices) {
    }

    bool is_task_valid(const InspectorTask& task) {
      return slp::Inspector::is_task_valid(task) &&
          copied_vertices_.find(task.vertex) == copied_vertices_.end();//we have not copied the vertex yet
    }
  };
}

template <typename TerminalSymbol>
EndomorphismSLP<TerminalSymbol>& EndomorphismSLP<TerminalSymbol>::operator*=(const EndomorphismSLP<TerminalSymbol>& a) {
	std::unordered_map<slp::Vertex, slp::Vertex> new_vertices;//a's vertices to new vertices correspondence

	for (auto root_iterator: a.images_) {
		const slp::Vertex image_root = root_iterator->second;
		//for each root we go over the tree using inspector,
		//attach terminal vertices to the roots of our endomorphism, and copy the tree above
		internal::EndomorphismSLPToCopyInspector inspector(image_root);
		while (!inspector.stopped()) {
			const slp::Vertex& current_vertex = *inspector;
			if (new_vertices.find(current_vertex) == new_vertices.end()) {//it was not copied yet
			  terminal_vertex t_vertex(current_vertex);
				if (t_vertex != slp::Vertex::Null) {//the vertex is terminal so map it to our corresponding root
				  const TerminalSymbol& symbol = t_vertex.terminal_symbol();
				  bool is_positive = is_positive_terminal_symbol(symbol);
					auto our_root = slp(is_positive ? symbol : -symbol);
					new_vertices.insert(std::make_pair(current_vertex,
              is_positive ? slp(symbol) : slp(-symbol).negate()));
				} else {//for a nonterminal we already processed its children because postorder inspector
					auto left = new_vertices.find(current_vertex.left_child())->second;
					auto right = new_vertices.find(current_vertex.right_child())->second;
					new_vertices.insert(std::make_pair(current_vertex, slp::NonterminalVertex(left, right)));
				}
			}
			++inspector;
		}
	}

	//we update our roots to the new ones when necessary
	for (auto iterator: a.images_) {
		images_[iterator->first] = new_vertices.find(iterator->second);
	}
	return *this;
}



} /* namespace crag */
#endif /* CRAG_FREE_GROUPS_ENDOMORPHISM_SLP_H_ */

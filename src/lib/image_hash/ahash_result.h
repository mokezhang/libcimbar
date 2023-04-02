/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "bit_extractor.h"
#include <array>
#include <iostream>
#include <utility>

namespace image_hash
{

class ahash_result
{
public:
	static const int FAST = 5;
	static const int ALL = 9;

	// out from the center.
	// 4 == center.
	// 5, 7, 3, 1 == sides.
	// 8, 0, 2, 6 == corners.
	// mode=FAST will discard the corner checks.
	static constexpr std::array<unsigned, 9> _ORDER = {4, 5, 7, 3, 1, 8, 0, 2, 6};

public:
	class iterator
	{
	public:
		iterator(const ahash_result& hr, unsigned i=0)
			: _hr(hr)
			, _i(i)
		{}

		std::pair<unsigned, uint64_t> operator*()
		{
			unsigned idx = _hr._ORDER[_i];
			return {idx, _hr._results[idx]};
		}

		iterator& operator++()
		{
			++_i;
			return *this;
		}

		bool operator!=(const iterator& other) const
		{
			return _i != other._i;
		}

	protected:
		const ahash_result& _hr;
		unsigned _i;
	};

public:
	ahash_result(const uint64_t& bits, unsigned mode=ALL)
		: _bits(bits)
		, _mode(mode)
	{
		if (mode == ALL)
			_results = extract_all();
		else
			_results = extract_fast();
	}

	std::array<uint64_t, 9> extract_all() const
	{
		bit_extractor<uint64_t, 49, 5> be(_bits);
		return {
			// top row -- top left bit is the start bit (0). bottom right is end bit.
			be.extract_tuple( be.pattern(0) ), // left
			be.extract_tuple( be.pattern(1) ),
			be.extract_tuple( be.pattern(2) ), // right
			// middle row
			be.extract_tuple( be.pattern(7) ),
			be.extract_tuple( be.pattern(8) ),
			be.extract_tuple( be.pattern(9) ),
			// bottom row
			be.extract_tuple( be.pattern(14) ),
			be.extract_tuple( be.pattern(15) ),
			be.extract_tuple( be.pattern(16) )
		};
	}

	std::array<uint64_t, 9> extract_fast() const
	{
		bit_extractor<uint64_t, 49, 5> be(_bits);
		// skip the corners
		return {
			0,
			be.extract_tuple( be.pattern(1) ),
			0,
			// middle row
			be.extract_tuple( be.pattern(7) ),
			be.extract_tuple( be.pattern(8) ),
			be.extract_tuple( be.pattern(9) ),
			// bottom row
			0,
			be.extract_tuple( be.pattern(15) ),
			0
		};
	}

	iterator begin() const
	{
		return iterator(*this);
	}

	iterator end() const
	{
		return iterator(*this, _mode);
	}

	uint64_t operator[](unsigned idx) const
	{
		return _results[idx];
	}

	size_t size() const
	{
		return _results.size();
	}

protected:
	uint64_t _bits;
	int _mode;
	std::array<uint64_t, 9> _results;
};

}

#include "unittest.h"

#include "FloodDecodePositions.h"

#include <iostream>
#include <string>
#include <vector>

TEST_CASE( "FloodDecodePositionsTest/testSimple", "[unit]" )
{
	const unsigned posCount = 12400;

	FloodDecodePositions cells(9, 112, 8, 6);
	std::set<unsigned> remainingPos;
	for (int i = 0; i < posCount; ++i)
		remainingPos.insert(i);

	// test a handful of coordinates. We'll just make sure we go through the rest in some order.
	assertFalse( cells.done() );

	unsigned i;
	CellPositions::coordinate xy;
	CellDrift drift;

	// validate seed points
	unsigned count = 0;

	for (int c : {0, 1, 2, 3})
	{
		// 0
		std::tie(i, xy, drift) = cells.next();
		assertEquals(0, drift.x());
		assertEquals(0, drift.y());

		if (i == 0)
		{
			assertEquals(62, xy.first);
			assertEquals(8, xy.second);
			drift.updateDrift(1, 1);
			cells.update(i, drift, 1);
		}
		else if (i == 99)
		{
			assertEquals(953, xy.first);
			assertEquals(8, xy.second);
			cells.update(i, drift, 2);
		}
		else if (i == 12300)
		{
			assertEquals(62, xy.first);
			assertEquals(1007, xy.second);
			cells.update(i, drift, 2);
		}
		else if (i == 12399)
		{
			assertEquals(953, xy.first);
			assertEquals(1007, xy.second);
			cells.update(i, drift, 2);
		}
		else
			FAIL("i ?= " << i);

		remainingPos.erase(i);
		++count;
	}

	// we did "update(..., 1) for index=1. So the next cells should be the ones adjacent to 0: 1, 100.
	for (int c : {0, 1})
	{
		std::tie(i, xy, drift) = cells.next();
		if (i == 1)
		{
			assertEquals(71, xy.first);
			assertEquals(8, xy.second);
		}
		else if (i == 100)
		{
			assertEquals(62, xy.first);
			assertEquals(17, xy.second);
		}
		else
			FAIL("i ?= " << i);

		assertEquals(1, drift.x());
		assertEquals(1, drift.y());

		cells.update(i, drift, 3);
		remainingPos.erase(i);
		++count;
	}

	// as for the rest... we'll just make sure we hit them all
	while (!cells.done())
	{
		std::tie(i, xy, drift) = cells.next();
		cells.update(i, drift, 1);
		remainingPos.erase(i);
		++count;
	}

	assertEquals(posCount, count);
	assertEquals(0, remainingPos.size());
}
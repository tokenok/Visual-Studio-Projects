///////////////////////////////////////////////////////////
// TeleportPath.cpp
//
// Abin (abinn32@yahoo.com)
///////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TeleportPath.h"
#include "ArrayEx.h"
#include "Matrix.h"

#include <math.h>
#include "ExScreen.h"
#include "ExAim.h"

#define RANGE_INVALID	10000  // invalid range flag

/////////////////////////////////////////////////////////////////////
// Path Finding Result
/////////////////////////////////////////////////////////////////////
enum {   PATH_FAIL = 0,     // Failed, error occurred or no available path
		 PATH_CONTINUE,	    // Path OK, destination not reached yet
		 PATH_REACHED };    // Path OK, destination reached(Path finding completed successfully)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTeleportPath::CTeleportPath(WORD** pCollisionMap, int cx, int cy)
{
	m_ppTable = pCollisionMap;
	m_nCX = cx;
	m_nCY = cy;
	m_ptStart = { 0 };
	m_ptEnd = { 0 };
}

BOOL CTeleportPath::MakeDistanceTable()
{	
	if (m_ppTable == NULL)
		return FALSE;

	// convert the graph into a distance table
	for (WORD x = 0; x < m_nCX; x++)	
	{
		for (WORD y = 0; y < m_nCY; y++)
		{
			if ((m_ppTable[x][y] % 2) == 0)
				m_ppTable[x][y] = ExAim::CalculateDistance(x, y, m_ptEnd.x, m_ptEnd.y);
			else
				m_ppTable[x][y] = RANGE_INVALID;
		}
	}

	m_ppTable[m_ptEnd.x][m_ptEnd.y] = 1;	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////
// The "Get Best Move" Algorithm
//
// Originally developed by Niren7, Modified by Abin
/////////////////////////////////////////////////////////////////////
BOOL CTeleportPath::GetBestMove(COORDS& pos, int nAdjust)
{	
	if(ExAim::CalculateDistance(m_ptEnd, pos) <= D2EX_TP_RANGE)
	{
		pos = m_ptEnd;
		return PATH_REACHED; // we reached the destination
	}

	if (!IsValidIndex(pos.x, pos.y))
		return PATH_FAIL; // fail

	Block(pos, nAdjust);

	COORDS p, best;
	int value = RANGE_INVALID;

	for (p.x = pos.x - D2EX_TP_RANGE; p.x <= pos.x + D2EX_TP_RANGE; p.x++)
	{
		for (p.y = pos.y - D2EX_TP_RANGE; p.y <= pos.y + D2EX_TP_RANGE; p.y++)
		{			
			if (!IsValidIndex(p.x, p.y))
				continue;
		
			if (m_ppTable[p.x][p.y] < value && ExAim::CalculateDistance(p, pos) <= D2EX_TP_RANGE)
			{				
				value = m_ppTable[p.x][p.y];
				best = p;					
			}			
		}
	}

	if (value >= RANGE_INVALID)
		return PATH_FAIL; // no path at all	
	
	pos = best;
	Block(pos, nAdjust);	
	return PATH_CONTINUE; // ok but not reached yet
}

vector<COORDS> CTeleportPath::FindTeleportPath(COORDS ptStart, COORDS ptEnd)
{
	vector<COORDS> v_Output; 

	if (m_nCX <= 0 || m_nCY <= 0 || m_ppTable == NULL)
		return v_Output;
	
	m_ptStart = ptStart;
	m_ptEnd = ptEnd;

	MakeDistanceTable();

	v_Output.push_back(ptStart); // start point

	COORDS pos = ptStart;
	COORDS prevpos = ptStart;

	int nRes = GetBestMove(ptStart);

	while(nRes != PATH_FAIL)
	{

		if (nRes != PATH_REACHED && ExAim::CalculateDistance(v_Output.back(), pos) < 20)
		{
			ExScreen::PrintPartyTextEx(COL_RED, "AT: Trying to avoid teleport loop, because dist == %d...", ExAim::CalculateDistance(v_Output.back(), pos));
			prevpos.x++;
			prevpos.y++;
			pos = prevpos;
			continue;
		}

		if (ExAim::CalculateDistance(v_Output.back(), pos) > D2EX_TP_RANGE)
		{
			prevpos.x--;
			prevpos.y--;
			pos = prevpos;
			ExScreen::PrintPartyTextEx(COL_RED, "AT: Calculated distance is > %d", D2EX_TP_RANGE);
			continue;
		}
		
		if (nRes == PATH_FAIL)
		{
			ExScreen::PrintPartyTextEx(COL_RED, "AT: PATH FAILED!");
			prevpos.x--;
			prevpos.y--;
			pos = prevpos;
			continue;
		}

		v_Output.push_back(pos);

		if(nRes == PATH_REACHED) 
			return v_Output;

		if(v_Output.size()>255) 
			break;

		prevpos = pos;
		nRes = GetBestMove(pos);
	}	

	v_Output.clear();
	return v_Output;
}

void CTeleportPath::Block(COORDS pos, int nRange)
{
	nRange = max(nRange, 1);

	for (WORD i = pos.x - nRange; i < pos.x + nRange; i++)
	{
		for (WORD j = pos.y - nRange; j < pos.y + nRange; j++)
		{
			if (IsValidIndex(i, j))
				m_ppTable[i][j] = RANGE_INVALID;
		}
	}
}



BOOL CTeleportPath::IsValidIndex(WORD x, WORD y) const
{
	return x >= 0 && x < m_nCX && y >= 0 && y < m_nCY;
}


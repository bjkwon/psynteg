// Adaptive.cpp: implementation of the CAdaptive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "psynteg.h"
#include "Adaptive.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAdaptive::CAdaptive()
:factor(false)
{
	descending = 1;
	dualSeries = 0;
	down = 0;
	up = 0;
	finalRev4Avg = 0;
	initialPt = 0.0;
	memset((void*)reversal, 0, sizeof(reversal));
	memset((void*)stepsize, 0, sizeof(stepsize));
	m_str_RefVal[0]='\0';
}

CAdaptive::~CAdaptive()
{
}

int CAdaptive::reversalUpdate (double variable)
{
	// return 1 means the end
	++rev;
	//Append variable at the end of the thresholdCandidate array 
	thresholdCandidate[nCandidates++]=variable;

	if (rev==reversal[revStage])
	{
		rev = 0;
		++revStage;
	}
	return revStage==maxReversalStage;
}

int CAdaptive::Proceed(int corr, double *variable)
{
//  variable [in/out]: updates the variable according to the correct-incorrect history and current response.

	int i, temp1, temp2;

	correctHistory[trial] = corr;
	temp1=0;	temp2=0;
	if (corr==1) // correct answer
	{
		++hitStreak;
		missStreak=0;
		if (hitStreak==down) // time to make it harder
		{
			for (i=trial-down; (i>trial-down-up) && (i>=0) ; i--)
				temp2 += (correctHistory[i]==0);
			if (temp2==up)
			{
				if (reversalUpdate (*variable))
					return 1;
			}
			if (descending)
				if (!factor)
					*variable -= stepsize[revStage];
				else
					*variable /= stepsize[revStage];
			else
				if (!factor)
					*variable += stepsize[revStage];
				else
					*variable *= stepsize[revStage];
			hitStreak = 0;
		}
	}
	else if (corr==0)// incorrect answer
	{
		fineStreak = 0; // because, the normal step size is at least one, i.e., one incorrect answer should clear fineStreak
		++missStreak;
		hitStreak=0;
		if (missStreak==up) // time to make it easier
		{
			for (i=trial-up; (i>trial-down-up) && (i>=0) ; i--)
				temp2 += (correctHistory[i]==1);
			if (temp2==down)
			{
				if (reversalUpdate (*variable))
					return 1;
			}
			if (descending) 
				if (!factor)
					*variable += stepsize[revStage];
				else
					*variable *= stepsize[revStage];
			else
				if (!factor)
					*variable -= stepsize[revStage];
				else
					*variable /= stepsize[revStage];
			missStreak = 0;
		}
	}
	trial++;
	return 0;
}

void CAdaptive::InitAdaptive(int *corrHist, double *thrCand)
{
	nCandidates = 0;
	trial = 0;
	rev = 0;
	revStage = 0;
	hitStreak = 0;
	missStreak = 0;
	fineStreak = 0;
	thresholdCandidate = thrCand;
	correctHistory = corrHist;
	correctHistory[0] = ANY_NUMBER_INT;
	maxReversalStage = GetFirstNonZeroItems(reversal,5);
}

int CAdaptive::GetFirstNonZeroItems(int *array, int length_limit)
{
	int ind=0;
	while (1)
	{
		if (array[ind] == 0)
		{
			return ind;
		}
		else 
			ind++;
		if (ind > length_limit)
			return -1; //error!! 
	}
}

void CAdaptive::SetRefStr(char *str_RefVal)
{
	strcpy(m_str_RefVal, str_RefVal);
}
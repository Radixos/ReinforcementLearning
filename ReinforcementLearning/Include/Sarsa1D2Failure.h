#pragma once

#include "Includes.h"

class Sarsa1D2Failure
{
private:
	double epsilon = 0.01f;	//percentage of actions which are chosen randomly
	double gamma = 0.9f;	//discount factor (The closer to 1 gamma is (e.g. winning the chess game), the more important it is for the agent
						//to maximize gaining rewards in bigger time perspective. The closer to 0 the gamma is (e.g. breaking the pawn),
						//the more important it is for the agent to gain reward immediately. )
	double alfa = 0.001f;	//learning rate of Q function
	double lambda = 0.2f;	//forgetting factor ("freshness factor")

	int steps = 0;
	int const max_attempt = 1000;
	int const max_steps = 1000;

	int const static actionsCount = 2;	//there is always only 2 actions that can be performed in 1D environment
	int const actions[actionsCount] = { 0, 1 };	//1D actions: 0 - left, 1 - right

	int* reinforcementTable;	//declaring the environment
	double** Q;	//action value function
	double** e_Q;	//eligibility values function (activity traces, pl: slady aktywnosci)

	bool t0 = true;
	int attempt = 1;	//attempt number (starting as 1 and incrementing by 1)
	int greedyActionsIndicies[actionsCount] = { 0 };

	int nextState;
	int currentState;
	int action;
	int nextAction;

	std::list <int> path;
public:
	/*
		@brief learning how to avoid punishment
		@param reinforcementTableCount size of the environment
		@param nextState place where the agent starts
		@param useSarsaLambda a boolean to say whether to use Sarsa(lambda) or Sarsa(0)
	*/
	Sarsa1D2Failure(int reinforcementTableCount, int startState, bool useSarsaLambda);
};
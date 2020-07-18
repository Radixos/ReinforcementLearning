#pragma once

#include "Includes.h"

class Sarsa1D
{
private:
	double epsilon = 0.05f;	//percentage of actions which are chosen randomly
	double gamma = 0.9f;	//discount factor (The closer to 1 gamma is (e.g. winning the chess game), the more important it is for the agent
						//to maximize gaining rewards in bigger time perspective. The closer to 0 the gamma is (e.g. breaking the pawn),
						//the more important it is for the agent to gain reward immediately. )
	double alfa = 0.5f;	//learning rate of Q function
	double lambda = 0.9f;	//forgetting factor ("freshness factor")

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
	std::list <int> optimalPath;
public:
	/*
		@brief learning how to reach the target
		@param reinforcementTableCount size of the environment
		@param rewardPlace position of the reward
		@param startState place where the agent starts
		@param useSarsalambda a boolean to say whether to use Sarsa(lambda) or Sarsa(0)
		@return Console output of agent actions
	*/
	Sarsa1D(int reinforcementTableCount, int rewardPlace, int startState, bool useSarsaLambda);
};
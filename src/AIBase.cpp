#include "AIBase.h"

int MCTSAi::makeDecision(const Game& a_oGame)
{
	//Create the list of action,
	std::vector<int> _action;
	std::vector<int> _bestScore;
	a_oGame.getValidActions(_action);
	std::vector<int>::iterator itr = _action.begin();

	Game* _newGame;

	for (itr; itr != _action.end(); ++itr)
	{
		//set the end value to 0.
		int _result = 0;
		//For all the max playthrough
		for (int playIdx = 0; playIdx < m_iPlayouts; ++playIdx)
		{
			//clone the current game
			_newGame = a_oGame.clone();
			//perform the action
			_newGame->performAction(*itr);

			if (_newGame->getCurrentGameState() == Game::PLAYER_TWO)
			{
				return *itr;
			}
			//now need to simulate a game.
			while (_newGame->getCurrentGameState() == Game::UNKNOWN)
			{
				std::vector<int> _copyAction;
				_newGame->getValidActions(_copyAction);

				int _randAct = rand() % _copyAction.size();
				_newGame->performAction(_copyAction[_randAct]);
				
				if (_newGame->getCurrentGameState() == Game::PLAYER_ONE)
					_result += 1;
				else if (_newGame->getCurrentGameState() == Game::PLAYER_TWO)
					_result -= 1;
			}
			delete _newGame;
		}
		_bestScore.push_back(_result);
	}
	int _bestIdx = 0;
	for (int i = 0; i != _bestScore.size(); ++i)
	{
		if (_bestScore[i] < _bestScore[_bestIdx])
		{
			_bestIdx = i;
		}
	}
	return _action[_bestIdx];
}
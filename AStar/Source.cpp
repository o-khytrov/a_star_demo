#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include<vector>
using namespace std;

struct sNode {
	bool bObstacle = false;
	bool bVisited = false;
	float fGlobalGoal;
	float fLocalGoal;
	int x;
	int y;
	vector<sNode*> vecNeighbours;
	sNode* parent;
};

class Win :public olc::PixelGameEngine
{
public:
	sNode* nodes = nullptr;
	int nMapWidth = 16;
	int nMapHeight = 16;

	sNode* nodeStart = nullptr;
	sNode* nodeEnd = nullptr;

	Win()
	{
		this->sAppName = "A-Star Algorithm demo";
	}
	void Solve_AStar()
	{
		
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				
				nodes[y * nMapWidth + x].bVisited = false;
				nodes[y * nMapWidth + x].fGlobalGoal = INFINITY;
				nodes[y * nMapWidth + x].fLocalGoal = INFINITY;
				nodes[y * nMapWidth + x].parent = nullptr;
			}

		}

		auto distance = [](sNode* a, sNode* b)
		{
			return sqrt((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
		};

		auto heuristic = [distance](sNode* a, sNode* b) {
			return distance(a, b);
		};
		sNode* nodeCurrent = nodeStart;

		nodeStart->fLocalGoal = 0.0f;
		nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);

		list<sNode*> listNotTestedNodes;
		listNotTestedNodes.push_back(nodeStart);

		while (!listNotTestedNodes.empty())
		{
			listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs) {return lhs->fGlobalGoal < rhs->fGlobalGoal; });
			while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
				listNotTestedNodes.pop_front();

			if (listNotTestedNodes.empty())
				break;

			nodeCurrent = listNotTestedNodes.front();
			nodeCurrent->bVisited = true;

			for (auto nodeNeighbour : nodeCurrent->vecNeighbours)
			{
				if (!nodeNeighbour->bVisited && nodeNeighbour->bObstacle == 0)
					listNotTestedNodes.push_back(nodeNeighbour);
				float fPossibleLowerGoal = nodeCurrent->fGlobalGoal + distance(nodeCurrent, nodeNeighbour);
			
				if (fPossibleLowerGoal < nodeNeighbour->fLocalGoal)
				{
					nodeNeighbour->parent = nodeCurrent;
					nodeNeighbour->fLocalGoal = fPossibleLowerGoal;
					nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
				}
			}
		}

	}
	bool OnUserCreate() override
	{

		nodes = new sNode[nMapHeight * nMapHeight];
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				nodes[y * nMapWidth + x].x = x;
				nodes[y * nMapWidth + x].y = y;
				nodes[y * nMapWidth + x].bObstacle = false;
				nodes[y * nMapWidth + x].parent = nullptr;
				nodes[y * nMapWidth + x].bVisited = false;
			
			}

		}


		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				if (y > 0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x + 0)]);

				if (y < nMapHeight - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x + 0)]);

				if (x > 0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x - 1)]);

				if (x < nMapWidth - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x + 1)]);
			}
		}
		nodeStart = &nodes[(nMapHeight / 2) * nMapWidth + 1];
		nodeEnd = &nodes[(nMapHeight / 2) * nMapWidth + nMapWidth - 2];
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		int nNodeSize = 9;
		int nNodeBorder = 5;
		Clear(olc::BLACK);
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				for (auto n : nodes[y * nMapWidth + x].vecNeighbours)
				{
					DrawLine(nNodeBorder * (x + 1) + x * nNodeSize + nNodeSize / 2, nNodeBorder * (y + 1) + y * nNodeSize + nNodeSize / 2,
						n->x * nNodeSize + nNodeSize / 2 + nNodeBorder * (n->x + 1), n->y * nNodeSize + nNodeSize / 2 + nNodeBorder * (n->y + 1),olc::BLUE);
				}
			}
		}
		int nSelectedNodeX = GetMouseX() / (nNodeSize + nNodeBorder);
		int nSelectedNodeY = GetMouseY() / (nNodeSize + nNodeBorder);
		if (GetMouse(0).bReleased)
		{
			if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)
				if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)
				{
					if (GetKey(olc::Key(olc::SHIFT)).bHeld)
					{
						nodeStart = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];
					}
					if (GetKey(olc::Key(olc::CTRL)).bHeld)
					{
						nodeEnd = &nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX];
					}
					else
					{
						nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle = !nodes[nSelectedNodeY * nMapWidth + nSelectedNodeX].bObstacle;
					}
					
					Solve_AStar();
				}
			
		}
		for (int x = 0; x < nMapWidth; x++)
		{
			for (int y = 0; y < nMapHeight; y++)
			{
				olc::Pixel color = olc::BLUE;

				if (nSelectedNodeX == x && nSelectedNodeY == y)
					color = olc::CYAN;

				if (nodes[y * nMapWidth + x].bObstacle)
					color = olc::DARK_GREY;
				
				if (nodes[y * nMapWidth + x].bVisited)
				{
					color.a = 150;
				}
				if (&nodes[y * nMapWidth + x] == nodeStart)
					color = olc::GREEN;
				
				if (&nodes[y * nMapWidth + x] == nodeEnd)
					color = olc::RED;
				FillRect({ x * nNodeSize + nNodeBorder * (x + 1) ,y * nNodeSize + nNodeBorder * (y + 1) }, { nNodeSize, nNodeSize }, color);

			}
		}
		if (nodeEnd != nullptr)
		{
			sNode* p = nodeEnd;
			while (p->parent != nullptr)
			{
				DrawLine(nNodeBorder * (p->x + 1) + p->x * nNodeSize + nNodeSize / 2, nNodeBorder * (p->y + 1) + p->y * nNodeSize + nNodeSize / 2,
					p->parent->x * nNodeSize + nNodeSize / 2 + nNodeBorder * (p->parent->x + 1), p->parent->y * nNodeSize + nNodeSize / 2 + nNodeBorder * (p->parent->y + 1), olc::YELLOW);
				p = p->parent;
			}
		}
		return true;
	}
};
int main()
{
	Win win;
	if (win.Construct(250, 250, 3, 3))
		win.Start();
	return 0;
}
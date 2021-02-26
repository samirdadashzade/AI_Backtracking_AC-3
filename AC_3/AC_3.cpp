#include <cstdio>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <stdio.h>
using namespace std;

struct edge
{
	int from; 
	int to;
	vector<int> fromDomain;
	vector<int> toDomain;

	edge(int from, int to, int colors) : from(from), to(to) 
	{
		for (int i = 1; i <= colors; i++)
		{
			fromDomain.push_back(i);
			toDomain.push_back(i);
		}
	}

	edge(int from, int to, vector<int> fromDomain, vector<int> toDomain) : from(from), to(to), fromDomain(fromDomain), toDomain(toDomain)
	{}
};

string str;
int from, to, colors; 
vector<edge> arcs;
set<int> variables;
map<int, int> solution;


//Helper functions

void loadData() {
	//read vertexes file as standart input
	freopen("input.txt", "r", stdin);

	scanf("%[^\n]%*c", str);
	scanf("%[^\n]%*c", str);

	scanf("colors = %d\n", &colors);
	scanf("%[^\n]%*c", str);

	//load vertexes
	while (scanf("%d,%d", &from, &to) == 2)
	{
		variables.insert(from);
		variables.insert(to);
		arcs.push_back(edge(from, to, colors));
	}
}

bool cmp(pair<int, int>& a, pair<int, int>& b)
{
	return a.second < b.second;
}

vector<pair<int, int>> sortMap(map<int, int>& M)
{
	vector<pair<int, int> > A;

	for (auto& it : M) {
		A.push_back(it);
	}

	sort(A.begin(), A.end(), cmp);

	return A;
}


//Checking
bool isAssigned(map<int, int>& assignment, int key)
{
	map<int, int>::iterator itr = assignment.find(key);
	if (itr != assignment.end())
	{
		return true;
	}
	return false;
}

bool isConsistent(map<int, int>& assignment)
{
	for (int i = 0; i < arcs.size(); i++)
	{
		edge arc = arcs[i];

		if (!isAssigned(assignment, arc.from) || !isAssigned(assignment, arc.to))
		{
			continue;
		}

		if (assignment[arc.from] == assignment[arc.to])
		{
			return false;
		}
	}

	return true;
}


int selectUnassignedVariable(map<int, int>& assignment)
{
	set<int>::iterator itr;
	for (itr = variables.begin(); itr != variables.end(); itr++)
	{
		if (!isAssigned(assignment, *itr))
		{
			return *itr;
		}
	}

	return -1;
}

bool revise(edge arc)
{
	bool removed = false;
	for (int i = 0; i < arc.fromDomain.size(); i++)
	{
		bool hasConsistency = false;
		for (int j = 0; j < arc.toDomain.size(); j++)
		{
			if (arc.fromDomain[i] != arc.toDomain[j])
			{
				hasConsistency = true;
			}
		}

		if (!hasConsistency)
		{
			//remove i from fromDomain
			remove(arc.fromDomain.begin(), arc.fromDomain.end(), arc.fromDomain[i]);
			removed = true;
		}
	}

	return removed;
}


//Heuristics
int MRV(vector<edge> arcs, map<int, int> assignment)
{
	int minDomain = colors + 1;
	int minNode = -1;
	for (int i = 0; i < arcs.size(); i++)
	{
		if (arcs[i].fromDomain.size() < minDomain && !isAssigned(assignment, arcs[i].from))
		{
			minNode = arcs[i].from;
		}
		else if (arcs[i].toDomain.size() < minDomain && !isAssigned(assignment, arcs[i].to))
		{
			minNode = arcs[i].to;
		}
	}

	return minNode;
}

vector<int> LCV(int node, vector<edge> arcs)
{
	map<int, int> order;
	int index = -1;
	for (int i = 0; i < arcs.size(); i++)
	{
		vector<int> nodeDomain;

		if (arcs[i].from == node)
		{
			nodeDomain = arcs[i].fromDomain;
		}
		else if (arcs[i].to == node)
		{
			nodeDomain = arcs[i].toDomain;
		}

		for (int j = 0; j < nodeDomain.size(); j++)
		{
			int c = nodeDomain[j];
			int min = colors + 1;

			for (int k = 0; k < arcs.size(); k++)
			{
				vector<int> searchDomain;

				if (arcs[k].from == node)
				{
					searchDomain = arcs[k].toDomain;
				}
				else if (arcs[k].to == node)
				{
					searchDomain = arcs[k].fromDomain;
				}

				int temp = searchDomain.size();

				if (find(searchDomain.begin(), searchDomain.end(), c) != searchDomain.end())
				{
					temp--;
				}

				if (temp < min)
				{
					min = temp;
				}
			}
			order[c] = min;
		}

	}

	vector<pair<int, int>> sortedOrders = sortMap(order);

	vector<int> sortedColors;
	for (int i = 0; i < sortedOrders.size(); i++)
	{
		sortedColors.push_back(sortedOrders[i].first);
	}
	
	return sortedColors;
}


bool AC_3(vector<edge>& arcs, map<int, int> assignment)
{
	vector<edge> newArcs(arcs);
	for (int i = 0; i < newArcs.size(); i++)
	{
		if (isAssigned(assignment, newArcs[i].from))
		{
			map<int, int>::iterator itr = assignment.find(newArcs[i].from);
			newArcs[i].fromDomain = vector<int>(itr->second);
		}
		else if (isAssigned(assignment, newArcs[i].to))
		{
			map<int, int>::iterator itr = assignment.find(newArcs[i].to);
			newArcs[i].toDomain = vector<int>(itr->second);
		}
	}

	queue<edge> q;
	for (int i = 0; i < newArcs.size(); i++)
	{
		q.push(newArcs[i]);
	}

	while (q.size() > 0)
	{
		edge arc = q.front();
		if (revise(arc)) //arc domains is updated
		{
			if (arc.fromDomain.size() == 0)
			{
				return false;
			}

			for (int j = 0; j < newArcs.size(); j++)
			{
				edge other = arcs[j];

				//skip the same arc
				if ((other.from == arc.from && other.to == arc.to) ||
					(other.to == arc.from && other.from == arc.to))
				{
					continue;
				}

				if (other.from == arc.from)
				{
					q.push(edge(other.to, arc.from, other.toDomain, arc.fromDomain));
				} 
				else if (other.to == arc.from)
				{
					q.push(edge(other.from, arc.from, other.fromDomain, arc.fromDomain));
				}
			}
		}
	}

	return true;
}


map<int, int> backtrack(map<int, int>& assignment)
{
	//check whether assignment is complete
	if (assignment.size() == variables.size())
	{
		return assignment;
	}

	int node = selectUnassignedVariable(assignment);

	for (int color = 1; color <= colors; color++)
	{
		map<int, int> newAssignment(assignment);
		newAssignment[node] = color;

		if (isConsistent(newAssignment))
		{
			map<int, int> result = backtrack(newAssignment);

			if (result.size() > 0)
			{
				return result;
			}
		}
	}

	return map<int, int>();
}

map<int, int> backtrackAC_3(map<int, int>& assignment)
{
	//check whether assignment is complete
	if (assignment.size() == variables.size())
	{
		return assignment;
	}

	int node = MRV(arcs, assignment);

	vector<int> lcvColors = LCV(node, arcs);

	for (int i = 0; i <= lcvColors.size(); i++)
	{
		int color = lcvColors[i];

		map<int, int> newAssignment(assignment);
		newAssignment[node] = color;

		if (isConsistent(newAssignment))
		{
			bool hasInference = AC_3(arcs, newAssignment);
			//TODO use inference

			map<int, int> result = backtrack(newAssignment);

			if (result.size() > 0)
			{
				return result;
			}
		}
	}

	return map<int, int>();
}


int main(void)
{
	loadData();
	map<int, int> result = backtrackAC_3(solution);

	if (result.size() == 0)
	{
		printf("No possible result");
		return 0;
	}

	map<int, int>::iterator itr;
	for (itr = result.begin(); itr != result.end(); itr++)
	{
		printf("%d -> %d\n", itr->first, itr->second);
	}

	printf("\n");
	return 0;
}


#include <vector>
#include <iostream>
#include <Vector.h>
#include <OpenGLViewer.h>
#include <math.h>

class Node;
class NodeTile
{
public:
	NodeTile();
	~NodeTile();
	void initNodes();
	//Node ** nodes;
	std::vector<Node*> nodes;
};
class Node
{
public:
	Node() : divide(false)
	{				
	}

	NodeTile * nodeTile;	
	bool divide;
	Vec3f color;
};

NodeTile::NodeTile()
{
	nodes.resize(8);
	//nodes = new Node*[8];
	for(int i = 0; i < 8; ++i)
		nodes[i] = 0;
}

NodeTile::~NodeTile()
{
	//delete [] nodes;
}

void NodeTile::initNodes()
{
	for(int i = 0; i < 8; ++i)
		nodes[i] = new Node();
}

struct Voxel
{
	Voxel(Vec3f p, float scale) : pos(p), scale(scale)
	{}
	Vec3f pos;
	float scale;
};

class CPUOctree
{
public:

	CPUOctree() 
	{
	
	}


	void
	buildTree(int levels, std::vector<Vec3i> & fragList)
	{
		
		LEVELS = levels;
		_fragList = &fragList;
		rootTile = new NodeTile();
		rootTile->initNodes();
		NodeTile * nTile = new NodeTile();
		nTile->initNodes();
		rootTile->nodes[0]->nodeTile = nTile;
		rootTile->nodes[0]->divide = true;
		_nodePool.push_back(rootTile);
		_nodePool.push_back(nTile);
		currIndex = 1;
		oldIndex = 0;


		for(int i = 1; i <= LEVELS; ++i) {
			_flagNodes(i);
			if(i >= LEVELS)
				break;
			_subdivideNodes(i);
			oldIndex = currIndex;
			currIndex = _nodePool.size()-1;
		}
	}

	std::vector<Voxel> &
	getVoxels()
	{
		return _voxels;
	}

	void
	buildVoxel(int level, float dim)
	{
		_voxels.clear();
		int currentLevel =1;
		NodeTile* nodeTile = rootTile->nodes[0]->nodeTile;
		Vec3f xi(0.0,0.0,0.0);
		_dim = dim;
		//create VectorTable
		
		vectorTable.push_back(Vec3f(0,0,0));
		vectorTable.push_back(Vec3f(1,0,0));
		vectorTable.push_back(Vec3f(0,1,0));
		vectorTable.push_back(Vec3f(1,1,0));
		vectorTable.push_back(Vec3f(0,0,1));
		vectorTable.push_back(Vec3f(1,0,1));
		vectorTable.push_back(Vec3f(0,1,1));
		vectorTable.push_back(Vec3f(1,1,1));	

		recursiveTravel(nodeTile,currentLevel, level, xi); 
		
		
	}



private:
	std::vector<Vec3i> * _fragList;
	NodeTile * rootTile;
	std::vector<NodeTile *> _nodePool;
	int LEVELS;
	int currIndex, oldIndex;
	std::vector<Vec3f> vectorTable;
	float _dim;
	//For drawing
	std::vector<Voxel> _voxels;


//------------------------------------------------------------------------------

	void recursiveTravel(NodeTile* nodeTile, int currentLevel, int finalLevel, Vec3f xi)
	{
		for(int i = 0; i < 8; ++i)
		{
			if(nodeTile->nodes[i]->divide)
			{
				float scale = _dim/pow(2.0f,currentLevel);
				Vec3f xiTemp = xi + vectorTable[i]*scale;
				if(currentLevel == finalLevel)
				{
					Voxel v(xiTemp,scale);
					_voxels.push_back(v);
				}
				else
				{
					recursiveTravel(nodeTile->nodes[i]->nodeTile,currentLevel + 1,finalLevel,xiTemp);
				}
			}

		}
	}
	void _flagNodes(int level)
	{
		
		//Always start at root

		for(int j = 0; j < _fragList->size(); ++j) {
			//Fetch the correct node at level "level" given a voxel fragment		

			NodeTile * nodeTile;
			Node * node = rootTile->nodes[0];
			int i = 1;
			do {
				nodeTile = node->nodeTile;
				node = nodeTile->nodes[_idx((*_fragList)[j],i)];
				++i;
			} while(i <= level);
			
			node->divide = true;
		}
	}

	//
	// Index is the forst node that has been flagged´at the current level
	// nNodes tells us the number of active nodes on the current level
	//

	void _subdivideNodes(int level)
	{		
		for(int i = oldIndex; i < currIndex; ++i) {
			Node ** nodes = &_nodePool[i+1]->nodes[0];
			for(int n = 0; n < 8; ++n) {
				if(nodes[n]->divide) {
					nodes[n]->nodeTile = new NodeTile();
					nodes[n]->nodeTile->initNodes(); //This will be a step int the gpu impl.
					_nodePool.push_back(nodes[n]->nodeTile);
				}
					
			}
			
		}
	}

	//Return the coorect tile index given a position and a level
	int 
	_idx(const Vec3i & p, int level)
	{
		//floor(x/2^(LEVELS-levels)) - floor(x/2^(LEVELS-levels+1)) :D D::D:D:D:D:D
		int shift = LEVELS-level; //
		int tmp = p(0) >> shift;
		int tmp2 = p(0) >> (shift+1);
		int i = (p(0) >> shift) - 2*(p(0) >> (shift+1));
		int j = (p(1) >> shift) - 2*(p(1) >> (shift+1));
		int k = (p(2) >> shift) - 2*(p(2) >> (shift+1));
		return (i + 2*j + 4*k); // i + 2*(j + 2*k)
	}
};
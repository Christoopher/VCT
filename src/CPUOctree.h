
#include <vector>
#include <iostream>

template <class T>
struct Vec3
{
	Vec3(T x, T y, T z)
	{
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
	}

	template <class U>
	Vec3(const Vec3<U> & v)
	{
		pos[0] = v(0);
		pos[1] = v(1);
		pos[2] = v(2);
	}
	
	Vec3()
	{
		for(int i = 0; i < 3; ++i)
		{
			pos[i]=T();
		}
	}


	T operator()(int i) const
	{
		return pos[i];
	}

	T operator()(int i)
	{
		return pos[i];
	}

	T pos[3];	
};

typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;

template <class T>
std::ostream & operator<<(std::ostream & o, const Vec3<T> & p)
{
	o << "[" << p(0) << ", " << p(1) << ", " << p(2) << "]";
	return o;
}

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

class CPUOctree
{
public:

	CPUOctree(int levels, std::vector<Vec3i> & fragList) : LEVELS(levels), _fragList(fragList)
	{
		rootTile = new NodeTile();
		rootTile->initNodes();
		NodeTile * nTile = new NodeTile();
		nTile->initNodes();
		rootTile->nodes[0]->nodeTile = nTile;
		_nodePool.push_back(rootTile);
		_nodePool.push_back(nTile);
		currIndex = 1;
		oldIndex = 0;
		buildTree();
	}

	void
	buildTree()
	{		

		for(int i = 1; i <= LEVELS; ++i) {
			flagNodes(i);
			if(i >= LEVELS)
				break;
			subdivideNodes(i);
			oldIndex = currIndex;
			currIndex = _nodePool.size()-1;

		}
		//write values

	}

	

	void flagNodes(int level)
	{
		
		//Always start at root

		for(int j = 0; j < _fragList.size(); ++j) {
			//Fetch the correct node at level "level" given a voxel fragment		

			NodeTile * nodeTile;
			Node * node = rootTile->nodes[0];
			int i = 1;
			do {
				nodeTile = node->nodeTile;
				node = nodeTile->nodes[_idx(_fragList[j],i)];
				++i;
			} while(i <= level);
			
			node->divide = true;
			
			/* This is what we want to achieve for level 0
			root->nodes[0]->nodes[1]->divide = true;
			root->nodes[0]->nodes[2]->divide = true;
			root->nodes[0]->nodes[2]->divide = true;

			root->nodes[0]->nodes[1] = new Node();
			root->nodes[0]->nodes[2] = new Node();
			root->nodes[0]->nodes[2] = new Node();
			*/

		}
	}

	//
	// Index is the forst node that has been flagged´at the current level
	// nNodes tells us the number of active nodes on the current level
	//

	void subdivideNodes(int level)
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



private:
	const std::vector<Vec3i> & _fragList;
	NodeTile * rootTile;
	std::vector<NodeTile *> _nodePool;
	const int LEVELS;
	int currIndex, oldIndex;
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
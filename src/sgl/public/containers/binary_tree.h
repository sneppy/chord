#pragma once

#include "core_types.h"
#include "hal/platform_memory.h"
#include "hal/malloc_ansi.h"
#include "templates/const_ref.h"
#include "templates/is_trivially_copyable.h"
#include "templates/functional.h"

//////////////////////////////////////////////////
// Binary Node
//////////////////////////////////////////////////

/**
 * @struct BinaryNode<T>containers/binary_tree.h
 * 
 * A standalone node of a binary tree.
 * The template type must define at least
 * operator<() and operator>() for node
 * comparison.
 */
template<typename T, typename CompareT = Compare<T>>
struct GCC_ALIGN(32) BinaryNode
{
	template<typename, typename, typename> friend class BinaryTree;
	
public:
	/// Parent node
	BinaryNode * parent;

	/// Left child node
	BinaryNode * left;

	/// Right child node
	BinaryNode * right;

	/// Successor node
	BinaryNode * next;

	/// Predecessor node
	BinaryNode * prev;

	/// Node data
	T data;

	/// Color of the node
	enum NodeColor : uint8
	{
		BLACK	= 0,
		RED		= 1
	} color;

public:
	/// Default constructor
	FORCE_INLINE BinaryNode(
		typename ConstRef<T>::Type _data,
		NodeColor _color = NodeColor::RED,
		BinaryNode* _parent = nullptr,
		BinaryNode* _left = nullptr,
		BinaryNode* _right = nullptr
	)
		: parent(_parent)
		, left(_left)
		, right(_right)
		, next(nullptr)
		, prev(nullptr)
		, data(_data)
		, color(_color)
	{
		if (left)	prev = left->getMax();
		if (right)	next = right->getMin();
	}

	/// Returns true if parent matches color
	/// @{
	FORCE_INLINE bool isBlack()	{ return color == NodeColor::BLACK; }
	FORCE_INLINE bool isRed()	{ return color == NodeColor::RED; }
	/// @}

	/// Get root of this tree
	FORCE_INLINE BinaryNode * getRoot()
	{
		return parent ? parent->getRoot() : this;
	}

	/**
	 * Get subtree max height starting from this node
	 * 
	 * TODO: not tail-recursive
	 * 
	 * @param [in] depth initial depth of this node
	 * @return subtree height
	 */
	uint32 getMaxHeight(uint32 depth = 0)
	{
		uint32 leftDepth	= left != nullptr ? left->getHeight(depth + 1) : 0;
		uint32 rightDepth	= right != nullptr ? right->getHeight(depth + 1) : 0;
		return leftDepth > rightDepth ? leftDepth : rightDepth;
	}

	/**
	 * Get subtree min height starting from this node
	 * 
	 * TODO: not tail-recursive
	 * 
	 * @param [in] depth initial depth of this node
	 * @return subtree height
	 */
	uint32 getMinHeight(uint32 depth = 0)
	{
		uint32 leftDepth	= left != nullptr ? left->getHeight(depth + 1) : 0;
		uint32 rightDepth	= right != nullptr ? right->getHeight(depth + 1) : 0;
		return leftDepth < rightDepth ? leftDepth : rightDepth;
	}

	/// Get subtree leftmost node (contains min value)
	FORCE_INLINE BinaryNode * getMin()
	{
		return left == nullptr ? this : left->getMin();
	}

	/// Get subtree rightmost node (contains max value)
	FORCE_INLINE BinaryNode * getMax()
	{
		return right == nullptr ? this : right->getMax();
	}

	/**
	 * Finds node that matches data
	 * 
	 * @param [in] search search data
	 * @return node if found, nullptr otherwise
	 */
	/// Search begins from this node
	FORCE_INLINE BinaryNode * find(typename ConstRef<T>::Type search)
	{
		// Compare search key and node key
		const int32 compare = CompareT()(search, data);

		if (compare < 0)
			return left ? left->find(search) : nullptr;
		else if (compare > 0)
			return right ? right->find(search) : nullptr;
		else
			return this;
	}
	
protected:
	/// Set node as left child
	FORCE_INLINE BinaryNode * setLeftChild(BinaryNode * node)
	{
		if (node) node->parent = this;
		return (left = node);
	}

	/// Set node as right child
	FORCE_INLINE BinaryNode * setRightChild(BinaryNode * node)
	{
		if (node) node->parent = this;
		return (right = node);
	}

	/// Set previous node
	FORCE_INLINE BinaryNode * setPrevNode(BinaryNode * node)
	{
		if (prev != nullptr)
			prev->next = node;
		
		node->prev = prev;
		this->prev = node;
		prev->next = this;

		return node;
	}

	/// Set next node
	FORCE_INLINE BinaryNode * setNextNode(BinaryNode * node)
	{
		if (next != nullptr)
			next->prev = node;
		
		node->next = next,
		this->next = node,
		next->prev = this;

		return node;
	}

public:
	/**
	 * Insert a node in this subtree
	 * 
	 * The tree structure spawning from this node
	 * is traversed until a suitable leaf is found.
	 * The tree structure may be changed to satisfy
	 * the R&B tree properties
	 * 
	 * @param [in] node binary node operand
	 * @return inserted node
	 * @{
	 */
	FORCE_INLINE BinaryNode * insert(BinaryNode * node)
	{
		if (CompareT()(node->data, data) < 0)
			return left
				? left->insert(node)
				: (
					// Update linear relationships
					setPrevNode(node),
					// Insert and return node
					setLeftChild(node)->repair()
				);
		else
			return right
				? right->insert(node)
				: (
					// Update linear relationships
					setNextNode(node),
					// Insert and return node
					setRightChild(node)->repair()
				);
	}
	
	/// If node already exists, don't reinsert it
	FORCE_INLINE BinaryNode * insertUnique(BinaryNode * node)
	{
		// Compare inserting node data against node data
		int32 compare = CompareT()(node->data, data);
		
		if (compare < 0)
			return left
				? left->insert(node)
				: (
					// Update linear relationships
					this->prev = node,
					prev->next = this,

					// Insert and return node
					setLeftChild(node)->repair()
				);
		else if (compare > 0)
			return right
				? right->insert(node)
				: (
					// Update linear relationships
					this->next = node,
					next->prev = this,

					// Insert and return node
					setRightChild(node)->repair()
				);
		else
			return this;
	}
	/// @}

	/// Delete this node from tree
	/// @return node evicted from tree
	BinaryNode * remove()
	{
		// Proceed with normal bt deleteion, then repair
		// @ref http://www.mathcs.emory.edu/~cheung/Courses/171/Syllabus/9-BinTree/BST-delete2.html
		
		BinaryNode * succ = this;

		// Get actual successor
		if (left != nullptr && right != nullptr)
			moveOrCopy(data, (succ = right->getMin())->data);
		
		// Remove left or right child of successor
		BinaryNode * repl = nullptr;
		if (succ->left != nullptr)
		{
			if (((repl = succ->left)->next = succ->next) != nullptr)
				repl->next->prev = repl;
		}
		else if (succ->right != nullptr)
		{
			if (((repl = succ->right)->prev = succ->prev) != nullptr)
				repl->prev->next = repl;
		}
		else
		{
			if (succ->prev) succ->prev->next = succ->next;
			if (succ->next) succ->next->prev = succ->prev;
		}

		// Replace successor
		// If we had only one subtree then left may be non-null
		if (succ->parent != nullptr)
			(succ->parent->left == succ)
				// Set*Child also checks if repl is null
				? succ->parent->setLeftChild(repl)
				: succ->parent->setRightChild(repl);
		else if (repl != nullptr)
			repl->parent = nullptr;

		
		if (succ->isBlack())
			// Repair rb structure
			repairRemoved(repl, repl ? repl->parent : succ->parent);
		
		return succ;
	}

	/// Repair tree structure after insertion
	/// starting from this node
	/// @return self
	BinaryNode * repair()
	{
		// Case 0: I'm (g)root
		if (parent == nullptr)
		{
			color = NodeColor::BLACK;
			return this;
		}

		// Case 1: parent is black
		if (parent->isBlack())
		{
			color = NodeColor::RED;
			return this;
		}
		else
		{
			// Get relatives
			BinaryNode
				* grand = parent->parent,
				* uncle = grand ? (grand->left == parent ? grand->right : grand->left) : nullptr;
			
			// Case 2: uncle exists and is red
			if (uncle && uncle->isRed())
			{
				uncle->color = parent->color = NodeColor::BLACK;
				grand->color = NodeColor::RED;

				// Repair grand
				grand->repair();
			}
			else
			{
				// Case 3: uncle is black or doesn't exist

				// Bring me on the outside
				if (grand->left == parent)
				{
					if (parent->right == this)
					{
						// Note, I'm being pushed as root of the subtree
						parent->rotateLeft(),
						parent->rotateRight();

						color			= NodeColor::BLACK;
						right->color	= NodeColor::RED;
					}
					else
					{
						grand->rotateRight();

						parent->color	= NodeColor::BLACK;
						grand->color	= NodeColor::RED;
					}
				}
				else
				{
					if (parent->left == this)
					{
						// Note, I'm being pushed as root of the subtree
						parent->rotateRight(),
						parent->rotateLeft();

						color		= NodeColor::BLACK;
						left->color	= NodeColor::RED;
					}
					else
					{
						grand->rotateLeft();

						parent->color	= NodeColor::BLACK;
						grand->color	= NodeColor::RED;
					}
				}
			}
		}

		return this;
	}

	/**
	 * Repair tree structure after node deletion
	 * 
	 * Since NIL leaf are considered black nodes
	 * we must run the repair algorithm even if
	 * the node is NIL. For this reason we cannot
	 * use a non-static function
	 * 
	 * @param [in] node node to repair
	 * @param [in] parent node parent, necessary if node is NIL
	 */
	static void repairRemoved(BinaryNode * node, BinaryNode * parent)
	{
		/// Good ol' Wikipedia
		/// @ref https://en.wikipedia.org/wiki/Red%E2%80%93black_tree#Removal

		// Case -1: node is null and parent is null
		if (node == nullptr && parent == nullptr)
			; // Do nothing

		// Case 0: node is red or is root
		else if (node && (node->isRed() || parent == nullptr))
			node->color = NodeColor::BLACK;
		
		// Left child
		else if (parent->left == node)
		{
			BinaryNode * sibling = parent->right;

			// Case 1: sibling is red
			if (sibling->isRed())
			{
				sibling->color	= NodeColor::BLACK;
				parent->color	= NodeColor::RED;

				// Rotate around parent and update sibling
				parent->rotateLeft();
				sibling = parent->right;
			}

			// Case 2: sibling is black with black children
			if (
				sibling->isBlack() &&
				(sibling->left == nullptr || sibling->left->isBlack()) &&
				(sibling->right == nullptr || sibling->right->isBlack())
			)
			{
				sibling->color = NodeColor::RED;
				// Recursive call
				repairRemoved(parent, parent->parent);
			}
			else
			{
				// Case 3: sibling is black and inner child is red
				if (sibling->left != nullptr && sibling->left->isRed())
				{
					sibling->color			= NodeColor::RED;
					sibling->left->color	= NodeColor::BLACK;

					// Rotate around sibling, so that red child is outer
					// We also need to update the sibling
					sibling->rotateRight();
					sibling = sibling->parent;
				}

				// Case 4: sibling is black and outer child is red
				{
					sibling->color			= parent->color;
					parent->color			= NodeColor::BLACK;
					sibling->right->color	= NodeColor::BLACK;

					// Rotate around parent
					parent->rotateLeft();
				}
			}
		}
		// Right child
		else
		{
			BinaryNode * sibling = parent->left;

			if (sibling->isRed())
			{
				sibling->color	= NodeColor::BLACK;
				parent->color	= NodeColor::RED;

				// Rotate around parent and update sibling
				parent->rotateRight();
				sibling = parent->left;
			}
			
			if (
				sibling->isBlack() &&
				(sibling->left == nullptr || sibling->left->isBlack()) &&
				(sibling->right == nullptr || sibling->right->isBlack())
			)
			{
				sibling->color = NodeColor::RED;
				// Recursive call
				repairRemoved(parent, parent->parent);
			}
			else
			{
				// Case 3: sibling is black and inner child is red
				if (sibling->right != nullptr && sibling->right->isRed())
				{
					sibling->color			= NodeColor::RED;
					sibling->right->color	= NodeColor::BLACK;

					// Rotate around sibling, so that red child is outer
					// We also need to update the sibling
					sibling->rotateLeft();
					sibling = sibling->parent;
				}

				// Case 4: sibling is black and outer child is red
				{
					sibling->color			= parent->color;
					parent->color			= NodeColor::BLACK;
					sibling->left->color	= NodeColor::BLACK;

					// Rotate around parent
					parent->rotateRight();
				}
			}
		}
	}

protected:
	/// Rotate left with this node as pivot
	FORCE_INLINE void rotateLeft()
	{
		// Replace me with right child
		if (parent == nullptr)
			right->parent = nullptr;
		else if (parent->left == this)
			parent->setLeftChild(right);
		else
			parent->setRightChild(right);

		// Set right-left as my right child
		BinaryNode * prevRight = right;
		setRightChild(right->left);
		
		// Set me as left child
		prevRight->setLeftChild(this);
	}

	/// Rotate right with this node as pivot
	FORCE_INLINE void rotateRight()
	{
		// Replace me with left child
		if (parent == nullptr)
			left->parent = nullptr;
		else if (parent->left == this)
			parent->setLeftChild(left);
		else
			parent->setRightChild(left);

		// Set left-right as my left child
		BinaryNode* prevLeft = left;
		setLeftChild(left->right);
		
		// Set me as right child
		prevLeft->setRightChild(this);
	}

#if SGL_BUILD_DEBUG
public:
	/// Print node and descendants
	void print(FILE * out = stdout, int32 depth = 0)
	{
		if (depth == 0)
			fprintf(out, "%c\n", isBlack() ? 'B' : 'R');
		else
			fprintf(out, "%*c%c\n", depth, ' ', isBlack() ? 'B' : 'R');
		
		if (left)	left->print(out, depth + 1);
		if (right)	right->print(out, depth + 1);
	}
#endif
};

/// Node reference type
template<typename T, typename CompareT = Compare<T>>
using BinaryNodeRef = BinaryNode<T, CompareT>*;

//////////////////////////////////////////////////
// Iterators
//////////////////////////////////////////////////

/// Forward declare iterator types
template<typename> struct NodeIterator;
template<typename> struct TreeIterator;

/**
 * @struct TreeIterator
 * 
 * Walks the tree using the next node link
 */
template<typename U>
struct TreeIterator
{
	template<typename, typename, typename> friend class BinaryTree;
	
private:
	/// Current node
	BinaryNodeRef<U> node;

public:
	/// Default constructor
	FORCE_INLINE TreeIterator(BinaryNodeRef<U> _node = nullptr)
		: node(_node) {}
	
	/// Increments iterator
	/// @{
	FORCE_INLINE TreeIterator & operator++()
	{
		if (LIKELY(node != nullptr)) node = node->next;
		return *this;
	}

	/// Post increment
	FORCE_INLINE TreeIterator operator++(int32)
	{
		TreeIterator self = *this;
		operator++();
		return self;
	}
	/// @}
	
	/// Increments iterator
	/// @{
	FORCE_INLINE TreeIterator & operator--()
	{
		if (LIKELY(node != nullptr)) node = node->prev;
		return *this;
	}

	/// Post increment
	FORCE_INLINE TreeIterator operator--(int32)
	{
		TreeIterator self = *this;
		operator--();
		return self;
	}
	/// @}

	/// Iterators comparison
	/// @{
	FORCE_INLINE bool operator==(const TreeIterator<U> & other) const { return node == other.node; }
	FORCE_INLINE bool operator!=(const TreeIterator<U> & other) const { return node != other.node; }
	/// @}

	/// Dereferences iterator
	/// @{
	FORCE_INLINE U & operator* () const { return node->data; };
	FORCE_INLINE U * operator->() const { return &(node->data); };
	/// @}
};

/**
 * @struct NodeIterator
 * 
 * Follow a path starting from the given node
 * down to the tree leaves using the provided
 * key as a mean to choose next subtree
 */
template<typename U>
struct GCC_ALIGN(32) NodeIterator
{
	template<typename, typename, typename> friend class BinaryTree;

private:
	/// Current node
	BinaryNodeRef<U> node;

	/// Search value
	U search;

private:
	/// Default constructor, private
	NodeIterator() :
		node(nullptr),
		search() {}
	
	/// Initialize iterator with search value and node
	NodeIterator(typename ConstRef<U>::Type _search, BinaryNodeRef<U> _node)
		: node(_node)
		, search(_search)
	{
		// Find first node
		if (node) node = node->find(search);
	}

public:
	/// Increment iterator
	FORCE_INLINE NodeIterator<U> & operator++()
	{
		if (node = node->right)
			node = node->find(search);
		
		return *this;
	}

	/// Iterator comparison
	/// @{
	FORCE_INLINE bool operator==(const NodeIterator<U> & other) const { return node == other.node; }
	FORCE_INLINE bool operator!=(const NodeIterator<U> & other) const { return node != other.node; }
	/// @}

	/// Access Node data
	/// @{
	FORCE_INLINE U & operator* () const { return node->data; };
	FORCE_INLINE U * operator->() const { return &(node->data); };
	/// @}

	/// Remove node and move to next
	/// ! This method is not safe, for we have no knowledge of the tree root node
	/// ! Use Tree::remove method
	FORCE_INLINE NodeIterator<U> & remove()
	{
		if (LIKELY(node != nullptr))
		{
			// Advance iterator
			auto removed = node;
			operator++();

			// Remove node
			removed->remove();
		}
	}

	/// Convert to tree iterator
	FORCE_INLINE operator TreeIterator<U>() const
	{
		return TreeIterator<U>(node);
	}
};

//////////////////////////////////////////////////
// Binary tree
//////////////////////////////////////////////////

/**
 * @class Tree containers/tree.h
 * 
 * A templated red-black tree.
 * @see BinaryNode
 */
template<typename T, typename CompareT = Compare<T>, typename AllocT = MallocAnsi>
class GCC_ALIGN(32) BinaryTree
{
	template<typename, typename, typename>				friend class BinaryTree;
	template<typename, typename, typename, typename>	friend class Map;

public:
	/// Node type
	using Node		= BinaryNode<T, CompareT>;
	using NodeRef	= BinaryNodeRef<T, CompareT>;

	/// Define iterator types
	/// @{
	using Iterator		= NodeIterator<T>;
	using ConstIterator	= NodeIterator<const T>;

	using TreeIterator		= ::TreeIterator<T>;
	using ConstTreeIterator	= ::TreeIterator<const T>;
	/// @}

protected:
	/// Allocator used to allocate new nodes
	AllocT * allocator;
	bool bHasOwnAllocator;

	/// Root node
	NodeRef root;

	/// Num of nodes
	uint64 numNodes;

public:
	/// Default constructor
	FORCE_INLINE BinaryTree(AllocT * _allocator = reinterpret_cast<AllocT*>(gMalloc)) :
		allocator(_allocator),
		bHasOwnAllocator(_allocator == nullptr),
		root(nullptr),
		numNodes(0)
	{
		// Create own allocator
		if (bHasOwnAllocator)
			allocator = new AllocT;
	}

protected:
	/// Create a new node using the class allocator
	FORCE_INLINE NodeRef createNode(typename ConstRef<T>::Type data)
	{
		return new (reinterpret_cast<NodeRef>(allocator->malloc(sizeof(Node)))) Node(data);
	}

	/// Recursively replicate structure of another tree
	template<typename U>
	void replicateStructure(NodeRef replica, BinaryNodeRef<U> original)
	{
		// Create left
		if (original->left)
		{
			if (replica->left)
				// Don't recreate node
				moveOrCopy(replica->left->data, original->left->data);
			else
				replica->setLeftChild(createNode(original->left->data));
			
			replicateStructure(replica->left, original->left);
		}
		else if (replica->left)
			// Remove unused nodes
			empty(replica->left);

		// Create right
		if (original->right)
		{
			if (replica->right)
				// Don't recreate node
				moveOrCopy(replica->right->data, original->right->data);
			else
				replica->setRightChild(createNode(original->right->data));
			
			replicateStructure(replica->right, original->right);
		}
		else if (replica->right)
			// Remove unused nodes
			empty(replica->right);
	}

public:
	/// Copy constructor
	FORCE_INLINE BinaryTree(const BinaryTree<T, AllocT> & other) : BinaryTree(nullptr)
	{
		if (other.root)
			// Copy the tree structure as-is
			replicateStructure(root = createNode(other.root->data), other.root);
		
		numNodes = other.numNodes;
	}

	/// Copy constructor (different allocator)
	template<typename AllocU>
	FORCE_INLINE BinaryTree(const BinaryTree<T, AllocU> & other) : BinaryTree(nullptr)
	{
		if (other.root)
			// Copy the tree structure as-is
			replicateStructure(root = createNode(other.root->data), other.root);
		
		numNodes = other.numNodes;
	}

	/// Move constructor
	FORCE_INLINE BinaryTree(BinaryTree<T, AllocT> && other) :
		allocator(other.allocator),
		bHasOwnAllocator(other.bHasOwnAllocator),
		root(other.root),
		numNodes(other.numNodes)
	{
		other.bHasOwnAllocator = false;
		other.root = nullptr;
	}

	/// Copy assignment
	FORCE_INLINE BinaryTree<T, AllocT> & operator=(const BinaryTree<T, AllocT> & other)
	{
		// Copy the tree structure as-is
		if (other.root)
		{
			if (root)
			{
				moveOrCopy(root->data, other.root->data);
				replicateStructure(root, other.root);
			}
			else
				replicateStructure(root = createNode(other.root->data), other.root);
		}
		
		numNodes = other.numNodes;
	}

	/// Copy assignment (different allocator)
	template<typename AllocU>
	FORCE_INLINE BinaryTree<T, AllocT> & operator=(const BinaryTree<T, AllocU> & other)
	{
		// Copy the tree structure as-is
		if (other.root)
		{
			if (root)
			{
				moveOrCopy(root->data, other.root->data);
				replicateStructure(root, other.root);
			}
			else
				replicateStructure(root = createNode(other.root->data), other.root);
		}
		
		numNodes = other.numNodes;
	}

	/// Move assignment
	FORCE_INLINE BinaryTree<T, AllocT> & operator=(BinaryTree<T, AllocT> && other)
	{
		allocator			= other.allocator;
		bHasOwnAllocator	= other.bHasOwnAllocator;
		root				= other.root;
		numNodes			= other.numNodes;

		other.bHasOwnAllocator	= false;
		other.root				= nullptr;
	}

	/// Destructor
	FORCE_INLINE ~BinaryTree()
	{
		// Empty tree
		empty(root);

		// Delete own allocator
		if (bHasOwnAllocator)
			delete allocator;
	}

public:
	/// Get number of nodes
	/// @{
	FORCE_INLINE uint64 getSize() const { return numNodes; }
	FORCE_INLINE uint64 getCount() const { return numNodes; }
	/// @}

	/// Get leftmost node
	/// @{
	FORCE_INLINE Node *			getMin()		{ return root ? root->getMin() : nullptr; }
	FORCE_INLINE const Node *	getMin() const	{ return root ? root->getMin() : nullptr; }
	/// @}

	/// Get rightmost node
	/// @{
	FORCE_INLINE Node *			getMax()		{ return root ? root->getMax() : nullptr; }
	FORCE_INLINE const Node *	getMax() const	{ return root ? root->getMax() : nullptr; }
	/// @}

	/**
	 * Find node matching search
	 * 
	 * @param [in] search search operand
	 * @return node iterator
	 */
	FORCE_INLINE Iterator find(typename ConstRef<T>::Type search) const
	{
		return Iterator(search, root);
	}

	/// Returns an iterator that matches end of search (nil leaf)
	FORCE_INLINE Iterator nil() const { return Iterator(); }

	/// Returns linear iterators
	/// @{
	FORCE_INLINE TreeIterator		begin()			{ return TreeIterator(getMin()); }
	FORCE_INLINE ConstTreeIterator	begin() const	{ return ConstTreeIterator(getMin()); }

	FORCE_INLINE TreeIterator		end()		{ return TreeIterator(); }
	FORCE_INLINE ConstTreeIterator	end() const	{ return ConstTreeIterator(); }

	FORCE_INLINE TreeIterator		last()			{ return TreeIterator(getMax()); }
	FORCE_INLINE ConstTreeIterator	last() const	{ return TreeIterator(getMax()); }
	/// @}

	/**
	 * Insert a new node with the provided data
	 * 
	 * @param [in] data data to insert in node
	 */
	T & insert(typename ConstRef<T>::Type data)
	{
		if (LIKELY(root != nullptr))
		{
			NodeRef node = root->insert(createNode(data));
			++numNodes;
			
			root = root->getRoot();
			
			return node->data;
		}
		else
		{
			root = createNode(data);
			root->color = Node::NodeColor::BLACK;
			numNodes = 1;

			return root->data;
		}
	}

	/**
	 * Insert a new node with the provided data
	 * 
	 * @param [in] data data to insert in node
	 */
	T & insertUnique(typename ConstRef<T>::Type data)
	{
		if (LIKELY(root != nullptr))
		{
			NodeRef node		= createNode(data);
			NodeRef actualNode	= root->insertUnique(node);

			if (node == actualNode)
			{
				++numNodes;
				root = root->getRoot();
			}
			else
				allocator->free(node);

			return actualNode->data;
		}
		else
		{
			root = createNode(data);
			root->color = Node::NodeColor::BLACK;
			numNodes = 1;

			return root->data;
		}
	}

	/**
	 * Remove node with key search
	 * 
	 * * The iterator version is to preferred
	 * 
	 * @param [in] search key search
	 * @param [in] it iterator
	 * @return removed node
	 * @{
	 */
	void remove(typename ConstRef<T>::Type search)
	{
		if (LIKELY(root))
		{
			// Find and remove
			NodeRef node = root->find(search);
			if (node)
			{
				NodeRef evicted = node->remove();
				--numNodes;

				/// Update root
				if (evicted == root)
					root = root->right;
				else
					root = root->getRoot();
				
				// Dealloc evicted node
				allocator->free(evicted);
			}
		}
	}
	void remove(ConstTreeIterator it)
	{
		if (it.node)
		{
			NodeRef evicted = it.node->remove();
			--numNodes;

			// Update root
			if (evicted == root)
				root = root->right;
			else
				root = root->getRoot();

			// Dealloc evicted node
			allocator->free(evicted);
		}
	}
	void remove(TreeIterator it)
	{
		if (it.node)
		{
			NodeRef evicted = it.node->remove();
			--numNodes;

			// Update root
			if (evicted == root)
				root = root->left ? root->left : root->right;
			else
				root = root->getRoot();

			// Dealloc evicted node
			allocator->free(evicted);
		}
	}
	/// @}

	/// Recursively removes all nodes of the tree
	/// @{
	FORCE_INLINE void empty() { return empty(root); }

protected:
	void empty(NodeRef node)
	{
		if (node)
		{
			NodeRef
				left	= node->left,
				right	= node->right;
			
			// Dealloc node
			allocator->free(node);

			// Depth first
			empty(left), empty(right);
		}
	}
	///@}
};
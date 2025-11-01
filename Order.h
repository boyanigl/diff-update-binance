#ifndef ORDER_H
#define ORDER_H

typedef unsigned long long uint64;

template <typename G>
struct Order {
    G price;
    G quantity;
};

template <typename T>
class Orders {
private:
    struct Node {
        T data;
        Node* left;
        Node* right;
        unsigned short height;
        Node(const T& d) : data(d), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;

    int height(Node* n) { return n ? n->height : 0; }

    int getBalance(Node* n) { return n ? height(n->left) - height(n->right) : 0; }

    // Rotations
    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = 1 + max(height(y->left), height(y->right));
        x->height = 1 + max(height(x->left), height(x->right));

        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = 1 + max(height(x->left), height(x->right));
        y->height = 1 + max(height(y->left), height(y->right));


        return y;
    }

    // Recursive insert OR only setting quantity if existing node with balancing
    Node* insertOrSetQuantity(Node* node, const T& key) {
        if (!node) {
            return new Node(key);
        }

        if (key.price < node->data.price)
            node->left = insertOrSetQuantity(node->left, key);
        else if (key.price > node->data.price)
            node->right = insertOrSetQuantity(node->right, key);
		// if double/float is used EPSILON check is NEEDED !!!
        // else if(key.price - node->data.price <= 1e-9 && node->data.price - key.price <= 1e-9) {
		else {
			/* equal case*/
			/* case when the requested price is equal to the current price on the node*/
            node->data.quantity = key.quantity;
            return node;
        }

        // Update height
        node->height = 1 + max(height(node->left), height(node->right));

        // Balance
        int balance = getBalance(node);

        // Left heavy
        if (balance > 1 && key.price < node->left->data.price)
            return rotateRight(node);
        // Right heavy
        if (balance < -1 && key.price > node->right->data.price)
            return rotateLeft(node);
        // Left Right
        if (balance > 1 && key.price > node->left->data.price) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right Left
        if (balance < -1 && key.price < node->right->data.price) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    // Find node with minimum price
    Node* minValueNode(Node* node) const{
        if(!node)  return nullptr;
        Node* current = node;
        while (current->left)
            current = current->left;
        return current;
    }

	// Find node with maximum price
    Node* maxValueNode(Node* node) const{
        if(!node)  return nullptr;
        Node* current = node;
        while (current->right)
            current = current->right;
        return current;
    }

    // Remove node (if needed)
    Node* removeNode(Node* node, const T& key) {
        if (!node)
            return node;

        if (key.price < node->data.price)
            node->left = removeNode(node->left, key);
        else if (key.price > node->data.price)
            node->right = removeNode(node->right, key);
        else {
			//
            // if (node->data.quantity > key.quantity) {
			// 	/* Reducing quantity, but no need of deletion*/
            //     node->data.quantity -= key.quantity;
            //     return node;
            // }

			if (!node->left && !node->right){
				/* No children, remove the node*/
				delete node;
				return nullptr;
			}

            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                delete node;
                return temp;
            }


            Node* minNodeFromRightTree = minValueNode(node->right);
            node->data = minNodeFromRightTree->data;
            node->right = removeNode(node->right, minNodeFromRightTree->data);
        }


        // Update height
        node->height = 1 + max(height(node->left), height(node->right));

        // Balance
        int balance = getBalance(node);

        // Rotations
        if (balance > 1 && getBalance(node->left) >= 0)
            return rotateRight(node);
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && getBalance(node->right) <= 0)
            return rotateLeft(node);
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    void print(Node* root) {
        if (!root)
            return;
        print(root->left);
        printf("Price: %.2f, Quantity: %.2f\n", root->data.price, root->data.quantity);
        print(root->right);
    }

    void clear(Node* root) {
        if (!root)
            return;
        clear(root->left);
        clear(root->right);
        delete root;
    }

	void printStructure(Node* node, int depth = 0) const{
    if (!node) return;
    printStructure(node->right, depth + 1);
    for (int i = 0; i < depth; i++) cout << "    ";
    cout << node->data.price << " (h=" << node->height << ")\n";
    printStructure(node->left, depth + 1);
	}

    void removeOrder(const T& order) { 
		/* Deletion in Orders is O(logn) in Time complexity*/
		root = removeNode(root, order); 
	}


public:
    Orders() : root(nullptr) {}
    Orders(std::vector<T> orders) : root(nullptr) {
		/* Constructor with argument of vector of type Order*/
		/* Needed for the initial snapshot loading*/
		for(const T order : orders){
			updateOrder(order);
		}
	}
    ~Orders() { clear(root); }

    // void addOrder(const T& order) { 
	// 	/* Insertion in Orders is O(logn) in Time complexity*/
	// 	root = insertOrSetQuantity(root, order); 
	// }


	void updateOrder(const T& order){
		/* Update in Orders is O(logn) in Time complexity*/
		// Order<uint64> tempOrder;
		// tempOrder.quantity = static_cast<uint64>(order.quantity * 1e8); // to avoid float/double precision issues
		// tempOrder.price = static_cast<uint64>(order.price * 1e8) ; 
		if(order.quantity == 0u){
			removeOrder(order);
		}else{
			root = insertOrSetQuantity(root, order);
		}
	}

    void printOrders() const{ //print(root);
							
		printStructure(root); 
	}

    uint64 getMaxPrice() const { 
		/* Currently is traversing the most right branch with Time complexity of O(logn)*/
		/* To do: the complexity could be reduced to O(1) with updating the max element when inserting/deleting */
        
		return root ? maxValueNode(root)->data.price : 0; 
	}

	uint64 getMinPrice() const { 
		/* Currently is traversing the most left branch with Time complexity of O(logn)*/
		return root ? minValueNode(root)->data.price : 0;
	}
};

#endif
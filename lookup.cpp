#include <iostream>
#include <string>
#include <vector>

class Node {
public:
    std::string name;
    std::vector<Node*> children;

    Node(const std::string& n)
        : name(n)
    {
    }
};

class GTree {
public:
    Node* root;

    GTree()
    {
        root = new Node("/");
    }

    void insertNode(Node* parent, Node* child)
    {
        parent->children.push_back(child);
    }

    Node* findNode(Node* parent, const std::string& name)
    {
        for (Node* child : parent->children) {
            if (child->name == name) {
                return child;
            }
        }
        return nullptr;
    }
};

void route_mkdir_until(const char* path, GTree& tree)
{
    std::string fullPath(path);
    size_t pos = fullPath.find_first_of('/');

    Node* currentNode = tree.root;
    while (pos != std::string::npos) {
        std::string nodeName = fullPath.substr(0, pos);
        Node* nextNode = tree.findNode(currentNode, nodeName);

        if (!nextNode) {
            nextNode = new Node(nodeName);
            tree.insertNode(currentNode, nextNode);
        }

        currentNode = nextNode;
        pos = fullPath.find_first_of('/', pos + 1);
    }

    // Create the final node in the path
    std::string finalNodeName = fullPath.substr(fullPath.find_last_of('/') + 1);
    Node* finalNode = new Node(finalNodeName);
    tree.insertNode(currentNode, finalNode);
}

int main()
{
    GTree tree;

    // Example usage
    route_mkdir_until("/path/to/uri", tree);

    // Print the tree structure
    std::cout << tree.root->name << std::endl;
    for (Node* child1 : tree.root->children) {
        std::cout << "- " << child1->name << std::endl;
        for (Node* child2 : child1->children) {
            std::cout << "  - " << child2->name << std::endl;
            for (Node* child3 : child2->children) {
                std::cout << "    - " << child3->name << std::endl;
            }
        }
    }

    return 0;
}

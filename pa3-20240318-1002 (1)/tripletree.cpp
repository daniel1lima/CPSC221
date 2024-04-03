/**
 * @file        tripletree.cpp
 * @description Student-implemented functions of a ternary tree for CPSC 221 PA3.
 *
 *              THIS FILE WILL BE SUBMITTED FOR GRADING
 */

#include "tripletree.h"

 /**
      * Constructor that builds a TripleTree out of the given PNG.
      *
      * The TripleTree represents the subimage from (0,0) to (w-1, h-1) where
      * w-1 and h-1 are the largest valid image coordinates of the original PNG.
      * Every node corresponds to a rectangle of pixels in the original PNG,
      * represented by an (x,y) pair for the upper left corner of the
      * square and two integers for the number of pixels on the width and
      * height dimensions of the rectangular region the node defines.
      *
      * The node's three children correspond to a partition
      * of the node's rectangular region into three approximately equal-size strips.
      *
      * If the rectangular region is taller than it is wide, the region is divided
      * into horizontal strips:
      *  +-------+
      *  |   A   |
      *  |       |
      *  +-------+
      *  |   B   |
      *  |       |    (Split tall)
      *  +-------+
      *  |   C   |
      *  |       |
      *  +-------+
      *
      * If the rectangular region is wider than it is tall, the region is divided
      * into vertical strips:
      *  +---------+---------+---------+
      *  |    A    |    B    |    C    |
      *  |         |         |         |    (Split wide)
      *  +---------+---------+---------+
      *
      * Your regions are not guaranteed to have dimensions exactly divisible by 3.
      * If the dimensions of your rectangular region are 3p x q or q x 3p where 3p
      * represents the length of the long side, then your rectangular regions will
      * each have dimensions p x q (or q x p)
      *
      * If the dimensions are (3p+1) x q, subregion B gets the extra pixel of size
      * while subregions A and C have dimensions p x q.
      *
      * If the dimensions are (3p+2) x q, subregions A and C each get an extra pixel
      * of size, while subregion B has dimensions p x q.
      *
      * If the region to be divided is a square, then apply the Split wide behaviour.
      *
      * Every leaf in the constructed tree corresponds to a pixel in the PNG.
      *
      * @param imIn - the input image used to construct the tree
      */
TripleTree::TripleTree(PNG& imIn) {
    // add your implementation below

    pair<unsigned int, unsigned int> ul(0, 0);
    this->root = BuildNode(imIn, ul, imIn.width(), imIn.height());
	
}

/**
 * Render returns a PNG image consisting of the pixels
 * stored in the tree. It may be used on pruned trees. Draws
 * every leaf node's rectangle onto a PNG canvas using the
 * average color stored in the node.
 *
 * You may want a recursive helper function for this.
 */
PNG TripleTree::Render() const {
    // replace the line below with your implementation
    // return PNG();

    PNG im(this->root->width, this->root->height);
    renderRecursive(im, this->root);
    return im;
}

/*
 * Prune function trims subtrees as high as possible in the tree.
 * A subtree is pruned (cleared) if all of its leaves are within
 * tol of the average color stored in the root of the subtree.
 * Pruning criteria should be evaluated on the original tree, not
 * on a pruned subtree. (we only expect that trees would be pruned once.)
 *
 * You may want a recursive helper function for this.
 *
 * @param tol - maximum allowable RGBA color distance to qualify for pruning
 */
void TripleTree::Prune(double tol) {
    // add your implementation below
    PruneHelper(root, tol);

}

void TripleTree::PruneHelper(Node* node, double tolerance) {
    if (node == nullptr) return;

    if (node->A == nullptr && node->B == nullptr && node->C == nullptr) return;

    if (WithinTolerance(node, node->avg, tolerance)) {
        clearChildren(node);
        return;
    }

    PruneHelper(node->A, tolerance);
    PruneHelper(node->B, tolerance);
    PruneHelper(node->C, tolerance);
}

bool TripleTree::WithinTolerance(Node* node, RGBAPixel avg, double tolerance) {
    if (node == nullptr) return true;

    if (node->A == nullptr && node->B == nullptr && node->C == nullptr) return node->avg.distanceTo(avg) <= tolerance;

    return WithinTolerance(node->A, avg, tolerance) &&
           WithinTolerance(node->B, avg, tolerance) &&
           WithinTolerance(node->C, avg, tolerance);
}

void TripleTree::clearChildren(Node* node) {
    if (node == nullptr) return;
    Clear(node->A);
    Clear(node->B);
    Clear(node->C);
}

/**
 * Rearranges the tree contents so that when rendered, the image appears
 * to be mirrored horizontally (flipped over a vertical axis).
 * This may be called on pruned trees and/or previously flipped/rotated trees.
 *
 * You may want a recursive helper function for this.
 */

void TripleTree::FlipHorizontal() {
    horizontalFlip(root);
}

/**
 * Rearranges the tree contents so that when rendered, the image appears
 * to be rotated 90 degrees counter-clockwise.
 * This may be called on pruned trees and/or previously flipped/rotated trees.
 *
 * You may want a recursive helper function for this.
 */
void TripleTree::RotateCCW() {
    // add your implementation below

    // Need to swap root's width and height.
    swapHeightWidth(root);
    // root->upperleft will still be (0, 0).

    recursiveRotateCCW(root);

    return;
}

/*
 * Returns the number of leaf nodes in the tree.
 *
 * You may want a recursive helper function for this.
 */
int TripleTree::NumLeaves() const {
    // replace the line below with your implementation

    return recursiveNumLeaves(root);
}

/**
     * Destroys all dynamically allocated memory associated with the
     * current TripleTree object. To be completed for PA3.
     * You may want a recursive helper function for this one.
     */
void TripleTree::Clear() {
    // add your implementation below
	Clear(root); //start from the root
    root = NULL; 
}

/**
 * Copies the parameter other TripleTree into the current TripleTree.
 * Does not free any memory. Called by copy constructor and operator=.
 * You may want a recursive helper function for this one.
 * @param other - The TripleTree to be copied.
 */
void TripleTree::Copy(const TripleTree& other) {
    // add your implementation below
	if (this == &other) { //check if the tree is the same prior to copying
        return;
    }

    Clear();

    root = copyNode(other.root);
}

/**
 * Private helper function for the constructor. Recursively builds
 * the tree according to the specification of the constructor.
 * 
 * Note: Can't do iterative color averaging: https://piazza.com/class/lr2vkjsws5p7bb/post/1722
 * 
 * @param im - reference image used for construction
 * @param ul - upper left point of node to be built's rectangle.
 * @param w - width of node to be built's rectangle.
 * @param h - height of node to be built's rectangle.
 */
Node* TripleTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
    // replace the line below with your implementation

    if ((w == 0) || (h == 0)){
        return nullptr;
    }

    Node* returnNode = new Node(ul, w, h);

    if ((w == 1) && (h == 1)){

        returnNode->avg = *im.getPixel(ul.first, ul.second);
        return returnNode;
    }

    int dimen = std::max(w, h);
    int dividedA = dimen / 3;
    int dividedB = dividedA;
    int dividedC = dividedA;

    if (dimen % 3 == 1){
        dividedB++;
    } else if (dimen % 3 == 2){
        dividedA++;
        dividedC++;
    }

    if (w >= h) {
    
        pair<unsigned int, unsigned int> ul_B(ul.first + dividedA, ul.second);
        pair<unsigned int, unsigned int> ul_C(ul.first + dividedA + dividedB, ul.second);
        
        returnNode->A = BuildNode(im, ul, dividedA, h);
        returnNode->B = BuildNode(im, ul_B, dividedB, h);
        returnNode->C = BuildNode(im, ul_C, dividedC, h);

        returnNode->avg = avgColor(returnNode, dividedA, dividedB);

    } else {
        pair<unsigned int, unsigned int> ul_B(ul.first, ul.second + dividedA);
        pair<unsigned int, unsigned int> ul_C(ul.first, ul.second + dividedA + dividedB);

        returnNode->A = BuildNode(im, ul, w, dividedA);
        returnNode->B = BuildNode(im, ul_B, w, dividedB);
        returnNode->C = BuildNode(im, ul_C, w, dividedC);

        returnNode->avg = avgColor(returnNode, dividedA, dividedB);
    }

    return returnNode;
}

/* ===== IF YOU HAVE DEFINED PRIVATE MEMBER FUNCTIONS IN tripletree_private.h, IMPLEMENT THEM HERE ====== */

/**
 * Return the WEIGHTED average color of the children nodes of this node
 * 
 * Assumes that the node has at least 2 children.
*/
RGBAPixel TripleTree::avgColor(Node* node, int dividedA, int dividedB){
    int dimen = std::max(node->width, node->height);

    int totalR = (node->A->avg.r + node->C->avg.r) * dividedA;
    int totalG = (node->A->avg.g + node->C->avg.g) * dividedA;
    int totalB = (node->A->avg.b + node->C->avg.b) * dividedA;
    int totalA = (node->A->avg.a + node->C->avg.a) * dividedA;

    if (node->B != NULL){
        totalR += node->B->avg.r * dividedB;
        totalG += node->B->avg.g * dividedB;
        totalB += node->B->avg.b * dividedB;
        totalA += node->B->avg.a * dividedB;
    }

    return RGBAPixel(totalR/dimen, totalG/dimen, totalB/dimen, totalA/dimen);
}

/**
 * recursive helper function for render()
*/
void TripleTree::renderRecursive(PNG& im, Node* node) const {

    if (node == nullptr){
        return;
    }

    if (node->A == nullptr){

        for (unsigned int x = node->upperleft.first; x < node->upperleft.first + node->width; x++){
            for (unsigned int y = node->upperleft.second; y < node->upperleft.second + node->height; y++){
                *im.getPixel(x, y) = node->avg;
            }
        }
        
    } else {

        renderRecursive(im, node->A);
        if (node->B != nullptr){
            renderRecursive(im, node->B);
        }
        renderRecursive(im, node->C);
    }
}

/**
 * recursive helper function for clear()
*/
void TripleTree::Clear(Node*& node) {
    //null base case
    if (node == NULL) {
        return;
    }

    Clear(node->A); 
    Clear(node->B); 
    Clear(node->C); 

    delete node;
    node = NULL;
}

/**
 * recursive helper function for copy()
*/
Node* TripleTree::copyNode(Node* other) {
    if (other == NULL) { 
        return NULL;
    }


    Node* newNode = new Node(other->upperleft, other->width, other->height);
    newNode->avg = other->avg;


    newNode->A = copyNode(other->A);
    newNode->B = copyNode(other->B);
    newNode->C = copyNode(other->C);

    return newNode;
}

/**
 * Recursive helper function for prune that does the pruning
*/
void TripleTree::recursivePrune(Node* node, RGBAPixel& color, double tol){
// nothing right now
return;
}

/**
 * Recursive helper function for NumLeaves, returns numleaves
*/
int TripleTree::recursiveNumLeaves(Node* node) const {
    if (root == NULL){
        return 0;
    }

    Node* rootNode = node;
    int numLeaves = 0;

    if (node->A != NULL) {
        recursiveNumLeaves(node->A);
    }
    if (node->B != NULL) {
        recursiveNumLeaves(node->B);
    }
    if (node->C != NULL) {
        recursiveNumLeaves(node->C);
    }

    numLeaves++;
    return numLeaves;
}

/**
 * Recursie helper function for rotateCCW
*/
void TripleTree::recursiveRotateCCW(Node* node){

    if (node == NULL){
        return;
    }

    if (node->A == NULL){
        return;
    }

    int b_min_dim = 0;

    // Swap children dimensions:
    swapHeightWidth(node->A);
    if (node->B != NULL){
        swapHeightWidth(node->B);
        b_min_dim = std::min(node->B->height, node->B->width);
    }
    swapHeightWidth(node->C);


    if ((2*node->A->height + b_min_dim) == node->height){
        
        Node* temp = node->C;
        node->C = node->A;
        node->A = temp;

        node->A->upperleft = node->upperleft;
        if (node->B != NULL){
            node->B->upperleft.first = node->upperleft.first;
            node->B->upperleft.second = node->upperleft.second + node->A->height;
        }
        node->C->upperleft.first = node->upperleft.first;
        node->C->upperleft.second = node->upperleft.second + node->A->height + b_min_dim;

    } else {
        node->A->upperleft = node->upperleft;
        if (node->B != NULL){
            node->B->upperleft.first = node->upperleft.first + node->A->width;
            node->B->upperleft.second = node->upperleft.second;
        }
        node->C->upperleft.first = node->upperleft.first + node->A->width + b_min_dim;
        node->C->upperleft.second = node->upperleft.second;
    }

    recursiveRotateCCW(node->A);
    recursiveRotateCCW(node->B);
    recursiveRotateCCW(node->C);
}

/**
 * Helper function for rotate
 * Swaps the values in the height and width fields of node
*/
void TripleTree::swapHeightWidth(Node* node){
    unsigned int temp = node->height;
    node->height = node->width;
    node->width = temp;
}







void TripleTree::horizontalFlip(Node* subRoot) {
  if (subRoot == nullptr) {
        return;
    }

    subRoot->upperleft = {root->width - subRoot->upperleft.first - subRoot->width, subRoot->upperleft.second};

    if (subRoot->width >= subRoot->height) {
        //swap node A and B
        Node* tempNode = subRoot->A;
        subRoot->A = subRoot->C;
        subRoot->C = tempNode;
    }

    if (subRoot->A) {
        horizontalFlip(subRoot->A);
        horizontalFlip(subRoot->B);
        horizontalFlip(subRoot->C);
    }
}









// PRUNE FUNCTIONS

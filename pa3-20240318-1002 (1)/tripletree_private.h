/**
 * @file        tripletree_private.h
 * @description student declarations of private functions
 *              for TripleTree, CPSC 221 PA3
 *
 *              THIS FILE WILL BE SUBMITTED.
 *
 *              Simply declare your function prototypes here.
 *              No other scaffolding is necessary.
 */

 // begin your declarations below

RGBAPixel avgColor(Node* node, int dividedA, int dividedB);

void renderRecursive(PNG& im, Node* node) const;

void Clear(Node*& node);

Node* copyNode(Node* other);

double maxDistToChildColor(Node* node, RGBAPixel& color) const;

void recursivePrune(Node* node, RGBAPixel& color, double tol);

int recursiveNumLeaves(Node* node) const; 

void recursiveRotateCCW(Node* node);

void swapHeightWidth(Node* node);

void horizontalFlip(Node* node);

void verticalFlip(Node* node);

void PruneHelper(Node* node, double tolerance);

bool ShouldPrune(RGBAPixel avgColor, Node* node, double tolerance);

bool WithinTolerance(Node* node, RGBAPixel avg, double tolerance);

void clearChildren(Node* node);
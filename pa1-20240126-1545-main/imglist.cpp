/**
 *  @file        imglist.cpp
 *  @description Contains partial implementation of ImgList class
 *               for CPSC 221 PA1
 *               Function bodies to be completed by student
 * 
 *  THIS FILE WILL BE SUBMITTED
 */

#include "imglist.h"

#include <math.h> // provides fmax, fmin, and fabs functions

/*********************
* CONSTRUCTORS, ETC. *
*********************/

/**
 * Default constructor. Makes an empty list
 */
ImgList::ImgList() {
    // set appropriate values for all member attributes here
    northwest = NULL;
    southeast = NULL;
}

/**
 * Creates a list from image data
 * @pre img has dimensions of at least 1x1
 */
ImgList::ImgList(PNG& img) {
    // Build the linked node structure and set the member attributes appropriately

    ImgNode *prevRow[img.width()];
 
    for (unsigned int y = 0; y < img.height(); ++y) {
        ImgNode *prevNode = nullptr;

        for (unsigned int x=0; x < img.width(); ++x) {
            ImgNode* currNode = new ImgNode();
            RGBAPixel* pixel = img.getPixel(x, y);

            // check if this is northwest (first node)
            if (x==0 && y==0) {
                northwest = currNode;
            }

            // check if this is southeast (last node)
            if (x==(img.width() - 1)  && y == (img.height() -1)) {
                southeast = currNode;
            }            

            currNode->colour.r = pixel->r;
            currNode->colour.g = pixel->g;
            currNode->colour.b = pixel->b;
            currNode->colour.a = pixel->a;

            currNode->skipup = 0;
            currNode->skipdown = 0;
            currNode->skipright = 0;
            currNode->skipleft = 0;

            // top
            if (y != 0) {
                currNode->north = prevRow[x];
            } else {
                currNode->north = nullptr;
            }

            // left
            if (x != 0) {
                currNode->west = prevNode;
            } else {
                currNode->west = nullptr;
            }

            //set prevNode east
            if (prevNode != nullptr) {
                currNode->west->east = currNode;
            }
            // set currNode east to null
            currNode->east = nullptr;

            //set aboveNode south
            if (currNode->north != nullptr) {
                currNode->north->south = currNode;
            }
            // set currNode south to null
            currNode->south = nullptr;

            // update
            prevNode = currNode;
            prevRow[x] = currNode;
        }
    }
}
 
/************
* ACCESSORS *
************/

/**
 * Returns the horizontal dimension of this list (counted in nodes)
 * Note that every row will contain the same number of nodes, whether or not
 *   the list has been carved.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionX() const {
    // replace the following line with your implementation
    int dmx = 1;
    ImgNode *currX = northwest;
    while (currX->east != nullptr) {
        ++dmx;
        currX = currX->east;
    }
    return dmx;
}

/**
 * Returns the vertical dimension of the list (counted in nodes)
 * It is useful to know/assume that the grid will never have nodes removed
 *   from the first or last columns. The returned value will thus correspond
 *   to the height of the PNG image from which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   y dimension.
 */
unsigned int ImgList::GetDimensionY() const {
    // replace the following line with your implementation
   int dmy = 1;
    ImgNode *currY = northwest;
    while (currY->south != nullptr) {
        ++dmy;
        currY = currY->south;
    }

    return dmy;
}

/**
 * Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
 * The returned value will thus correspond to the width of the PNG image from
 *   which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionFullX() const {
    // replace the following line with your implementation
    int dmx = 1;
    int skips = 0;
    ImgNode *currX = northwest;
    while (currX->east != nullptr) {
        dmx += 1;
        skips += currX->skipright;
        currX = currX->east;

    }
    return (dmx + skips);
}

/**
 * Returns a pointer to the node which best satisfies the specified selection criteria.
 * The first and last nodes in the row cannot be returned.
 * @pre rowstart points to a row with at least 3 physical nodes
 * @pre selectionmode is an integer in the range [0,1]
 * @param rowstart - pointer to the first node in a row
 * @param selectionmode - criterion used for choosing the node to return
 *          0: minimum "brightness" across row, not including extreme left or right nodes
 *          1: node with minimum total of "colour difference" with its left neighbour and with its right neighbour.
 *        In the (likely) case of multiple candidates that best match the criterion,
 *        the left-most node satisfying the criterion (excluding the row's starting node)
 *        will be returned.
 * A note about "brightness" and "colour difference":
 * For PA1, "brightness" will be the sum over the RGB colour channels, multiplied by alpha.
 * "colour difference" between two pixels can be determined
 * using the "distanceTo" function found in RGBAPixel.h.
 */
ImgNode* ImgList::SelectNode(ImgNode* rowstart, int selectionmode) {
    // add your implementation below

    //first and last can't be returned

    ImgNode* curr = rowstart->east;
    ImgNode* best = rowstart;
    double min = 1000;  // rlly high to start

    if (selectionmode == 0) {

        while (curr->east != nullptr) {
            double brightness = (curr->colour.r + curr->colour.g + curr->colour.b) * curr->colour.a;
            if (brightness < min) {
                best = curr;
                min = brightness;
                 }
            curr = curr->east; 
            }
        } else if (selectionmode == 1) {
            while (curr->east != nullptr) {
                double distanceLeft = curr->colour.distanceTo(curr->west->colour);
                double distanceRight = curr->colour.distanceTo(curr->east->colour);

                double totalDist = distanceLeft + distanceRight;

                if (totalDist < min) {
                    best = curr;
                    min = totalDist;
                }
                curr = curr->east;
            }
        }

        return best;
    }

/**
 * Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
 * @pre fillmode is an integer in the range of [0,2]
 * @param fillgaps - whether or not to fill gaps caused by carving
 *          false: render one pixel per node, ignores fillmode
 *          true: render the full width of the original image,
 *                filling in missing nodes using fillmode
 * @param fillmode - specifies how to fill gaps
 *          0: solid, uses the same colour as the node at the left of the gap
 *          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
 *          2: linear gradient between the colour (all channels) of the nodes at the left and right of the gap
 *             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
 *             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
 *             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
 *             Like fillmode 1, use the smaller difference interval for hue,
 *             and the smaller-valued average for diametric hues
 */
PNG ImgList::Render(bool fillgaps, int fillmode) const {
    // Add/complete your implementation below


    ImgNode *curr = northwest;

    ImgNode *currY = northwest;

    if (!fillgaps) {
        PNG outpng;

        outpng.resize(GetDimensionX(), GetDimensionY());

        for (unsigned int y = 0; y < GetDimensionY(); ++y) { 
            for (unsigned int x = 0; x < GetDimensionX(); ++x) {
                RGBAPixel *pixel = outpng.getPixel(x, y);

                *pixel = curr->colour;
                curr = curr->east;
            }
            currY = currY->south;
            curr = currY;
        }

    return outpng; 
    }

    PNG outpng;

    outpng.resize(GetDimensionFullX(), GetDimensionY());

    switch (fillmode) {
        case 0:
            for (unsigned int y = 0; y < GetDimensionY(); ++y) {
                for (unsigned int x = 0; x < GetDimensionFullX(); ++x) {

                    RGBAPixel *pixel = outpng.getPixel(x, y);
                    *pixel = curr->colour;

                    int skipCount = curr->skipright;
                    while (skipCount > 0) {
                        ++x;
                        RGBAPixel *pixel = outpng.getPixel(x, y);
                        *pixel = curr->colour;
                        --skipCount;
                    }
                    curr = curr->east;
                }
                currY = currY->south;
                curr = currY;
            }
            break;
        case 1:
            for (unsigned int y = 0; y < GetDimensionY(); ++y) {
                for (unsigned int x = 0; x < GetDimensionFullX(); ++x) 
                {
                    RGBAPixel *pixel = outpng.getPixel(x, y);
                    *pixel = curr->colour;

                    int skipCount = curr->skipright;
                    if (skipCount > 0) {
                        RGBAPixel right = curr->east->colour;
                        RGBAPixel left = curr->colour;

                        int avgR = (right.r + left.r) / 2;
                        int avgG = (right.g + left.g) / 2;
                        int avgB = (right.b + left.b) / 2;
                        double avgA = (right.a + left.a) / 2;

                        while (skipCount > 0) {
                            ++x;

                            RGBAPixel *newPixel = outpng.getPixel(x, y);

                            newPixel->r = avgR;
                            newPixel->g = avgG;
                            newPixel->b = avgB;
                            newPixel->a = avgA;
                            --skipCount;
                        }
                    }
                    curr = curr->east;
                }
                currY = currY->south;
                curr = currY;
            }
            break;
        case 2:
            for (unsigned int y = 0; y < GetDimensionY(); ++y) {
                for (unsigned int x = 0; x < GetDimensionFullX(); ++x) {
                    RGBAPixel *pixel = outpng.getPixel(x, y);
                    *pixel = curr->colour;

                    int skipCount = curr->skipright;
                    double divisor = skipCount + 1;

                    if (skipCount > 0) {
                        RGBAPixel right = curr->east->colour;
                        RGBAPixel left = curr->colour;
                        
                        double num = 0;

                        while (skipCount > 0) {
                            ++x;
                            ++num;
                            double division = num / divisor;

                            int r0 = (int) (left.r + (right.r - left.r) * division);
                            int g0 = (int) (left.g + (right.g - left.g) * division);
                            int b0 = (int) (left.b + (right.b - left.b) * division);
                            double a0 = (left.a + (right.a - left.a) * division);

                            RGBAPixel *newPixel = outpng.getPixel(x, y);

                            newPixel->r = r0;
                            newPixel->g = g0;
                            newPixel->b = b0;
                            newPixel->a = a0;
                            --skipCount;
                        }
                    }
                    curr = curr->east;
                }
                currY = currY->south;
                curr = currY;
            }
            break;
    }
    return outpng;
}

/************
* MODIFIERS *
************/

/**
 * Removes exactly one node from each row in this list, according to specified criteria.
 * The first and last nodes in any row cannot be carved.
 * @pre this list has at least 3 nodes in each row
 * @pre selectionmode is an integer in the range [0,1]
 * @param selectionmode - see the documentation for the SelectNode function.
 * @param this list has had one node removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */

// 0: minimum "brightness" across row, not including extreme left or right nodes
//  *          1: node with minimum total of "colour difference" with its left neighbour and with its right neighbour.
//  *        In the (likely) case of multiple candidates that best match the criterion,
//  *        the left-most node satisfying the criterion (excluding the row's starting node)
//  *        will be returned.

void ImgList::Carve(int selectionmode) {
    // add your implementation here
    ImgNode *currY = northwest;

    for (unsigned int y = 0; y < GetDimensionY(); ++y) {

        ImgNode *selected = SelectNode(currY, selectionmode);

        ImgNode *prevNode = selected->west;
        ImgNode *nextNode = selected->east;
        nextNode->west = prevNode;
        prevNode->east = nextNode;

        if (selected->south != nullptr) {
            selected->south->skipup += 1;
            if (selected->north != nullptr) {
                selected->south->north = selected->north;
            } else {
                selected->south->north = nullptr;
            }
        }

        if (selected->north != nullptr) {
            selected->north->skipdown += 1;
            if (selected->south != nullptr) {
                selected->north->south = selected->south;
            } else {
                selected->north->south = nullptr;
            }
        }

        int skippingleft = selected->skipleft + 1;
        while (skippingleft > 0) {
            nextNode->skipleft += 1;
            --skippingleft;
        }




        int skippingright = selected->skipright + 1;
        while (skippingright > 0) {
            prevNode->skipright += 1;
            --skippingright;
        }

        delete selected;

        currY = currY->south;
    }

    return;
}

// note that a node on the boundary will never be selected for removal
/**
 * Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
 * based on specific selection criteria.
 * Note that this should remove one node from every row, repeated "rounds" times,
 * and NOT remove "rounds" nodes from one row before processing the next row.
 * @pre selectionmode is an integer in the range [0,1]
 * @param rounds - number of nodes to remove from each row
 *        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
 *        i.e. Ensure that the final list has at least two nodes in each row.
 * @post this list has had "rounds" nodes removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 */
void ImgList::Carve(unsigned int rounds, int selectionmode) {

    unsigned int maxRounds = GetDimensionX() - 2;

    if (rounds > maxRounds) {
        rounds = maxRounds;
    }

    while (rounds > 0) {
        if (GetDimensionX() == 2) {
            break;
        }

        Carve(selectionmode);
        --rounds;
    }
    return;
}


/*
 * Helper function deallocates all heap memory associated with this list,
 * puts this list into an "empty" state. Don't forget to set your member attributes!
 * @post this list has no currently allocated nor leaking heap memory,
 *       member attributes have values consistent with an empty list.
 */
void ImgList::Clear() {
    ImgNode *currNode = northwest;
    ImgNode *currY = northwest;

    while (currY != nullptr) {
        currY = currY->south;

        while (currNode != nullptr) {
            ImgNode *temp = currNode;

            currNode = currNode->east;
            delete temp;
        }
        currNode = currY;
    }
    northwest = nullptr;
    southeast = nullptr;

}

/**
 * Helper function copies the contents of otherlist and sets this list's attributes appropriately
 * @pre this list is empty
 * @param otherlist - list whose contents will be copied
 * @post this list has contents copied from by physically separate from otherlist
 */
void ImgList::Copy(const ImgList& otherlist) {
    // add your implementation here
    Clear();
    ImgNode *prevRow[otherlist.GetDimensionFullX()];
    ImgNode *currOtherNode = otherlist.northwest;
    ImgNode *otherY = otherlist.northwest;
    int actX = 0;

    for (unsigned int y = 0; y < otherlist.GetDimensionY(); ++y) {

        ImgNode *prevNode = nullptr;

        for (unsigned int x = 0; x < otherlist.GetDimensionY(); ++x) {
            RGBAPixel currPixel = currOtherNode->colour;
            ImgNode *currNode = new ImgNode();

            if (x == 0 && y == 0) {
                northwest = currNode;
            }

            if (x == (otherlist.GetDimensionX() - 1) && y == (otherlist.GetDimensionY() - 1)) {
                southeast = currNode;
            }

            currNode->colour.r = currPixel.r;
            currNode->colour.g = currPixel.g;
            currNode->colour.b = currPixel.b;
            currNode->colour.a = currPixel.a;

            currNode->skipup = currOtherNode->skipup;
            currNode->skipdown = currOtherNode->skipdown;
            currNode->skipright = currOtherNode->skipright;
            currNode->skipleft = currOtherNode->skipleft;

            if (y != 0) {
                if (prevRow[actX] != nullptr) {
                    currNode->north = nullptr;
                }
                currNode->north = prevRow[actX];
            } else {
                currNode->north = nullptr;
            }

            if (x != 0) {
                currNode->west = prevNode;
            } else {
                currNode->west = nullptr;
            }

            if (prevNode != nullptr) {
                currNode->west->east = currNode;
            }
            currNode->east = nullptr;

            if (currNode->north != nullptr) {
                currNode->north->south = currNode;
            }
            currNode->south = nullptr;

            prevNode = currNode;
            prevRow[x] = currNode;
            int count = 1;
            int skips = currNode->skipright;
            while (skips > 0) {
                prevRow[x + count] = nullptr;
                count++;
                skips--;

            }
            actX += skips;
            actX++;
            currOtherNode = currOtherNode->east;
        }

        currOtherNode = otherY->south;
        otherY = otherY->south;
        actX = 0;
    }
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist-private.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/


#include "MinHeap.h"

//Constructs MinHeap with userInput containing bookID of books user inputed
//and reviews containing Key: userID, Value: vectors of bookID this user liked
MinHeap::MinHeap(vector<int> userInput, unordered_map<int, vector<int>> reviews)
{
    for (auto it = reviews.begin(); it != reviews.end(); ++it)
    {
        vector<int> likedBooks = it->second;
        int factor = 0;

        //check how many inputted books this user liked
        for (int i = 0; i < userInput.size(); ++i)
        {
            if (find(likedBooks.begin(), likedBooks.end(), userInput[i])
                                                    != likedBooks.end())
                ++factor;
        }

        //Update rating score for each reviewed books
        for (int i = 0; i < likedBooks.size(); ++i)
        {
            int bookID = likedBooks[i];
            //if this is not the inputted book itself, insert to rating score list
            if (find(userInput.begin(), userInput.end(), bookID) == userInput.end())
            {
                if (ratingScore.find(bookID) == ratingScore.end())
                    ratingScore[bookID] = factor;
                else
                    ratingScore[bookID] += factor;
            }
        }
    }

    //Print ratingScore for debugging
    // for (auto it = ratingScore.begin(); it != ratingScore.end(); ++it)
    //     cout << "BookID: " << it->first << ", Similarity: " << it->second << endl;
}

MinHeap::MinHeap(unordered_map<int, int> _ratingScore)
{
    ratingScore = _ratingScore;
}

void MinHeap::insert(int bookID)
{
    int insertInd = heap.size();
    heap.push_back(bookID);

    if (insertInd == 0)
        return;
    
    int parentInd = (insertInd - 1)/2;

    auto insertPair = ratingScore.find(heap[insertInd]);
    auto parentPair = ratingScore.find(heap[parentInd]);
    int insertScore = insertPair->second;
    int parentScore = parentPair->second;

    //If there is multiple books with same similarity score, keep the earlier ones
    while (insertInd != 0 && parentInd >= 0 && insertScore < parentScore)
    {
        int temp = heap[parentInd];
        heap[parentInd] = heap[insertInd];
        heap[insertInd] = temp;
        insertInd = parentInd;
        parentInd = (insertInd - 1)/2;
        
        //update similarity scores
        parentPair = ratingScore.find(heap[parentInd]);
        parentScore = parentPair->second;
    }
}

int MinHeap::pop()
{
    if (heap.size() == 0)
    {
        cout << "Can't remove - heap is empty!" << endl;
        return -1;
    }
    else if (heap.size() == 1)
    {
        int min = heap[0];
        heap.clear();
        return min;
    }

    int min = heap[0];
    heap[0] = heap[heap.size() - 1];
    vector<int>::iterator it = --heap.end();
    heap.erase(it);

    int currentScore = ratingScore.find(heap[0])->second;
    int currentInd = 0;

    int childInd1, childInd2;
    int childScore1, childScore2;
    int compare = -1;

    while (currentInd != compare) //while the element keeps heapyfied down
    {
        compare = currentInd;
        childInd1 = currentInd * 2 + 1;
        childInd2 = currentInd * 2 + 2;

        if (childInd1 < heap.size())
        {
            childScore1 = ratingScore.find(heap[childInd1])->second;
            //if second child index is valid, check both children and swap down
            if (childInd2 < heap.size())
            {
                childScore2 = ratingScore.find(heap[childInd2])->second;
                //if at least one of the child is smaller than current index
                if (childScore1 < currentScore || childScore2 < currentScore)
                {
                    //if first child is smaller, swap
                    if (childScore1 < childScore2)
                    {
                        int temp = heap[currentInd];
                        heap[currentInd] = heap[childInd1];
                        heap[childInd1] = temp;
                        currentInd = childInd1;
                    }
                    else //if second child is smaller, swap
                    {
                        int temp = heap[currentInd];
                        heap[currentInd] = heap[childInd2];
                        heap[childInd2] = temp;
                        currentInd = childInd2;
                    }
                }
            }
            else //if second child not valid, just check first child
            {
                if (childScore1 < currentScore)
                {
                    int temp = heap[currentInd];
                    heap[currentInd] = heap[childInd1];
                    heap[childInd1] = temp;
                    currentInd = childInd1;
                }
            }
        }

        currentScore = ratingScore.find(heap[currentInd])->second;
    }

    return min;
}

//Returns minimum value in the heap
int MinHeap::min()
{
    if (heap.size() == 0)
        return -1; //if no books inserted, return -1
    return heap[0]; 
}

//prints heap, not in order for similarity score
void MinHeap::print()
{
    for (int i = 0; i < heap.size() - 1; ++i)
        cout << heap[i] << ", ";
    cout << heap[heap.size() - 1] << endl;
}

//returns array of bookID's in order, but destroys the heap
vector<int> MinHeap::returnBooks()
{
    int size = heap.size();
    vector<int> books(size);
    for (int i = size - 1; i >= 0; --i)
        books.at(i) = pop();

    //for debugging purposes
    if (debug)
    {
        for (int i = 0; i < size; ++i)
        {
            cout << "Book #" << i + 1 << ": " << books[i] << ", ";
            cout << ratingScore[books[i]] << " people also liked this book" << endl;
        }
    }

    return books;
}

//update heap with bookNum of recommended books
//returns array with recommended books, in decreasing order of similarity score
vector<int> MinHeap::recommendBooks(int bookNum)
{
    unordered_map<int, int>::iterator it = ratingScore.begin();
    for (; it != ratingScore.end(); ++it)
    {
        int bookID = min();
        int currentScore;
        if (bookID != -1)
            currentScore = ratingScore.find(min())->second;
        else
            currentScore = -1;
        
        if (it->second > currentScore)
        {
            insert(it->first);
            if (heap.size() > bookNum)
                pop();
        }
    }

    return returnBooks();
}
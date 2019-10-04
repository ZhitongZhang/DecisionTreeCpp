//ZHITONG ZHANG  COMP135 HW1 
//DECISION TREE FROM SCATCH in C++

// INCLUDES
#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <cmath>
#include <unordered_map>
#include <sstream>
#include <iomanip>
//USING STANDARD NAMESPACE
using namespace std;

// STRUCT AND CLASSES USED
struct Feat{
    int feature;
    string types;
};
class Node{
    public:
    vector<Node *> child;
    vector<char> label;

    int attribute; //attributes are int because each int is mapped to an actual trait
    Node(int attri){                     //attri = 100 will represent a poisonous node, and 101 will represent a edible
        attribute = attri;

    }
    Node(){
        attribute = -1;
    }
};
//input: Node*, vector<Feat>, string, int reference
//output: none
//functionality: prints the tree with attribute and attribute number
void printTree(Node* root, vector<Feat> features, string s, int& branchnum){
    if (root->child.size() == 0){
        if (root->attribute == 100){
            s += " Poison.";
            cout << "Branch[" << branchnum << "]" << s << endl;
            branchnum++;
        }
        else{
            s += " Edible.";
            cout << "Branch[" << branchnum << "]" << s << endl;
            branchnum++;
        }
    }
    else{
        s += ":  Attribute #";
        s += to_string(root->attribute);
        s += ": ";
        for (auto feats : features){
            if (root->attribute == feats.feature){
                stringstream ss(feats.types);
                string str;
                int count = 0;
                while (ss >> str){
                    s += str;
                    printTree(root->child[count], features, s, branchnum);
                    s = s.substr(0, s.length() - 1);
                    count++;
                }
            }
        }
    }
}
//input: Node*
//output: none
//functionality: free memory for the tree
void deletetree(Node* root){
    if (root == NULL)
        return;
    for (auto childs : root->child)
        deletetree(childs);
    delete root;
} 
//INPUT: vector<string> 
//OUTPUT: char
//functionality: returns most common last char ('e' or 'p') within a vector of string. 
char mostfreqlabel(vector<string> train){
    unordered_map<char,int> hash;
    for (auto datas : train){
        hash[datas[datas.length() - 1]]++;
    }
    if (hash['p'] > hash['e'])
        return 'p';
    return 'e';
} 
//INPUT: vector<string>
//OUTPUT: none
//functionality: prints the strings in the container; main purpose was to aid in catching bugs
void printvec(vector<string> the_vec){
    for (auto vec: the_vec){
        cout << vec << endl;
    }
}
//INPUT: Node pointer, string, and vector<Feat>
// OUTPUT: bool
//functionality: checks if the tree is a correct predictor for mushroom
bool correct(Node* root,string mushroom, vector<Feat> features){
    if (root->child.size() == 0){  // at leaf
        if (root->attribute == 100 and mushroom[mushroom.length() - 1] == 'p') //100 is poisionous
            return true;
        else if (root->attribute == 100 and mushroom[mushroom.length() - 1] == 'e')
            return false;
        else if (root->attribute == 101 and mushroom[mushroom.length() - 1] == 'e')
            return true;
        else if (root->attribute == 101 and mushroom[mushroom.length() - 1] == 'p')
            return false;
    }
    char currattribute = mushroom[root->attribute * 2];
    for (auto feats: features){
        if (feats.feature == root->attribute){
            stringstream ss(feats.types);
            int count = 0;
            string s;
            while (ss >> s){
                if (s[0] == currattribute)
                    return correct(root->child[count], mushroom, features);
                count++;
            }
        }
    }
    return false;
}
//INPUT = vector<string>
//OUTPUT = bool
//functionality: checks of all label within a vector<string> is the same
bool allsamelabel(vector<string> incrementvec){
    int pcount = 0, ecount = 0;
    for (auto mushrooms : incrementvec){
        if (mushrooms[mushrooms.length() - 1] == 'e')
            ecount++;
        else
            pcount++;
    }
    if (ecount > 0 and pcount > 0) 
        return false;
    return true;
}
//INPUT: int
//output: Node*
//functionality: creates new node of int value key
Node *newNode(int key) 
{ 
    Node *temp = new Node; 
    temp->attribute = key; 
    return temp; 
} 
//INPUT: vector<string>
//output: float
//functionality: counts amount of edible mushrooms within the vector
float countedible(vector<string> incrementvec){
    float  ecount = 0;
    for (int i = 0; i < incrementvec.size() ; i++){
        if (incrementvec[i][incrementvec[i].length() - 1] == 'e')
        ecount++;
    }
    return ecount;

}
//Input: vector<Feat> and vector<string>
//output: int
//functionality: returns most the feature that has the most information gain.
int mostimportanti(vector<Feat> features, vector<string> incrementvec){
    //cout << " in ";
    float edibleprob = countedible(incrementvec) / (incrementvec.size());
    float poisionprob = (incrementvec.size() - countedible(incrementvec)) / (incrementvec.size());
    float overall_entropy = -1 * (edibleprob * log2(edibleprob) + poisionprob * log2(poisionprob));
    float bestgain = 0, currentropy;
    int bestfeature;
    for (auto feats : features){
        stringstream ss(feats.types);
        string s;
        float sum = 0;
        while (ss >> s){
            float ediblecount = 0, amountoftype = 0;
            for (auto data : incrementvec){
                if (s[0] == data[feats.feature * 2]){
                    amountoftype++;
                    if(data[data.length() - 1] == 'e')
                        ediblecount++;
                }
            }
            //cout << amountoftype << " " << ediblecount << endl;
            if (ediblecount != 0) // to make sure log0 = 1
                edibleprob = ediblecount / amountoftype;
            else 
                edibleprob = 1;
            if (amountoftype - ediblecount != 0) //making sure log0 = 1 
                poisionprob = (amountoftype - ediblecount) / amountoftype; //possible float-int problem here?
            else
                poisionprob = 1;
            currentropy = -1 * (edibleprob*log2(edibleprob) + poisionprob*log2(poisionprob));
            sum += (amountoftype / incrementvec.size()) * currentropy;
        }
        //cout << sum << " ";
        if (overall_entropy - sum > bestgain){
            bestgain = overall_entropy - sum;
            bestfeature = feats.feature;
        }
    }
    return bestfeature;
}
//Input: vector<Feat> and vector<string>
//output: int
//functionality: returns most the feature based on basic counting
int mostimportantb(vector<Feat> features, vector<string> incrementvec){
     float poisoncount, ediblecount;
     string s;
     float count, correct, max = 0;
     int bestfeature = -1; //just to catch errors
     for (auto feats : features){
         stringstream ss(feats.types); //branches
         count = 0;
         correct = 0;
         while (ss >> s){
            poisoncount = 0;
            ediblecount = 0;
            for (auto data : incrementvec){
                if (s[0] == data[feats.feature * 2]){
                    if (data[data.length() - 1] == 'e')
                        ediblecount++;
                    else
                        poisoncount++;
                }
                count += ediblecount + poisoncount;
                if (ediblecount > poisoncount)
                    correct += ediblecount;
                else
                    correct += poisoncount;
            }
         }
         if (correct / count > max){
             max = correct / count;
             bestfeature = feats.feature;
         }
     }
     return bestfeature;
 }
 //Input: vector<string>, vector<Feat>, char, string, vector<Feat>
 //Output: Node*
 //functionality: builds the decision tree
Node* DecisionTreeTraining(vector<string> incrementvec, vector<Feat> features, char parent_guess, string heuristic,const vector<Feat> fixedfeature){
    char guess = mostfreqlabel(incrementvec);
    Node *toreturn = new Node; 
    if (incrementvec.size() == 0){
        if (parent_guess == 'p')
            toreturn->attribute = 100;
        else
            toreturn->attribute = 101;
        return toreturn;
    }
    else if (allsamelabel(incrementvec) or features.size() == 0){
       // printvec(incrementvec);
        // cout << "first cond " << endl; 
        if (guess == 'p')
            toreturn->attribute = 100; //100 is poision
        else
            toreturn->attribute = 101; //100 is edible
        return toreturn; 
    }
    else{
        int currfeature;
        if (heuristic == "i")
            currfeature = mostimportanti(features, incrementvec);
        else
            currfeature = mostimportantb(features, incrementvec);
        Node *currnode = newNode(currfeature);
        //cout << fixedfeature[currfeature].types << endl;
        stringstream ss(fixedfeature[currfeature].types); //utilize stringstream to get rid of whitespaces
        string f; //the current feature of attribute
        vector<string> newdata;
        features.erase(features.begin() + currfeature);
        while (ss >> f){
            //cout << f[0] << endl;
            for (auto mushrooms : incrementvec){
                if (mushrooms[currfeature * 2] == f[0]){ //the index of the respective features are 2 * the feature number
                    newdata.push_back(mushrooms); //cout << "match!" << endl;
                }
            }
            Node* subtree = DecisionTreeTraining(newdata, features, guess, heuristic, fixedfeature);
            currnode->child.push_back(subtree);
            currnode->label.push_back(f[0]);
            newdata.clear();
        }
        return currnode;
    }
}
//Input: vector<string> reference, int, vector<string> reference
//Output: none
//functionality: splits the training set and testing set
void traintestsplit(vector<string> &data, int size, vector<string> &train){
    srand(time(NULL));
    while (size != 0){
        int randint = rand() %  data.size();
        train.push_back(data[randint]);
        data.erase(data.begin() + randint);
        size--;
    }
}
//Input: int reference, int reference, string reference
//output: bool
// functionality: Reads in the 3 parameters needed
bool readin(int &size, int &increment, string &heuristic)
{
    cout << "size of training set? -must be greater than or equal to 250, less than or equal to 1000, and a multiple of 250 -\n ";
    cin >> size;
    if (size < 250 or size > 1000 or size % 250 != 0)
    {
        cout << "size must be between 250 and 1000, inclusive, and must be divisible by 250\n";
        return false;
    }
    cout << "How much to increment? -increment must be 10, 25, or 50- \n";
    cin >> increment;
    if (increment != 50 and increment != 25 and increment != 10)
    {
        cout << "increment must be 10 25 or 50\n";
        return false;
    }
    cout << "basic count heuristic or info-theoretic heuristic? - enter b for bch and i for ith\n";
    cin >> heuristic;
    if (heuristic != "i" and heuristic != "b")
    {
        cout << "please either enter i or b\n";
        return false;
    }
    return true;
}
//driver function
int main()
{
    int size, increment;
    string heuristic;
    if (not readin(size, increment, heuristic))
        return 1;
    vector<string> data;
    ifstream infile;
    cout << "Loading Data from database.\n";

    infile.open("mushroom_data.txt");
    string s;
    while (getline(infile,s)){
        data.push_back(s);
    }
    infile.close();
    vector<string> train;
    cout << "Collecting set of 250 training examples.\n";
    traintestsplit(data, size, train);
    vector<string> incrementvec(train.begin(),train.begin() + increment);
    vector<Feat> features;
    Feat to_push;
    ifstream infile2;
    cout << "Loading Property Information from file.\n";
    infile2.open("properties.txt");
    int i = 0;
    while (getline(infile2,s)){
        size_t found = s.find(":");
        s = s.substr(found + 1);
        to_push.feature = i;
        to_push.types = s;
        features.push_back(to_push);
        i++;
    }
    infile2.close();
    float correctpercentage, correctcount = 0;
    Node *root;
    int sum = increment;
    vector<float> successrates;
    while (sum <= train.size()){
        cout << "Running with " << sum << " examples in training set.\n\n";
        vector<string> incrementvec(train.begin(),train.begin() + sum);
        root = DecisionTreeTraining(incrementvec, features, 'p', heuristic, features);
        correctcount = 0;
        for (auto mushrooms : data){
            if (correct(root, mushrooms, features))
                correctcount++;
        }
        if (sum != train.size()) // since dont want our last tree to be deleted b/c we want it to be printed.
            deletetree(root); //to free the memory
        correctpercentage = correctcount / data.size();
        successrates.push_back(correctpercentage);
        cout << "Given current tree, there are " << correctcount << " correct classifications out of " << data.size() << " possible (a success rate of " << setprecision(6) << correctpercentage * 100 << " percent).\n\n";
        
        sum += increment;
        // need a function to destruct tree
        incrementvec.clear();
    } 
    int branches = 0;
    cout << "\nFinal Decision Tree\n";
    printTree(root, features, "" , branches);
    deletetree(root);

    cout << "\nStatistics\n";
    int incre = increment;
    int j = 0;
    while (incre <= size){
        cout << "Training set size: " << incre << ".   Success:   " << successrates[j] * 100 << " percent. \n";
        j++; 
        incre += increment;
    }
    return 0;
}